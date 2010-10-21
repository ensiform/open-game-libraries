// ==============================================================================
//! @file
//! @brief	Allocator
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

#ifndef __OG_ALLOCATOR_H__
#define __OG_ALLOCATOR_H__

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	// ==============================================================================
	//! Allocator
	//! 
	//! Allocates a list of objects instead of manually allocating each object
	// ==============================================================================
	template<class T>
	class Allocator {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	granularity	How many objects to allocate at once
		// ==============================================================================
		Allocator( int granularity=4096 );

		// ==============================================================================
		//! Destructor
		// ==============================================================================
		~Allocator();

		// ==============================================================================
		//! Allocate a new object
		//!
		//! @return	Pointer to the object
		//!
		//! Gets the first free object in the allocation chunk,
		//! and creates a new chunk if no free objects are left
		// ==============================================================================
		T *Alloc( void );

	private:

		// ==============================================================================
		//! Create/Append a new chunk
		// ==============================================================================
		void CreateChunk( void );

		// ==============================================================================
		//! Holds an array of T with the size of granularity
		// ==============================================================================
		struct allocChunk_t {
			T *	list;
			int	last;
		};
		List<allocChunk_t>allocationList;	//!< List of all chunks
		int granularity;					//!< The granularity
	};
//! @}
}

#endif
