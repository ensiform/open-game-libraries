/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Text Object
-----------------------------------------

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
===========================================================================
*/

#include "FontText.h"

/*
==============================================================================

  ogFontText

==============================================================================
*/

/*
================
ogFontText::ogFontText
================
*/
ogFontText::ogFontText() {
	font = NULL;
	align = og::Font::LEFT;
}
ogFontText::ogFontText( int size, int align ) {
	og::Dict dict;
	dict.SetInt("fontAlign", align);
	dict.SetInt("fontSize", size);
	Init( dict );
}

/*
================
ogFontText::Init
================
*/
void ogFontText::Init( const og::Dict &dict ) {
	align = static_cast<og::Font::Align>( og::Math::Clamp( dict.GetInt("fontAlign", "0"), 0, 2) );
	offset = dict.GetVec2("fontOffset");
	xOffset = offset.x;
	
	og::Color color = dict.GetColor("fontColor");
	font = og::Font::Create( dict.GetString("font", "Arial"), dict.GetFloat("fontSize", "12"), dict.GetFloat("fontSpacing", "1"),
		color.r, color.g, color.b, color.a );
}

/*
================
ogFontText::SetWidth
================
*/
void ogFontText::SetWidth( float width ) {
	if ( width <= 0.0f ) {
		align = og::Font::LEFT;
		offset.x = xOffset;
	} else {
		if ( align == og::Font::RIGHT )
			offset.x = xOffset + width;
		else if ( align == og::Font::CENTER )
			offset.x = xOffset + width * 0.5f;
	}
}

/*
================
ogFontText::Draw
================
*/
void ogFontText::Draw( float x, float y ) {
	if ( !font )
		return;

	font->DrawString( x + offset.x, y + offset.y, value.c_str(), align );
}

/*
================
ogFontText::CalcWidth
================
*/
float ogFontText::CalcWidth( void ) {
	if ( !font )
		return 0.0f;
	return font->CalcWidth( value.c_str() );
}
