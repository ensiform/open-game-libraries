// ==============================================================================
//! @file
//! @brief	Pair Container
//! @author	Ensiform
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2007-2010 Lusito Software
// ==============================================================================
//
// The Open Game Libraries.
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// ==============================================================================

#ifndef __OG_PAIR_H__
#define __OG_PAIR_H__

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	// ==============================================================================
	//! Pair
	// ==============================================================================
	template<class T1, class T2>
	class Pair {
	public:
		// ==============================================================================
		//! Default constructor
		// ==============================================================================
		Pair() {}

		// ==============================================================================
		//! Constructor
		//!
		//! @param	T1	Initial value for the first element
		//! @param	T2	Initial value for the second element
		// ==============================================================================
		Pair( const T1& t1, const T2& t2 ) { first = t1; second = t2; }

		// ==============================================================================
		//! Copy the first and second element from the other object
		//!
		//! @param	other	The object to copy from
		//!
		//! @return	A reference to this object
		// ==============================================================================
		Pair<T1, T2> &	operator=( const Pair<T1, T2> &other );

		// ==============================================================================
		//! Compare the first and second element to the ones from the other object
		//!
		//! @param	other	The object to compare to
		//!
		//! @return	true if the first and second element of this object equal the ones from the other object
		// ==============================================================================
		bool			operator==( const Pair<T1, T2> &other );

		// ==============================================================================
		//! Compare the first and second element to the ones from the other object
		//!
		//! @param	other	The object to compare to
		//!
		//! @return	false if the first and second element of this object equal the ones from the other object
		// ==============================================================================
		bool			operator!=( const Pair<T1, T2> &other );

	public:
		T1				first;		//!< The first element
		T2				second;		//!< The second element
	};
//! @}
}

#endif
