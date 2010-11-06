/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Math namespace
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

#ifndef __OG_MATH_H__
#define __OG_MATH_H__

#include <og/Shared/Shared.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>

#ifdef INFINITY
	#undef INFINITY
#endif

//! @todo	Use simd instructions for vector, matrix, quat, angles, etc.. wherever it seems good.
//! @todo	Doxygen comments
//! @todo	Unit tests ?
namespace og {
	/*
	==============================================================================

	  Math

	==============================================================================
	*/
	namespace Math {
		int		Abs( int i );
		float	Fabs( float f );

		float	Round( float f );
		float	Floor( float f );
		float	Ceil( float f );

		int		Ftoi( float f );
		int		FtoiFast( float f );
		long	Ftol( float f );

		float	Sign( float f );

		float	Fmod( float numerator, float denominator );

		float	Modf( float f, float& i );
		float	Modf( float f );

		float	Sqrt( float f );
		float	InvSqrt( float f );
		float	RSqrt( float f );

		float	Log( float f );					// natural log
		float	Log2( float f );				// log base 2
		float	Log10( float f );				// common log (log base 10)
		float	Pow( float base, float exp );	// base raised to the power of exp
		float	Exp( float f );					// e raised to the power of f

		bool	IsPowerOfTwo( int x );
		int		HigherPowerOfTwo( int x );
		int		LowerPowerOfTwo( int x );
		int		FloorPowerOfTwo( int x );
		int		CeilPowerOfTwo( int x );
		int		ClosestPowerOfTwo( int x );
		int		Digits( int x );

		float	Sin( float f );
		float	ASin( float f );
		float	Cos( float f );
		float	ACos( float f );
		float	Tan( float f );
		float	ATan( float f );
		float	ATan( float f1, float f2 );
		void	SinCos( float f, float &s, float &c );

		float	Deg2Rad( float f );
		float	Rad2Deg( float f );

		float	Square( float v );
		float	Cube( float v );

		int		Sec2Ms( int sec );
		int		Ms2Sec( int ms );

		// Uses GCC Values where possible
		const float PI = 3.14159265358979323846f;
		const float TWO_PI = 6.28318530717958647692f;	// 2.0f*PI
		const float HALF_PI = 1.57079632679489661923f;	// 0.5f*PI
		const float E = 2.7182818284590452354f;
		const float DEG_TO_RAD = PI/180.0f;
		const float RAD_TO_DEG = 180.0f/PI;
		const float LN_2 = 0.69314718055994530942f;		// Log(2.0f)
		const float LN_10 = 2.30258509299404568402f;	// Log(10.0f)
		const float INV_LN_2 = 1.0f/LN_2;
		const float INV_LN_10 = 1.0f/LN_10;
		const float EPSILON = 1.192092896e-07f;
		const float INFINITY = 1e30f;
		const int	MASK_SIGNED = 0x7FFFFFFF;
	};
}

// Public Library Includes
#include "Random.h"
#include "Math.h"
#include "Vector.h"
#include "Rectangle.h"
#include "Matrix.h"
#include "Quat.h"
#include "Angles.h"
#include "Bounds.h"
#include "Color.h"

namespace og {
	Format &operator << ( Format &fmt, const Color &value );
	Format &operator << ( Format &fmt, const Vec2 &value );
	Format &operator << ( Format &fmt, const Vec3 &value );
	Format &operator << ( Format &fmt, const Vec4 &value );
	Format &operator << ( Format &fmt, const Angles &value );
	Format &operator << ( Format &fmt, const Rect &value );
	Format &operator << ( Format &fmt, const Quat &value );
	Format &operator << ( Format &fmt, const Mat2 &value );
	Format &operator << ( Format &fmt, const Mat3 &value );
}

// We include .inl files last, so we can access all classes here.
#include "Random.inl"
#include "Math.inl"
#include "Vector.inl"
#include "Rectangle.inl"
#include "Matrix.inl"
#include "Quat.inl"
#include "Angles.inl"
#include "Bounds.inl"
#include "Color.inl"

#endif
