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

#ifndef __OG_BOUNDS_H__
#define __OG_BOUNDS_H__

namespace og {
	class Vec3;
	const int MINS	= 0; // mins
	const int MAXS	= 1; // maxs

	/*
	==============================================================================

	  Bounds

	==============================================================================
	*/
	class Bounds {
	public:
		Bounds() { Zero(); }
		Bounds( const StringType &value ) { String::ToFloatArray( value, &bounds[0].x, 6 ); }
		explicit Bounds( float minsx, float minsy, float minsz, float maxsx, float maxsy, float maxsz ) { Set( minsx, minsy, minsz, maxsx, maxsy, maxsz ); }
		explicit Bounds( const Vec3 &mins, const Vec3 &maxs ) { Set( mins.x, mins.y, mins.z, maxs.x, maxs.y, maxs.z ); }
		explicit Bounds( const Vec3 &cpt ) { Set( cpt.x, cpt.y, cpt.z, cpt.x, cpt.y, cpt.z ); }
		Bounds( const Bounds &b ) { Set( b[MINS].x, b[MINS].y, b[MINS].z, b[MAXS].x, b[MAXS].y, b[MAXS].z ); }

		Vec3			bounds[2];

		void			Set( float minsx, float minsy, float minsz, float maxsx, float maxsy, float maxsz );
		void			Set( const Vec3 &mins, const Vec3 &maxs );
		void			Zero( void );
		bool			IsZero( void ) const;

		bool			Cmp( const Bounds &a ) const;
		bool			Cmp( const Bounds &a, float epsilon ) const;

		bool			ContainsPoint( const Vec3 &v ) const;
		bool			Intersects( const Bounds &b ) const;

		Vec3			GetSize( void ) const;
		Vec3			GetCenter( void ) const;
		float			GetVolume( void ) const;
		float			GetSurfaceArea( void ) const;

		Bounds			Translate( const Vec3 &t ) const;
		void			TranslateSelf( const Vec3 &t );

		Bounds			Expand( float f ) const;
		void			ExpandSelf( float f );

		const Vec3 &	operator[]( const int index ) const;
		Vec3 &			operator[]( const int index );

		bool			operator==(	const Bounds &a ) const;
		bool			operator!=(	const Bounds &a ) const;
	};
}

#endif
