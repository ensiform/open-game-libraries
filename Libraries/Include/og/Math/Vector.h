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

#ifndef __OG_VECTOR_H__
#define __OG_VECTOR_H__

namespace og {
	class Angles;

	/*
	==============================================================================

	  Vec2

	==============================================================================
	*/
	class Vec2 {
	public:
		Vec2() { Zero(); }
		Vec2( const StringType &value ) { String::ToFloatArray( value, &x, 2 ); }
		explicit Vec2( float x, float y ) { Set( x, y ); }
		Vec2( const Vec2 &v ) { Set( v.x, v.y ); }

		float			x;
		float			y;

		void			Set( float x, float y );
		void			Zero( void );
		bool			IsZero( void ) const;

		float			Length( void ) const;
		float			LengthFast( void ) const;
		float			LengthSqr( void ) const;

		float			Normalize( void );
		float			NormalizeFast( void );

		Vec2			Snap( void ) const;
		void			SnapSelf( void );

		static Vec2		Mid( const Vec2 &v1, const Vec2 &v2 );
		Vec2			Mid( const Vec2 &other ) const;

		void			Floor( void );
		void			Ceil( void );

		void			CapLength( float length );

		bool			Cmp( const Vec2 &vec ) const;
		bool			Cmp( const Vec2 &vec, float epsilon ) const;

		float			ToAngle( void ) const;
		void			FromAngle( float angle );

		// operators
		Vec2			operator-() const;
		float			operator[]( int index ) const;
		float &			operator[]( int index );

		bool			operator==( const Vec2 &other ) const;
		bool			operator!=( const Vec2 &other ) const;

		Vec2 &			operator=( const Vec2 &vec );

		void			operator+=( const Vec2 &vec );
		void			operator-=( const Vec2 &vec );
		void			operator*=( const Vec2 &vec );
		void			operator*=( float f );
		void			operator/=( const Vec2 &vec );
		void			operator/=( float f );

		float			operator*( const Vec2 &vec ) const;
		Vec2			operator*( float f ) const;
		friend Vec2		operator*( float f, const Vec2 &vec );

		float			operator/( const Vec2 &vec ) const;
		Vec2			operator/( float f ) const;

		Vec2			operator+( const Vec2 &vec ) const;
		Vec2			operator-( const Vec2 &vec ) const;
	};

	namespace c_vec2 {
		extern Vec2 origin;
		extern Vec2 unitx;
		extern Vec2 unity;
		extern Vec2 one;
	}

	/*
	==============================================================================

	  Vec3

	==============================================================================
	*/
	class Vec3 {
	public:
		Vec3() { Zero(); }
		Vec3( const StringType &value ) { String::ToFloatArray( value, &x, 3 ); }
		explicit Vec3( float x, float y, float z ) { Set( x, y, z ); }
		Vec3( const Vec3 &v ) { Set( v.x, v.y, v.z ); }

		float			x;
		float			y;
		float			z;

		void			Set( float x, float y, float z );
		void			Zero( void );
		bool			IsZero( void ) const;

		float			Length( void ) const;
		float			LengthFast( void ) const;
		float			LengthSqr( void ) const;

		float			Normalize( void );
		float			NormalizeFast( void );

		Vec3			Snap( void ) const;
		void			SnapSelf( void );

		// operator % ?
		Vec3			Cross( const Vec3 &other ) const;
		static Vec3		Cross( const Vec3 &v1, const Vec3 &v2 );

		// operator & ?
		float			Dot( const Vec3 &other ) const;
		static float	Dot( const Vec3 &v1, const Vec3 &v2 );

		static Vec3		Mid( const Vec3 &v1, const Vec3 &v2 );
		Vec3			Mid( const Vec3 &other ) const;

		void			Floor( void );
		void			Ceil( void );

		void			CapLength( float length );

		bool			Cmp( const Vec3 &vec ) const;
		bool			Cmp( const Vec3 &vec, float epsilon ) const;

		Angles			ToAngles( void ) const;

		// operators
		Vec3			operator-() const;
		float			operator[]( int index ) const;
		float &			operator[]( int index );

		bool			operator==( const Vec3 &other ) const;
		bool			operator!=( const Vec3 &other ) const;

		Vec3 &			operator=( const Vec3 &vec );

		void			operator+=( const Vec3 &vec );
		void			operator-=( const Vec3 &vec );
		void			operator*=( const Vec3 &vec );
		void			operator*=( float f );
		void			operator/=( const Vec3 &vec );
		void			operator/=( float f );

		float			operator*( const Vec3 &vec ) const;
		Vec3			operator*( float f ) const;
		friend Vec3		operator*( float f, const Vec3 &vec );

		float			operator/( const Vec3 &vec ) const;
		Vec3			operator/( float f ) const;

		Vec3			operator+( const Vec3 &vec ) const;
		Vec3			operator-( const Vec3 &vec ) const;
	};

	namespace c_vec3 {
		extern Vec3 origin;
		extern Vec3 unitx;
		extern Vec3 unity;
		extern Vec3 unitz;
		extern Vec3 one;
		extern Vec3 up;
		extern Vec3 down;
	}

	namespace c_movedir {
		extern Vec3 up;
		extern Vec3 down;
	}

	/*
	==============================================================================

	  Vec4

	==============================================================================
	*/
	class Vec4 {
	public:
		Vec4() { Zero(); }
		Vec4( const StringType &value ) { String::ToFloatArray( value, &x, 4 ); }
		explicit Vec4( float x, float y, float z, float w ) { Set( x, y, z, w ); }
		Vec4( const Vec4 &v ) { Set( v.x, v.y, v.z, v.w ); }

		float			x;
		float			y;
		float			z;
		float			w;

		void			Set( float x, float y, float z, float w );
		void			Zero( void );
		bool			IsZero( void ) const;

		float			Length( void ) const;
		float			LengthFast( void ) const;
		float			LengthSqr( void ) const;

		float			Normalize( void );
		float			NormalizeFast( void );

		Vec4			Snap( void ) const;
		void			SnapSelf( void );

		static Vec4		Mid( const Vec4 &v1, const Vec4 &v2 );
		Vec4			Mid( const Vec4 &other ) const;

		void			Floor( void );
		void			Ceil( void );

		void			CapLength( float length );

		bool			Cmp( const Vec4 &vec ) const;
		bool			Cmp( const Vec4 &vec, float epsilon ) const;

		// operators
		Vec4			operator-() const;
		float			operator[]( int index ) const;
		float &			operator[]( int index );

		bool			operator==( const Vec4 &other ) const;
		bool			operator!=( const Vec4 &other ) const;

		Vec4 &			operator=( const Vec4 &vec );

		void			operator+=( const Vec4 &vec );
		void			operator-=( const Vec4 &vec );
		void			operator*=( const Vec4 &vec );
		void			operator*=( float f );
		void			operator/=( const Vec4 &vec );
		void			operator/=( float f );

		float			operator*( const Vec4 &vec ) const;
		Vec4			operator*( float f ) const;
		friend Vec4		operator*( float f, const Vec4 &vec );

		float			operator/( const Vec4 &vec ) const;
		Vec4			operator/( float f ) const;

		Vec4			operator+( const Vec4 &vec ) const;
		Vec4			operator-( const Vec4 &vec ) const;
	};

	namespace c_vec4 {
		extern Vec4 origin;
		extern Vec4 unitx;
		extern Vec4 unity;
		extern Vec4 unitz;
		extern Vec4 unitw;
		extern Vec4 one;
	}

	/*
	==============================================================================

	  Vec5

	==============================================================================
	*/
	class Vec5 {
	public:
		Vec5() { Zero(); }
		Vec5( const StringType &value ) { String::ToFloatArray( value, &x, 5 ); }
		explicit Vec5( float x, float y, float z, float s, float t ) { Set( x, y, z, s, t ); }
		Vec5( const Vec5 &v ) { Set( v.x, v.y, v.z, v.s, v.t ); }

		float			x;
		float			y;
		float			z;
		float			s;
		float			t;

		void			Set( float x, float y, float z, float s, float t );
		void			Set( const Vec3 &vec, const Vec2 &st );
		void			Zero( void );
		bool			IsZero( void ) const;

		bool			Cmp( const Vec5 &vec ) const;
		bool			Cmp( const Vec5 &vec, float epsilon ) const;

		// operators
		Vec5			operator-() const;
		float			operator[]( int index ) const;
		float &			operator[]( int index );

		bool			operator==( const Vec5 &other ) const;
		bool			operator!=( const Vec5 &other ) const;

		Vec5 &			operator=( const Vec5 &vec );
		Vec5 &			operator=( const Vec3 &vec );
	};

	namespace c_vec5 {
		extern Vec5 origin;
		extern Vec5 unitx;
		extern Vec5 unity;
		extern Vec5 unitz;
		extern Vec5 units;
		extern Vec5 unitt;
		extern Vec5 one;
	}

	/*
	==============================================================================

	  VecX

	==============================================================================
	*/
	template< int dim >
	class VecX {
	public:
		VecX() { Zero(); }
		VecX( const StringType &value ) { String::ToFloatArray( value, v, dim ); }
		VecX( const VecX<dim> &vec ) { Set(vec); }
		explicit VecX( float vec, ... );
		VecX( const float *vec ) { Set(vec); }

		float			v[dim];

		void			Set( const VecX<dim> &vec );
		void			Set( const float *vec );
		//void			Set( float v0, ... );
		void			Zero( void );
		bool			IsZero( void ) const;

		float			Length( void ) const;
		float			LengthFast( void ) const;
		float			LengthSqr( void ) const;

		float			Normalize( void );
		float			NormalizeFast( void );

		VecX<dim>		Snap( void ) const;
		void			SnapSelf( void );

		void			Floor( void );
		void			Ceil( void );

		void			CapLength( float length );

		bool			Cmp( const VecX<dim> &vec ) const;
		bool			Cmp( const VecX<dim> &vec, float epsilon ) const;

		// operators
		VecX<dim>		operator-() const;
		float			operator[]( int index ) const;
		float &			operator[]( int index );

		bool			operator==( const VecX<dim> &other ) const;
		bool			operator!=( const VecX<dim> &other ) const;

		VecX<dim> &		operator=( const VecX<dim> &vec );

		void			operator+=( const VecX<dim> &vec );
		void			operator-=( const VecX<dim> &vec );
		void			operator*=( const VecX<dim> &vec );
		void			operator*=( float f );
		void			operator/=( const VecX<dim> &vec );
		void			operator/=( float f );

		float			operator*( const VecX<dim> &vec ) const;
		VecX<dim>		operator*( float f ) const;
		template<int> friend VecX<dim> operator*( float f, const VecX<dim> &vec );

		float			operator/( const VecX<dim> &vec ) const;
		VecX<dim>		operator/( float f ) const;

		VecX<dim>		operator+( const VecX<dim> &vec ) const;
		VecX<dim>		operator-( const VecX<dim> &vec ) const;
	};
}

#endif
