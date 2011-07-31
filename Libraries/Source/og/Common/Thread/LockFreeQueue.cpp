/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Lock Free Queue (wrapper class for the liblfds queue)
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

#include <og/Common/Thread/LockFreeQueue.h>

// Not exactly nice, but we can't include liblfds.h directly, causes problems
extern "C" {
  #if (defined _WIN64 && defined _MSC_VER && !defined WIN_KERNEL_BUILD)
    typedef unsigned __int64      atom_t;
  #endif

  #if (!defined _WIN64 && defined _WIN32 && defined _MSC_VER && !defined WIN_KERNEL_BUILD)
    typedef unsigned long int     atom_t;
  #endif

  #if (defined _WIN64 && defined _MSC_VER && defined WIN_KERNEL_BUILD)
    typedef unsigned __int64      atom_t;
  #endif

  #if (!defined _WIN64 && defined _WIN32 && defined _MSC_VER && defined WIN_KERNEL_BUILD)
    typedef unsigned long int     atom_t;
  #endif

  #if (defined __unix__ && defined __x86_64__ && __GNUC__)
    typedef unsigned long long int  atom_t;
  #endif

  #if (defined __unix__ && defined __i686__ && __GNUC__)
    typedef unsigned long int     atom_t;
  #endif

  #if (defined __unix__ && defined __arm__ && __GNUC__)
    typedef unsigned long int     atom_t;
  #endif
  struct queue_state;
  int queue_new( struct queue_state **sq, atom_t number_elements );
  void queue_delete( struct queue_state *qs, void (*user_data_delete_function)(void *user_data, void *user_state), void *user_state );

  int queue_enqueue( struct queue_state *qs, void *user_data );
  int queue_guaranteed_enqueue( struct queue_state *qs, void *user_data );
  int queue_dequeue( struct queue_state *qs, void **user_data );
}

namespace og {

LockFreeQueueVoid::LockFreeQueueVoid( int size ) {
	queue_new( &queueState, size );
}

LockFreeQueueVoid::~LockFreeQueueVoid() {
	queue_delete(queueState, NULL, NULL);
}

bool LockFreeQueueVoid::Produce( void *data ) {
	return queue_enqueue( queueState, data ) != 0;
}

bool LockFreeQueueVoid::ProduceSafe( void *data ) {
	return queue_guaranteed_enqueue( queueState, data ) != 0;
}

bool LockFreeQueueVoid::Consume( void **data ) {
	return queue_dequeue( queueState, data ) != 0;
}

}

