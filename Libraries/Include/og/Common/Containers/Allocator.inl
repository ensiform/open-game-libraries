/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Dynamic Buffer
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

#ifndef __OG_ALLOCATOR_INL__
#define __OG_ALLOCATOR_INL__

namespace og {

/*
==============================================================================

  Allocator

==============================================================================
*/

/*
================
Allocator::Allocator
================
*/
template<class T>
OG_INLINE Allocator<T>::Allocator( int _granularity ) : granularity(_granularity) {
	CreateChunk();
}

/*
================
Allocator::~Allocator
================
*/
template<class T>
OG_INLINE Allocator<T>::~Allocator() {
	int num = allocationList.Num();
	for( int i=0; i<num; i++ )
		delete[] allocationList[i].list;
	allocationList.Clear();
}

/*
================
Allocator::Free
================
*/
template<class T>
OG_INLINE void Allocator<T>::CreateChunk( void ) {
	allocChunk_t &chunk = allocationList.Alloc();
	chunk.last = 0;
	chunk.list = new T[granularity];
}

/*
================
Allocator::Alloc
================
*/

template<class T>
OG_INLINE T *Allocator<T>::Alloc( void ) {
	allocChunk_t &chunk = allocationList[allocationList.Num()-1];
	T *ret = &chunk.list[chunk.last];
	chunk.last++;
	if ( chunk.last >= granularity )
		CreateChunk();
	return ret;
}

}

#endif
