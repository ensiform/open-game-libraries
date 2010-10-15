/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Angles class
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

#ifndef __OG_ANGLES_H__
#define __OG_ANGLES_H__

namespace og {
	class Vec3;
	class Mat3;

	/*
	==============================================================================

	  Angles

	==============================================================================
	*/
	class Angles {
	public:
		enum { PITCH, YAW, ROLL }; // up/down, left/right, lean

		Angles() { Zero(); }
		Angles( const StringType &value ) { String::ToFloatArray( value, &pitch, 3 ); }
		explicit Angles( float pitch, float yaw, float roll ) { Set( pitch, yaw, roll ); }
		Angles( const Angles &ang ) { Set( ang[0], ang[1], ang[2] ); }

		float		pitch;
		float		yaw;
		float		roll;

		void		Set( float pitch, float yaw, float roll );
		void		Zero( void );
		bool		IsZero( void ) const;

		float		Length( void ) const;
		float		LengthFast( void ) const;
		float		LengthSqr( void ) const;

		void		Normalize360( void );
		void		Normalize180( void );

		void		CapLength( float length );

		bool		Cmp( const Angles &ang ) const;
		bool		Cmp( const Angles &ang, float epsilon ) const;

		void		ToVectors( Vec3 *forward, Vec3 *right = NULL, Vec3 *up = NULL ) const;
		Vec3		ToForward( void ) const;
		Mat3		ToMat3( void ) const;
		Quat		ToQuat( void ) const;

		// operators
		Angles			operator-() const;
		float			operator[]( int index ) const;
		float &			operator[]( int index );

		bool			operator==( const Angles &other ) const;
		bool			operator!=( const Angles &other ) const;

		Angles &		operator=( const Angles &ang );

		void			operator+=( const Angles &ang );
		void			operator-=( const Angles &ang );
		void			operator*=( const Angles &ang );
		void			operator*=( float f );
		void			operator/=( const Angles &ang );
		void			operator/=( float f );

		float			operator*( const Angles &ang ) const;
		Angles			operator*( float f ) const;
		friend Angles	operator*( float f, const Angles &ang );

		float			operator/( const Angles &ang ) const;
		Angles			operator/( float f ) const;

		Angles			operator+( const Angles &ang ) const;
		Angles			operator-( const Angles &ang ) const;
	};

	namespace c_angles {
		extern Angles zero;
		extern Angles pitch;
		extern Angles yaw;
		extern Angles roll;
		extern Angles one;
	}
}

#endif
