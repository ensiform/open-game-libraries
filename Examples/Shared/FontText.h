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

#ifndef __OG_FONT_TEXT_H__
#define __OG_FONT_TEXT_H__

#include <og/Common/Common.h>
#include <og/Math/Math.h>
#include <og/Font/Font.h>

/*
==============================================================================

  ogFontText

==============================================================================
*/

class ogFontText {
public:
	ogFontText();
	ogFontText( int size, int align );
	void	Init( const og::Dict &dict );
	void	SetWidth( float width );
	void	Draw( float x, float y );
	float	CalcWidth( void );

	og::Font *	font;
	og::Font::Align	align;
	og::Vec2	offset;
	float		xOffset;
	og::String	value;
};

#endif
