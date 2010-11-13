// ==============================================================================
//! @file
//! @brief	Image Loading Interface
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

#ifndef __OG_IMAGE_H__
#define __OG_IMAGE_H__

//! Open Game Libraries
namespace og {
//! @defgroup Image Image (Library)
//! You need to include <og/Image.h>
//! @see	UserCallbacks
//! @todo	See if thrown errors work without problems in the file loaders.
//! @todo	Later: Save Function for DDS files ?
//! @todo	Later: Update to latest versions of libPNG
//! @{
	class PreloadManager;
	class PreloadTask;

	// ==============================================================================
	//! Image Interface & Object
	//!
	//! Load/Save images
	//! @note	og::ThreadSafetyClass = og::TSC_NONE, should be og::TSC_MULTIPLE
	// ==============================================================================
	class Image {
	public:
	// Static Interface
	//! @addtogroup Image
	//! @{

		// ==============================================================================
		//! Initializes the image library with a default image.
		//!
		//! @param	fileSystem						The FileSystem object
		//! @param	defaultFilename					The image to use when the wanted one does not exist
		//! @param	user_glCompressedTexImage2DARB	Pass the glCompressedTexImage2DARB function ptr
		//!											if you want to be able to load DDS files. 
		//!
		//! @return	true if the default image was loaded, otherwise false.
		// ==============================================================================
		static bool		Init( FileSystemCore *fileSystem, const char *defaultFilename, void *user_glCompressedTexImage2DARB );

		// ==============================================================================
		//! Shutdown, kill all image objects
		//!
		//! Clears all images (even the default image)
		// ==============================================================================
		static void		Shutdown( void );

	// A few options to set
		// ==============================================================================
		//! Change RoundDown mode
		//!
		//! @param	limit	RoundDown limit is a size below which RoundDown will be ignored.\n
		//!					Set to 0 to disable RoundDown (default)
		//!
		//! When an image width or height is not a power of 2, it will be resized to the closest power of 2.\n
		//! If you enable RoundDown, and the closest power of 2 is bigger than the original size, it will choose the lower power of 2 instead.\n
		//! With RoundDown enabled, this will always take the lower power of 2, except it would get below the set limit.
		// ==============================================================================
		static void		SetRoundDown( int limit );

		// ==============================================================================
		//! PicMip is a forced reduction in size. The image width/height will be divided by (2 * PicMip value).
		//!
		//! @param	value	How much to reduce image quality
		//!
		//! No reduction will go below 1 px and the textures will always be scaled down to the maximum texture size of the gfx card if above. 
		// ==============================================================================
		static void		SetPicmip( int value );

		// ==============================================================================
		//! Set GL_TEXTURE_MIN/MAG_FILTER on all textures
		//!
		//! @param	min		GL_TEXTURE_MIN_FILTER value.
		//! @param	mag		GL_TEXTURE_MAG_FILTER value.
		// ==============================================================================
		static void		SetFilters( int min, int mag );

		// ==============================================================================
		//! Set Jpeg Quality (store)
		//!
		//! @param	quality		Quality range from 0 to 100
		// ==============================================================================
		static void		SetJpegQuality( int quality );

		// ==============================================================================
		//! Deny Precompressed (DDS) images
		//!
		//! @param	value		Enable or disable DDS file loading
		// ==============================================================================
		static void		DenyPrecompressed( bool value );

		// ==============================================================================
		//! Find an image by filename
		//!
		//! @param	filename	The (relative) file path
		//!
		//! @return	A pointer to the found object if successful,
		//!			otherwise a pointer to the default image.
		//!
		//! If no extension was given to Image::Find, it will try to add known extensions.
		// ==============================================================================
		static Image *	Find( const char *filename );

		// ==============================================================================
		//! Free an Image object
		//!
		//! @param	image	An image object previously created by Image::Find
		// ==============================================================================
		static void		Free( Image *image );

		// ==============================================================================
		//! Save an image, filetype depends on the extension.
		//!
		//! @param	filename	The (relative) file path
		//! @param	data		Raw image data, RGB(A)
		//! @param	width		Horizontal size
		//! @param	height		Vertical size
		//! @param	hasAlpha	If the data contains Alpha bits
		//!
		//! @return	true if successful, otherwise false
		// ==============================================================================
		static bool		Save( const char *filename, byte *data, int width, int height, bool hasAlpha );

		// ==============================================================================
		//! Reload images from their files
		//!
		//! @param	force	If set to false, it reloads only images that have
		//!					a newer date than when it was first loaded.
		//! @param	loader	Use a PreloadManager to reload the images that changed
		//!
		//! @return	The number of items reloaded / to be reloaded
		// ==============================================================================
		static uInt		ReloadImages( bool force, PreloadManager *preloadManager=NULL );

		// ==============================================================================
		//! Preload image files in a separate thread
		//!
		//! @param	filename	The filename of the image
		//!
		//! @return	The PreloadTask object to be added to a PreloadManager
		// ==============================================================================
		static PreloadTask *PreloadImage( const char *filename );
	//! @}

	// Object Interface
		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~Image() {}

		// ==============================================================================
		//! Bind the Texture
		//!
		//! Call this prior to rendering your object
		// ==============================================================================
		virtual void	BindTexture( void ) const = 0;

		// ==============================================================================
		//! Get image width in pixels
		//!
		//! @return Image width
		// ==============================================================================
		virtual uInt	GetWidth( void ) const = 0;

		// ==============================================================================
		//! Get image height in pixels
		//!
		//! @return Image height
		// ==============================================================================
		virtual uInt	GetHeight( void ) const = 0;
	};
//! @}
}

#endif
