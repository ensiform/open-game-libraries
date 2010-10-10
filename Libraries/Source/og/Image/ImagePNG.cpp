// ==============================================================================
//! @file
//! @brief	PNG File Support
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
#include <libpng/png.h>

namespace og {

/*
==============================================================================

  PNG Callbacks

==============================================================================
*/
/*
================
pngReadCallback
================
*/
static void pngReadCallback( png_structp png_ptr, png_bytep data, png_size_t length ) {
	File *file = (File *)png_ptr->io_ptr;
	file->Read( data, length );
}

/*
================
pngWriteCallback
================
*/
static void pngWriteCallback( png_structp png_ptr, png_bytep data, png_size_t length ) {
	File *file = (File *)png_ptr->io_ptr;
	file->Write( data, length );
}

/*
================
pngWriteFlushCallback
================
*/
static void pngWriteFlushCallback( png_structp png_ptr ) {
	File *file = (File *)png_ptr->io_ptr;
	file->Flush();
}

/*
================
pngWarning
================
*/
void pngWarning( png_structp png_ptr, png_const_charp message ) {
	User::Warning( Format("PNG: $*" ) << message );
}

/*
================
pngError
================
*/
void pngError( png_structp png_ptr, png_const_charp message ) {
	throw FileReadWriteError( message );
}

/*
==============================================================================

  ImageFilePNG

==============================================================================
*/
/*
================
ImageFilePNG::Open
================
*/
bool ImageFilePNG::Open( const char *filename ) {
	if ( imageFS == NULL )
		return false;

	File *file = imageFS->OpenRead( filename, true, true );
	if ( file == NULL )
		return false;

	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	try {
		png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, pngError, pngWarning );

		if ( png_ptr == NULL ) {
			User::Error( ERR_OUT_OF_MEMORY, "png_create_read_struct", "-1" );
			file->Close();
			return false;
		}

		info_ptr = png_create_info_struct(png_ptr);
		if ( info_ptr == NULL ) {
			png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
			User::Error( ERR_OUT_OF_MEMORY, "png_create_info_struct", "-1" );
			file->Close();
			return false;
		}

		png_set_read_fn( png_ptr, file, pngReadCallback );

		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, png_voidp_NULL);

		if ( info_ptr->color_type != PNG_COLOR_TYPE_RGB_ALPHA && info_ptr->color_type != PNG_COLOR_TYPE_RGB )
			throw FileReadWriteError( "Not of type RGB or RGBA" );

		height = info_ptr->height;
		width = info_ptr->width;
		hasAlpha = info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA;

		dynBuffers[curBuffer].CheckSize( info_ptr->rowbytes * height );
		byte *dst_line = dynBuffers[curBuffer].data;
		int x, y;
		for( y=0; y<height; y++, dst_line+=info_ptr->rowbytes ) {
			for( x=0; x<info_ptr->rowbytes; x++ )
				dst_line[x] = info_ptr->row_pointers[y][x];
		}

		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

		file->Close();
		return true;
	}
	catch( FileReadWriteError err ) {
		if ( png_ptr )
			png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		file->Close();
		User::Error( ERR_FILE_CORRUPT, Format("PNG: $*" ) << err.ToString(), filename );
		return false;
	}
}

/*
================
ImageFilePNG::SaveFile
================
*/
bool ImageFilePNG::SaveFile( const char *filename, byte *data, uInt width, uInt height, bool hasAlpha ) {
	if ( imageFS == NULL )
		return false;

	File *file = imageFS->OpenWrite(filename);
	if ( file == NULL )
		return false;

	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	try {
		png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, pngError, pngWarning );

		if ( png_ptr == NULL ) {
			User::Error( ERR_OUT_OF_MEMORY, "png_create_write_struct", "-1" );
			file->Close();
			return false;
		}

		info_ptr = png_create_info_struct(png_ptr);
		if ( info_ptr == NULL ) {
			png_destroy_write_struct( &png_ptr,  png_infopp_NULL );
			User::Error( ERR_OUT_OF_MEMORY, "png_create_info_struct", "-1" );
			file->Close();
			return false;
		}

		png_set_write_fn( png_ptr, file, pngWriteCallback, pngWriteFlushCallback );

		memset( info_ptr, 0, sizeof(png_info) );

		info_ptr->width = width;
		info_ptr->height = height;
		info_ptr->color_type = hasAlpha ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB;
		info_ptr->channels = hasAlpha ? 4 : 3;
		info_ptr->rowbytes = width * info_ptr->channels;
		info_ptr->bit_depth = 8;
		info_ptr->pixel_depth = info_ptr->bit_depth * info_ptr->channels;
		info_ptr->valid = PNG_INFO_IDAT;

		DynBuffer<byte *> buffer(info_ptr->height);
		info_ptr->row_pointers = buffer.data;
		
		byte *src_line = data;
		for( int row=0; row<height; row++, src_line+=info_ptr->rowbytes )
			info_ptr->row_pointers[row] = src_line;

		png_write_png( png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, png_voidp_NULL );

		info_ptr->row_pointers = NULL;

		png_destroy_write_struct( &png_ptr, &info_ptr );

		file->Close();
		return true;
	}
	catch( FileReadWriteError err ) {
		if ( png_ptr )
			png_destroy_write_struct( &png_ptr, &info_ptr );
		file->Close();
		User::Error( ERR_FILE_WRITEFAIL, Format("PNG: $*" ) << err.ToString(), filename );
		return false;
	}
}

}
