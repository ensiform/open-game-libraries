/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Ensiform
Purpose: Rectangle class
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

#ifndef __OG_RECTANGLE_H__
#define __OG_RECTANGLE_H__

namespace og {
	/*
	==============================================================================

	  Rect

	==============================================================================
	*/
	class Rect {
	public:
		Rect() { Zero(); }
		explicit Rect( float x, float y, float w, float h ) { Set( x, y, w, h ); }
		Rect( const Rect &r ) { Set( r.x, r.y, r.w, r.h ); }

		float			x;
		float			y;
		float			w;
		float			h;

		void			Set( float x, float y, float w, float h );
		void			Zero( void );
		bool			IsZero( void ) const;

		bool			Cmp( const Rect &rect ) const;
		bool			Cmp( const Rect &rect, float epsilon ) const;
		bool			Contains( const Vec2 &point );

		// operators
		float			operator[]( int index ) const;
		float &			operator[]( int index );

		bool			operator==( const Rect &other ) const;
		bool			operator!=( const Rect &other ) const;

		Rect &			operator=( const Rect &rect );
	};
}

#endif
