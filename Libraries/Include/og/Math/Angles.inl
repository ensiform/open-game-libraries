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

#ifndef __OG_ANGLES_INL__
#define __OG_ANGLES_INL__

namespace og {

/*
==============================================================================

  Angles

==============================================================================
*/

/*
================
Angles::Set
================
*/
OG_INLINE void Angles::Set( float _x, float _y, float _z ) {
	pitch = _x;
	yaw = _y;
	roll = _z;
}

/*
================
Angles::Zero
================
*/
OG_INLINE void Angles::Zero( void ) {
	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
}

/*
================
Angles::IsZero
================
*/
OG_INLINE bool Angles::IsZero( void ) const {
	const uInt *iv = reinterpret_cast<const uInt *>(&pitch);
	return !(iv[0] & Math::MASK_SIGNED) && !(iv[1] & Math::MASK_SIGNED) && !(iv[2] & Math::MASK_SIGNED);
}

/*
================
Angles::Length
================
*/
OG_INLINE float Angles::Length( void ) const {
	return Math::Sqrt( LengthSqr() );
}

/*
================
Angles::LengthFast
================
*/
OG_INLINE float Angles::LengthFast( void ) const {
	float lenSqr = LengthSqr();
	return lenSqr * Math::RSqrt( lenSqr );
}

/*
================
Angles::LengthSqr
================
*/
OG_INLINE float Angles::LengthSqr( void ) const {
	return ( pitch*pitch + yaw*yaw + roll*roll );
}

/*
================
Angles::CapLength
================
*/
OG_INLINE void Angles::CapLength( float length ) {
	if ( length == 0 )
		Zero();
	else {
		float lenSqr = LengthSqr();
		if ( lenSqr > length*length ) {
			float lenInv =  length * Math::InvSqrt( lenSqr );

			pitch *= lenInv;
			yaw *= lenInv;
			roll *= lenInv;
		}
	}
}

/*
================
Angles::Cmp
================
*/
OG_INLINE bool Angles::Cmp( const Angles &ang ) const {
	return ( pitch == ang.pitch && yaw == ang.yaw && roll == ang.roll );
}

/*
================
Angles::Cmp
================
*/
OG_INLINE bool Angles::Cmp( const Angles &ang, float epsilon ) const {
	return ( Math::Fabs( pitch - ang.pitch ) <= epsilon && Math::Fabs( yaw - ang.yaw ) <= epsilon
			&& Math::Fabs( roll - ang.roll ) <= epsilon );
}

/*
=================
Angles::ToYaw
=================
*/
OG_INLINE float Angles::ToYaw( const float yaw, const float pitch ) {
	float newyaw;

	if ( yaw == 0 && pitch == 0 )
		newyaw = 0;
	else {
		if ( pitch )
			newyaw = ( Math::ATan( yaw, pitch ) * 180 / Math::PI );
		else if ( yaw > 0 )
			newyaw = 90;
		else
			newyaw = 270;

		if ( newyaw < 0 )
			newyaw += 360;
	}

	return newyaw;
}
OG_INLINE float Angles::ToYaw( const Angles &ang ) {
	return ToYaw( ang.yaw, ang.pitch );
}
OG_INLINE float Angles::ToYaw( void ) const {
	return ToYaw( yaw, pitch );
}

/*
================
Angles::operator-
================
*/
OG_INLINE Angles Angles::operator-() const {
	return Angles( -pitch, -yaw, -roll );
}

/*
================
Angles::operator[]
================
*/
OG_INLINE float Angles::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index < 3 );
	return (&pitch)[index];
}

/*
================
Angles::operator[]
================
*/
OG_INLINE float &Angles::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index < 3 );
	return (&pitch)[index];
}

/*
================
Angles::operator==
================
*/
OG_INLINE bool Angles::operator==( const Angles &other ) const {
	return Cmp(other);
}

/*
================
Angles::operator!=
================
*/
OG_INLINE bool Angles::operator!=( const Angles &other ) const {
	return !Cmp(other);
}

/*
================
Angles::operator=
================
*/
OG_INLINE Angles &Angles::operator=( const Angles &other ) {
	Set( other.pitch, other.yaw, other.roll );
	return *this;
}

/*
================
Angles::operator+=
================
*/
OG_INLINE void Angles::operator+=( const Angles &ang ) {
	pitch += ang.pitch;
	yaw += ang.yaw;
	roll += ang.roll;
}

/*
================
Angles::operator-=
================
*/
OG_INLINE void Angles::operator-=( const Angles &ang ) {
	pitch -= ang.pitch;
	yaw -= ang.yaw;
	roll -= ang.roll;
}

/*
================
Angles::operator*=
================
*/
OG_INLINE void Angles::operator*=( const Angles &ang ) {
	pitch *= ang.pitch;
	yaw *= ang.yaw;
	roll *= ang.roll;
}
OG_INLINE void Angles::operator*=( float f ) {
	pitch *= f;
	yaw *= f;
	roll *= f;
}

/*
================
Angles::operator/=
================
*/
OG_INLINE void Angles::operator/=( const Angles &ang ) {
	pitch /= ang.pitch;
	yaw /= ang.yaw;
	roll /= ang.roll;
}
OG_INLINE void Angles::operator/=( float f ) {
	float invf = 1.0f/f; // multiplication is faster than division, so we just reverse the process.
	pitch *= invf;
	yaw *= invf;
	roll *= invf;
}

/*
================
Angles::operator*
================
*/
OG_INLINE float Angles::operator*( const Angles &ang ) const {
	return (pitch * ang.pitch + yaw * ang.yaw + roll * ang.roll);
}
OG_INLINE Angles Angles::operator*( float f ) const {
	return Angles( pitch * f, yaw * f, roll * f );
}
OG_INLINE Angles operator*( float f, const Angles &ang ) {
	return Angles( ang.pitch * f, ang.yaw * f, ang.roll * f );
}

/*
================
Angles::operator/
================
*/
OG_INLINE float Angles::operator/( const Angles &ang ) const {
	return (pitch / ang.pitch + yaw / ang.yaw + roll / ang.roll);
}
OG_INLINE Angles Angles::operator/( float f ) const {
	float invf = 1.0f/f; // multiplication is faster than division, so we just reverse the process.
	return Angles( pitch * invf, yaw * invf, roll * invf );
}

/*
================
Angles::operator+
================
*/
OG_INLINE Angles Angles::operator+( const Angles &ang ) const {
	return Angles( pitch + ang.pitch, yaw + ang.yaw, roll + ang.roll );
}

/*
================
Angles::operator-
================
*/
OG_INLINE Angles Angles::operator-( const Angles &ang ) const {
	return Angles( pitch - ang.pitch, yaw - ang.yaw, roll - ang.roll );
}

}

#endif
