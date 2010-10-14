// ==============================================================================
//! @file
//! @brief	JPEG File Support
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

#include <setjmp.h> 
extern "C" { 
	#define XMD_H
	#include <jpeg/jpeglib.h>
	#undef XMD_H
}

namespace og {
/*
==============================================================================

  JPG Callbacks

==============================================================================
*/
const int INPUT_BUF_SIZE	= 4096;
const int OUTPUT_BUF_SIZE	= 4096;

struct og_jpeg_error_mgr : jpeg_error_mgr {
  jmp_buf setjmp_buffer;
};
struct og_jpeg_destination_mgr : public jpeg_destination_mgr {
	File *file;
	JOCTET *buffer; 
}; 
struct og_jpeg_source_mgr : public jpeg_source_mgr {
	File *file;
	JOCTET *buffer; 
}; 

/*
================
jpegErrorExit
================
*/
METHODDEF(void) jpegErrorExit( j_common_ptr cinfo ) {
	char buffer[JMSG_LENGTH_MAX];
	(*cinfo->err->format_message)(cinfo,buffer);
	throw FileReadWriteError( buffer );
}

/*
================
jpegOutputMessage
================
*/
METHODDEF(void) jpegOutputMessage( j_common_ptr cinfo ) {
	char buffer[JMSG_LENGTH_MAX];
	(*cinfo->err->format_message)(cinfo,buffer);
	User::Warning( Format("Jpeg: $*" ) << buffer );
}

/*
================
jpgInitSource
================
*/
METHODDEF(void) jpgInitSource( j_decompress_ptr cinfo ) {
	og_jpeg_source_mgr *src = (og_jpeg_source_mgr *) cinfo->src;
	src->buffer = (JOCTET *)(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, INPUT_BUF_SIZE * sizeof(JOCTET));
}

/*
================
jpgFillInputBuffer
================
*/
METHODDEF(boolean) jpgFillInputBuffer( j_decompress_ptr cinfo ) {
	og_jpeg_source_mgr *src = (og_jpeg_source_mgr *) cinfo->src;

	if ( src->file->Eof() ) {
		// Insert a fake EOI marker
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		src->next_input_byte = src->buffer;
		src->bytes_in_buffer = 2;

		//! @todo	decide what to do here
		User::Error( ERR_FILE_CORRUPT, "Jpeg: Unexpected end of file", src->file->GetFileName() );
		return TRUE; 
	}
	int available = src->file->Size() - src->file->Tell();
	int toRead = Min(available, INPUT_BUF_SIZE);
	src->file->Read( src->buffer, toRead );
	src->next_input_byte = src->buffer;
	src->bytes_in_buffer = toRead;
	return TRUE; 
}

/*
================
jpgSkipInputData
================
*/
METHODDEF(void) jpgSkipInputData( j_decompress_ptr cinfo, long num_bytes ) {
	og_jpeg_source_mgr *src = (og_jpeg_source_mgr *) cinfo->src;

	// sometimes, jpeg tries to read more than exists, so we need to prevent a seek exception.
	if ( (num_bytes + src->file->Tell()) > src->file->Size() )
		num_bytes = src->file->Size() - src->file->Tell();
	if ( num_bytes > 0 )
		src->file->Seek( num_bytes, SEEK_CUR );
}

/*
================
jpgTermSource
================
*/
METHODDEF(void) jpgTermSource( j_decompress_ptr cinfo ){
}

/*
================
jpgInitDestination
================
*/
METHODDEF(void) jpgInitDestination( j_compress_ptr cinfo ) {
	og_jpeg_destination_mgr *dest = (og_jpeg_destination_mgr *) cinfo->dest;

	dest->buffer = (JOCTET *) (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		OUTPUT_BUF_SIZE * sizeof(JOCTET));
	dest->next_output_byte = dest->buffer;
	dest->free_in_buffer = OUTPUT_BUF_SIZE;
}

/*
================
jpgTermDestination
================
*/
METHODDEF(void) jpgTermDestination( j_compress_ptr cinfo ) {
	og_jpeg_destination_mgr *dest = (og_jpeg_destination_mgr *) cinfo->dest;

	size_t datacount = OUTPUT_BUF_SIZE - dest->free_in_buffer;
	if ( datacount > 0 )
		dest->file->Write( dest->buffer, datacount );
}

/*
================
jpgEmptyOutputBuffer
================
*/
METHODDEF(boolean) jpgEmptyOutputBuffer( j_compress_ptr cinfo ) {
	og_jpeg_destination_mgr *dest = (og_jpeg_destination_mgr *) cinfo->dest;

	dest->file->Write( dest->buffer, OUTPUT_BUF_SIZE );
	dest->next_output_byte = dest->buffer;
	dest->free_in_buffer = OUTPUT_BUF_SIZE;
	return TRUE;
}
 

/*
==============================================================================

  ImageFileJPG

==============================================================================
*/
/*
================
ImageFileJPG::Open
================
*/
bool ImageFileJPG::Open( const char *filename ) {
	if ( imageFS == NULL )
		return false;

	File *file = imageFS->OpenRead( filename, true, true );
	if ( !file )
		return NULL;

	struct jpeg_decompress_struct cinfo;
	try {
		og_jpeg_error_mgr jerr;

		cinfo.err = jpeg_std_error(&jerr);
		jerr.error_exit = jpegErrorExit;
		jerr.output_message = jpegOutputMessage;

		jpeg_create_decompress( &cinfo );

		og_jpeg_source_mgr *src = (struct og_jpeg_source_mgr *) (*cinfo.mem->alloc_small)
							((j_common_ptr) &cinfo, JPOOL_PERMANENT, sizeof(og_jpeg_source_mgr));
		cinfo.src = src;

		src->init_source = jpgInitSource;
		src->fill_input_buffer = jpgFillInputBuffer;
		src->skip_input_data = jpgSkipInputData;
		src->resync_to_restart = jpeg_resync_to_restart;
		src->term_source = jpgTermSource;
		src->file = file;
		src->bytes_in_buffer = 0;
		src->next_input_byte = NULL;

		jpeg_read_header( &cinfo, TRUE );

		if ( cinfo.out_color_space != JCS_RGB ) {
			jpeg_destroy_decompress( &cinfo );
			file->Close();
			User::Error( ERR_BAD_FILE_FORMAT, "JPEG: File is not using RGB colorspace.", filename );
			return false;
		}

		jpeg_start_decompress( &cinfo );

		width = cinfo.image_width;
		height = cinfo.image_height;
		hasAlpha = false;

		uInt stride = width * 3;
		int size = stride * height;
		
		dynBuffers[curBuffer].CheckSize( size );
		byte *dst_line = dynBuffers[curBuffer].data;

		JSAMPARRAY tempbuf = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, stride, 1 );

		while ( cinfo.output_scanline < cinfo.output_height ) {
			jpeg_read_scanlines( &cinfo, tempbuf, 1 );
			memcpy( dst_line, tempbuf[0], stride );
			dst_line += stride;
		}

		jpeg_finish_decompress( &cinfo );
		jpeg_destroy_decompress( &cinfo );

		file->Close();
		return true;
	}
	catch( FileReadWriteError err ) {
		jpeg_destroy_decompress(&cinfo);
		file->Close();
		User::Error( ERR_FILE_CORRUPT, Format("Jpeg: $*" ) << err.ToString(), filename );
		return false;
	}
}

/*
================
ImageFileJPG::SaveFile
================
*/
bool ImageFileJPG::SaveFile( const char *filename, byte *data, uInt width, uInt height, bool hasAlpha ) {
	if ( imageFS == NULL )
		return false;

	File *file = imageFS->OpenWrite(filename);
	if ( !file )
		return false;

	try {
		struct jpeg_compress_struct cinfo;
		og_jpeg_error_mgr jerr;

		cinfo.err = jpeg_std_error(&jerr);
		jerr.error_exit = jpegErrorExit;
		jerr.output_message = jpegOutputMessage;

		jpeg_create_compress( &cinfo );

		og_jpeg_destination_mgr *dest = (og_jpeg_destination_mgr *) (*cinfo.mem->alloc_small)
											((j_common_ptr) &cinfo, JPOOL_PERMANENT, sizeof(og_jpeg_destination_mgr)); 
		cinfo.dest = dest;
		dest->init_destination = jpgInitDestination;
		dest->empty_output_buffer = jpgEmptyOutputBuffer;
		dest->term_destination = jpgTermDestination;
		dest->file = file;

		cinfo.image_width = width;
		cinfo.image_height = height;
		cinfo.input_components = 3;
		cinfo.in_color_space = JCS_RGB;
		jpeg_set_defaults( &cinfo );

		jpeg_set_quality(&cinfo, ImageEx::jpegQuality, TRUE);

		jpeg_start_compress(&cinfo, TRUE);

		DynBuffer<byte> buffer;
		JSAMPLE *image_buffer;
		int stride = cinfo.image_width * 3;
		if ( !hasAlpha ) 
			image_buffer = data;
		else {
			buffer.CheckSize(stride*width);
			byte *dst_pixel = image_buffer = buffer.data;
			byte *src_pixel = data;
			int xy = width*height;
			// Read RGB
			for( int i=0; i<xy; i++, dst_pixel+=3, src_pixel+=4 ) {
				dst_pixel[0] = src_pixel[0];
				dst_pixel[1] = src_pixel[1];
				dst_pixel[2] = src_pixel[2];
			}
		}

		JSAMPROW row_pointer[1];
		while ( cinfo.next_scanline < cinfo.image_height ) {
			row_pointer[0] = &image_buffer[cinfo.next_scanline * stride];
			jpeg_write_scanlines( &cinfo, row_pointer, 1 );
		}
		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);

		file->Close();
		return true;
	}
	catch( FileReadWriteError err ) {
		file->Close();
		User::Error( ERR_FILE_WRITEFAIL, Format("Jpeg: $*" ) << err.ToString(), filename );
		return false;
	}
}

}
