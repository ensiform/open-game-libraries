// ==============================================================================
//! @file
//! @brief	Public Font Interface
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2007-2010 Lusito Software
//! @note	This will not work on OpenGL 3
//! @note	Thread safety class: Unknown
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

#ifndef __OG_FONT_H__
#define __OG_FONT_H__

#include <og/Shared/Shared.h>

//! Open Game Libraries
namespace og {
//! @defgroup Font Font (Library)
//! You need to include <og/Font/Font.h>
//! @see	UserCallbacks
//! @{

	// ==============================================================================
	//! Font Interface & Object.
	//! @todo	Make it create VBO's instead of drawing them manually
	//! @todo	Make it OpenGL 3 compatible
	//! @todo	Test unicode more.
	//!
	//! Creates basic font drawing.
	// ==============================================================================
	class Font {
	public:
	// Static Interface
	//! @addtogroup Font
	//! @{

		// ==============================================================================
		//! Initializes the font library with a default font Family.
		//!
		//! @param	fileSystem		The FileSystem object
		//! @param	defaultFamily	The font-family to use when the wanted one does not exist
		//!
		//! @return	true if the default font was loaded, otherwise false.
		// ==============================================================================
		static bool		Init( FileSystemCore *fileSystem, const char *defaultFamily );

		// ==============================================================================
		//! Shutdown, kill all font objects
		//!
		//! Clears all fonts (even the default font), so you need to re-init if you want to use it again.
		// ==============================================================================
		static void		Shutdown( void );

		// ==============================================================================
		//! Call SetScreenScale if your call to glOrtho does not match your real window/screen size.
		//!
		//! @param	scale	Screen scale value ( i.e. realHeight / orthoHeight ), \b default is 1.0
		//!
		//! @note	You only need to set this once on startup and on screen size events.\n
		//!			No need to call it, if the screen height and glOrtho height are the same.
		//!
		//! All font drawings take the glOrtho size to scale themselves onto the screen.\n
		//! So if you have a 1600x1200 screen, but glOrtho uses 800x600 (for ex. to make the gui look the same at all time),\n
		//! you will get a bitmapped font with only half the resolution you needed it to be.\n
		//! To avoid this, set the screen scale to (realHeight / orthoHeight).\n
		// ==============================================================================
		static void		SetScreenScale( float scale );

		// ==============================================================================
		//! Create a Font object
		//!
		//! @param	family					The font-family name
		//! @param	size					Size in glOrtho height units
		//! @param	spacing					How much space between chars
		//! @param	defR, defG, defB, defA	Default color
		//!
		//! @return	Newly created font object.
		// ==============================================================================
		static Font *	Create( const char *family, float size, float spacing, float defR, float defG, float defB, float defA );

		// ==============================================================================
		//! Free a Font object
		//!
		//! @param	font	A font object previously created by Font::Create
		// ==============================================================================
		static void		Free( Font *font );
	//! @}

	// Object Interface
		//! Horizontal text align
		enum Align { LEFT, RIGHT, CENTER };

		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~Font() {}

		// ==============================================================================
		//! Calculate the width of the passed string
		//!
		//! @param	text	UTF-8 string
		//!
		//! @return	Width of the passed string
		//!
		//! @note	color codes will be ignored
		//! @see	DrawString
		// ==============================================================================
		virtual float	CalcWidth( const char *text ) const = 0;

		// ==============================================================================
		//! Calculate how much of the string can be drawn within the given width
		//!
		//! @param	text		UTF-8 string
		//! @param	maxWidth	Maximum width
		//!
		//! @return	Number of characters from \a text that fit into \a maxWidth
		//!
		//! @note	color codes will be ignored
		//! @see	DrawString
		// ==============================================================================
		virtual int		CalcStringLength( const char *text, float maxWidth ) const = 0;

		// ==============================================================================
		//! Draw the string onto the screen
		//!
		//! @param	x, y	Distance from left, top
		//! @param	text	UTF-8 string
		//! @param	align	Horizontal align
		//!
		//! If the string contains a ^ + a digit(0-9) the text after that is colored with a predefined color:
		//! @li 0: the default color gets restored.
		//! @li 1: red
		//! @li 2: green
		//! @li 3: yellow
		//! @li 4: blue
		//! @li 5: cyan
		//! @li 6: magenta
		//! @li 7: white
		//! @li 8: gray
		//!
		//! You can create any color you want if you start with "^c" and add 3 hex digits (short html color code).\n
		//! For example ^cF0F will create a pink color. 
		// ==============================================================================
		virtual void	DrawString( float x, float y, const char *text, Align align=LEFT ) const = 0;
	};

//! @}
}

#endif
