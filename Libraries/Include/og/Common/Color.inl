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

#ifndef __OG_COLOR_INL__
#define __OG_COLOR_INL__

namespace og {

/*
==============================================================================

  Color

==============================================================================
*/

/*
================
Color::Set
================
*/
OG_INLINE void Color::Set( float _r, float _g, float _b, float _a ) {
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}

/*
================
Color::Clamp
================
*/
OG_INLINE void Color::Clamp( void ) {
	r = Math::Clamp( r, 1.0f, 0.0f );
	g = Math::Clamp( g, 1.0f, 0.0f );
	b = Math::Clamp( b, 1.0f, 0.0f );
	a = Math::Clamp( a, 1.0f, 0.0f );
}

/*
================
Color::Cmp
================
*/
OG_INLINE bool Color::Cmp( const Color &col ) const {
	return ( r == col.r && g == col.g && b == col.b && a == col.a );
}

/*
================
Color::Cmp
================
*/
OG_INLINE bool Color::Cmp( const Color &col, float epsilon ) const {
	return ( Math::Fabs( r - col.r ) <= epsilon && Math::Fabs( g - col.g ) <= epsilon
			&& Math::Fabs( b - col.b ) <= epsilon && Math::Fabs( a - col.a ) <= epsilon );
}

/*
================
Color::ToBytes
================
*/
OG_INLINE void Color::ToBytes( byte &_r, byte &_g, byte &_b, byte &_a ) const {
	_r = static_cast<byte>( Math::Clamp( r, 0.0f, 1.0f ) * 255.0f );
	_g = static_cast<byte>( Math::Clamp( g, 0.0f, 1.0f ) * 255.0f );
	_b = static_cast<byte>( Math::Clamp( b, 0.0f, 1.0f ) * 255.0f );
	_a = static_cast<byte>( Math::Clamp( a, 0.0f, 1.0f ) * 255.0f );
}

/*
================
Color::operator[]
================
*/
OG_INLINE float Color::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index < 4 );
	return (&r)[index];
}

/*
================
Color::operator[]
================
*/
OG_INLINE float &Color::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index < 4 );
	return (&r)[index];
}

/*
================
Color::operator==
================
*/
OG_INLINE bool Color::operator==( const Color &other ) const {
	return Cmp(other);
}

/*
================
Color::operator!=
================
*/
OG_INLINE bool Color::operator!=( const Color &other ) const {
	return !Cmp(other);
}

/*
================
Color::operator=
================
*/
OG_INLINE Color &Color::operator=( const Color &other ) {
	Set( other.r, other.g, other.b, other.a );
	return *this;
}

/*
================
Color::operator+=
================
*/
OG_INLINE void Color::operator+=( const Color &col ) {
	r += col.r;
	g += col.g;
	b += col.b;
	a += col.a;
}

/*
================
Color::operator-=
================
*/
OG_INLINE void Color::operator-=( const Color &col ) {
	r -= col.r;
	g -= col.g;
	b -= col.b;
	a -= col.a;
}

/*
================
Color::operator*=
================
*/
OG_INLINE void Color::operator*=( const Color &col ) {
	r *= col.r;
	g *= col.g;
	b *= col.b;
	a *= col.a;
}
OG_INLINE void Color::operator*=( float f ) {
	r *= f;
	g *= f;
	b *= f;
	a *= f;
}

/*
================
Color::operator/=
================
*/
OG_INLINE void Color::operator/=( const Color &col ) {
	r /= col.r;
	g /= col.g;
	b /= col.b;
	a /= col.a;
}
OG_INLINE void Color::operator/=( float f ) {
	*this *= 1.0f/f; // multiplication is faster than division, so we just reverse the process.
}

/*
================
Color::operator*
================
*/
OG_INLINE float Color::operator*( const Color &col ) const {
	return (r * col.r + g * col.g + b * col.b + a * col.a);
}
OG_INLINE Color Color::operator*( float f ) const {
	return Color( r * f, g * f, b * f, a * f );
}
OG_INLINE Color operator*( float f, const Color &col ) {
	return Color( col.r * f, col.g * f, col.b * f, col.a * f );
}

/*
================
Color::operator/
================
*/
OG_INLINE float Color::operator/( const Color &col ) const {
	return (r / col.r + g / col.g + b / col.b + a / col.a);
}
OG_INLINE Color Color::operator/( float f ) const {
	float invf = 1.0f/f; // multiplication is faster than division, so we just reverse the process.
	return Color( r * invf, g * invf, b * invf, a * invf );
}

/*
================
Color::operator+
================
*/
OG_INLINE Color Color::operator+( const Color &col ) const {
	return Color( r + col.r, g + col.g, b + col.b, a + col.a );
}

/*
================
Color::operator-
================
*/
OG_INLINE Color Color::operator-( const Color &col ) const {
	return Color( r - col.r, g - col.g, b - col.b, a - col.a );
}

}

#endif
