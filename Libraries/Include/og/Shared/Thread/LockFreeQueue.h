// ==============================================================================
//! @file
//! @brief	Lock Free Queue and Low Lock Queue
//! @author	Herb Sutter: http://www.drdobbs.com/high-performance-computing/210604448
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


#ifndef __OG_LOCK_FREE_QUEUE_H__
#define __OG_LOCK_FREE_QUEUE_H__

#include <og/Shared/Thread/Thread.h>

//! Open Game Libraries
namespace og {
//! @defgroup Shared Shared (Library)
//! @{

	// ==============================================================================
	//! Atomic exchange pointer
	//!
	//! @param	target		The target
	//! @param	exchange	The exchange address
	//!
	//! @return	The initial address pointed to by target
	// ==============================================================================
	OG_INLINE void *AtomicExchangePointer( void **target, void *exchange ) {
		void *old;
#if OG_ASM_MSVC
		__asm {
			mov eax, exchange;
			mov edx, target;
			lock xchg [edx], eax;
			mov old, eax;
		}
#elif OG_ASM_GNU
		__asm__ __volatile__(
			"lock xchg %0,%1"
			: "=r" (old), "=m" (*target)
			:  "0" (exchange)
		);
#endif
	}

	// ==============================================================================
	//! Atomic compare exchange pointer
	//!
	//! @param	target		The target
	//! @param	exchange	The exchange address
	//! @param	comparand	The address to compare to target
	//!
	//! @return	The initial address pointed to by target
	// ==============================================================================
	OG_INLINE void *AtomicCompareExchangePointer( void **target, void *exchange, void *comparand ) {
		void *old;
#if OG_ASM_MSVC
		__asm {
			mov eax, comparand;
			mov ecx, exchange;
			mov edx, target;
			lock cmpxchg [edx], ecx;
			mov old, eax;
		}
#elif OG_ASM_GNU(
		__asm__ __volatile__(
			"lock cmpxchg %3,%1"
			: "=a" (old), "=m" (*(target))
			:  "0" (comparand), "r" (exchange)
		);
#endif
		return old;
	}

	// ==============================================================================
	//! Lock free queue for single producer single consumer scenarios
	// ==============================================================================
	template<typename type>
	class LockFreeQueue {
	private:
		// ==============================================================================
		//! A node
		// ==============================================================================
		struct Node {

			// ==============================================================================
			//! Constructor
			//!
			//! @param	val	The value
			// ==============================================================================
			Node( type *val ) : value(val), next(NULL) { }

			type *	value;	//!< The value ( NULL for the divider )
			Node *	next;	//!< The next node
		};
		Node *	first;		//!< The first node
		Node *	divider;	//!< The divider node
		Node *	last;		//!< The last node

	public:
		// ==============================================================================
		//! Default constructor ( create divider node and set first/last to the divider )
		// ==============================================================================
		LockFreeQueue() { first = divider = last = new Node(NULL); }

		// ==============================================================================
		//! Destructor ( delete all nodes )
		// ==============================================================================
		~LockFreeQueue() {
			while( first != NULL ) {
				Node* tmp = first;
				first = tmp->next;
				delete tmp;
			}
		}

		// ==============================================================================
		//! Add an entry to the queue
		//!
		//! @param	value	A pointer to the object
		// ==============================================================================
		void Produce( type *value ) {
			last->next = new Node(value);
			AtomicExchangePointer( (void **)&last, last->next );

			// remove consumed
			for( void *p = first; AtomicCompareExchangePointer( (void **)&p, NULL, divider ) && p; p = first ) {
				Node* tmp = first;
				first = first->next;
				delete tmp;
			}
		}

		// ==============================================================================
		//! Get the next entry in the queue and remove it from the queue
		//!
		//! @return	NULL if the queue is empty, otherwise the next entry
		// ==============================================================================
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

	// ==============================================================================
	//! Low lock queue for multi producer multi consumer scenarios
	// ==============================================================================
	template<typename type>
	class LowLockQueue {
	private:
		LockFreeQueue<type>	queue;			//!< The queue
		ogst::mutex			producerLock;	//!< The producer lock
		ogst::mutex			consumerLock;	//!< The consumer lock

	public:
		// ==============================================================================
		//! @copydoc LockFreeQueue::Produce
		// ==============================================================================
		void Produce( type *t ) {
			producerLock.lock();
			queue.Produce( t );
			producerLock.unlock();
		}

		// ==============================================================================
		//! @copydoc LockFreeQueue::Consume
		// ==============================================================================
		type *Consume( void ) {
			ogst::unique_lock<ogst::mutex> lock(consumerLock);
			return queue.Consume();
		}
	};
	//! @}
}

#endif
