/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Quaternion class
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

#ifndef __OG_QUAT_INL__
#define __OG_QUAT_INL__

namespace og {

/*
==============================================================================

  Quat

==============================================================================
*/

/*
================
Quat::Quat
================
*/
OG_INLINE Quat::Quat() {
	Zero();
}
OG_INLINE Quat::Quat( float _x, float _y, float _z, float _w ) {
	Set( _x, _y, _z, _w );
}
OG_INLINE Quat::Quat( float qsrc[4] ) {
	Set( qsrc[0], qsrc[1], qsrc[2], qsrc[3] );
}

/*
================
Quat::Set
================
*/
OG_INLINE void Quat::Set( float _x, float _y, float _z, float _w ) {
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

/*
================
Quat::Zero
================
*/
OG_INLINE void Quat::Zero( void ) {
	Set( 0.0f, 0.0f, 0.0f, 0.0f );
}

/*
================
Quat::IsZero
================
*/
OG_INLINE bool Quat::IsZero( void ) const {
	const uInt *iv = reinterpret_cast<const uInt *>(&x);
	return !(iv[0] & Math::MASK_SIGNED) && !(iv[1] & Math::MASK_SIGNED)
		&& !(iv[2] & Math::MASK_SIGNED) && !(iv[3] & Math::MASK_SIGNED);
}

/*
================
Quat::Identity
================
*/
OG_INLINE void Quat::Identity( void ) {
	Set( 0.0f, 0.0f, 0.0f, 1.0f );
}

/*
================
Quat::Conjugate
================
*/
OG_INLINE void Quat::Conjugate( void ) {
	x *= -1.0f;
	y *= -1.0f;
	z *= -1.0f;
}

/*
================
Quat::Normalize
================
*/
OG_INLINE float Quat::Normalize( void ) {
	float length = x*x + y*y + z*z + w*w;
	if( length != 0.0f )
		*this *= Math::InvSqrt( length );

	return length;
}

/*
================
Quat::NormalizeFast
================
*/
OG_INLINE float Quat::NormalizeFast( void ) {
	float length = x*x + y*y + z*z + w*w;
	if( length != 0.0f )
		*this *= Math::RSqrt( length );

	return length;
}

/*
================
Quat::Inverse
================
*/
OG_INLINE float Quat::Inverse( void ) {
	Conjugate();

	return Normalize();
}

/*
================
Quat::Cmp
================
*/
OG_INLINE bool Quat::Cmp( const Quat &other ) const {
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

/*
================
Quat::Cmp
================
*/
OG_INLINE bool Quat::Cmp( const Quat &other, float epsilon ) const {
	return ( Math::Fabs( x - other.x ) <= epsilon && Math::Fabs( y - other.y ) <= epsilon
			&& Math::Fabs( z - other.z ) <= epsilon && Math::Fabs( w - other.w ) <= epsilon );
}

/*
================
Quat::Dot

DotProduct
================
*/
OG_INLINE float Quat::Dot( const Quat &q1, const Quat &q2 ) {
	return ( q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w );
}
OG_INLINE float Quat::Dot( const Quat &other ) const {
	return Dot( *this, other );
}

/*
================
Quat::operator[]
================
*/
OG_INLINE const float &Quat::operator[]( int index ) const {
	return (&x)[index];
}
OG_INLINE float &Quat::operator[]( int index ) {
	return (&x)[index];
}

/*
================
Quat::operator-
================
*/
OG_INLINE Quat Quat::operator-() const {
	return Quat( -x, -y, -z, -w );
}

/*
================
Quat::operator==
================
*/
OG_INLINE bool Quat::operator==( const Quat &other ) const {
	return Cmp(other);
}

/*
================
Quat::operator!=
================
*/
OG_INLINE bool Quat::operator!=( const Quat &other ) const {
	return !Cmp(other);
}

/*
================
Quat::operator=
================
*/
OG_INLINE Quat &Quat::operator=( const Quat &other ) {
	Set( other.x, other.y, other.z, other.w );
	return *this;
}

/*
================
Quat::operator+=
================
*/
OG_INLINE void Quat::operator+=( const Quat &other ) {
	Set( x + other.x, y + other.y, z + other.z, w + other.w );
}

/*
================
Quat::operator-=
================
*/
OG_INLINE void Quat::operator-=( const Quat &other ) {
	Set( x - other.x, y - other.y, z - other.z, w - other.w );
}

/*
================
Quat::operator*=
================
*/
OG_INLINE void Quat::operator*=( const Quat &other ) {
	float _x = w * other.x + x * other.w + y * other.z - z * other.y;
	float _y = w * other.y + y * other.w + z * other.x - x * other.z;
	float _z = w * other.z + z * other.w + x * other.y - y * other.x;
	float _w = w * other.w - x * other.x - y * other.y - z * other.z;
	Set( _x, _y, _z, _w );
}
OG_INLINE void Quat::operator*=( float f ) {
	x *= f;
	y *= f;
	z *= f;
	w *= f;
}

/*
================
Quat::operator*
================
*/
OG_INLINE Quat Quat::operator*( const Quat &other ) const {
	float _x = w * other.x + x * other.w + y * other.z - z * other.y;
	float _y = w * other.y + y * other.w + z * other.x - x * other.z;
	float _z = w * other.z + z * other.w + x * other.y - y * other.x;
	float _w = w * other.w - x * other.x - y * other.y - z * other.z;
	return Quat( _x, _y, _z, _w );
}
OG_INLINE Quat operator*( float f, const Quat &other ) {
	return Quat( other.x * f, other.y * f, other.z * f, other.w * f );
}

/*
================
Quat::operator+
================
*/
OG_INLINE Quat Quat::operator+( const Quat &other ) const {
	return Quat( x + other.x, y + other.y, z + other.z, w + other.w );
}

/*
================
Quat::operator-
================
*/
OG_INLINE Quat Quat::operator-( const Quat &other ) const {
	return Quat( x - other.x, y - other.y, z - other.z, w - other.w );
}

}

#endif
