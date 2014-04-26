// ==============================================================================
//! @file
//! @brief	Image Loading
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

#include <og/Common/Thread/PreloadManager.h>
#include <og/Gloot/glmin.h>
#include <og/Image.h>
#include <og/Math.h>
#include <GL/glext.h>
#include "ImageEx.h"

namespace og {
class ImagePreloadJob;
FileSystemCore *imageFS = OG_NULL;

typedef byte *( *imageLoadFunc_t )( const char *filename, uInt &width, uInt &height, bool &hasAlpha );
typedef bool ( *imageSaveFunc_t )( const char *filename, byte *data, uInt width, uInt height, bool hasAlpha );
extern PFNGLCOMPRESSEDTEXIMAGE2DARBPROC og_glCompressedTexImage2DARB;

const int MAX_PICMIP = 32; //! @todo	what max value would be good ?

bool	ImageEx::denyPrecompressed = false;
int		ImageEx::roundDownLimit = 0;
int		ImageEx::picmip = 0;
int		ImageEx::minFilter = GL_LINEAR;
int		ImageEx::magFilter = GL_LINEAR;
int		ImageEx::maxTextureSize = 256;
int		ImageEx::jpegQuality = 90;

static DictEx<ImageEx> imageList;
static DictEx<ImageFile *> imageFileTypes;
static Image *defaultImage = OG_NULL;

class ImagePreloadTask : public PreloadTask {
public:
	ImagePreloadTask( const char *_filename ) : filename(_filename), file(OG_NULL) {}
	~ImagePreloadTask() {
		delete file;
	}
	bool	Preload( void ) {
		int index = ImageEx::GetFileTypeIndex( filename );
		if ( index != -1 ) {
			file = imageFileTypes[index]->GetNew();
			if ( file->Open( filename.c_str() ) )
				return true;
			delete file;
			file = OG_NULL;
		}
		return false;
	}
	void	Synchronize( void ) {
		if ( file ) {
			ImageEx &img = imageList[filename.c_str()];
			if ( file->Upload( img ) )
				img.time = imageFS->FileTime( filename.c_str() );
		}
	}

private:
	String filename;
	ImageFile *file;
};

/*
==============================================================================

  Image

==============================================================================
*/
/*
================
Image::Init
================
*/
bool Image::Init( FileSystemCore *fileSystem, const char *defaultFilename, void *user_glCompressedTexImage2DARB ) {
	OG_ASSERT( fileSystem != OG_NULL );
	OG_ASSERT( defaultFilename != OG_NULL );

	imageFS = fileSystem;

	static ImageFileTGA tgaHandler;
	static ImageFilePNG pngHandler;
	static ImageFileJPG jpgHandler;
	static ImageFileDDS ddsHandler;

	imageFileTypes["tga"] = &tgaHandler;
	imageFileTypes["png"] = &pngHandler;
	imageFileTypes["jpg"] = &jpgHandler;

	og_glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)user_glCompressedTexImage2DARB;
	if ( og_glCompressedTexImage2DARB )
		imageFileTypes["dds"] = &ddsHandler;
	else {
		ImageEx::denyPrecompressed = true;
		User::Warning("glCompressedTexImage2DARB not found, ignoring DDS files.");
	}
	
	glGetIntegerv( GL_MAX_TEXTURE_SIZE, (GLint *)&ImageEx::maxTextureSize );

	defaultImage = Image::Find( defaultFilename );
	return defaultImage != OG_NULL;
}

/*
================
Image::Shutdown
================
*/
void Image::Shutdown( void ) {
	imageList.Clear();
	imageFileTypes.Clear();
	defaultImage = OG_NULL;
	imageFS = OG_NULL;
}

/*
================
Image::ReloadImages
================
*/
uInt Image::ReloadImages( bool force, PreloadManager *preloadManager ) {
	if ( preloadManager == OG_NULL )
		Image::SetFilters( ImageEx::minFilter, ImageEx::magFilter );

	uInt numReloads = 0;
	int num = imageList.Num();
	for( int i=0; i<num; i++ ) {
		if ( imageList[i].ReloadImage( force, preloadManager ) )
			numReloads++;
	}

	return numReloads;
}

/*
================
Image::Find
================
*/
Image *Image::Find( const char *filename ) {
	if ( filename[0] == '\0' )
		return defaultImage;

	int index = imageList.Find( filename );
	if ( index != -1 )
		return &imageList[index];

	ImageEx *img = &imageList[filename];
	if ( !img->UploadImage( filename ) ) {
		imageList.Remove( imageList.Find( filename ) );
		return defaultImage;
	}
	return img;
}
/*
================
Image::Free
================
*/
void Image::Free( Image *image ) {
	//! @todo	can this be done nicer ?
	int max = imageList.Num();
	for( int i=0; i<max; i++ ) {
		if ( &imageList[i] == image ) {
			imageList.Remove(i);
			return;
		}
	}
}

/*
================
Image::Save
================
*/
bool Image::Save( const char *filename, byte *data, int width, int height, bool hasAlpha ) {
	String extension;
	String::GetFileExtension( filename, String::ByteLength(filename), extension );

	int index = -1;
	if ( !extension.IsEmpty() )
		index = imageFileTypes.Find( extension.c_str() );
	if ( index == -1 ) {
		User::Warning( Format("Unknown image type for file '$*'" ) << filename );
		return false;
	}

	return imageFileTypes[index]->Save( filename, data, width, height, hasAlpha );
}

/*
================
Image::SetRoundDown
================
*/
void Image::SetRoundDown( int limit ) {
	ImageEx::roundDownLimit = Math::ClosestPowerOfTwo( Clamp( limit, 2, ImageEx::maxTextureSize ) );
}

/*
================
Image::SetPicmip
================
*/
void Image::SetPicmip( int value ) {
	ImageEx::picmip = Clamp( value, 0, MAX_PICMIP );
}

/*
================
Image::SetFilters
================
*/
void Image::SetFilters( int min, int mag ) {
	ImageEx::minFilter = min;
	ImageEx::magFilter = mag;

	// Update filters for all images.
	int num = imageList.Num();
	for( int i=0; i<num; i++ ) {
		ImageEx &img = imageList[i];
		if ( img.mipmap ) {
			img.BindTexture();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ImageEx::minFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ImageEx::magFilter);
		}
	}
}

/*
================
Image::SetJpegQuality
================
*/
void Image::SetJpegQuality( int quality ) {
	ImageEx::jpegQuality = Clamp( quality, 0, 100 );
}

/*
================
Image::DenyPrecompressed
================
*/
void Image::DenyPrecompressed( bool value ) {
	ImageEx::denyPrecompressed = value;
}

/*
================
Image::PreloadImage
================
*/
PreloadTask *Image::PreloadImage( const char *filename ) {
	return new ImagePreloadTask(filename);
}

/*
==============================================================================

  ImageEx

==============================================================================
*/
/*
================
ImageEx::~ImageEx
================
*/
ImageEx::~ImageEx() {
	if ( glTextureNum )
		glDeleteTextures( 1, &glTextureNum );
}

/*
================
ImageEx::BindTexture
================
*/
void ImageEx::BindTexture( void ) const {
	glBindTexture(GL_TEXTURE_2D, glTextureNum );
}

/*
================
ImageEx::UploadImage
================
*/
bool ImageEx::UploadImage( const char *filename ) {
	if ( imageFS == OG_NULL )
		return false;

	fullpath = filename;
	int index = ImageEx::GetFileTypeIndex( fullpath );

	if ( !imageFileTypes[index]->Open( fullpath.c_str() ) )
		return false;
	if ( !imageFileTypes[index]->Upload( *this ) )
		return false;

	time = imageFS->FileTime( fullpath.c_str() );
	return true;
}

/*
================
ImageEx::ReloadImage
================
*/
bool ImageEx::ReloadImage( bool force, PreloadManager *preloadManager ) {
	if ( imageFS == OG_NULL )
		return false;

	time_t newTime = imageFS->FileTime( fullpath.c_str() );
	if ( !force && newTime > time )
		return false;
	String extension = fullpath.GetFileExtension();
	int index = imageFileTypes.Find( extension.c_str() );
	if ( index == -1 ) {
		User::Warning( Format("Unknown image type for file '$*'" ) << fullpath );
		return false;
	}
	if ( preloadManager != OG_NULL ) {
		preloadManager->AddTask( new ImagePreloadTask(fullpath.c_str()) );
		return true;
	}
	if ( !imageFileTypes[index]->Open( fullpath.c_str() ) )
		return false;
	if ( !imageFileTypes[index]->Upload( *this ) )
		return false;
	time = newTime;
	return true;
}

/*
================
ImageEx::GetFileTypeIndex
================
*/
int ImageEx::GetFileTypeIndex( String &filename ) {
	String extension = filename.GetFileExtension();

	// If no extension was given, try to find an image with the known extensions
	if ( extension.IsEmpty() ) {
		static const char *validExtensions[4] = { "dds", "tga", "png", "jpg" };
		String newFilename;
		// skips dds if denyPrecompressed is set.
		for( int i=ImageEx::denyPrecompressed; i<4; i++ ) {
			newFilename = Format( "$*.$*") << filename << validExtensions[i];
			if ( imageFS->FileExists( newFilename.c_str() ) ) {
				filename = newFilename;
				extension = validExtensions[i];
				break;
			}
		}
	}
	int index = imageFileTypes.Find( extension.c_str() );
	if ( index == -1 )
		User::Warning( Format("Unknown image type for file '$*'" ) << filename );
	return index;
}

/*
==============================================================================

  ImageFileNoDXT

==============================================================================
*/
/*
================
ImageFileNoDXT::Upload
================
*/
bool ImageFileNoDXT::Upload( ImageEx &image ) {
	if ( !isLoaded )
		return false;

	image.width = width;
	image.height = height;
	image.mipmap = false; //! @todo	generate some or leave mimaps to dds ?

	int pixelSize = hasAlpha?4:3;
	ResampleAsNeeded();

	glGenTextures( 1, &image.glTextureNum );
	glBindTexture( GL_TEXTURE_2D, image.glTextureNum );
	glTexImage2D( GL_TEXTURE_2D, 0, pixelSize, width, height, 0, hasAlpha?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, dynBuffers[curBuffer].data );

	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	isLoaded = false;
	return true;
}

/*
================
ImageFileNoDXT::ResampleAsNeeded
================
*/
void ImageFileNoDXT::ResampleAsNeeded( void ) {
	// OpenGL needs power of 2 sizes.
	int newWidth = Math::IsPowerOfTwo( width ) ? width : Math::ClosestPowerOfTwo( width );
	int newHeight = Math::IsPowerOfTwo( height ) ? height : Math::ClosestPowerOfTwo( height );

	// The user wants to round down images if new w/h are bigger than original
	if ( ImageEx::roundDownLimit != 0 ) {
		if ( newWidth > width && (newWidth >> 1) >= ImageEx::roundDownLimit )
			newWidth >>= 1;
		if ( newHeight > height && (newHeight >> 1) >= ImageEx::roundDownLimit )
			newHeight >>= 1;
	}
	// Scale down even further
	if ( ImageEx::picmip ) {
		newWidth >>= ImageEx::picmip;
		newHeight >>= ImageEx::picmip;
	}
	// Can't get bigger than maxTextureSize
	newWidth = Clamp( newWidth, 1, ImageEx::maxTextureSize );
	newHeight = Clamp( newHeight, 1, ImageEx::maxTextureSize );

	if ( width != newWidth || height != newHeight )
		ResampleFast( newWidth, newHeight );
}

/*
================
ImageFileNoDXT::ResampleFast
================
*/
void ImageFileNoDXT::ResampleFast( uInt newWidth, uInt newHeight  ) {
	DynBuffer<byte> *srcBuffer = &dynBuffers[curBuffer];
	DynBuffer<byte> *resampleBuffer = &dynBuffers[!curBuffer];

	int pixelSize = (hasAlpha ? 4 : 3);
	resampleBuffer->CheckSize( newWidth * newHeight * pixelSize );

	float x_ratio = static_cast<float>(width) / static_cast<float>(newWidth);
	float y_ratio = static_cast<float>(height) / static_cast<float>(newHeight);

	byte *src_pixel;
	byte *dest_pixel = resampleBuffer->data;
	int ypos;
	for ( int y=0; y<newHeight; y++ ) {
		ypos = Math::FtoiFast(y_ratio * y) * width;
		for ( int x=0; x<newWidth; x++ ) {
			src_pixel = srcBuffer->data + pixelSize * (Math::FtoiFast(x_ratio * x) + ypos);
			memcpy( dest_pixel, src_pixel, pixelSize );
			dest_pixel += pixelSize;
		}
	}

	// Swap Buffers
	curBuffer = !curBuffer;

	width = newWidth;
	height = newHeight;
}

}
