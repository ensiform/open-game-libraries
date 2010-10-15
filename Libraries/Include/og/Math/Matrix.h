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

#ifndef __OG_MATRIX_H__
#define __OG_MATRIX_H__

namespace og {
	class Vec2;
	class Vec3;
	class Quat;

	const float MATRIX_INVERSE_EPSILON	= 0.00000000000001f;
	const float MATRIX_EPSILON			= 0.000001f;

	/*
	==============================================================================

	  Mat2 - 2x2 matrix

	==============================================================================
	*/
	class Mat2 {
	public:
		Mat2() { Zero(); }
		Mat2( const StringType &value ) { String::ToFloatArray( value, &mat[0].x, 4 ); }
		explicit Mat2( const Vec2 &v0, const Vec2 &v1 );
		explicit Mat2( float x0, float y0, float x1, float y1 );
		explicit Mat2( float msrc[2][2] );

		void		Zero( void );
		bool		IsZero( void ) const;
		void		Identity( void );
		bool		IsIdentity( float epsilon = MATRIX_EPSILON ) const;

		bool		Cmp( const Mat2 &other ) const;
		bool		Cmp( const Mat2 &other, float epsilon ) const;

		float		Determinant( void ) const;
		void		Rotation( float angle );
		Mat2		Transpose( void ) const;
		void		TransposeSelf( void );
		Mat2		Inverse( void ) const;		// returns the inverse ( m * m.Inverse() = identity )
		bool		InverseSelf( void );		// returns false if determinant is zero

		// operators
		Mat2			operator-() const;

		const Vec2 &	operator[]( int index ) const;
		Vec2 &			operator[]( int index );

		bool			operator==( const Mat2 &other ) const;
		bool			operator!=( const Mat2 &other ) const;

		Mat2 &			operator=( const Mat2 &other );

		void			operator+=( const Mat2 &other );
		void			operator-=( const Mat2 &other );
		void			operator*=( float f );
		void			operator*=( const Mat2 &other );
		friend Vec2 &	operator*=( Vec2 &v, const Mat2 &m );

		Mat2			operator*( const Mat2 &other ) const;
		Vec2			operator*( const Vec2 &v ) const;
		Mat2			operator*( float f ) const;
		friend Mat2		operator*( float f, const Mat2 &m );
		friend Vec2		operator*( const Vec2 &vec, const Mat2 &m );

		Mat2			operator+( const Mat2 &other ) const;
		Mat2			operator-( const Mat2 &other ) const;

	private:
		Vec2			mat[2];
	};

	namespace c_mat2 {
		extern Mat2 zero;
		extern Mat2 identity;
	}

	/*
	==============================================================================

	  Mat3 - 3x3 matrix (column-major)

	==============================================================================
	*/
	class Mat3 {
	public:
		Mat3() { Zero(); }
		Mat3( const StringType &value ) { String::ToFloatArray( value, &mat[0].x, 9 ); }
		explicit Mat3( const Vec3 &v0, const Vec3 &v1, const Vec3 &v2 );
		explicit Mat3( float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2 );
		explicit Mat3( float msrc[3][3] );

		void		Zero( void );
		bool		IsZero( void ) const;
		void		Identity( void );
		bool		IsIdentity( float epsilon = MATRIX_EPSILON ) const;

		bool		Cmp( const Mat3 &other ) const;
		bool		Cmp( const Mat3 &other, float epsilon ) const;

		float		Determinant( void ) const;
		Mat3		Transpose( void ) const;
		void		TransposeSelf( void );
		Mat3		TransposeMultiply( const Mat3 &m ) const;
		Vec3		TransposeMultiply( const Vec3 &v ) const;
		Mat3		Inverse( void ) const;
		bool		InverseSelf( void );

		Quat		ToQuat( void ) const;

		// operators
		Mat3			operator-() const;

		const Vec3 &	operator[]( int index ) const;			// Get column vector
		Vec3 &			operator[]( int index );				// Get column vector

		bool			operator==( const Mat3 &other ) const;
		bool			operator!=( const Mat3 &other ) const;

		Mat3 &			operator=( const Mat3 &other );

		void			operator+=( const Mat3 &other );
		void			operator-=( const Mat3 &other );
		void			operator*=( float f );
		void			operator*=( const Mat3 &other );
		friend Vec3 &	operator*=( Vec3 &v, const Mat3 &m );
		void			operator/=( float f );

		Mat3			operator*( const Mat3 &other ) const;
		Vec3			operator*( const Vec3 &v ) const;
		Mat3			operator*( float f ) const;
		friend Mat3		operator*( float f, const Mat3 &m );
		friend Vec3		operator*( const Vec3 &vec, const Mat3 &m );

		Mat3			operator/( float f ) const;
		friend Mat3		operator/( float f, const Mat3 &m );

		Mat3			operator+( const Mat3 &other ) const;
		Mat3			operator-( const Mat3 &other ) const;

	private:
		Vec3			mat[3];
	};

	namespace c_mat3 {
		extern Mat3 zero;
		extern Mat3 identity;
	}
}

#endif
