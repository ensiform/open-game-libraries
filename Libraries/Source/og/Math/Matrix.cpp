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

#include <og/Math.h>

namespace og {

/*
==============================================================================

  Mat2 - 2x2 matrix

==============================================================================
*/

namespace c_mat2 {
	Mat2 zero    ( 0.0f, 0.0f, 0.0f, 0.0f );
	Mat2 identity( 1.0f, 0.0f, 0.0f, 1.0f );
}

/*
============
Mat2::InverseSelf
============
*/
bool Mat2::InverseSelf( void ) {
	// det is determinant which can also be considered a scale factor
	float det, invDet, x0;

	// 2x2 mat det is det(mat) = x0*y1 - y0*x1
	det = Determinant();

	if ( Math::Fabs( det ) < MATRIX_INVERSE_EPSILON )
		return false;

	invDet = 1.0f / det;

	x0 = mat[0][0];
	mat[0][0] = mat[1][1] * invDet;
	mat[0][1] = -mat[0][1] * invDet;
	mat[1][0] = -mat[1][0] * invDet;
	mat[1][1] = x0 * invDet;

	return true;
}

/*
================
Mat2::operator*=
================
*/
void Mat2::operator*=( const Mat2 &other ) {
	Vec2 old = mat[0];
	mat[0].x = old.x * other[0].x + old.y * other[1].x;
	mat[0].y = old.x * other[0].y + old.y * other[1].y;

	old = mat[1];
	mat[1].x = old.x * other[0].x + old.y * other[1].x;
	mat[1].y = old.x * other[0].y + old.y * other[1].y;
}

/*
================
Mat2::operator*
================
*/
Mat2 Mat2::operator*( const Mat2 &other ) const {
	return Mat2(
		mat[0].x * other[0].x + mat[0].y * other[1].x,
		mat[0].x * other[0].y + mat[0].y * other[1].y,

		mat[1].x * other[0].x + mat[1].y * other[1].x,
		mat[1].x * other[0].y + mat[1].y * other[1].y );
}


/*
==============================================================================

  Mat3 - 3x3 matrix

==============================================================================
*/

namespace c_mat3 {
	Mat3 zero    ( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
	Mat3 identity( 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f );
}

/*
================
Mat3::Determinant
================
*/
float Mat3::Determinant( void ) const {
	float det_1 = mat[1].x * mat[2].y - mat[1].y * mat[2].x;
	float det_2 = mat[1].x * mat[2].z - mat[1].z * mat[2].x;
	float det_3 = mat[1].y * mat[2].z - mat[1].z * mat[2].y;

	return mat[0].x * det_3 - mat[0].y * det_2 + mat[0].z * det_1;
}

/*
================
Mat3::TransposeMultiply
================
*/
Mat3 Mat3::TransposeMultiply( const Mat3 &m ) const {
	// can this be done cleaner or with asm?
	return Mat3(	mat[0].x * m[0].x + mat[1].x * m[1].x + mat[2].x * m[2].x,
					mat[0].x * m[0].y + mat[1].x * m[1].y + mat[2].x * m[2].y,
					mat[0].x * m[0].z + mat[1].x * m[1].z + mat[2].x * m[2].z,
					mat[0].y * m[0].x + mat[1].y * m[1].x + mat[2].y * m[2].x,
					mat[0].y * m[0].y + mat[1].y * m[1].y + mat[2].y * m[2].y,
					mat[0].y * m[0].z + mat[1].y * m[1].z + mat[2].y * m[2].z,
					mat[0].z * m[0].x + mat[1].z * m[1].x + mat[2].z * m[2].x,
					mat[0].z * m[0].y + mat[1].z * m[1].y + mat[2].z * m[2].y,
					mat[0].z * m[0].z + mat[1].z * m[1].z + mat[2].z * m[2].z );
}
Vec3 Mat3::TransposeMultiply( const Vec3 &v ) const {
	return Vec3(	mat[0].x * v.x + mat[0].y * v.y + mat[0].z * v.z,
					mat[1].x * v.x + mat[1].y * v.y + mat[1].z * v.z,
					mat[2].x * v.x + mat[2].y * v.y + mat[2].z * v.z );
}

/*
================
Mat3::TransposeSelf
================
*/
void Mat3::TransposeSelf( void ) {
	// can this be done cleaner or with asm?
	float xy, xz, yz;

	xy = mat[0].y;
	mat[0].y = mat[1].x;
	mat[1].x = xy;
	xz = mat[0].z;
	mat[0].z = mat[2].x;
	mat[2].x = xz;
	yz = mat[1].z;
	mat[1].z = mat[2].y;
	mat[2].y = yz;
}

/*
	0 1 2
	3 4 5
	6 7 8

	0x 1x 2x
	0y 1y 2y
	0z 1z 2z
*/

/*
============
Mat3::InverseSelf
============
*/
bool Mat3::InverseSelf( void ) {
	Mat3 mInverse;

	mInverse[0].x = mat[1].y * mat[2].z - mat[1].z * mat[2].y;
	mInverse[0].y = mat[0].z * mat[2].y - mat[0].y * mat[2].z;
	mInverse[0].z = mat[0].y * mat[1].z - mat[0].z * mat[1].y;
	mInverse[1].x = mat[1].z * mat[2].x - mat[1].x * mat[2].z;
	mInverse[1].y = mat[0].x * mat[2].z - mat[0].z * mat[2].x;
	mInverse[1].z = mat[0].z * mat[1].x - mat[0].x * mat[1].z;
	mInverse[2].x = mat[1].x * mat[2].y - mat[1].y * mat[2].x;
	mInverse[2].y = mat[0].y * mat[2].x - mat[0].x * mat[2].y;
	mInverse[2].z = mat[0].x * mat[1].y - mat[0].y * mat[1].x;

	float d = mat[0].x * mInverse[0].x + mat[0].y * mInverse[1].x + mat[0].z * mInverse[2].x;

	if ( Math::Fabs( d ) < MATRIX_INVERSE_EPSILON )
		return false;

	mInverse /= d;

	mat[0] = mInverse[0];
	mat[1] = mInverse[1];
	mat[2] = mInverse[2];

	return true;
}

/*
================
Mat3::ToQuat
================
*/
Quat Mat3::ToQuat( void ) const {
	Quat quat;
	float tr = mat[0][0] + mat[1][1] + mat[2][2];
	if( tr > 0.00001f ) {
		float s = Math::Sqrt( tr + 1.0f );
		quat.w = s * 0.5f;
		s = 0.5f / s;
		quat.x = (mat[2][1] - mat[1][2]) * s;
		quat.y = (mat[0][2] - mat[2][0]) * s;
		quat.z = (mat[1][0] - mat[0][1]) * s;
	} else {
		int i = 0;
		if (mat[1][1] > mat[0][0])
			i = 1;
		if (mat[2][2] > mat[i][i])
			i = 2;
		int j = (i + 1) % 3;
		int k = (i + 2) % 3;

		float s = Math::Sqrt( mat[i][i] - (mat[j][j] + mat[k][k]) + 1.0f );

		quat[i] = s * 0.5f;
		if( s != 0.0f )
			s = 0.5f / s;
		quat[j] = (mat[j][i] + mat[i][j]) * s;
		quat[k] = (mat[k][i] + mat[i][k]) * s;
		quat[3] = (mat[k][j] - mat[j][k]) * s;
	}

	quat.Normalize();
	return quat;
}

/*
================
Mat3::operator*=
================
*/
void Mat3::operator*=( const Mat3 &other ) {
	Vec3 old = mat[0];
	mat[0].x = old.x * other[0].x + old.y * other[1].x + old.z * other[2].x;
	mat[0].y = old.x * other[0].y + old.y * other[1].y + old.z * other[2].y;
	mat[0].z = old.x * other[0].z + old.y * other[1].z + old.z * other[2].z;

	old = mat[1];
	mat[1].x = old.x * other[0].x + old.y * other[1].x + old.z * other[2].x;
	mat[1].y = old.x * other[0].y + old.y * other[1].y + old.z * other[2].y;
	mat[1].z = old.x * other[0].z + old.y * other[1].z + old.z * other[2].z;

	old = mat[2];
	mat[2].x = old.x * other[0].x + old.y * other[1].x + old.z * other[2].x;
	mat[2].y = old.x * other[0].y + old.y * other[1].y + old.z * other[2].y;
	mat[2].z = old.x * other[0].z + old.y * other[1].z + old.z * other[2].z;
}

/*
================
Mat3::operator*
================
*/
Mat3 Mat3::operator*( const Mat3 &other ) const {
	return Mat3(
		mat[0].x * other[0].x + mat[0].y * other[1].x + mat[0].z * other[2].x,
		mat[0].x * other[0].y + mat[0].y * other[1].y + mat[0].z * other[2].y,
		mat[0].x * other[0].z + mat[0].y * other[1].z + mat[0].z * other[2].z,

		mat[1].x * other[0].x + mat[1].y * other[1].x + mat[1].z * other[2].x,
		mat[1].x * other[0].y + mat[1].y * other[1].y + mat[1].z * other[2].y,
		mat[1].x * other[0].z + mat[1].y * other[1].z + mat[1].z * other[2].z,

		mat[2].x * other[0].x + mat[2].y * other[1].x + mat[2].z * other[2].x,
		mat[2].x * other[0].y + mat[2].y * other[1].y + mat[2].z * other[2].y,
		mat[2].x * other[0].z + mat[2].y * other[1].z + mat[2].z * other[2].z );
}

}
