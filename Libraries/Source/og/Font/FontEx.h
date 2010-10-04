// ==============================================================================
//! @file
//! @brief	OpenGL Bitmap Font
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

#ifndef __OG_FONT_EX_H__
#define __OG_FONT_EX_H__

#include <og/Font/Font.h>
#include <og/Common/Common.h>
#include <og/Image/Image.h>
#include <map>

//! Open Game Libraries
namespace og {
//! @addtogroup Internal
//! @{

//! @defgroup FontEx FontEx (Library)
//! Not to be used directly
//! @see	Font
//! @{

	class FontEx;
	class GlyphInfo;

	typedef std::map<int, float> KerningMap;
	typedef std::pair<int, float> KerningPair;

	typedef std::map<uInt, GlyphInfo *> GlyphMap;
	typedef std::pair<uInt, GlyphInfo *> GlyphPair;

	// ==============================================================================
	//! Contains glyph information
	// ==============================================================================
	class GlyphInfo {
	public:
		Vec2		texCoords[2];			//!< Texture coordinates
		Vec2		size;					//!< Dimensions
		Vec2		offset;					//!< Relative offset from the current drawing position
		float		xadvance;				//!< How much to advance to the next character by default
		uInt		page;					//!< On which texture of the fontfile is it drawn
		KerningMap	kerningMap;				//!< All possible kerning pairs for this glyph

		// ==============================================================================
		//! Check if there is a kerning match and get its offset
		//!
		//! @param	nextCharId				Id of the next character
		//!
		//! @return	Kerning offset if found, otherwise 0
		// ==============================================================================
		float		GetKerningOffset( int nextCharId ) const;
	};

	// ==============================================================================
	//! Loads and uses font files from the disk
	// ==============================================================================
	class FontFile {
	public:
		FontFile();
		~FontFile();

		// ==============================================================================
		//! Try opening a font file
		//!
		//! @param	filename	The (relative) file path
		//!
		//! @return	true on success, otherwise false
		// ==============================================================================
		bool			Open( const char *filename );

		// ==============================================================================
		//! @copydoc Font::CalcWidth
		//!
		//! @param	font	Font object currently in use
		// ==============================================================================
		float			CalcWidth( const FontEx *font, const char *text ) const;

		// ==============================================================================
		//! @copydoc Font::CalcStringLength
		//!
		//! @param	font	Font object currently in use
		// ==============================================================================
		int				CalcStringLength( const FontEx *font, const char *text, float maxWidth ) const;

		// ==============================================================================
		//! @copydoc Font::DrawString
		//!
		//! @param	font	Font object currently in use
		// ==============================================================================
		void			DrawString( const FontEx *font, float x, float y, const char *text, Font::Align align=Font::LEFT ) const;

		// ==============================================================================
		//! Compare two font files in their line height
		//!
		//! @param	a	First font file
		//! @param	b	Second font file
		//!
		//! @return	0 if equal, positive if a > b, otherwise negative
		// ==============================================================================
		static int		Compare( const FontFile &a, const FontFile &b );

		// ==============================================================================
		//! Get the line height
		//!
		//! @return	Line height
		// ==============================================================================
		const uShort	GetLineHeight( void ) { return lineHeight; }

	private:
		String			name;				//!< Family name
		GlyphMap		glyphs;				//!< All containing glyphs
		List<Image *>	pageImages;			//!< Textures
		uShort			lineHeight;			//!< Line height / font-size
		float			fontBase;			//!< Font base ( y-offset )
	};

	// ==============================================================================
	//! Groups multiple font files (sizes) together to one family
	// ==============================================================================
	class FontFamily {
	public:
		// ==============================================================================
		//! Open all font files belonging to this family
		//!
		//! @param	name	Family name
		//!
		//! @return	true on success, otherwise false
		// ==============================================================================
		bool			Open( const char *name );

		// ==============================================================================
		//! Find the specified family in cache, if not available try to load it
		//!
		//! @param	name	Family name
		//!
		//! @return	New FontFamily object on success, otherwise the default family.
		// ==============================================================================
		static FontFamily *	Find( const char *name );

	private:
		friend class	FontEx;
		friend class	Font;
		ListEx<FontFile> fontFiles;			//!< All font files belonging to this family
		int				numUsers;			//!< How many times this family is currently in use.
	};

	// ==============================================================================
	//! Font Object to be used
	// ==============================================================================
	class FontEx : public Font {
	public:
		// ---------------------- Public Font Interface -------------------

		float			CalcWidth( const char *text ) const;
		int				CalcStringLength( const char *text, float maxWidth ) const;
		void			DrawString( float x, float y, const char *text, Align align=LEFT ) const;

		// ---------------------- Internal FontEx Members -------------------
		
	public:
		~FontEx();

		// ==============================================================================
		//! Initialize a new font object
		//!
		//! @param	family					The font-family name
		//! @param	size					Size in glOrtho height units
		//! @param	spacing					How much space between chars
		//! @param	defR, defG, defB, defA	Default color
		// ==============================================================================
		void			Init( const char *family, float size, float spacing, float defR, float defG, float defB, float defA );

		// ==============================================================================
		//! Update font size ( resolution )
		//!
		//! Called from FontEx::Init and on Font::SetScreenScale.\n
		//! Calculates \a fileIndex and \a scale
		// ==============================================================================
		void			UpdateSize( void );

		FontFamily *	family;				//!< Family to use
		int				fileIndex;			//!< Index to family->fontFiles, in order to draw the best resolution
		float			fontSize;			//!< Wanted font-size
		float			scale;				//!< Scale to apply when drawing
		float			spacing;			//!< Extra x-spacing between characters
		Color			defaultColor;		//!< Default color
	};

//! @}
//! @}
}

#endif
