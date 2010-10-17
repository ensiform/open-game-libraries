/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Herb Sutter: http://www.drdobbs.com/high-performance-computing/210604448
Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Lock Free Queue
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

#ifndef __OG_LOCK_FREE_QUEUE_H__
#define __OG_LOCK_FREE_QUEUE_H__

#include <og/Shared/Thread/Thread.h>

#if OG_WIN32
	#include <windows.h>
#endif

namespace og {
	// use for single producer and single consumer only
	template <typename type>
	class LockFreeQueue {
	private:
		struct Node {
			Node( type *val ) : value(val), next(NULL) { }
			Node() : next(NULL) {}
			type *value;
			Node* next;
		};
		Node* first;
		Node *divider, *last;

	public:
		LockFreeQueue() { first = divider = last = new Node; }
		~LockFreeQueue() {
			while( first != NULL ) {
				Node* tmp = first;
				first = tmp->next;
				delete tmp;
			}
		}

		void Produce( type *t ) {
			last->next = new Node(t);
			InterlockedExchangePointer( &last, last->next );

			// remove consumed
			for( void *p = first; InterlockedCompareExchangePointer( &p, NULL, divider ) && p; p = first ) {
				Node* tmp = first;
				first = first->next;
				delete tmp;
			}
		}
		type *Consume( void ) {
			void *p = divider;
			InterlockedCompareExchangePointer( &p, NULL, last );

			if( p ) {
				type *result = divider->next->value;
				InterlockedExchangePointer( &divider, divider->next );
				return result;
			}
			return NULL;
		}
	};

	// multiple producer + multiple consumer
	template <typename type>
	class LowLockQueue {
	private:
		LockFreeQueue<type> queue;
		Mutex	producerLock;
		Mutex	consumerLock;

	public:
		void Produce( type *t ) {
			producerLock.Lock();
			queue.Produce( t );
			producerLock.Unlock();
		}
		type *Consume( void ) {
			consumerLock.Lock();
			type *ret = queue.Consume();
			consumerLock.Unlock();
			return ret;
		}
	};
}

#endif
