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

#include <og/Math/Math.h>

namespace og {

Format &operator << ( Format &fmt, const Vec2 &value ) {
	int precision = fmt.GetFloatPrecision();
	if ( precision >= 0 )
		fmt.TryPrint( "%.*f %.*f",
				precision, value.x, precision, value.y );
	else
		fmt.TryPrint( "%f %f",
				value.x, value.y );
	return fmt.Finish();
}

Format &operator << ( Format &fmt, const Vec3 &value ) {
	int precision = fmt.GetFloatPrecision();
	if ( precision >= 0 )
		fmt.TryPrint( "%.*f %.*f %.*f",
				precision, value.x, precision, value.y, precision, value.z );
	else
		fmt.TryPrint( "%f %f %f",
				value.x, value.y, value.z );
	return fmt.Finish();
}

Format &operator << ( Format &fmt, const Vec4 &value ) {
	int precision = fmt.GetFloatPrecision();
	if ( precision >= 0 )
		fmt.TryPrint( "%.*f %.*f %.*f %.*f",
				precision, value.x, precision, value.y, precision, value.z, precision, value.w );
	else
		fmt.TryPrint( "%f %f %f %f",
				value.x, value.y, value.z, value.w );
	return fmt.Finish();
}

Format &operator << ( Format &fmt, const Angles &value ) {
	int precision = fmt.GetFloatPrecision();
	if ( precision >= 0 )
		fmt.TryPrint( "%.*f %.*f %.*f",
				precision, value.pitch, precision, value.yaw, precision, value.roll );
	else
		fmt.TryPrint( "%f %f %f",
				value.pitch, value.yaw, value.roll );
	return fmt.Finish();
}

Format &operator << ( Format &fmt, const Rect &value ) {
	int precision = fmt.GetFloatPrecision();
	if ( precision >= 0 )
		fmt.TryPrint( "%.*f %.*f %.*f %.*f",
				precision, value.x, precision, value.y, precision, value.w, precision, value.h );
	else
		fmt.TryPrint( "%f %f %f %f",
				value.x, value.y, value.w, value.h );
	return fmt.Finish();
}

Format &operator << ( Format &fmt, const Quat &value ) {
	int precision = fmt.GetFloatPrecision();
	if ( precision >= 0 )
		fmt.TryPrint( "%.*f %.*f %.*f %.*f",
				precision, value.x, precision, value.y, precision, value.z, precision, value.w );
	else
		fmt.TryPrint( "%f %f %f %f",
				value.x, value.y, value.z, value.w );
	return fmt.Finish();
}

Format &operator << ( Format &fmt, const Mat2 &value ) {
	int precision = fmt.GetFloatPrecision();
	if ( precision >= 0 )
		fmt.TryPrint( "%.*f %.*f %.*f %.*f",
				precision, value[0].x, precision, value[0].y, precision, value[1].x, precision, value[1].y );
	else
		fmt.TryPrint( "%f %f %f %f",
				value[0].x, value[0].y, value[1].x, value[1].y );
	return fmt.Finish();
}

Format &operator << ( Format &fmt, const Mat3 &value ) {
	int precision = fmt.GetFloatPrecision();
	if ( precision >= 0 )
		fmt.TryPrint( "%.*f %.*f %.*f %.*f %.*f %.*f %.*f %.*f %.*f",
				precision, value[0].x, precision, value[0].y, precision, value[0].z,
				precision, value[1].x, precision, value[1].y, precision, value[1].z,
				precision, value[2].x, precision, value[2].y, precision, value[2].z );
	else
		fmt.TryPrint( "%f %f %f %f %f %f %f %f %f",
				value[0].x, value[0].y, value[0].z,
				value[1].x, value[1].y, value[1].z,
				value[2].x, value[2].y, value[2].z );
	return fmt.Finish();
}

}
