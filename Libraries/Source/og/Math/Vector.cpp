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

#include <og/Math.h>

namespace og {

/*
==============================================================================

  Vec2

==============================================================================
*/

namespace c_vec2 {
	Vec2 origin( 0.0f, 0.0f );
	Vec2 unitx ( 1.0f, 0.0f );
	Vec2 unity ( 0.0f, 1.0f );
	Vec2 one   ( 1.0f, 1.0f );
}

/*
================
Vec2::ToAngle
================
*/
float Vec2::ToAngle( void ) const {
	if ( IsZero() )
		return 0.0f;
	
	float angle = Math::Rad2Deg( Math::ATan( y, x ) );
	if ( angle < 0.0f )
		angle += 360.0f;
	return angle;
}
/*
================
Vec2::FromAngle
================
*/
void Vec2::FromAngle( float angle ) {
	Math::SinCos( Math::Deg2Rad( angle ), y, x );
	Normalize();
}

/*
==============================================================================

  Vec3

==============================================================================
*/

namespace c_vec3 {
	Vec3 origin( 0.0f, 0.0f, 0.0f );
	Vec3 unitx ( 1.0f, 0.0f, 0.0f );
	Vec3 unity ( 0.0f, 1.0f, 0.0f );
	Vec3 unitz ( 0.0f, 0.0f, 1.0f );
	Vec3 one   ( 1.0f, 1.0f, 1.0f );
}

/*
================
Vec3::ToAngles
================
*/
Angles Vec3::ToAngles( void ) const {
	if ( x == 0.0f && y == 0.0f )
		return Angles( (z > 0.0f) ? 90.0f : 270.0f, 0.0f, 0.0f );
	else {
		float yaw = Math::Rad2Deg( Math::ATan( y, x ) );
		float pitch = Math::Rad2Deg( Math::ATan( z, Math::Sqrt( x*x + y*y ) ) );

		if ( yaw < 0.0f )
			yaw += 360.0f;
		if ( pitch < 0.0f )
			pitch += 360.0f;
		return Angles( pitch, yaw, 0.0f );
	}
}

/*
==============================================================================

  Vec4

==============================================================================
*/

namespace c_vec4 {
	Vec4 origin( 0.0f, 0.0f, 0.0f, 0.0f );
	Vec4 unitx ( 1.0f, 0.0f, 0.0f, 0.0f );
	Vec4 unity ( 0.0f, 1.0f, 0.0f, 0.0f );
	Vec4 unitz ( 0.0f, 0.0f, 1.0f, 0.0f );
	Vec4 unitw ( 0.0f, 0.0f, 0.0f, 1.0f );
	Vec4 one   ( 1.0f, 1.0f, 1.0f, 1.0f );
}

/*
==============================================================================

  Vec5

==============================================================================
*/

namespace c_vec5 {
	Vec5 origin( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
	Vec5 unitx ( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );
	Vec5 unity ( 0.0f, 1.0f, 0.0f, 0.0f, 0.0f );
	Vec5 unitz ( 0.0f, 0.0f, 1.0f, 0.0f, 0.0f );
	Vec5 units ( 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
	Vec5 unitt ( 0.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	Vec5 one   ( 1.0f, 1.0f, 1.0f, 1.0f, 1.0f );
}

/*
==============================================================================

  VecX

==============================================================================
*/

// Anything for VecX?  Maybe some typedefs to commonly used sized ones?

}
