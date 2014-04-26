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

#include <og/Gloot/glmin.h>
#include <og/FileSystem.h>
#include "FontEx.h"

namespace og {
FileSystemCore *fontFS = OG_NULL;

OG_INLINE void FontSetColor( const Color &color ) { glColor4f( color.r, color.g, color.b, color.a ); }

//! @todo	make thread safe
static float screenScale = 1.0f;
static DictEx<FontFamily> fontFamilies;
static FontFamily *defaultFamily = OG_NULL;
static LinkedList<FontEx> fontList;

const byte	MASKBITS	= 0x3F;
const byte	MASK1BIT	= 0x80;
const byte	MASK2BIT	= 0xC0;
const byte	MASK3BIT	= 0xE0;
const byte	MASK4BIT	= 0xF0;
const byte	MASK5BIT	= 0xF8;
const byte	MASK6BIT	= 0xFC;
const byte	MASK7BIT	= 0xFE;

/*
================
Utf8ToCharId
================
*/
uInt Utf8ToCharId( const char *data, uInt *numBytes ) {
	if( data[0] < MASK1BIT && data[0] >= 0 ) {
		*numBytes = 1;
		return data[0];
	} else if((data[0] & MASK3BIT) == MASK2BIT) {
		*numBytes = 2;
		return ((data[0] & 0x1F) << 6) | (data[1] & MASKBITS);
	} else if( (data[0] & MASK4BIT) == MASK3BIT ) {
		*numBytes = 3;
		return ((data[0] & 0x0F) << 12) | ( (data[1] & MASKBITS) << 6) | (data[2] & MASKBITS);
	} else if( (data[0] & MASK5BIT) == MASK4BIT ) {
		*numBytes = 4;
		return ((data[0] & 0x07) << 18) | ((data[1] & MASKBITS) << 12) |
				((data[2] & MASKBITS) << 6) | (data[3] & MASKBITS);
	} else if( (data[0] & MASK6BIT) == MASK5BIT ) {
		*numBytes = 5;
		return ((data[0] & 0x03) << 24) | ((data[1] & MASKBITS) << 18) | ((data[2] & MASKBITS) << 12) |
				((data[3] & MASKBITS) << 6) | (data[4] & MASKBITS);
	} else if( (data[0] & MASK7BIT) == MASK6BIT ) {
		*numBytes = 6;
		return ((data[0] & 0x01) << 30) | ((data[1] & MASKBITS) << 24) | ((data[2] & MASKBITS) << 18) |
				((data[3] & MASKBITS) << 12) | ((data[4] & MASKBITS) << 6) | (data[5] & MASKBITS);
	}
	OG_DEBUG_BREAK();
	*numBytes = 1;
	return '\0';
}

/*
==============================================================================

  GlyphInfo

==============================================================================
*/
/*
================
GlyphInfo::GetKerningOffset
================
*/
float GlyphInfo::GetKerningOffset( int nextCharId ) const {
	KerningMap::const_iterator it = kerningMap.find( nextCharId );
	if ( it != kerningMap.end() )
		return it->second;
	return 0.0f;
}

/*
==============================================================================

  FontFile

==============================================================================
*/
/*
================
FontFile::FontFile
================
*/
FontFile::FontFile() {
	lineHeight = 0;
	fontBase = 0;
}

/*
================
FontFile::~FontFile
================
*/
FontFile::~FontFile() {
	GlyphMap::iterator it;
	for( it=glyphs.begin(); it != glyphs.end(); it++ )
		delete it->second;

	int max = pageImages.Num();
	for( int i=0; i<max; i++ )
		Image::Free( pageImages[i] );
}

/*
================
FontFile::Open
================
*/
bool FontFile::Open( const char *filename ) {
	if ( fontFS == OG_NULL )
		return false;

	File *file = fontFS->OpenRead( filename );
	if ( !file )
		return false;

	try {
		char magic[3];
		byte version;
		file->Read( magic, 3 );
		version = file->ReadByte();
		if ( String::Cmpn( magic, "BMF", 3 ) != 0 || version != 3 ) {
			file->Close();
			User::Error( ERR_BAD_FILE_FORMAT, "File is no BMF Version 3 file.", filename );
			return false;
		}

		byte	unusedByte;
		uShort	base;
		uShort	imageWidth;
		uShort	imageHeight;
		uShort	numPages;

		uInt	id;
		uShort  x;
		uShort  y;
		uShort  width;
		uShort  height;
		short	xoffset;
		short	yoffset;
		short	xadvance;
		byte	page;

		uInt	first;
		uInt	second;
		short	amount; 

		DynBuffer<char> dynBuffer;
		String	fontImage;

		float	imageScaleX;
		float	imageScaleY;

		String	path = filename;
		path.StripFilename();

		bool	hasBlock[5] = { false, false, false, false, false };
		byte	type;
		int		size, pos;
		while ( !file->Eof() ) {
			type = file->ReadByte();
			size = file->ReadInt();

			if ( size <= 0 )
				throw FileReadWriteError( FileReadWriteError::READ );

			switch( type ) {
				case 1: // Info
					file->Seek( 14, SEEK_CUR );
					// uShort: font Size (we're using lineheight instead)
					// byte: flags, bit 0: smooth, bit 1: unicode, bit 2: italic, bit 3: bold
					// byte: charSet
					// uShort: stretchH

					// byte: aa
					// byte: paddingUp
					// byte: paddingRight
					// byte: paddingDown
					// byte: paddingLeft
					// byte: spacingHoriz
					// byte: spacingVert
					// byte: outline

					// Read name
					size -= 14;
					if ( size <= 0 )
						throw FileReadWriteError( FileReadWriteError::READ );
					dynBuffer.CheckSize( size );
					file->Read( dynBuffer.data, size );
					name = dynBuffer.data;
					break;

				case 2: // Common
					lineHeight	= file->ReadUshort();
					base		= file->ReadUshort();
					imageWidth	= file->ReadUshort();
					imageHeight	= file->ReadUshort();
					numPages	= file->ReadUshort();
					
					file->Seek( 5, SEEK_CUR );
					// byte: flags, Bit 8: packed
					// byte: alphaChnl
					// byte: redChnl
					// byte: greenChnl
					// byte: blueChnl

					imageScaleX = 1.0f / imageWidth;
					imageScaleY = 1.0f / imageHeight;
					fontBase = base * imageScaleY;
					break;

				case 3: // Pages
					dynBuffer.CheckSize( size );
					file->Read( dynBuffer.data, size );

					for( pos=0; pos<size; ) {
						fontImage = path;
						fontImage += dynBuffer.data + pos;
						fontImage.StripFileExtension();
						pageImages.Append( Image::Find( fontImage.c_str() ) );
						pos += String::ByteLength( dynBuffer.data + pos ) + 1;
					}

					break;
				case 4: // Chars
					for( pos=0; pos<size; pos+=20 ) {
						id			= file->ReadUint();
						x			= file->ReadUshort();
						y			= file->ReadUshort();
						width		= file->ReadUshort();
						height		= file->ReadUshort();
						xoffset		= file->ReadShort();
						yoffset		= file->ReadShort();
						xadvance	= file->ReadShort();
						page		= file->ReadByte(); 
						unusedByte	= file->ReadByte(); // chnl

						// Create glyph data
						GlyphInfo *glyph = new GlyphInfo;
						glyph->texCoords[0].Set( x * imageScaleX, y * imageScaleY );
						glyph->texCoords[1].Set( glyph->texCoords[0].x + width * imageScaleX, glyph->texCoords[0].y + height * imageScaleY );
						glyph->size.Set( width, height );
						glyph->offset.Set( xoffset, yoffset );
						glyph->xadvance = xadvance;
						glyph->page = page;
						glyphs[id] = glyph;
					}
					break;

				case 5: // Kerning Pairs
					for( pos=0; pos<size; pos+=10 ) {
						first	= file->ReadUint();
						second	= file->ReadUint();
						amount	= file->ReadShort();
						
						GlyphMap::iterator it = glyphs.find( first );
						if ( it == glyphs.end() )
							User::Warning("Found no match for kerning pair!");
						else
							it->second->kerningMap[second] = amount;
					}
					break;

				default:
					User::Warning("Unknown font block type found!");
					file->Close();
					return false;
			}
			hasBlock[type-1] = true;
		}
		// check if all blocks except kernings are available.
		for( int i=0; i<4; i++ ) {
			if ( !hasBlock[i] )
				throw FileReadWriteError(FileReadWriteError::READ);
		}
		file->Close();
		return true;
	}
	catch( FileReadWriteError &err ) {
		file->Close();
		User::Error( ERR_FILE_CORRUPT, Format("Font: $*" ) << err.ToString(), filename );
		return false;
	}
}

/*
================
FontFile::CalcWidth
================
*/
float FontFile::CalcWidth( const FontEx *font, const char *text ) const {
	OG_ASSERT( font != OG_NULL && text != OG_NULL );
	if ( text[0] == '\0' )
		return 0.0f;

	float width = 0;
	uInt charId, escapeLength, charLen;

	GlyphMap::const_iterator it;
	Color lastColor;
	const GlyphInfo *glyph = OG_NULL;
	for( int i=0; ; i++ ) {
		// Skip escape colors
		while( 1 ) {
			escapeLength = String::GetEscapeColorLength( text+i );
			if ( !escapeLength )
				break;
			i += escapeLength;
		}
		charId = Utf8ToCharId( text+i, &charLen );
		if ( charId == '\0' )
			break;

		it = glyphs.find( charId );
		if ( it == glyphs.end() || it->second->size.IsZero() ) {
			charId = '?';
			it = glyphs.find( charId );
			OG_ASSERT( it != glyphs.end() );
		}
		glyph = it->second;
		width += glyph->xadvance;

		// Skip escape colors
		while( 1 ) {
			escapeLength = String::GetEscapeColorLength( text+i+charLen );
			if ( !escapeLength )
				break;
			i += escapeLength;
		}

		charId = Utf8ToCharId( text+i+charLen, &charLen );
		if ( charId == '\0' )
			break;

		width += glyph->GetKerningOffset( charId ) + font->spacing;
	}

	return width * font->scale;
}

/*
================
FontFile::CalcStringLength

How many chars can be drawn at the given maxWidth
================
*/
int FontFile::CalcStringLength( const FontEx *font, const char *text, float maxWidth ) const {
	OG_ASSERT( font != OG_NULL && text != OG_NULL );
	if ( text[0] == '\0' )
		return 0;

	float width = 0;
	int count = 0;
	uInt charId, escapeLength, charLen;

	GlyphMap::const_iterator it;
	Color lastColor;
	const GlyphInfo *glyph = OG_NULL;
	for( int i=0; ; i++ ) {
		// Skip escape colors
		while( 1 ) {
			escapeLength = String::GetEscapeColorLength( text+i );
			if ( !escapeLength )
				break;
			i += escapeLength;
		}
		charId = Utf8ToCharId( text+i, &charLen );
		if ( charId == '\0' )
			break;

		it = glyphs.find( charId );
		if ( it == glyphs.end() || it->second->size.IsZero() ) {
			charId = '?';
			it = glyphs.find( charId );
			OG_ASSERT( it != glyphs.end() );
		}
		glyph = it->second;

		width += glyph->xadvance * font->scale;
		if ( width >= maxWidth )
			return count;
		count++;

		// Skip escape colors
		while( 1 ) {
			escapeLength = String::GetEscapeColorLength( text+i+charLen );
			if ( !escapeLength )
				break;
			i += escapeLength;
		}

		charId = Utf8ToCharId( text+i+charLen, &charLen );
		if ( charId == '\0' )
			break;

		width += (glyph->GetKerningOffset( charId ) + font->spacing) * font->scale;
		if ( width >= maxWidth )
			return count;
	}

	return count;
}

/*
================
FontFile::DrawString
================
*/
void FontFile::DrawString( const FontEx *font, float x, float y, const char *text, Font::Align align ) const {
	OG_ASSERT( font != OG_NULL && text != OG_NULL );
	if ( text[0] == '\0' )
		return;

	//! @todo	move the align stuff to the user ? that way he could store the position easier.
	if ( align == Font::RIGHT )
		x -= CalcWidth( font, text );
	else if ( align == Font::CENTER )
		x -= CalcWidth( font, text ) * 0.5f;

	x = Math::Round( x );
	y = Math::Round( y + font->scale * fontBase );

	FontSetColor( font->defaultColor );

	float posLeft, posRight, posTop, posBottom;

	GlyphMap::const_iterator it;
	Color lastColor;
	const GlyphInfo *glyph = OG_NULL;
	uInt charId, escapeLength, charLen;
	int page = -1;
	glBegin(GL_QUADS);

	// Check escape colors before the first char
	bool updateColor = false;
	int i=0;
	while( text[i] == '^' ) {
		escapeLength = Color::GetEscapeColor( text+i+1, lastColor, font->defaultColor );
		if ( !escapeLength )
			break;
		i += escapeLength + 1;
		updateColor = true;
	}
	charId = Utf8ToCharId( text+i, &charLen );
	if ( charId == '\0' )
		return;

	while(1) {
		it = glyphs.find( charId );
		if ( it == glyphs.end() || it->second->size.IsZero() ) {
			charId = '?';
			it = glyphs.find( charId );
			OG_ASSERT( it != glyphs.end() );
		}
		glyph = it->second;

		if ( updateColor ) {
			FontSetColor( lastColor );
			updateColor = false;
		}

		// Bind the correct texture
		if ( page != glyph->page ) {
			page = glyph->page;
			glEnd();
			pageImages[page]->BindTexture();
			glBegin(GL_QUADS);
		}

		posLeft = x + glyph->offset.x * font->scale;
		posRight = posLeft + glyph->size.x * font->scale;
		posTop = y + glyph->offset.y * font->scale;
		posBottom = posTop + glyph->size.y * font->scale;

		glTexCoord2f( glyph->texCoords[0].x, glyph->texCoords[0].y );	glVertex2f( posLeft, posTop );
		glTexCoord2f( glyph->texCoords[0].x, glyph->texCoords[1].y );	glVertex2f( posLeft, posBottom );
		glTexCoord2f( glyph->texCoords[1].x, glyph->texCoords[1].y );	glVertex2f( posRight, posBottom );
		glTexCoord2f( glyph->texCoords[1].x, glyph->texCoords[0].y );	glVertex2f( posRight, posTop );

		// Get the next char
		i += charLen;
		// Check escape colors before the first char
		while( text[i] == '^' ) {
			escapeLength = Color::GetEscapeColor( text+i+1, lastColor, font->defaultColor );
			if ( !escapeLength )
				break;
			i += escapeLength + 1;
			updateColor = true;
		}
		charId = Utf8ToCharId( text+i, &charLen );
		if ( charId == '\0' )
			break;
		x += (glyph->GetKerningOffset( charId ) + glyph->xadvance + font->spacing) * font->scale;
	}

	glEnd();
	FontSetColor( c_color::white );
}

/*
================
FontFile::Compare
================
*/
OG_INLINE int FontFile::Compare( const FontFile &a, const FontFile &b ) {
	return a.lineHeight - b.lineHeight;
}

/*
==============================================================================

  FontFamily

==============================================================================
*/
/*
================
FontFamily::Open
================
*/
bool FontFamily::Open( const char *name ) {
	if ( fontFS == OG_NULL )
		return false;

	OG_ASSERT( name != OG_NULL );
	FileList *files = fontFS->GetFileList( Format( "fonts/$*" ) << name, ".fnt", (LF_FILES | LF_CHECK_LOCAL | LF_CHECK_ARCHIVED) );
	if ( !files )
		return false;

	for( int i=0; i<files->Num(); i++ ) {
		if ( !fontFiles.Alloc().Open( files->GetName(i) ) )
			fontFiles.Remove( fontFiles.Num()-1 );
	};

	fontFS->FreeFileList( files );

	if ( fontFiles.IsEmpty() )
		return false;

	// needs to sort fontFiles by size
	fontFiles.Sort( FontFile::Compare, true );
	return true;
}

/*
================
FontFamily::Find
================
*/
FontFamily *FontFamily::Find( const char *name ) {
	OG_ASSERT( name != OG_NULL );
	int index = fontFamilies.Find( name );
	if ( index == -1 ) {
		FontFamily &font = fontFamilies[name];
		// try allocating it
		if ( !font.Open( name ) ) {
			fontFamilies.Remove( name );
			return defaultFamily;
		}
		font.numUsers = 1;
		return &font;
	}
	fontFamilies[index].numUsers++;
	return &fontFamilies[index];
}

/*
==============================================================================

  FontEx

==============================================================================
*/
/*
================
FontEx::~FontEx
================
*/
FontEx::~FontEx() {
	family->numUsers--;
}

/*
================
FontEx::Init
================
*/
void FontEx::Init( const char *_family, float size, float _spacing, float defR, float defG, float defB, float defA ) {
	OG_ASSERT( _family != OG_NULL );
	family = FontFamily::Find( _family );
	fontSize = size;
	spacing = _spacing;
	defaultColor.Set( defR, defG, defB, defA );
	UpdateSize();
}

/*
================
FontEx::UpdateSize
================
*/
void FontEx::UpdateSize( void ) {
	// Find closest size for the current screen size
	uShort mid;
	uShort wantedSize = Math::FtoiFast(fontSize*screenScale);
	int max = family->fontFiles.Num()-1;
	for( int i=0; i<max; i++ ) {
		FontFile &currentFile = family->fontFiles[i];
		mid = currentFile.GetLineHeight() + ( (family->fontFiles[i+1].GetLineHeight() - currentFile.GetLineHeight()) / 2 );
		if ( mid > wantedSize ) {
			fileIndex = i;
			scale = fontSize / currentFile.GetLineHeight();
			return;
		}
	}
	fileIndex = max;
	scale = fontSize / family->fontFiles[fileIndex].GetLineHeight();
}

/*
================
FontEx::CalcWidth
================
*/
float FontEx::CalcWidth( const char *text ) const {
	return family->fontFiles[fileIndex].CalcWidth( this, text );
}

/*
================
FontEx::CalcStringLength
================
*/
int FontEx::CalcStringLength( const char *text, float maxWidth ) const {
	return family->fontFiles[fileIndex].CalcStringLength( this, text, maxWidth );
}

/*
================
FontEx::DrawString
================
*/
void FontEx::DrawString( float x, float y, const char *text, Align align ) const {
	family->fontFiles[fileIndex].DrawString( this, x, y, text, align );
}


/*
==============================================================================

  Font

==============================================================================
*/
/*
================
Font::Init
================
*/
bool Font::Init( FileSystemCore *fileSystem, const char *_defaultFamily ) {
	OG_ASSERT( fileSystem != OG_NULL );
	OG_ASSERT( _defaultFamily != OG_NULL );
	fontFS = fileSystem;
	defaultFamily = FontFamily::Find( _defaultFamily );
	return defaultFamily != OG_NULL;
}

/*
================
Font::Shutdown
================
*/
void Font::Shutdown( void ) {
	fontList.Clear();
	fontFamilies.Clear();
	defaultFamily = OG_NULL;
	fontFS = OG_NULL;
}

/*
================
Font::SetScreenScale
================
*/
void Font::SetScreenScale( float scale ) {
	screenScale = scale;
	LinkedList<FontEx>::nodeType *node = fontList.GetFirstNode();
	while( node != OG_NULL ) {
		node->value.UpdateSize();
		node = node->GetNext();
	}
}

/*
================
Font::Create
================
*/
Font *Font::Create( const char *family, float size, float spacing, float defR, float defG, float defB, float defA ) {
	fontList.Alloc().Init( family, size, spacing, defR, defG, defB, defA );
	return &fontList.GetLastNode()->value;
}

/*
================
Font::Free
================
*/
void Font::Free( Font *font ) {
	OG_ASSERT( font != OG_NULL );

	FontEx *fontEx = static_cast<FontEx *>(font);

	// No need to keep font families that are no longer in use.
	// Remember, defaultFamily is always occupied with at least one user,
	// which is correct, since we always want it to be available.
	// It will be freed upon Font::Shutdown.
	if ( fontEx->family->numUsers == 0 ) {
		int num = fontFamilies.Num();
		for( int i=0; i<num; i++ ) {
			if ( &fontFamilies[i] == fontEx->family ) {
				fontFamilies.Remove(i);
				break;
			}
		}
	}
	fontList.Remove( fontEx->node );
}

}
