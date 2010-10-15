/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Color class
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

#ifndef __OG_COLOR_H__
#define __OG_COLOR_H__

namespace og {
	/*
	==============================================================================

	  Color

	==============================================================================
	*/
	class Color {
	public:
		Color() { Set( 1.0f, 1.0f, 1.0f, 1.0f ); }
		Color( const StringType &value ) { String::ToFloatArray( value, &r, 4 ); }
		explicit Color( float r, float g, float b, float a=1.0f ) { Set( r, g, b, a ); }
		Color( const Color &c ) { Set( c.r, c.g, c.b, c.a ); }

		float			r;
		float			g;
		float			b;
		float			a;

		void			Set( float r, float g, float b, float a=1.0f );
		void			ClampSelf( void );

		bool			Cmp( const Color &col ) const;
		bool			Cmp( const Color &col, float epsilon ) const;

		void			ToBytes( byte &_r, byte &_g, byte &_b, byte &_a ) const;

#if 0
		//! @todo	add HSV functions ?
		void			GetHSV( float &h, float &s, float &v ) const;
		void			SetHSV( float h, float s, float v );
#endif

		// Escape Colors
		static int		GetEscapeColor( const char *str, Color &destColor, const Color &defaultColor );

		// operators
		float			operator[]( int index ) const;
		float &			operator[]( int index );

		bool			operator==( const Color &other ) const;
		bool			operator!=( const Color &other ) const;

		Color &		operator=( const Color &col );

		void			operator+=( const Color &col );
		void			operator-=( const Color &col );
		void			operator*=( const Color &col );
		void			operator*=( float f );
		void			operator/=( const Color &col );
		void			operator/=( float f );

		float			operator*( const Color &col ) const;
		Color			operator*( float f ) const;
		friend Color	operator*( float f, const Color &col );

		float			operator/( const Color &col ) const;
		Color			operator/( float f ) const;

		Color			operator+( const Color &col ) const;
		Color			operator-( const Color &col ) const;
	};

	namespace c_color {
		extern Color transparent;
		extern Color white;
		extern Color black;

		extern Color gray;
		extern Color gray_dark;
		extern Color gray_light;

		extern Color red;
		extern Color green;
		extern Color blue;

		extern Color cyan;
		extern Color magenta;
		extern Color yellow;

		extern Color orange;
		extern Color purple;
		extern Color pink;
		extern Color brown;
	}
}

#endif
