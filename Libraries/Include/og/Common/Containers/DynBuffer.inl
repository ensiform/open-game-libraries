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

#ifndef __OG_DYNBUFFER_INL__
#define __OG_DYNBUFFER_INL__

namespace og {

/*
==============================================================================

  DynBuffer

==============================================================================
*/

/*
================
DynBuffer::DynBuffer
================
*/
template< class type >
OG_INLINE DynBuffer<type>::DynBuffer( int _size ) {
	size = _size;
	if ( size > 0 )
		data = new type[size];
	else
		data = NULL;
}

/*
================
DynBuffer::~DynBuffer
================
*/
template< class type >
OG_INLINE DynBuffer<type>::~DynBuffer() {
	Clear();
}

/*
================
DynBuffer::Clear
================
*/
template< class type >
OG_INLINE void DynBuffer<type>::Clear( void ) {
	if ( data ) {
		delete[] data;
		data = NULL;
	}
	size = 0;
}

/*
================
DynBuffer::CheckSize
================
*/
template< class type >
OG_INLINE void DynBuffer<type>::CheckSize( int newSize ) {
	if ( newSize > size ) {
		if ( data )
			delete[] data;

		size = newSize;
		data = new type[size];
	}
}

}

#endif
