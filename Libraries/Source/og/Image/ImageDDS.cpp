// ==============================================================================
//! @file
//! @brief	DDS File Support
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2007-2010 Lusito Software
// ==============================================================================
//
// The Open Game Libraries.
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// ==============================================================================

#include <og/Image/Image.h>
#include "ImageEx.h"

#include <og/Gloot/glmin.h>
#include <GL/glext.h>

namespace og {
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC og_glCompressedTexImage2DARB = NULL;

struct ddsHeader{
	char magic[4];
	uInt size;
	uInt flags;
	uInt height;
	uInt width;
	uInt linearSize;
	uInt depth;
	uInt mipMapCount;
	uInt reserved1[11];

	struct pixelFormat_s {
		uInt size;
		uInt flags;
		char fourCC[4];
		uInt bpp;
		uInt bitMaskRed;
		uInt bitMaskGreen;
		uInt bitMaskBlue;
		uInt bitMaskAlpha;
	} pixelFormat;

	uInt caps[4];
	uInt reserved2;
};

/*
==============================================================================

  ImageFileDDS

==============================================================================
*/

/*
================
ImageFileDDS::UploadFile
================
*/
bool ImageFileDDS::UploadFile( const char *filename, ImageEx &image ) {
	if ( !Open( filename ) )
		return false;

	image.width = width;
	image.height = height;
	image.mipmap = numMipmaps > 1;

	glGenTextures( 1, &image.glTextureNum );
	glBindTexture( GL_TEXTURE_2D, image.glTextureNum );

	if ( image.mipmap ) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ImageEx::minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ImageEx::magFilter);
	} else {
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}

	uInt mipSize;
	uInt mipNum = 0;
	int mipWidth = width;
	int mipHeight = height;
	uInt blockSize = ( dxtFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ) ? 8 : 16;
	byte *src_data = dynBuffer.data;

	// Picmip
	int picMip = ImageEx::picmip;

	//! @todo	currently, this does not read dds files without mipmaps.. maybe it should be i<= ?
	for( int i=0; i<numMipmaps; i++ ) {
		mipSize = ((mipWidth + 3) / 4) * ((mipHeight + 3) / 4) * blockSize;

		if ( picMip )
			picMip--;
		else
			og_glCompressedTexImage2DARB( GL_TEXTURE_2D, mipNum++, dxtFormat, mipWidth, mipHeight, 0, mipSize, src_data );

		mipWidth = Max(mipWidth >> 1, 1);
		mipHeight = Max(mipHeight >> 1, 1);

		src_data += mipSize;
	}
	return true;
}

/*
================
ImageFileDDS::SaveFile
================
*/
bool ImageFileDDS::SaveFile( const char *filename, byte *data, uInt width, uInt height, bool hasAlpha ) {
	User::Error( ERR_BAD_FILE_FORMAT, "Can not save DDS files.", filename );
	return false;
}

/*
================
ImageFileDDS::Open
================
*/
bool ImageFileDDS::Open( const char *filename ) {
	if ( imageFS == NULL )
		return false;

	File *file = imageFS->OpenRead( filename, true, true );
	if ( file == NULL )
		return false;

	try {
		// Check the file header
		ddsHeader header;
		file->Read( &header, sizeof(ddsHeader) );
		if ( String::Cmpn( header.magic, "DDS ", 4 ) != 0 ) {
			file->Close();
			User::Error( ERR_BAD_FILE_FORMAT, "File is no DDS file.", filename );
			return false;
		}

		width = header.width;
		height = header.height;
		numMipmaps = header.mipMapCount;

		int dxtNumber = -1;
		if ( String::Cmpn( header.pixelFormat.fourCC, "DXT", 3 ) == 0 )
			dxtNumber = header.pixelFormat.fourCC[3] - '0';

		switch( dxtNumber ) {
			case 1:
				dxtFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				break;
			case 3:
				dxtFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				break;
			case 5:
				dxtFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				break;
			default:
				file->Close();
				User::Error( ERR_BAD_FILE_FORMAT, "File has unsupported compression method, must be DXT1/3/5.", filename );
				return false;
		};

		int bufSize = file->Size() - file->Tell();
		dynBuffer.CheckSize( bufSize );

		file->Read( dynBuffer.data, bufSize );

		file->Close();
		return true;
	}
	catch( FileReadWriteError err ) {
		file->Close();
		User::Error( ERR_FILE_CORRUPT, Format("DDS: $*" ) << err.ToString(), filename );
		return false;
	}
}

}
