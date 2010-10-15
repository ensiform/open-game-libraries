/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Hash Index ( Fast access to string based indices )
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

#ifndef __OG_HASHINDEX_INL__
#define __OG_HASHINDEX_INL__

namespace og {

/*
==============================================================================

  HashIndex

==============================================================================
*/

/*
================
HashIndex::First
================
*/
OG_INLINE int HashIndex::First( int hash ) const {
	if ( initialized ) {
		findNode = nodeList[(hash & mask)];
		if ( findNode )
			return findNode->index;
	}
	return -1;
}

/*
================
HashIndex::Next
================
*/
OG_INLINE int HashIndex::Next( void ) const {
	if ( initialized && findNode ) {
		findNode = findNode->next;
		if ( findNode )
			return findNode->index;
	}
	return -1;
}

/*
================
HashIndex::GenerateKey
================
*/
OG_INLINE int HashIndex::GenerateKey( const char *value, bool caseSensitive ) {
	return FNV32(value, caseSensitive);
}

}

#endif
