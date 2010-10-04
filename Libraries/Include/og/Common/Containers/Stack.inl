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

#ifndef __OG_STACK_INL__
#define __OG_STACK_INL__

namespace og {

/*
==============================================================================

  Stack

==============================================================================
*/

/*
============
Stack::Push

Allocates a new element on the stack
and sets it to element
============
*/
template< class type >
OG_INLINE void Stack<type>::Push( const type &element ) {
	stack.AddToEnd(element);
}

/*
============
Stack::Pop

Removes the last element in the stack
============
*/
template< class type >
OG_INLINE void Stack<type>::Pop( void ) {
	OG_ASSERT( !IsEmpty() );
	stack.Remove( stack.GetLastNode() );
}

/*
============
Stack::Top

Retrieves the last element in the stack
============
*/
template< class type >
OG_INLINE type& Stack<type>::Top( void ) {
	OG_ASSERT( !IsEmpty() );
	return stack.GetLastNode()->value;
}

/*
============
Stack::Top

Retrieves the last element in the stack
============
*/
template< class type >
OG_INLINE const type& Stack<type>::Top( void ) const {
	OG_ASSERT( !IsEmpty() );
	return stack.GetLastNode()->value;
}

/*
============
Stack::Clear

Clears the stack and the elements allocated.
============
*/
template< class type >
OG_INLINE void Stack<type>::Clear( void ) {
	stack.Clear();
}

/*
============
Stack::IsEmpty

Checks to see if stack is empty
============
*/
template< class type >
OG_INLINE bool Stack<type>::IsEmpty( void ) const {
	return stack.IsEmpty();
}

/*
============
Stack::Num

Number of used elements in stack
============
*/
template< class type >
OG_INLINE int Stack<type>::Num( void ) const {
	return stack.Num();
}

/*
==============================================================================

  Queue

==============================================================================
*/

/*
============
Queue::Push

Allocates a new element on the queue
and sets it to element
============
*/
template< class type >
OG_INLINE void Queue<type>::Push( const type &element ) {
	queue.AddToEnd(element);
}

/*
============
Queue::Pop

Removes the last element in the queue
============
*/
template< class type >
OG_INLINE void Queue<type>::Pop( void ) {
	OG_ASSERT( !IsEmpty() );
	queue.Remove( queue.GetFirstNode() );
}

/*
============
Queue::Front

Retrieves the first element in the queue
============
*/
template< class type >
OG_INLINE type& Queue<type>::Front( void ) {
	OG_ASSERT( !IsEmpty() );
	return queue.GetFirstNode()->value;
}

/*
============
Queue::Front

Retrieves the first element in the queue
============
*/
template< class type >
OG_INLINE const type& Queue<type>::Front( void ) const {
	OG_ASSERT( !IsEmpty() );
	return queue.GetFirstNode()->value;
}

/*
============
Queue::Clear

Clears the queue and the elements allocated.
============
*/
template< class type >
OG_INLINE void Queue<type>::Clear( void ) {
	queue.Clear();
}

/*
============
Queue::IsEmpty

Checks to see if queue is empty
============
*/
template< class type >
OG_INLINE bool Queue<type>::IsEmpty( void ) const {
	return queue.IsEmpty();
}

/*
============
Queue::Num

Number of used elements in queue
============
*/
template< class type >
OG_INLINE int Queue<type>::Num( void ) const {
	return queue.Num();
}

}

#endif
