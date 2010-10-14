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

#ifndef __OG_RECTANGLE_INL__
#define __OG_RECTANGLE_INL__

namespace og {

/*
==============================================================================

  Rect

==============================================================================
*/

/*
================
Rect::Set
================
*/
OG_INLINE void Rect::Set( float _x, float _y, float _w, float _h ) {
	x = _x;
	y = _y;
	w = _w;
	h = _h;
}

/*
================
Rect::Zero
================
*/
OG_INLINE void Rect::Zero( void ) {
	Set( 0.0f, 0.0f, 0.0f, 0.0f );
}

/*
================
Rect::IsZero
================
*/
OG_INLINE bool Rect::IsZero( void ) const {
	const uInt *iv = reinterpret_cast<const uInt *>(&x);
	return !(iv[0] & Math::MASK_SIGNED) && !(iv[1] & Math::MASK_SIGNED) && !(iv[2] & Math::MASK_SIGNED) && !(iv[3] & Math::MASK_SIGNED);
}

/*
================
Rect::Cmp
================
*/
OG_INLINE bool Rect::Cmp( const Rect &rect ) const {
	return ( x == rect.x && y == rect.y && w == rect.w && h == rect.h );
}

/*
================
Rect::Cmp
================
*/
OG_INLINE bool Rect::Cmp( const Rect &rect, float epsilon ) const {
	return ( Math::Fabs( x - rect.x ) <= epsilon && Math::Fabs( y - rect.y ) <= epsilon
			&& Math::Fabs( w - rect.w ) <= epsilon && Math::Fabs( h - rect.h ) <= epsilon );
}

/*
================
Rect::Contains
================
*/
OG_INLINE bool Rect::Contains( const Vec2 &point ) {
	return ( point.x >= x && (point.x-w) <= x && point.y >= y && (point.y-h) <= y );
}

/*
================
Rect::operator[]
================
*/
OG_INLINE float Rect::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index < 4 );
	return (&x)[index];
}

/*
================
Rect::operator[]
================
*/
OG_INLINE float &Rect::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index < 4 );
	return (&x)[index];
}

/*
================
Rect::operator==
================
*/
OG_INLINE bool Rect::operator==( const Rect &other ) const {
	return Cmp(other);
}

/*
================
Rect::operator!=
================
*/
OG_INLINE bool Rect::operator!=( const Rect &other ) const {
	return !Cmp(other);
}

/*
================
Rect::operator=
================
*/
OG_INLINE Rect &Rect::operator=( const Rect &other ) {
	Set( other.x, other.y, other.w, other.h );
	return *this;
}

}

#endif
