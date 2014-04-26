// ==============================================================================
//! @file
//! @brief	Targa File Support
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

#include <og/Image.h>
#include "ImageEx.h"

namespace og {
/*
==============================================================================

  ImageFileTGA
	
==============================================================================
*/
const int TGA_RLE_COMPRESSED_BIT	= 0x80;
const int TGA_RLE_SIZE_BITS			= 0x7F;
const int TGA_FLAG_TOPDOWN			= 0x20;

/*
================
ImageFileTGA::Open
================
*/
bool ImageFileTGA::Open( const char *filename ) {
	isLoaded = false;
	if ( imageFS == OG_NULL )
		return false;

	File *file = imageFS->OpenRead( filename, true, true );
	if ( !file )
		return false;

	try {
		// Read Header
		byte id_length, colorMapType, imageTypeCode;
		uShort colorMapOrigin, colorMapLength;
		byte colorMapDepth;
		uShort x,y,w,h;
		byte bpp,imageDescriptor;

		id_length		= file->ReadByte();
		colorMapType	= file->ReadByte();
		imageTypeCode	= file->ReadByte();
		colorMapOrigin	= file->ReadUshort();
		colorMapLength	= file->ReadUshort();
		colorMapDepth	= file->ReadByte();
		x				= file->ReadUshort();
		y				= file->ReadUshort();
		w				= file->ReadUshort();
		h				= file->ReadUshort();
		bpp				= file->ReadByte();
		imageDescriptor	= file->ReadByte();

		if ( colorMapType != 0 ) {
			file->Close();
			User::Error( ERR_BAD_FILE_FORMAT, "Colormaps not allowed in Targa files.", filename);
			return false;
		}
		if ( imageTypeCode != 2 && imageTypeCode != 3 && imageTypeCode != 10 ) {
			file->Close();
			User::Error( ERR_BAD_FILE_FORMAT, "Only Targa files of type 2, 3 or 10 allowed.", filename);
			return false;
		}
		if ( bpp != 32 && bpp != 24 && imageTypeCode != 3 ) {
			file->Close();
			User::Error( ERR_BAD_FILE_FORMAT, "Only 24/32 bit Targa files allowed.", filename);
			return false;
		}
		// Skip comment
		if ( id_length != 0 )
			file->Seek( id_length, SEEK_CUR );

		width = w;
		height = h;
		hasAlpha = bpp == 32;
		bool topDown = (imageDescriptor & TGA_FLAG_TOPDOWN) != 0;

		if ( imageTypeCode == 2 )
			ReadType2( file, topDown );
		else if ( imageTypeCode == 3 )
			ReadType3( file, topDown );
		else
			ReadType10( file, topDown );

		file->Close();
		isLoaded = true;
		return true;
	}
	catch( FileReadWriteError &err ) {
		file->Close();
		User::Error( ERR_FILE_CORRUPT, Format("Targa: $*" ) << err.ToString(), filename );
		return false;
	}
}

/*
================
ImageFileTGA::ReadType2

RGB and RGBA
================
*/
void ImageFileTGA::ReadType2( File *file, bool topDown ) {
	int size = width * height * ( hasAlpha ? 4 : 3 );
	dynBuffers[curBuffer].CheckSize( size );
	byte *dst_data = dynBuffers[curBuffer].data;

	if ( hasAlpha ) {
		if ( topDown ) {
			// Read BGRA
			for( int i=0; i<size; i+=4 ) {
				dst_data[i+2]	= file->ReadByte();
				dst_data[i+1]	= file->ReadByte();
				dst_data[i]		= file->ReadByte();
				dst_data[i+3]	= file->ReadByte();
			}
		} else {
			int x, y;
			int lineLen = width*4;
			byte *dst_pixel;
			for( y=height-1; y>=0; y-- ) {
				dst_pixel = dst_data + lineLen*y;
				// Read BGRA
				for( x=0; x<width; x++, dst_pixel+=4 ) {
					dst_pixel[2] = file->ReadByte();
					dst_pixel[1] = file->ReadByte();
					dst_pixel[0] = file->ReadByte();
					dst_pixel[3] = file->ReadByte();
				}
			}
		}
	} else {
		if ( topDown ) {
			// Read BGR
			for( int i=0; i<size; i+=3 ) {
				dst_data[i+2]	= file->ReadByte();
				dst_data[i+1]	= file->ReadByte();
				dst_data[i]		= file->ReadByte();
			}
		} else {
			int x, y;
			int lineLen = width*3;
			byte *dst_pixel;
			for( y=height-1; y>=0; y-- ) {
				dst_pixel = dst_data + lineLen*y;
				// Read BGR
				for( x=0; x<width; x++, dst_pixel+=3 ) {
					dst_pixel[2] = file->ReadByte();
					dst_pixel[1] = file->ReadByte();
					dst_pixel[0] = file->ReadByte();
				}
			}
		}
	}
}

/*
================
ImageFileTGA::ReadType3

Grayscale
================
*/
void ImageFileTGA::ReadType3( File *file, bool topDown ) {
	int size = width * height * 3;
	dynBuffers[curBuffer].CheckSize( size );
	byte *dst_data = dynBuffers[curBuffer].data;

	if ( topDown ) {
		// Read Gray
		for( int i=0; i<size; i+=3 )
			dst_data[i] = dst_data[i+1] = dst_data[i+2] = file->ReadByte();
	} else {
		int x, y;
		int lineLen = width*3;
		byte *dst_pixel;
		for( y=height-1; y>=0; y-- ) {
			dst_pixel = dst_data + lineLen*y;
			// Read Gray
			for( x=0; x<width; x++, dst_pixel+=3 )
				dst_pixel[0] = dst_pixel[1] = dst_pixel[2] = file->ReadByte();
		}
	}
}

/*
================
ImageFileTGA::ReadType10

Run Length Encoded
================
*/
void ImageFileTGA::ReadType10( File *file, bool topDown ) {
	int pixelSize = hasAlpha ? 4 : 3;
	int size = width * height * pixelSize;
	dynBuffers[curBuffer].CheckSize( size );
	byte *dst_ptr;

	int lineStep;
	if ( topDown ) {
		dst_ptr = dynBuffers[curBuffer].data;
		lineStep = 0;
	} else {
		dst_ptr = dynBuffers[curBuffer].data + (height - 1)* width * pixelSize;
		lineStep = -2 * width * pixelSize;
	}

	int x = 0;
	int y = 0;
	byte rlePacketInfo;
	byte red,green,blue,alpha;
	int rlePacketSize, toRead;
	while( y < height ) {
		rlePacketInfo = file->ReadByte();
		rlePacketSize = 1 + (rlePacketInfo & TGA_RLE_SIZE_BITS);

		if ( rlePacketInfo & TGA_RLE_COMPRESSED_BIT ) {
			// The next rlePacketSize pixels will be this color
			blue	= file->ReadByte();
			green	= file->ReadByte();
			red		= file->ReadByte();
			if ( hasAlpha )
				alpha = file->ReadByte();

			// Process rlePacketSize Pixels
			while( rlePacketSize && y < height ) {
				toRead = width - x;
				if ( toRead <= rlePacketSize )
					rlePacketSize -= toRead;
				else {
					toRead = rlePacketSize;
					rlePacketSize = 0;
				}

				x += toRead;
				while( toRead-- ) {
					*dst_ptr++ = red;
					*dst_ptr++ = green;
					*dst_ptr++ = blue;
					if ( hasAlpha )
						*dst_ptr++ = alpha;
				}
				if ( x == width ) {
					x = 0;
					y++;
					dst_ptr += lineStep;
				}
			}
		} else {
			// The next rlePacketSize pixels will be all different
			while ( rlePacketSize && y < height ) {
				toRead = width - x;
				if ( toRead < rlePacketSize )
					rlePacketSize -= toRead;
				else {
					toRead = rlePacketSize;
					rlePacketSize = 0;
				}

				x += toRead;
				while( toRead-- ) {
					dst_ptr[2] = file->ReadByte();
					dst_ptr[1] = file->ReadByte();
					dst_ptr[0] = file->ReadByte();
					if ( hasAlpha )
						dst_ptr[3] = file->ReadByte();
					dst_ptr+=pixelSize;
				}
				if ( x == width ) {
					x = 0;
					y++;
					dst_ptr += lineStep;
				}
			}
		}
	}
}

/*
================
ImageFileTGA::Save
================
*/
bool ImageFileTGA::Save( const char *filename, byte *data, uInt width, uInt height, bool hasAlpha ) {
	if ( imageFS == OG_NULL )
		return false;

	File *file = imageFS->OpenWrite( filename );
	if ( !file )
		return false;

	try {
		// Write Header
		file->WriteByte( 0 );		// id_length
		file->WriteByte( 0 );		// colorMapType
		file->WriteByte( 2 );		// imageTypeCode
		file->WriteUshort( 0 );		// colorMapOrigin
		file->WriteUshort( 0 );		// colorMapLength
		file->WriteByte( 0 );		// colorMapDepth
		file->WriteUshort( 0 );		// x
		file->WriteUshort( 0 );		// y
		file->WriteUshort( width );	// width
		file->WriteUshort( height );// height
		file->WriteByte( hasAlpha ? 32 : 24 ); // bpp
		file->WriteByte( 0 );		// imageDescriptor

		int pixelSize = hasAlpha ? 4 : 3;
		int size = width * height * pixelSize;

		// Write Data
		if ( hasAlpha ) {
			// RGBA
			int x, y;
			int lineLen = width*4;
			byte *src_pixel;
			for( y=height-1; y>=0; y-- ) {
				src_pixel = data + lineLen*y;
				// Write BGRA
				for( x=0; x<width; x++, src_pixel+=4 ) {
					file->WriteByte( src_pixel[2] );
					file->WriteByte( src_pixel[1] );
					file->WriteByte( src_pixel[0] );
					file->WriteByte( src_pixel[3] );
				}
			}
		} else {
			int x, y;
			int lineLen = width*3;
			byte *src_pixel;
			for( y=height-1; y>=0; y-- ) {
				src_pixel = data + lineLen*y;
				// Write BGR
				for( x=0; x<width; x++, src_pixel+=3 ) {
					file->WriteByte( src_pixel[2] );
					file->WriteByte( src_pixel[1] );
					file->WriteByte( src_pixel[0] );
				}
			}
		}
		file->Close();
		return true;
	}
	catch( FileReadWriteError &err ) {
		file->Close();
		User::Error( ERR_FILE_WRITEFAIL, Format("Targa: $*" ) << err.ToString(), filename );
		return false;
	}
}

}
