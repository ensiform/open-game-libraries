/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit) & Ensiform
Purpose: Pair Container
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

#ifndef __OG_PAIR_INL__
#define __OG_PAIR_INL__

namespace og {

/*
==============================================================================

  Pair

==============================================================================
*/

/*
================
Pair::operator=
================
*/
template<class T1, class T2>
OG_INLINE Pair<T1, T2> &Pair<T1, T2>::operator=( const Pair<T1, T2> &other ) {
	first = other.first;
	second = other.second;
	return *this;
}

/*
================
Pair::operator==
================
*/
template<class T1, class T2>
OG_INLINE bool Pair<T1, T2>::operator==( const Pair<T1, T2> &other ) {
	return (first == other.first) && (second == other.second);
}

/*
================
Pair::operator!=
================
*/
template<class T1, class T2>
OG_INLINE bool Pair<T1, T2>::operator!=( const Pair<T1, T2> &other ) {
	return (first != other.first) || (second != other.second);
}

}

#endif
