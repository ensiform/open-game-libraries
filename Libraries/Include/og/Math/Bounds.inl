/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Ensiform
Purpose: Bounding Box class
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

#ifndef __OG_BOUNDS_INL__
#define __OG_BOUNDS_INL__

namespace og {

/*
==============================================================================

  Bounds

==============================================================================
*/

/*
================
Bounds::Set
================
*/
OG_INLINE void Bounds::Set( float minsx, float minsy, float minsz, float maxsx, float maxsy, float maxsz ) {
	bounds[MINS].Set( minsx, minsy, minsz );
	bounds[MAXS].Set( maxsx, maxsy, maxsz );
}

/*
================
Bounds::Set
================
*/
OG_INLINE void Bounds::Set( const Vec3 &mins, const Vec3 &maxs ) {
	bounds[MINS] = mins;
	bounds[MAXS] = maxs;
}

/*
================
Bounds::Zero
================
*/
OG_INLINE void Bounds::Zero( void ) {
	bounds[MINS].Zero();
	bounds[MAXS].Zero();
}

/*
================
Bounds::IsZero
================
*/
OG_INLINE bool Bounds::IsZero( void ) const {
	return (bounds[MINS].IsZero() && bounds[MAXS].IsZero());
}

/*
================
Bounds::Cmp
================
*/
OG_INLINE bool Bounds::Cmp( const Bounds &b ) const {
	return ( bounds[MINS].Cmp( b.bounds[MINS] ) && bounds[MAXS].Cmp( b.bounds[MAXS] ) );
}

/*
================
Bounds::Cmp
================
*/
OG_INLINE bool Bounds::Cmp( const Bounds &b, float epsilon ) const {
	return ( bounds[MINS].Cmp( b.bounds[MINS], epsilon ) && bounds[MAXS].Cmp( b.bounds[MAXS], epsilon ) );
}

/*
================
Bounds::ContainsPoint
================
*/
OG_INLINE bool Bounds::ContainsPoint( const Vec3 &v ) const {
	for( int i=0; i<3; i++) {
		if (v[i] < bounds[MINS][i] || v[i] > bounds[MAXS][i])
			return false;
	}
	return true;
}

/*
================
Bounds::Intersects
================
*/
OG_INLINE bool Bounds::Intersects( const Bounds &b ) const {
	for( int i=0; i<3; i++) {
		if (b.bounds[MAXS][i] < bounds[MINS][i] || b.bounds[MINS][i] > bounds[MAXS][i])
			return false;
	}
	return true;
}

/*
================
Bounds::GetSize
================
*/
OG_INLINE Vec3 Bounds::GetSize( void ) const {
	return ( bounds[MAXS] - bounds[MINS] );
}

/*
================
Bounds::GetCenter
================
*/
OG_INLINE Vec3 Bounds::GetCenter( void ) const {
	return ( GetSize() * 0.5f );
}

/*
================
Bounds::GetVolume

l*w*h
================
*/
OG_INLINE float Bounds::GetVolume( void ) const {
	// In space we don't have negative volumes so we should
	// just drop out to 0.0f if anything somehow would cause
	// this to return negative.
	if ( bounds[MINS].x >= bounds[MAXS].x || bounds[MINS].y >= bounds[MAXS].y || bounds[MINS].z >= bounds[MAXS].z ) {
		return 0.0f;
	}
	return ( ( bounds[MAXS].x - bounds[MINS].x ) * ( bounds[MAXS].y - bounds[MINS].y ) * ( bounds[MAXS].z - bounds[MINS].z ) );
}

/*
================
Bounds::GetSurfaceArea

http://en.wikipedia.org/wiki/Area_(geometry)#Surface_area_of_3-dimensional_figures

Rectangular Box: 2(lw + lh + wh)
================
*/
OG_INLINE float Bounds::GetSurfaceArea( void ) const {
	Vec3 s = GetSize();
	return 2.f * (s.x * s.y + s.x * s.z + s.y * s.z);
}

/*
================
Bounds::Translate
================
*/
OG_INLINE Bounds Bounds::Translate( const Vec3 &t ) const {
	Bounds b = *this;
	b.TranslateSelf(t);
	return b;
}

/*
================
Bounds::TranslateSelf
================
*/
OG_INLINE void Bounds::TranslateSelf( const Vec3 &t ) {
	bounds[MINS] += t;
	bounds[MAXS] += t;
}

/*
================
Bounds::Expand
================
*/
OG_INLINE Bounds Bounds::Expand( float f ) const {
	Bounds b = *this;
	b.ExpandSelf(f);
	return b;
}

/*
================
Bounds::ExpandSelf
================
*/
OG_INLINE void Bounds::ExpandSelf( float f ) {
	bounds[MINS].x -= f;
	bounds[MINS].y -= f;
	bounds[MINS].z -= f;

	bounds[MAXS].x += f;
	bounds[MAXS].y += f;
	bounds[MAXS].z += f;
}

/*
================
Bounds::operator[]
================
*/
OG_INLINE const Vec3 &Bounds::operator[]( const int index ) const {
	OG_ASSERT( index >= 0 && index < 2 );
	return bounds[index];
}
OG_INLINE Vec3 &Bounds::operator[]( const int index ) {
	OG_ASSERT( index >= 0 && index < 2 );
	return bounds[index];
}

/*
================
Bounds::operator==
================
*/
OG_INLINE bool Bounds::operator==( const Bounds &a ) const {
	return Cmp( a );
}

/*
================
Bounds::operator!=
================
*/
OG_INLINE bool Bounds::operator!=( const Bounds &a ) const {
	return !Cmp( a );
}

}

#endif
