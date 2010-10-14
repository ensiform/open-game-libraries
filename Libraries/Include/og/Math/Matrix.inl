/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Ensiform
Purpose: Matrix class
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

#ifndef __OG_MATRIX_INL__
#define __OG_MATRIX_INL__

namespace og {

/*
==============================================================================

  Mat2 - 2x2 matrix (row-major)

==============================================================================
*/

/*
================
Mat2::Mat2
================
*/
OG_INLINE Mat2::Mat2( const Vec2 &v0, const Vec2 &v1 ) {
	mat[0] = v0;
	mat[1] = v1;
}
OG_INLINE Mat2::Mat2( float x0, float y0, float x1, float y1 ) {
	mat[0].Set( x0, y0 );
	mat[1].Set( x1, y1 );
}
OG_INLINE Mat2::Mat2( float msrc[2][2] ) {
	// faster than setting each item
	memcpy( mat, msrc, 4*sizeof(float) );
}

/*
================
Mat2::Zero
================
*/
OG_INLINE void Mat2::Zero( void ) {
	mat[0].Zero();
	mat[1].Zero();
}

/*
================
Mat2::IsZero
================
*/
OG_INLINE bool Mat2::IsZero( void ) const {
	return (mat[0].IsZero() && mat[1].IsZero());
}

/*
================
Mat2::Identity

 1 0
 0 1
================
*/
OG_INLINE void Mat2::Identity( void ) {
	mat[0].Set( 1.0f, 0.0f );
	mat[1].Set( 0.0f, 1.0f );
}

/*
================
Mat2::IsIdentity
================
*/
OG_INLINE bool Mat2::IsIdentity( const float epsilon ) const {
	return Cmp( c_mat2::identity, epsilon );
}

/*
================
Mat2::Cmp
================
*/
OG_INLINE bool Mat2::Cmp( const Mat2 &other ) const {
	return ( mat[0].Cmp(other[0]) && mat[1].Cmp(other[1]) );
}
OG_INLINE bool Mat2::Cmp( const Mat2 &other, float epsilon ) const {
	return ( mat[0].Cmp(other[0], epsilon) && mat[1].Cmp(other[1], epsilon) );
}

/*
================
Mat2::Determinant
================
*/
OG_INLINE float Mat2::Determinant( void ) const {
	return mat[0].x * mat[1].y - mat[0].y * mat[1].x;
}

/*
================
Mat2::Rotation
================
*/
OG_INLINE void Mat2::Rotation( float angle ) {
	float a;
	float b;
	Math::SinCos( angle, a, b );

	mat[ 0 ].Set( a, b );
	mat[ 1 ].Set( -b, a );
}

/*
================
Mat2::Transpose

 x1 x2    x1 y1
       =>
 y1 y2    x2 y2
================
*/
OG_INLINE Mat2 Mat2::Transpose( void ) const {
	return Mat2( mat[0].x, mat[1].x, mat[0].y, mat[1].y );
}

/*
================
Mat2::TransposeSelf

 x1 x2    x1 y1
       =>
 y1 y2    x2 y2
================
*/
OG_INLINE void Mat2::TransposeSelf( void ) {
	float xy;

	xy = mat[0].y;
	mat[0].y = mat[1].x;
	mat[1].x = xy;
}

/*
============
Mat2::Inverse
============
*/
OG_INLINE Mat2 Mat2::Inverse( void ) const {
	Mat2 invMat = *this;
	invMat.InverseSelf();
	return invMat;
}

/*
================
Mat2::operator-
================
*/
OG_INLINE Mat2 Mat2::operator-() const {
	return Mat2( -mat[0].x, -mat[0].y, -mat[1].x, -mat[1].y );
}

/*
================
Mat2::operator[]
================
*/
OG_INLINE const Vec2 &Mat2::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index < 2 );
	return mat[index];
}

/*
================
Mat2::operator[]
================
*/
OG_INLINE Vec2 &Mat2::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index < 2 );
	return mat[index];
}

/*
================
Mat2::operator==
================
*/
OG_INLINE bool Mat2::operator==( const Mat2 &other ) const {
	return Cmp(other);
}

/*
================
Mat2::operator!=
================
*/
OG_INLINE bool Mat2::operator!=( const Mat2 &other ) const {
	return !Cmp(other);
}

/*
================
Mat2::operator=
================
*/
OG_INLINE Mat2 &Mat2::operator=( const Mat2 &other ) {
	// faster than setting each item
	memcpy( &mat[0].x, &other.mat[0].x, sizeof(mat) );
	return *this;
}

/*
================
Mat2::operator+=
================
*/
OG_INLINE void Mat2::operator+=( const Mat2 &other ) {
	mat[0] += other[0];
	mat[1] += other[1];
}

/*
================
Mat2::operator-=
================
*/
OG_INLINE void Mat2::operator-=( const Mat2 &other ) {
	mat[0] -= other[0];
	mat[1] -= other[1];
}

/*
================
Mat2::operator*=
================
*/
OG_INLINE void Mat2::operator*=( float f ) {
	mat[0] *= f;
	mat[1] *= f;
}
OG_INLINE Vec2 &operator*=( Vec2 &v, const Mat2 &m ) {
	v = m * v;
	return v;
}

/*
================
Mat2::operator*
================
*/
OG_INLINE Vec2 Mat2::operator*( const Vec2 &v ) const {
	return Vec2( mat[0].x * v.x + mat[0].y * v.y, mat[1].x * v.x + mat[1].y * v.y );
}
OG_INLINE Mat2 Mat2::operator*( float f ) const {
	return Mat2( mat[0].x * f, mat[0].y * f, mat[1].x * f, mat[1].y * f );
}
OG_INLINE Mat2 operator*( float f, const Mat2 &m ) {
	return m * f;
}
OG_INLINE Vec2 operator*( const Vec2 &v, const Mat2 &m ) {
	return m * v;
}

/*
================
Mat2::operator+
================
*/
OG_INLINE Mat2 Mat2::operator+( const Mat2 &other ) const {
	return Mat2( mat[0].x + other[0].x, mat[0].y + other[0].y, mat[1].x + other[1].x, mat[1].y + other[1].y );
}

/*
================
Mat2::operator-
================
*/
OG_INLINE Mat2 Mat2::operator-( const Mat2 &other ) const {
	return Mat2( mat[0].x - other[0].x, mat[0].y - other[0].y, mat[1].x - other[1].x, mat[1].y - other[1].y );
}

/*
==============================================================================

  Mat3 - 3x3 matrix (column-major)

==============================================================================
*/

/*
================
Mat3::Mat3
================
*/
OG_INLINE Mat3::Mat3( const Vec3 &v0, const Vec3 &v1, const Vec3 &v2 ) {
	mat[0] = v0;
	mat[1] = v1;
	mat[2] = v2;
}
OG_INLINE Mat3::Mat3( float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2 ) {
	mat[0].Set( x0, y0, z0 );
	mat[1].Set( x1, y1, z1 );
	mat[2].Set( x2, y2, z2 );
}
OG_INLINE Mat3::Mat3( float msrc[3][3] ) {
	// faster than setting each item
	memcpy( mat, msrc, sizeof(mat) );
}

/*
================
Mat3::Zero
================
*/
OG_INLINE void Mat3::Zero( void ) {
	mat[0].Zero();
	mat[1].Zero();
	mat[2].Zero();
}

/*
================
Mat3::IsZero
================
*/
OG_INLINE bool Mat3::IsZero( void ) const {
	return (mat[0].IsZero() && mat[1].IsZero() && mat[2].IsZero());
}

/*
================
Mat3::Identity

 1 0 0
 0 1 0
 0 0 1
================
*/
OG_INLINE void Mat3::Identity( void ) {
	mat[0].Set( 1.0f, 0.0f, 0.0f );
	mat[1].Set( 0.0f, 1.0f, 0.0f );
	mat[2].Set( 0.0f, 0.0f, 1.0f );
}

/*
================
Mat3::IsIdentity
================
*/
OG_INLINE bool Mat3::IsIdentity( const float epsilon ) const {
	return Cmp( c_mat3::identity, epsilon );
}

/*
================
Mat3::Cmp
================
*/
OG_INLINE bool Mat3::Cmp( const Mat3 &other ) const {
	return ( mat[0].Cmp(other[0]) && mat[1].Cmp(other[1]) && mat[2].Cmp(other[2]) );
}
OG_INLINE bool Mat3::Cmp( const Mat3 &other, float epsilon ) const {
	return ( mat[0].Cmp(other[0], epsilon) && mat[1].Cmp(other[1], epsilon) && mat[2].Cmp(other[2], epsilon) );
}

/*
================
Mat3::Transpose
================
*/
OG_INLINE Mat3 Mat3::Transpose( void ) const {
	return Mat3( mat[0].x, mat[1].x, mat[2].x, mat[0].y, mat[1].y, mat[2].y, mat[0].z, mat[1].z, mat[2].z );
}

/*
============
Mat3::Inverse
============
*/
OG_INLINE Mat3 Mat3::Inverse( void ) const {
	Mat3 invMat = *this;
	invMat.InverseSelf();
	return invMat;
}

/*
================
Mat3::operator-
================
*/
OG_INLINE Mat3 Mat3::operator-() const {
	return Mat3( -mat[0].x, -mat[0].y, -mat[0].z, -mat[1].x, -mat[1].y, -mat[1].z, -mat[2].x, -mat[2].y, -mat[2].z );
}

/*
================
Mat3::operator[]
================
*/
OG_INLINE const Vec3 &Mat3::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index < 3 );
	return mat[index];
}

/*
================
Mat3::operator[]
================
*/
OG_INLINE Vec3 &Mat3::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index < 3 );
	return mat[index];
}

/*
================
Mat3::operator==
================
*/
OG_INLINE bool Mat3::operator==( const Mat3 &other ) const {
	return Cmp(other);
}

/*
================
Mat3::operator!=
================
*/
OG_INLINE bool Mat3::operator!=( const Mat3 &other ) const {
	return !Cmp(other);
}

/*
================
Mat3::operator=
================
*/
OG_INLINE Mat3 &Mat3::operator=( const Mat3 &other ) {
	// faster than setting each item
	memcpy( &mat[0].x, &other.mat[0].x, sizeof(mat) );
	return *this;
}

/*
================
Mat3::operator+=
================
*/
OG_INLINE void Mat3::operator+=( const Mat3 &other ) {
	mat[0] += other[0];
	mat[1] += other[1];
	mat[2] += other[2];
}

/*
================
Mat3::operator-=
================
*/
OG_INLINE void Mat3::operator-=( const Mat3 &other ) {
	mat[0] -= other[0];
	mat[1] -= other[1];
	mat[2] -= other[2];
}

/*
================
Mat3::operator*=
================
*/
OG_INLINE void Mat3::operator*=( float f ) {
	mat[0] *= f;
	mat[1] *= f;
	mat[2] *= f;
}
OG_INLINE Vec3 &operator*=( Vec3 &v, const Mat3 &m ) {
	v = m * v;
	return v;
}

/*
================
Mat3::operator*=
================
*/
OG_INLINE void Mat3::operator/=( float f ) {
	if ( f == 0.0f ) {
		Zero();
		return;
	}

	*this *= 1.0f / f;
}

/*
================
Mat3::operator*
================
*/
OG_INLINE Vec3 Mat3::operator*( const Vec3 &v ) const {
	return Vec3( mat[0].x * v.x + mat[0].y * v.y + mat[0].z * v.z, mat[1].x * v.x + mat[1].y * v.y + mat[1].z * v.z, mat[2].x * v.x + mat[2].y * v.y + mat[2].z * v.z );
}
OG_INLINE Mat3 Mat3::operator*( float f ) const {
	return Mat3( mat[0].x * f, mat[0].y * f, mat[0].z * f, mat[1].x * f, mat[1].y * f, mat[1].z * f, mat[2].x * f, mat[2].y * f, mat[2].z * f );
}
OG_INLINE Mat3 operator*( float f, const Mat3 &m ) {
	return m * f;
}
OG_INLINE Vec3 operator*( const Vec3 &v, const Mat3 &m ) {
	return m * v;
}

/*
================
Mat3::operator/
================
*/
OG_INLINE Mat3 Mat3::operator/( float f ) const {
	if ( Math::Fabs( f ) < MATRIX_INVERSE_EPSILON )
		return c_mat3::zero;

	float inv = 1.0f / f;
	return Mat3( mat[0].x * inv, mat[0].y * inv, mat[0].z * inv, mat[1].x * inv, mat[1].y * inv, mat[1].z * inv, mat[2].x * inv, mat[2].y * inv, mat[2].z * inv );
}
OG_INLINE Mat3 operator/( float f, const Mat3 &m ) {
	return m / f;
}

/*
================
Mat3::operator+
================
*/
OG_INLINE Mat3 Mat3::operator+( const Mat3 &other ) const {
	return Mat3( mat[0].x + other[0].x, mat[0].y + other[0].y, mat[0].z + other[0].z, mat[1].x + other[1].x, mat[1].y + other[1].y, mat[1].z + other[1].z, mat[2].x + other[2].x, mat[2].y + other[2].y, mat[2].z + other[2].z );
}

/*
================
Mat3::operator-
================
*/
OG_INLINE Mat3 Mat3::operator-( const Mat3 &other ) const {
	return Mat3( mat[0].x - other[0].x, mat[0].y - other[0].y, mat[0].z - other[0].z, mat[1].x - other[1].x, mat[1].y - other[1].y, mat[1].z - other[1].z, mat[2].x - other[2].x, mat[2].y - other[2].y, mat[2].z - other[2].z );
}

}

#endif
