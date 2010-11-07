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

#include <og/Math.h>

namespace og {

/*
==============================================================================

  Quat

==============================================================================
*/

namespace c_quat {
	Quat quat_zero	( 0.0f, 0.0f, 0.0f, 0.0f );
	Quat quat_identity( 0.0f, 0.0f, 0.0f, 1.0f );
}

/*
================
Quat::Slerp
================
*/
void Quat::Slerp( const Quat &from, const Quat &to, float t ) {
	Quat q;
	float cosom = from.Dot( to );
	if( cosom >= 0.0f ) 
		q = from;
	else { 
		cosom = -cosom;
		q = -from;
	}

	float scale0, scale1;
	if( (1.0f - cosom) > Math::EPSILON) {
		float omega = Math::ACos( cosom );
		float sinom = 1.0f / Math::Sin( omega );
		scale0 = Math::Sin( (1.0f - t) * omega ) * sinom;
		scale1 = Math::Sin( t * omega ) * sinom;
	} else { 
		scale0 = 1.0f - t;
		scale1 = t;
	}

	x = scale0 * q.x + scale1 * to.x;
	y = scale0 * q.y + scale1 * to.y;
	z = scale0 * q.z + scale1 * to.z;
	w = scale0 * q.w + scale1 * to.w;
}

/*
================
Quat::Lerp
================
*/
void Quat::Lerp( const Quat &from, const Quat &to, float t ) {
	Quat q = ( from.Dot( to ) >= 0.0f ) ? from : -from;

	float t0 = 1.0f - t;
	x = t0 * q.x + t * to.x;
	y = t0 * q.y + t * to.y;
	z = t0 * q.z + t * to.z;
	w = t0 * q.w + t * to.w;
}

/*
================
Quat::ToMat3
================
*/
Mat3 Quat::ToMat3( void ) const {
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = x+x; y2 = y+y; z2 = z+z;
	xx = x * x2; xy = x * y2; xz = x * z2;
	yy = y * y2; yz = y * z2; zz = z * z2;
	wx = w * x2; wy = w * y2; wz = w * z2;

	return Mat3(	1.0f - yy - zz, xy - wz, xz + wy,
					xy + wz, 1.0f - xx - zz, yz - wx,
					xz - wy, yz + wx, 1.0f - xx - yy );
}

/*
================
Quat::ToVectors
================
*/
void Quat::ToVectors( Vec3 *forward, Vec3 *right, Vec3 *up ) const {
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = x+x; y2 = y+y; z2 = z+z;
	xx = x * x2; xy = x * y2; xz = x * z2;
	yy = y * y2; yz = y * z2; zz = z * z2;
	wx = w * x2; wy = w * y2; wz = w * z2;

	if ( forward )
		forward->Set( 1.0f - yy - zz, xy - wz, xz + wy );
	if ( right )
		right->Set( -(xy + wz), -(1.0f - xx - zz), -(yz - wx) );
	if ( up )
		up->Set( xz - wy, yz + wx, 1.0f - xx - yy );
}

/*
================
Quat::ToAngles
================
*/
Angles Quat::ToAngles( void ) const {
	float x2 = x+x;
	float y2 = y+y;
	float z2 = z+z;
	Angles angles( Math::Rad2Deg( -Math::ASin( x*z2 + w*y2 ) ), 0.0f, 0.0f );

	// Nasty gimbal lock
	if ( Math::Fabs(angles.pitch) > 89.944f )
		angles.yaw = Math::Rad2Deg( -Math::ATan( x*y2 + w*z2, 1.0f - x*x2 - z*z2 ) );
	else {
		float cv = 1.0f - y*y2;
		angles.yaw = Math::Rad2Deg( Math::ATan( x*y2 - w*z2, cv - z*z2 ) );
		angles.roll = Math::Rad2Deg( Math::ATan( y*z2 - w*x2, cv - x*x2 ) );
	}
	return angles;

}
/*
================
Quat::operator*
================
*/
Vec3 Quat::operator*( const Vec3 &v ) const {
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = x+x; y2 = y+y; z2 = z+z;
	xx = x * x2; xy = x * y2; xz = x * z2;
	yy = y * y2; yz = y * z2; zz = z * z2;
	wx = w * x2; wy = w * y2; wz = w * z2;

	return Vec3(	(1.0f - yy - zz) * v.x + (xy - wz) * v.y + (xz + wy) * v.z,
					(xy + wz) * v.x + (1.0f - xx - zz) * v.y + (yz - wx) * v.z,
					(xz - wy) * v.x + (yz + wx) * v.y + (1.0f - xx - yy) * v.z );
}

}
