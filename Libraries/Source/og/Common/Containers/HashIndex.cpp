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

#include <og/Common/Common.h>

namespace og {

/*
==============================================================================

  HashIndex

==============================================================================
*/

//! @todo	use og::Allocator ?

/*
================
HashIndex::Init
================
*/
void HashIndex::Init( void ) {
	OG_ASSERT( hashSize > 0 && Math::IsPowerOfTwo( hashSize ) );
	nodeList = new HashIndexNode_s *[hashSize];
	memset(nodeList, 0, sizeof(HashIndexNode_s *) * hashSize);
	initialized = true;

	findNode=NULL;
}

/*
================
HashIndex::Clear
================
*/
void HashIndex::Clear( int newHashSize ) {
	if ( initialized ) {
		HashIndexNode_s *node;
		HashIndexNode_s *nextNode;
		for( int i=0; i<hashSize; i++ ) {
			if ( nodeList[i] ) {
				node = nodeList[i];
				while ( node ) {
					nextNode = node->next;
					delete node;
					node = nextNode;
				}
			}
		}
		delete[] nodeList;
		nodeList = NULL;
		allnodes.Clear();
		findNode = NULL;

		initialized = false;
	}
	if ( newHashSize != -1 ) {
		OG_ASSERT( newHashSize > 0 && Math::IsPowerOfTwo( newHashSize ) );
		hashSize = newHashSize;
	}
}

/*
================
HashIndex::Add
================
*/
void HashIndex::Add( int hash, int index ) {
	if ( !initialized )
		Init();

	int nodeIndex = (hash & mask);
	HashIndexNode_s *node = nodeList[nodeIndex];
	if ( node ) {
		while ( node->next ) {
			if ( node->index == index )
				return;
			node = node->next;
		}
		if ( node->index == index )
			return;

		node->next = new HashIndexNode_s;
		node = node->next;
	}
	else {
		node = nodeList[nodeIndex] = new HashIndexNode_s;
	}
	node->next = NULL;
	node->index = index;
	allnodes.Append(node);
}

/*
================
HashIndex::Remove
================
*/
void HashIndex::Remove( int hash, int index ) {
	if ( !initialized )
		return;

	int nodeIndex = (hash & mask);
	HashIndexNode_s *lastNode = NULL;
	HashIndexNode_s *node = nodeList[nodeIndex];

	OG_ASSERT(node); // this must exist
	while ( node ) {
		if ( node->index == index ) {
			if ( lastNode )
				lastNode->next = node->next;
			else
				nodeList[nodeIndex] = node->next;

			int i = allnodes.Find(node);
			allnodes.Remove(i);

			// decrease all nodes indices that are above the deleted nodes index
			int num = allnodes.Num();
			for( int i=0; i<num; i++ ) {
				if ( allnodes[i]->index > index )
					allnodes[i]->index--;
			}

			delete node;
			return;
		}
		lastNode = node;
		node = node->next;
	}
}

}
