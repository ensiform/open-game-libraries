// ==============================================================================
//! @file
//! @brief	Fowler/Noll/Vo Hash algorithm
//! @author	chongo <Landon Curt Noll>
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2007-2010 Lusito Software
//! @see	http://www.isthe.com/chongo/tech/comp/fnv/
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

#ifndef __OG_HASH_H__
#define __OG_HASH_H__

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

//! @defgroup CommonHash Hashing
//! @{

	//! FNV Hash
	namespace Hash {
		// ==============================================================================
		//! Hash string
		//!
		//! @param	value			The string to hash
		//! @param	caseSensitive	use case sensitive hashing
		//!
		//! @return	32 bit hash value
		// ==============================================================================
		uInt			Compute32( const char *value, bool caseSensitive );

		// ==============================================================================
		//! Hash string 64 bit
		//!
		//! @param	value			The string to hash
		//! @param	caseSensitive	use case sensitive hashing
		//!
		//! @return	64 bit hash value
		// ==============================================================================
		uLongLong		Compute64( const char *value, bool caseSensitive );
	}
//! @}
//! @}
}

#endif
