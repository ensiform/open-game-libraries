/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Vector classes
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

#ifndef __OG_VECTOR_INL__
#define __OG_VECTOR_INL__

namespace og {

/*
==============================================================================

  Vec2

==============================================================================
*/

/*
================
Vec2::Set
================
*/
OG_INLINE void Vec2::Set( float _x, float _y ) {
	x = _x;
	y = _y;
}

/*
================
Vec2::Zero
================
*/
OG_INLINE void Vec2::Zero( void ) {
	Set( 0.0f, 0.0f );
}

/*
================
Vec2::IsZero
================
*/
OG_INLINE bool Vec2::IsZero( void ) const {
	const uInt *iv = reinterpret_cast<const uInt *>(&x);
	return !(iv[0] & Math::MASK_SIGNED) && !(iv[1] & Math::MASK_SIGNED);
}

/*
================
Vec2::Length
================
*/
OG_INLINE float Vec2::Length( void ) const {
	return Math::Sqrt( LengthSqr() );
}

/*
================
Vec2::LengthFast
================
*/
OG_INLINE float Vec2::LengthFast( void ) const {
	float lenSqr = LengthSqr();
	return lenSqr * Math::RSqrt( lenSqr );
}

/*
================
Vec2::LengthSqr
================
*/
OG_INLINE float Vec2::LengthSqr( void ) const {
	return ( x*x + y*y );
}

/*
================
Vec2::Normalize
================
*/
OG_INLINE float Vec2::Normalize( void ) {
	float lenSqr = LengthSqr();
	float lenInv = Math::InvSqrt( lenSqr );

	*this *= lenInv;
	return lenSqr * lenInv;
}

/*
================
Vec2::NormalizeFast
================
*/
OG_INLINE float Vec2::NormalizeFast( void ) {
	float lenSqr = LengthSqr();
	float lenInv = Math::RSqrt( lenSqr );

	*this *= lenInv;
	return lenSqr * lenInv;
}

/*
================
Vec2::Snap
================
*/
OG_INLINE Vec2 Vec2::Snap( void ) const {
	Vec2 vec = *this;
	vec.SnapSelf();
	return vec;
}

/*
================
Vec2::SnapSelf
================
*/
OG_INLINE void Vec2::SnapSelf( void ) {
	x = Math::Floor( x + 0.5f );
	y = Math::Floor( y + 0.5f );
}

/*
================
Vec2::Mid
================
*/
OG_INLINE Vec2 Vec2::Mid( const Vec2 &v1, const Vec2 &v2 ) {
	return Vec2( (v1.x + v2.x) * 0.5f, (v1.y + v2.y) * 0.5f);
}
OG_INLINE Vec2 Vec2::Mid( const Vec2 &other ) const {
	return Mid( *this, other );
}

/*
================
Vec2::Floor
================
*/
OG_INLINE void Vec2::Floor( void ) {
	x = Math::Floor( x );
	y = Math::Floor( y );
}

/*
================
Vec2::Ceil
================
*/
OG_INLINE void Vec2::Ceil( void ) {
	x = Math::Ceil( x );
	y = Math::Ceil( y );
}

/*
================
Vec2::CapLength
================
*/
OG_INLINE void Vec2::CapLength( float length ) {
	if ( length == 0 )
		Zero();
	else {
		float lenSqr = LengthSqr();
		if ( lenSqr > length*length )
			*this *= length * Math::InvSqrt( lenSqr );
	}
}

/*
================
Vec2::Cmp
================
*/
OG_INLINE bool Vec2::Cmp( const Vec2 &vec ) const {
	return ( x == vec.x && y == vec.y );
}

/*
================
Vec2::Cmp
================
*/
OG_INLINE bool Vec2::Cmp( const Vec2 &vec, float epsilon ) const {
	return ( Math::Fabs( x - vec.x ) <= epsilon && Math::Fabs( y - vec.y ) <= epsilon );
}

/*
================
Vec2::operator-
================
*/
OG_INLINE Vec2 Vec2::operator-() const {
	return Vec2( -x, -y );
}

/*
================
Vec2::operator[]
================
*/
OG_INLINE float Vec2::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index < 2 );
	return (&x)[index];
}

/*
================
Vec2::operator[]
================
*/
OG_INLINE float &Vec2::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index < 2 );
	return (&x)[index];
}

/*
================
Vec2::operator==
================
*/
OG_INLINE bool Vec2::operator==( const Vec2 &other ) const {
	return Cmp(other);
}

/*
================
Vec2::operator!=
================
*/
OG_INLINE bool Vec2::operator!=( const Vec2 &other ) const {
	return !Cmp(other);
}

/*
================
Vec2::operator=
================
*/
OG_INLINE Vec2 &Vec2::operator=( const Vec2 &other ) {
	Set( other.x, other.y );
	return *this;
}

/*
================
Vec2::operator+=
================
*/
OG_INLINE void Vec2::operator+=( const Vec2 &vec ) {
	x += vec.x;
	y += vec.y;
}

/*
================
Vec2::operator-=
================
*/
OG_INLINE void Vec2::operator-=( const Vec2 &vec ) {
	x -= vec.x;
	y -= vec.y;
}

/*
================
Vec2::operator*=
================
*/
OG_INLINE void Vec2::operator*=( const Vec2 &vec ) {
	x *= vec.x;
	y *= vec.y;
}
OG_INLINE void Vec2::operator*=( float f ) {
	x *= f;
	y *= f;
}

/*
================
Vec2::operator/=
================
*/
OG_INLINE void Vec2::operator/=( const Vec2 &vec ) {
	x /= vec.x;
	y /= vec.y;
}
OG_INLINE void Vec2::operator/=( float f ) {
	float invf = 1.0f/f; // multiplication is faster than division, so we just reverse the process.
	x *= invf;
	y *= invf;
}

/*
================
Vec2::operator*
================
*/
OG_INLINE float Vec2::operator*( const Vec2 &vec ) const {
	return (x * vec.x + y * vec.y);
}
OG_INLINE Vec2 Vec2::operator*( float f ) const {
	return Vec2( x * f, y * f );
}
OG_INLINE Vec2 operator*( float f, const Vec2 &vec ) {
	return Vec2( vec.x * f, vec.y * f );
}

/*
================
Vec2::operator/
================
*/
OG_INLINE float Vec2::operator/( const Vec2 &vec ) const {
	return (x / vec.x + y / vec.y);
}
OG_INLINE Vec2 Vec2::operator/( float f ) const {
	float invf = 1.0f/f; // multiplication is faster than division, so we just reverse the process.
	return Vec2( x * invf, y * invf );
}

/*
================
Vec2::operator+
================
*/
OG_INLINE Vec2 Vec2::operator+( const Vec2 &vec ) const {
	return Vec2( x + vec.x, y + vec.y );
}

/*
================
Vec2::operator-
================
*/
OG_INLINE Vec2 Vec2::operator-( const Vec2 &vec ) const {
	return Vec2( x - vec.x, y - vec.y );
}

/*
==============================================================================

  Vec3

==============================================================================
*/

/*
================
Vec3::Set
================
*/
OG_INLINE void Vec3::Set( float _x, float _y, float _z ) {
	x = _x;
	y = _y;
	z = _z;
}

/*
================
Vec3::Zero
================
*/
OG_INLINE void Vec3::Zero( void ) {
	Set( 0.0f, 0.0f, 0.0f );
}

/*
================
Vec3::IsZero
================
*/
OG_INLINE bool Vec3::IsZero( void ) const {
	const uInt *iv = reinterpret_cast<const uInt *>(&x);
	return !(iv[0] & Math::MASK_SIGNED) && !(iv[1] & Math::MASK_SIGNED) && !(iv[2] & Math::MASK_SIGNED);
}

/*
================
Vec3::Length
================
*/
OG_INLINE float Vec3::Length( void ) const {
	return Math::Sqrt( LengthSqr() );
}

/*
================
Vec3::LengthFast
================
*/
OG_INLINE float Vec3::LengthFast( void ) const {
	float lenSqr = LengthSqr();
	return lenSqr * Math::RSqrt( lenSqr );
}

/*
================
Vec3::LengthSqr
================
*/
OG_INLINE float Vec3::LengthSqr( void ) const {
	return ( x*x + y*y + z*z );
}

/*
================
Vec3::Normalize
================
*/
OG_INLINE float Vec3::Normalize( void ) {
	float lenSqr = LengthSqr();
	float lenInv = Math::InvSqrt( lenSqr );

	*this *= lenInv;
	return lenSqr * lenInv;
}

/*
================
Vec3::NormalizeFast
================
*/
OG_INLINE float Vec3::NormalizeFast( void ) {
	float lenSqr = LengthSqr();
	float lenInv = Math::RSqrt( lenSqr );

	*this *= lenInv;
	return lenSqr * lenInv;
}

/*
================
Vec3::Snap
================
*/
OG_INLINE Vec3 Vec3::Snap( void ) const {
	Vec3 vec = *this;
	vec.SnapSelf();
	return vec;
}

/*
================
Vec3::SnapSelf
================
*/
OG_INLINE void Vec3::SnapSelf( void ) {
	x = Math::Floor( x + 0.5f );
	y = Math::Floor( y + 0.5f );
	z = Math::Floor( z + 0.5f );
}

/*
================
Vec3::Cross

CrossProduct
================
*/
OG_INLINE Vec3 Vec3::Cross( const Vec3 &other ) const {
	return Cross( *this, other );
}
OG_INLINE Vec3 Vec3::Cross( const Vec3 &v1, const Vec3 &v2 ) {
	return Vec3( v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x );
}

/*
================
Vec3::Dot

DotProduct
================
*/
OG_INLINE float Vec3::Dot( const Vec3 &v1, const Vec3 &v2 ) {
	return ( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z );
}
OG_INLINE float Vec3::Dot( const Vec3 &other ) const {
	return Dot( *this, other );
}

/*
================
Vec3::Mid
================
*/
OG_INLINE Vec3 Vec3::Mid( const Vec3 &v1, const Vec3 &v2 ) {
	return Vec3( (v1.x + v2.x) * 0.5f, (v1.y + v2.y) * 0.5f, (v1.z + v2.z) * 0.5f);
}
OG_INLINE Vec3 Vec3::Mid( const Vec3 &other ) const {
	return Mid( *this, other );
}

/*
================
Vec3::Floor
================
*/
OG_INLINE void Vec3::Floor( void ) {
	x = Math::Floor( x );
	y = Math::Floor( y );
	z = Math::Floor( z );
}

/*
================
Vec3::Ceil
================
*/
OG_INLINE void Vec3::Ceil( void ) {
	x = Math::Ceil( x );
	y = Math::Ceil( y );
	z = Math::Ceil( z );
}

/*
================
Vec3::CapLength
================
*/
OG_INLINE void Vec3::CapLength( float length ) {
	if ( length == 0 )
		Zero();
	else {
		float lenSqr = LengthSqr();
		if ( lenSqr > length*length )
			*this *= length * Math::InvSqrt( lenSqr );
	}
}

/*
================
Vec3::Cmp
================
*/
OG_INLINE bool Vec3::Cmp( const Vec3 &vec ) const {
	return ( x == vec.x && y == vec.y && z == vec.z );
}

/*
================
Vec3::Cmp
================
*/
OG_INLINE bool Vec3::Cmp( const Vec3 &vec, float epsilon ) const {
	return ( Math::Fabs( x - vec.x ) <= epsilon && Math::Fabs( y - vec.y ) <= epsilon
			&& Math::Fabs( z - vec.z ) <= epsilon );
}

/*
================
Vec3::operator-
================
*/
OG_INLINE Vec3 Vec3::operator-() const {
	return Vec3( -x, -y, -z );
}

/*
================
Vec3::operator[]
================
*/
OG_INLINE float Vec3::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index < 3 );
	return (&x)[index];
}

/*
================
Vec3::operator[]
================
*/
OG_INLINE float &Vec3::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index < 3 );
	return (&x)[index];
}

/*
================
Vec3::operator==
================
*/
OG_INLINE bool Vec3::operator==( const Vec3 &other ) const {
	return Cmp(other);
}

/*
================
Vec3::operator!=
================
*/
OG_INLINE bool Vec3::operator!=( const Vec3 &other ) const {
	return !Cmp(other);
}

/*
================
Vec3::operator=
================
*/
OG_INLINE Vec3 &Vec3::operator=( const Vec3 &other ) {
	Set( other.x, other.y, other.z );
	return *this;
}

/*
================
Vec3::operator+=
================
*/
OG_INLINE void Vec3::operator+=( const Vec3 &vec ) {
	x += vec.x;
	y += vec.y;
	z += vec.z;
}

/*
================
Vec3::operator-=
================
*/
OG_INLINE void Vec3::operator-=( const Vec3 &vec ) {
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
}

/*
================
Vec3::operator*=
================
*/
OG_INLINE void Vec3::operator*=( const Vec3 &vec ) {
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;
}
OG_INLINE void Vec3::operator*=( float f ) {
	x *= f;
	y *= f;
	z *= f;
}

/*
================
Vec3::operator/=
================
*/
OG_INLINE void Vec3::operator/=( const Vec3 &vec ) {
	x /= vec.x;
	y /= vec.y;
	z /= vec.z;
}
OG_INLINE void Vec3::operator/=( float f ) {
	*this *= 1.0f/f; // multiplication is faster than division, so we just reverse the process.
}

/*
================
Vec3::operator*
================
*/
OG_INLINE float Vec3::operator*( const Vec3 &vec ) const {
	return (x * vec.x + y * vec.y + z * vec.z);
}
OG_INLINE Vec3 Vec3::operator*( float f ) const {
	return Vec3( x * f, y * f, z * f );
}
OG_INLINE Vec3 operator*( float f, const Vec3 &vec ) {
	return Vec3( vec.x * f, vec.y * f, vec.z * f );
}

/*
================
Vec3::operator/
================
*/
OG_INLINE float Vec3::operator/( const Vec3 &vec ) const {
	return (x / vec.x + y / vec.y + z / vec.z);
}
OG_INLINE Vec3 Vec3::operator/( float f ) const {
	float invf = 1.0f/f; // multiplication is faster than division, so we just reverse the process.
	return Vec3( x * invf, y * invf, z * invf );
}

/*
================
Vec3::operator+
================
*/
OG_INLINE Vec3 Vec3::operator+( const Vec3 &vec ) const {
	return Vec3( x + vec.x, y + vec.y, z + vec.z );
}

/*
================
Vec3::operator-
================
*/
OG_INLINE Vec3 Vec3::operator-( const Vec3 &vec ) const {
	return Vec3( x - vec.x, y - vec.y, z - vec.z );
}

/*
==============================================================================

  Vec4

==============================================================================
*/

/*
================
Vec4::Set
================
*/
OG_INLINE void Vec4::Set( float _x, float _y, float _z, float _w ) {
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

/*
================
Vec4::Zero
================
*/
OG_INLINE void Vec4::Zero( void ) {
	Set( 0.0f, 0.0f, 0.0f, 0.0f );
}

/*
================
Vec4::IsZero
================
*/
OG_INLINE bool Vec4::IsZero( void ) const {
	const uInt *iv = reinterpret_cast<const uInt *>(&x);
	return !(iv[0] & Math::MASK_SIGNED) && !(iv[1] & Math::MASK_SIGNED) && !(iv[2] & Math::MASK_SIGNED) && !(iv[3] & Math::MASK_SIGNED);
}

/*
================
Vec4::Length
================
*/
OG_INLINE float Vec4::Length( void ) const {
	return Math::Sqrt( LengthSqr() );
}

/*
================
Vec4::LengthFast
================
*/
OG_INLINE float Vec4::LengthFast( void ) const {
	float lenSqr = LengthSqr();
	return lenSqr * Math::RSqrt( lenSqr );
}

/*
================
Vec4::LengthSqr
================
*/
OG_INLINE float Vec4::LengthSqr( void ) const {
	return ( x*x + y*y + z*z + w*w );
}

/*
================
Vec4::Normalize
================
*/
OG_INLINE float Vec4::Normalize( void ) {
	float lenSqr = LengthSqr();
	float lenInv = Math::InvSqrt( lenSqr );

	*this *= lenInv;
	return lenSqr * lenInv;
}

/*
================
Vec4::NormalizeFast
================
*/
OG_INLINE float Vec4::NormalizeFast( void ) {
	float lenSqr = LengthSqr();
	float lenInv = Math::RSqrt( lenSqr );

	*this *= lenInv;
	return lenSqr * lenInv;
}

/*
================
Vec4::Snap
================
*/
OG_INLINE Vec4 Vec4::Snap( void ) const {
	Vec4 vec = *this;
	vec.SnapSelf();
	return vec;
}

/*
================
Vec4::SnapSelf
================
*/
OG_INLINE void Vec4::SnapSelf( void ) {
	x = Math::Floor( x + 0.5f );
	y = Math::Floor( y + 0.5f );
	z = Math::Floor( z + 0.5f );
	w = Math::Floor( w + 0.5f );
}

/*
================
Vec4::Mid
================
*/
OG_INLINE Vec4 Vec4::Mid( const Vec4 &v1, const Vec4 &v2 ) {
	return Vec4( (v1.x + v2.x) * 0.5f, (v1.y + v2.y) * 0.5f, (v1.z + v2.z) * 0.5f, (v1.w + v2.w) * 0.5f);
}
OG_INLINE Vec4 Vec4::Mid( const Vec4 &other ) const {
	return Mid( *this, other );
}

/*
================
Vec4::Floor
================
*/
OG_INLINE void Vec4::Floor( void ) {
	x = Math::Floor( x );
	y = Math::Floor( y );
	z = Math::Floor( z );
	w = Math::Floor( w );
}

/*
================
Vec4::Ceil
================
*/
OG_INLINE void Vec4::Ceil( void ) {
	x = Math::Ceil( x );
	y = Math::Ceil( y );
	z = Math::Ceil( z );
	w = Math::Ceil( w );
}

/*
================
Vec4::CapLength
================
*/
OG_INLINE void Vec4::CapLength( float length ) {
	if ( length == 0 )
		Zero();
	else {
		float lenSqr = LengthSqr();
		if ( lenSqr > length*length )
			*this *= length * Math::InvSqrt( lenSqr );
	}
}

/*
================
Vec4::Cmp
================
*/
OG_INLINE bool Vec4::Cmp( const Vec4 &vec ) const {
	return ( x == vec.x && y == vec.y && z == vec.z && w == vec.w );
}

/*
================
Vec4::Cmp
================
*/
OG_INLINE bool Vec4::Cmp( const Vec4 &vec, float epsilon ) const {
	return ( Math::Fabs( x - vec.x ) <= epsilon && Math::Fabs( y - vec.y ) <= epsilon
			&& Math::Fabs( z - vec.z ) <= epsilon && Math::Fabs( w - vec.w ) <= epsilon );
}

/*
================
Vec4::operator-
================
*/
OG_INLINE Vec4 Vec4::operator-() const {
	return Vec4( -x, -y, -z, -w );
}

/*
================
Vec4::operator[]
================
*/
OG_INLINE float Vec4::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index < 4 );
	return (&x)[index];
}

/*
================
Vec4::operator[]
================
*/
OG_INLINE float &Vec4::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index < 4 );
	return (&x)[index];
}

/*
================
Vec4::operator==
================
*/
OG_INLINE bool Vec4::operator==( const Vec4 &other ) const {
	return Cmp(other);
}

/*
================
Vec4::operator!=
================
*/
OG_INLINE bool Vec4::operator!=( const Vec4 &other ) const {
	return !Cmp(other);
}

/*
================
Vec4::operator=
================
*/
OG_INLINE Vec4 &Vec4::operator=( const Vec4 &other ) {
	Set( other.x, other.y, other.z, other.w );
	return *this;
}

/*
================
Vec4::operator+=
================
*/
OG_INLINE void Vec4::operator+=( const Vec4 &vec ) {
	x += vec.x;
	y += vec.y;
	z += vec.z;
	w += vec.w;
}

/*
================
Vec4::operator-=
================
*/
OG_INLINE void Vec4::operator-=( const Vec4 &vec ) {
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	w -= vec.w;
}

/*
================
Vec4::operator*=
================
*/
OG_INLINE void Vec4::operator*=( const Vec4 &vec ) {
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;
	w *= vec.w;
}
OG_INLINE void Vec4::operator*=( float f ) {
	x *= f;
	y *= f;
	z *= f;
	w *= f;
}

/*
================
Vec4::operator/=
================
*/
OG_INLINE void Vec4::operator/=( const Vec4 &vec ) {
	x /= vec.x;
	y /= vec.y;
	z /= vec.z;
	w /= vec.w;
}
OG_INLINE void Vec4::operator/=( float f ) {
	*this *= 1.0f/f; // multiplication is faster than division, so we just reverse the process.
}

/*
================
Vec4::operator*
================
*/
OG_INLINE float Vec4::operator*( const Vec4 &vec ) const {
	return (x * vec.x + y * vec.y + z * vec.z + w * vec.w);
}
OG_INLINE Vec4 Vec4::operator*( float f ) const {
	return Vec4( x * f, y * f, z * f, w * f );
}
OG_INLINE Vec4 operator*( float f, const Vec4 &vec ) {
	return Vec4( vec.x * f, vec.y * f, vec.z * f, vec.w * f );
}

/*
================
Vec4::operator/
================
*/
OG_INLINE float Vec4::operator/( const Vec4 &vec ) const {
	return (x / vec.x + y / vec.y + z / vec.z + w / vec.w);
}
OG_INLINE Vec4 Vec4::operator/( float f ) const {
	float invf = 1.0f/f; // multiplication is faster than division, so we just reverse the process.
	return Vec4( x * invf, y * invf, z * invf, w * invf );
}

/*
================
Vec4::operator+
================
*/
OG_INLINE Vec4 Vec4::operator+( const Vec4 &vec ) const {
	return Vec4( x + vec.x, y + vec.y, z + vec.z, w + vec.w );
}

/*
================
Vec4::operator-
================
*/
OG_INLINE Vec4 Vec4::operator-( const Vec4 &vec ) const {
	return Vec4( x - vec.x, y - vec.y, z - vec.z, w - vec.w );
}

/*
==============================================================================

  Vec5

==============================================================================
*/

/*
================
Vec5::Set
================
*/
OG_INLINE void Vec5::Set( float _x, float _y, float _z, float _s, float _t ) {
	x = _x;
	y = _y;
	z = _z;
	s = _s;
	t = _t;
}

/*
================
Vec5::Zero
================
*/
OG_INLINE void Vec5::Zero( void ) {
	Set( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
}

/*
================
Vec5::IsZero
================
*/
OG_INLINE bool Vec5::IsZero( void ) const {
	const uInt *iv = reinterpret_cast<const uInt *>(&x);
	return !(iv[0] & Math::MASK_SIGNED) && !(iv[1] & Math::MASK_SIGNED) && !(iv[2] & Math::MASK_SIGNED) && !(iv[3] & Math::MASK_SIGNED) && !(iv[4] & Math::MASK_SIGNED);
}

/*
================
Vec5::Cmp
================
*/
OG_INLINE bool Vec5::Cmp( const Vec5 &vec ) const {
	return ( x == vec.x && y == vec.y && z == vec.z && s == vec.s && t == vec.t);
}

/*
================
Vec5::Cmp
================
*/
OG_INLINE bool Vec5::Cmp( const Vec5 &vec, float epsilon ) const {
	return ( Math::Fabs( x - vec.x ) <= epsilon && Math::Fabs( y - vec.y ) <= epsilon
		&& Math::Fabs( z - vec.z ) <= epsilon && Math::Fabs( s - vec.s ) <= epsilon
		&& Math::Fabs( t - vec.t ) <= epsilon );
}


/*
================
Vec5::operator-
================
*/
OG_INLINE Vec5 Vec5::operator-() const {
	return Vec5( -x, -y, -z, -s, -t );
}

/*
================
Vec5::operator[]
================
*/
OG_INLINE float Vec5::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index < 5 );
	return (&x)[index];
}

/*
================
Vec5::operator[]
================
*/
OG_INLINE float &Vec5::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index < 5 );
	return (&x)[index];
}

/*
================
Vec5::operator==
================
*/
OG_INLINE bool Vec5::operator==( const Vec5 &other ) const {
	return Cmp(other);
}

/*
================
Vec5::operator!=
================
*/
OG_INLINE bool Vec5::operator!=( const Vec5 &other ) const {
	return !Cmp(other);
}

/*
================
Vec5::operator=
================
*/
OG_INLINE Vec5 &Vec5::operator=( const Vec5 &other ) {
	Set( other.x, other.y, other.z, other.s, other.t );
	return *this;
}

/*
================
Vec5::operator=
================
*/
OG_INLINE Vec5 &Vec5::operator=( const Vec3 &other ) {
	Set( other.x, other.y, other.z, 0.0f, 0.0f );
	return *this;
}

/*
==============================================================================

  VecX

==============================================================================
*/

/*
================
VecX::VecX
================
*/
template< int dim >
OG_INLINE VecX<dim>::VecX( float vec, ... ) {
	v[0] = vec;
	OG_ASSERT( dim >= 2 );
	va_list argptr;
	va_start( argptr, vec );

	for(int i=1; i<dim; i++)
		v[i] = static_cast<float>(va_arg(argptr, double));

	va_end(argptr);
}

/*
================
VecX::Set
================
*/
template< int dim >
OG_INLINE void VecX<dim>::Set( const VecX<dim> &vec ) {
	for(int i=0; i<dim; i++)
		v[i] = vec[i];
}
template< int dim >
OG_INLINE void VecX<dim>::Set( const float *vec ) {
	*v = NULL;
	memcpy( v, vec, dim * sizeof( float ) );
}

/*
================
VecX::Zero
================
*/
template< int dim >
OG_INLINE void VecX<dim>::Zero( void ) {
	for(int i=0; i<dim; i++ )
		v[i] = 0.0f;
}

/*
================
VecX::IsZero
================
*/
template< int dim >
OG_INLINE bool VecX<dim>::IsZero( void ) const {
	const uInt *iv = reinterpret_cast<const uInt *>(v);
	for(int i=0; i<dim; i++) {
		if( iv[i] & Math::MASK_SIGNED )
			return false;
	}
	return true;
}

/*
================
VecX::Length
================
*/
template< int dim >
OG_INLINE float VecX<dim>::Length( void ) const {
	return Math::Sqrt( LengthSqr() );
}

/*
================
VecX::LengthFast
================
*/
template< int dim >
OG_INLINE float VecX<dim>::LengthFast( void ) const {
	float lenSqr = LengthSqr();
	return lenSqr * Math::RSqrt( lenSqr );
}

/*
================
VecX::LengthSqr
================
*/
template< int dim >
OG_INLINE float VecX<dim>::LengthSqr( void ) const {
	float ret = 0.0f;
	for(int i=0; i<dim; i++ )
		ret += v[i] * v[i];
	return ret;
}

/*
================
VecX::Normalize
================
*/
template< int dim >
OG_INLINE float VecX<dim>::Normalize( void ) {
	float lenSqr = LengthSqr();
	float lenInv = Math::InvSqrt( lenSqr );

	*this *= lenInv;
	return lenSqr * lenInv;
}

/*
================
VecX::NormalizeFast
================
*/
template< int dim >
OG_INLINE float VecX<dim>::NormalizeFast( void ) {
	float lenSqr = LengthSqr();
	float lenInv = Math::RSqrt( lenSqr );

	*this *= lenInv;
	return lenSqr * lenInv;
}

/*
================
VecX::Snap
================
*/
template< int dim >
OG_INLINE VecX<dim> VecX<dim>::Snap( void ) const {
	VecX<dim> vec = *this;
	vec.SnapSelf();
	return vec;
}

/*
================
VecX::SnapSelf
================
*/
template< int dim >
OG_INLINE void VecX<dim>::SnapSelf( void ) {
	for(int i=0; i<dim; i++ )
		v[i] = Math::Floor( v[i] + 0.5f );
}

/*
================
VecX::Floor
================
*/
template< int dim >
OG_INLINE void VecX<dim>::Floor( void ) {
	for(int i=0; i<dim; i++ )
		v[i] = Math::Floor( v[i] );
}

/*
================
VecX::Ceil
================
*/
template< int dim >
OG_INLINE void VecX<dim>::Ceil( void ) {
	for(int i=0; i<dim; i++ )
		v[i] = Math::Ceil( v[i] );
}

/*
================
VecX::CapLength
================
*/
template< int dim >
OG_INLINE void VecX<dim>::CapLength( float length ) {
	if ( length == 0 )
		Zero();
	else {
		float lenSqr = LengthSqr();
		if ( lenSqr > length*length )
			*this *= length * Math::InvSqrt( lenSqr );
	}
}

/*
================
VecX::Cmp
================
*/
template< int dim >
OG_INLINE bool VecX<dim>::Cmp( const VecX<dim> &vec ) const {
	for(int i=0; i<dim; i++ ) {
		if ( v[i] != vec[i] )
			return false;
	}
	return true;
}

/*
================
VecX::Cmp
================
*/
template< int dim >
OG_INLINE bool VecX<dim>::Cmp( const VecX<dim> &vec, float epsilon ) const {
	for(int i=0; i<dim; i++ ) {
		if ( Math::Fabs( v[i] - vec[i] ) <= epsilon )
			return false;
	}
	return true;
}

/*
================
VecX::operator-
================
*/
template< int dim >
OG_INLINE VecX<dim> VecX<dim>::operator-() const {
	VecX<dim> vec(*this);
	vec *= -1.0f;
	return vec;
}

/*
================
VecX::operator[]
================
*/
template< int dim >
OG_INLINE float VecX<dim>::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index < dim );
	return v[index];
}

/*
================
VecX::operator[]
================
*/
template< int dim >
OG_INLINE float &VecX<dim>::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index < dim );
	return v[index];
}

/*
================
VecX::operator==
================
*/
template< int dim >
OG_INLINE bool VecX<dim>::operator==( const VecX<dim> &other ) const {
	return Cmp(other);
}

/*
================
VecX::operator!=
================
*/
template< int dim >
OG_INLINE bool VecX<dim>::operator!=( const VecX<dim> &other ) const {
	return !Cmp(other);
}

/*
================
VecX::operator=
================
*/
template< int dim >
OG_INLINE VecX<dim> &VecX<dim>::operator=( const VecX<dim> &other ) {
	Set(other);
	return *this;
}

/*
================
VecX::operator+=
================
*/
template< int dim >
OG_INLINE void VecX<dim>::operator+=( const VecX<dim> &vec ) {
	for( int i=0; i<dim; i++ )
		v[i] += vec[i];
}

/*
================
VecX::operator-=
================
*/
template< int dim >
OG_INLINE void VecX<dim>::operator-=( const VecX<dim> &vec ) {
	for( int i=0; i<dim; i++ )
		v[i] -= vec[i];
}

/*
================
VecX::operator*=
================
*/
template< int dim >
OG_INLINE void VecX<dim>::operator*=( const VecX<dim> &vec ) {
	for( int i=0; i<dim; i++ )
		v[i] *= vec[i];
}
template< int dim >
OG_INLINE void VecX<dim>::operator*=( float f ) {
	for( int i=0; i<dim; i++ )
		v[i] *= f;
}

/*
================
VecX::operator/=
================
*/
template< int dim >
OG_INLINE void VecX<dim>::operator/=( const VecX<dim> &vec ) {
	for( int i=0; i<dim; i++ )
		v[i] /= vec[i];
}
template< int dim >
OG_INLINE void VecX<dim>::operator/=( float f ) {
	*this *= 1.0f/f; // multiplication is faster than division, so we just reverse the process.
}

/*
================
VecX::operator*
================
*/
template< int dim >
OG_INLINE float VecX<dim>::operator*( const VecX<dim> &vec ) const {
	float ret = 0.0f;
	for(int i=0; i<dim; i++ )
		ret += v[i] * vec[i];
	return ret;
}
template< int dim >
OG_INLINE VecX<dim> VecX<dim>::operator*( float f ) const {
	VecX<dim> vec(*this);
	vec *= f;
	return vec;
}
template< int dim >
OG_INLINE VecX<dim> operator*( float f, const VecX<dim> &vec ) {
	VecX<dim> _vec(vec);
	_vec *= f;
	return _vec;
}

/*
================
VecX::operator/
================
*/
template< int dim >
OG_INLINE float VecX<dim>::operator/( const VecX<dim> &vec ) const {
	float ret = 0.0f;
	for(int i=0; i<dim; i++ )
		ret += v[i] / vec[i];
	return ret;
}
template< int dim >
OG_INLINE VecX<dim> VecX<dim>::operator/( float f ) const {
	VecX<dim> vec(*this);
	vec /= f;
	return vec;
}

/*
================
VecX::operator+
================
*/
template< int dim >
OG_INLINE VecX<dim> VecX<dim>::operator+( const VecX<dim> &vec ) const {
	VecX<dim> _vec(*this);
	_vec += vec;
	return _vec;
}

/*
================
VecX::operator-
================
*/
template< int dim >
OG_INLINE VecX<dim> VecX<dim>::operator-( const VecX<dim> &vec ) const {
	VecX<dim> _vec(*this);
	_vec -= vec;
	return _vec;
}

}

#endif
