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

namespace og {
	OG_INLINE void AtomicExchangePointer( void **target, void *value ) {
#if OG_ASM_MSVC
		__asm {
			mov eax, value;
			mov edx, target;
			lock xchg [edx], eax;
		}
#elif OG_ASM_GNU
		void *old;
		__asm__ __volatile__(
			"lock xchg %0,%1"
			: "=r" old, "=m" (*target)
			:  "0" val
		);
#endif
	}
	OG_INLINE void *AtomicCompareExchangePointer( void **target, void *value, void *comparand ) {
		void *old = value;
#if OG_ASM_MSVC
		__asm {
			mov eax, comparand;
			mov ecx, value;
			mov edx, target;
			lock cmpxchg [edx], ecx;
		}
#elif OG_ASM_GNU(
		__asm__ __volatile__(
			"lock cmpxchg %3,%1"
			: "=a" old, "=m" (*target)
			:  "0" old, "r" comparand
		);
#endif
		return old;
	}


	// use for single producer and single consumer only
	template<typename type>
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
			AtomicExchangePointer( (void **)&last, last->next );

			// remove consumed
			for( void *p = first; AtomicCompareExchangePointer( (void **)&p, NULL, divider ) && p; p = first ) {
				Node* tmp = first;
				first = first->next;
				delete tmp;
			}
		}
		type *Consume( void ) {
			void *p = divider;
			AtomicCompareExchangePointer( (void **)&p, NULL, last );

			if( p ) {
				type *result = divider->next->value;
				AtomicExchangePointer( (void **)&divider, divider->next );
				return result;
			}
			return NULL;
		}
	};

	// multiple producer + multiple consumer
	template<typename type>
	class LowLockQueue {
	private:
		LockFreeQueue<type> queue;
		ogst::mutex	producerLock;
		ogst::mutex	consumerLock;

	public:
		void Produce( type *t ) {
			producerLock.lock();
			queue.Produce( t );
			producerLock.unlock();
		}
		type *Consume( void ) {
			ogst::unique_lock<ogst::mutex> lock(consumerLock);
			return queue.Consume();
		}
	};
}

#endif
