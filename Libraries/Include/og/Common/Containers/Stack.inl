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
template<class T>
OG_INLINE void Stack<T>::Push( const T &element ) {
	stack.AddToEnd(element);
}

/*
============
Stack::Pop

Removes the last element in the stack
============
*/
template<class T>
OG_INLINE void Stack<T>::Pop( void ) {
	OG_ASSERT( !IsEmpty() );
	stack.Remove( stack.GetLastNode() );
}

/*
============
Stack::Top

Retrieves the last element in the stack
============
*/
template<class T>
OG_INLINE T& Stack<T>::Top( void ) {
	OG_ASSERT( !IsEmpty() );
	return stack.GetLastNode()->value;
}

/*
============
Stack::Clear

Clears the stack and the elements allocated.
============
*/
template<class T>
OG_INLINE void Stack<T>::Clear( void ) {
	stack.Clear();
}

/*
============
Stack::IsEmpty

Checks to see if stack is empty
============
*/
template<class T>
OG_INLINE bool Stack<T>::IsEmpty( void ) const {
	return stack.IsEmpty();
}

/*
============
Stack::Num

Number of used elements in stack
============
*/
template<class T>
OG_INLINE int Stack<T>::Num( void ) const {
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
template<class T>
OG_INLINE void Queue<T>::Push( const T &element ) {
	queue.AddToEnd(element);
}

/*
============
Queue::Pop

Removes the last element in the queue
============
*/
template<class T>
OG_INLINE void Queue<T>::Pop( void ) {
	OG_ASSERT( !IsEmpty() );
	queue.Remove( queue.GetFirstNode() );
}

/*
============
Queue::Front

Retrieves the first element in the queue
============
*/
template<class T>
OG_INLINE T& Queue<T>::Front( void ) {
	OG_ASSERT( !IsEmpty() );
	return queue.GetFirstNode()->value;
}

/*
============
Queue::Clear

Clears the queue and the elements allocated.
============
*/
template<class T>
OG_INLINE void Queue<T>::Clear( void ) {
	queue.Clear();
}

/*
============
Queue::IsEmpty

Checks to see if queue is empty
============
*/
template<class T>
OG_INLINE bool Queue<T>::IsEmpty( void ) const {
	return queue.IsEmpty();
}

/*
============
Queue::Num

Number of used elements in queue
============
*/
template<class T>
OG_INLINE int Queue<T>::Num( void ) const {
	return queue.Num();
}

}

#endif
