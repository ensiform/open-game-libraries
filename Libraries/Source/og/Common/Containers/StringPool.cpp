/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: String Pool ( Big list of strings so not too many duplicates
         of a string exist in memory )
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

#include <og/Common.h>

namespace og {

/*
==============================================================================

  StringPool

==============================================================================
*/

/*
================
StringPool::Clear
================
*/
void StringPool::Clear( void ) {
	strings.Clear();
	hashIndex.Clear();
}

/*
================
StringPool::Alloc
================
*/
const PoolString *StringPool::Alloc( const char *value ) {
	int hash = hashIndex.GenerateKey( value, caseSensitive );
	if ( caseSensitive ) {
		for ( int i=hashIndex.First( hash ); i!=-1; i=hashIndex.Next() ) {
			if ( strings[i].Cmp( value ) == 0 ) {
				strings[i].inUse++;
				return &strings[i];
			}
		}
	} else {
		for ( int i=hashIndex.First( hash ); i!=-1; i=hashIndex.Next() ) {
			if ( strings[i].Icmp( value ) == 0 ) {
				strings[i].inUse++;
				return &strings[i];
			}
		}
	}

	hashIndex.Add( hash, strings.Num() );
	PoolString &poolStr = strings.Alloc();
	poolStr.Init( this, value );
	return &poolStr;
}

/*
================
StringPool::Free
================
*/
void StringPool::Free( const PoolString *poolStr ) {
	OG_ASSERT( poolStr->inUse >= 1 );
	OG_ASSERT( poolStr->pool == this );

	poolStr->inUse--;
	if ( poolStr->inUse <= 0 ) {
		int hash = hashIndex.GenerateKey( poolStr->c_str(), caseSensitive );
		int i;
		if ( caseSensitive ) {
			for ( i=hashIndex.First( hash ); i!=-1; i = hashIndex.Next() ) {
				if ( strings[i].Cmp( poolStr->c_str() ) == 0 )
					break;
			}
		} else {
			for ( i=hashIndex.First( hash ); i!=-1; i=hashIndex.Next() ) {
				if ( strings[i].Icmp( poolStr->c_str() ) == 0 )
					break;
			}
		}

		OG_ASSERT( i != -1 );
		OG_ASSERT( &strings[i] == poolStr );
		strings.Remove( i );
		hashIndex.Remove( hash, i );
	}
}

/*
================
StringPool::Copy
================
*/
const PoolString *StringPool::Copy( const PoolString *poolStr ) {
	OG_ASSERT( poolStr->inUse >= 1 );

	if ( poolStr->pool != this )
		return Alloc( poolStr->c_str() );

	poolStr->inUse++;
	return poolStr;
}

}
