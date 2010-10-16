// ==============================================================================
//! @file
//! @brief	Stacks and Queues
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

#ifndef __OG_STACK_H__
#define __OG_STACK_H__

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

//! @defgroup CommonContainers Containers
//! @{

	// ==============================================================================
	//! Stack list
	// ==============================================================================
	template<class type>
	class Stack {
	public:
		// ==============================================================================
		//! Push an object onto the stack
		//!
		//! @param	element	The element to push onto the stack
		// ==============================================================================
		void		Push( const type& element );

		// ==============================================================================
		//! Remove the top entry of the stack
		// ==============================================================================
		void		Pop( void );

		// ==============================================================================
		//! Retrieve the last element in the stack
		//!
		//! @return	The top entry of the stack
		// ==============================================================================
		type&		Top( void );

		// ==============================================================================
		//! Remove all entries from the list
		// ==============================================================================
		void		Clear( void );

		// ==============================================================================
		//! Check if this list is empty
		//!
		//! @return	true if empty, false if not
		// ==============================================================================
		bool		IsEmpty( void ) const;

		// ==============================================================================
		//! Get the number of entries
		//!
		//! @return	The current number of entries
		// ==============================================================================
		int			Num( void ) const;

	private:
		LinkedList<type>	stack;
	};

	// ==============================================================================
	//! Queue list
	// ==============================================================================
	template<class type>
	class Queue {
	public:
		// ==============================================================================
		//! Push an object onto the queue
		//!
		//! @param	element	The element to push onto the queue
		// ==============================================================================
		void		Push( const type& element );
		
		// ==============================================================================
		//! Remove the front entry of the queue
		// ==============================================================================
		void		Pop( void );

		// ==============================================================================
		//! Retrieve the first element in the queue
		//!
		//! @return	The first entry of the queue
		// ==============================================================================
		type&		Front( void );

		// ==============================================================================
		//! Remove all entries from the list
		// ==============================================================================
		void		Clear( void );

		// ==============================================================================
		//! Check if this list is empty
		//!
		//! @return	true if empty, false if not
		// ==============================================================================
		bool		IsEmpty( void ) const;

		// ==============================================================================
		//! Get the number of entries
		//!
		//! @return	The current number of entries
		// ==============================================================================
		int			Num( void ) const;

	private:
		LinkedList<type>	queue;
	};
//! @}
//! @}
}

#endif
