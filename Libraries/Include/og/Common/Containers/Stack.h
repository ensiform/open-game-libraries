/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Ensiform & Santo Pfingsten (TTK-Bandit)
Purpose: Stacks and Queues
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

#ifndef __OG_STACK_H__
#define __OG_STACK_H__

namespace og {
	/*
	==============================================================================

	  Stack

	==============================================================================
	*/
	template< class type >
	class Stack {
	public:
		void		Push( const type& element );
		void		Pop( void );
		type&		Top( void );
		const type&	Top( void ) const;

		void		Clear( void );
		bool		IsEmpty( void ) const;

		int			Num( void ) const;

	private:
		LinkedList<type>	stack;
	};

	/*
	==============================================================================

	  Queue

	==============================================================================
	*/
	template< class type >
	class Queue {
	public:
		void		Push( const type& element );
		void		Pop( void );
		type&		Front( void );
		const type&	Front( void ) const;

		void		Clear( void );
		bool		IsEmpty( void ) const;

		int			Num( void ) const;

	private:
		LinkedList<type>	queue;
	};
}

#endif
