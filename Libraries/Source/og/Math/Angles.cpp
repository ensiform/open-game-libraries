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

#include <og/Math/Math.h>

namespace og {

/*
==============================================================================

  Angles

==============================================================================
*/

namespace c_angles {
	Angles zero ( 0.0f, 0.0f, 0.0f );
	Angles pitch( 1.0f, 0.0f, 0.0f );
	Angles yaw  ( 0.0f, 1.0f, 0.0f );
	Angles roll ( 0.0f, 0.0f, 1.0f );
	Angles one  ( 1.0f, 1.0f, 1.0f );
}

const float NORMALIZE_CONST1 = 360.0f/65536.0f;
const float NORMALIZE_CONST2 = 65536.0f/360.0f;

/*
================
Angles::Normalize360
================
*/
void Angles::Normalize360( void ) {
	float *self = &pitch;
	for( int i=0; i<3; i++ )
		self[i] = NORMALIZE_CONST1 * (Math::Ftol(self[i] * NORMALIZE_CONST2) & 65535);
}

/*
================
Angles::Normalize180
================
*/
void Angles::Normalize180( void ) {
	float *self = &pitch;
	for( int i=0; i<3; i++ ) {
		self[i] = NORMALIZE_CONST1 * (Math::Ftol(self[i] * NORMALIZE_CONST2) & 65535);

		if (self[i] > 180.0f)
			self[i] -= 360.0f;
	}
}

/*
=================
Angles::ToVectors
=================
*/
void Angles::ToVectors( Vec3 *forward, Vec3 *right, Vec3 *up ) const {
	float sr, sp, sy, cr, cp, cy;

	Math::SinCos( Math::Deg2Rad( yaw ), sy, cy );
	Math::SinCos( Math::Deg2Rad( pitch ), sp, cp );
	Math::SinCos( Math::Deg2Rad( roll ), sr, cr );

	if ( forward )
		forward->Set( cp * cy, cp * sy, -sp );

	if ( right )
		right->Set( -sr * sp * cy + cr * sy, -sr * sp * sy + -cr * cy, -sr * cp );

	if ( up )
		up->Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );
}

/*
=================
Angles::ToForward
=================
*/
Vec3 Angles::ToForward( void ) const {
	float sp, sy, cp, cy;

	Math::SinCos( Math::Deg2Rad( yaw ), sy, cy );
	Math::SinCos( Math::Deg2Rad( pitch ), sp, cp );

	return Vec3( cp * cy, cp * sy, -sp );
}

/*
=================
Angles::ToMat3
=================
*/
Mat3 Angles::ToMat3( void ) const {
	Mat3 axis;
	ToVectors(&axis[0], &axis[1], &axis[2]);
	axis[1] *= -1; // inverse right to left
	return axis;
}

/*
=================
Angles::ToQuat
=================
*/
Quat Angles::ToQuat( void ) const {
	float sr, cr, sp, cp, sy, cy;
	Math::SinCos( Math::Deg2Rad( pitch ) * 0.5f, sp, cp );
	Math::SinCos( Math::Deg2Rad( roll ) * 0.5f, sr, cr );
	Math::SinCos( Math::Deg2Rad( yaw ) * 0.5f, sy, cy );

	float srcp = sr * cp;
	float crcp = cr * cp;
	float srsp = sr * sp;
	float crsp = cr * sp;

	return Quat( crsp*sy - srcp*cy, -crsp*cy - srcp*sy, srsp*cy - crcp*sy, crcp*cy + srsp*sy );
}

}
