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

#ifndef __OG_QUAT_H__
#define __OG_QUAT_H__

namespace og {
	/*
	==============================================================================

	  Quat

	==============================================================================
	*/
	class Quat {
	public:
		float		x;
		float		y;
		float		z;
		float		w;

		Quat();
		Quat( const StringType &value ) { String::ToFloatArray( value, &x, 4 ); }
		explicit Quat( float x, float y, float z, float w );
		explicit Quat( float qsrc[4] );

		void			Set( float x, float y, float z, float w );
		void			Zero( void );
		bool			IsZero( void ) const;
		void			Identity( void );

		void			Conjugate( void );
		float			Normalize( void );
		float			NormalizeFast( void );

#if 1 //! @todo	give this another look
		float			Inverse( void );
#else
		Quat			Inverse( void ) const;
		bool			InverseSelf( void );
#endif

		bool			Cmp( const Quat &other ) const;
		bool			Cmp( const Quat &other, float epsilon ) const;
		
		float			Dot( const Quat &other ) const;
		static float	Dot( const Quat &q1, const Quat &q2 );

		void			Slerp( const Quat &from, const Quat &to, float t );
		void			Lerp( const Quat &from, const Quat &to, float t );

		Mat3			ToMat3( void ) const;
		void			ToVectors( Vec3 *forward, Vec3 *right = OG_NULL, Vec3 *up = OG_NULL ) const;
		Angles			ToAngles( void ) const;

		// operators
		Quat			operator-() const;

		const float &	operator[]( int index ) const;
		float &			operator[]( int index );

		bool			operator==( const Quat &other ) const;
		bool			operator!=( const Quat &other ) const;

		Quat &			operator=( const Quat &other );

		void			operator+=( const Quat &other );
		void			operator-=( const Quat &other );
		void			operator*=( float f );
		void			operator*=( const Quat &other );

		Quat			operator*( const Quat &other ) const;
		Vec3			operator*( const Vec3 &v ) const;
		Quat			operator*( float f ) const;
		friend Quat		operator*( float f, const Quat &m );

		Quat			operator+( const Quat &other ) const;
		Quat			operator-( const Quat &other ) const;
	};

	namespace c_quat {
		extern Quat zero;
		extern Quat identity;
	}
}

#endif
