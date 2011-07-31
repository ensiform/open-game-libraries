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

#include <og/Common/Thread/Thread.h>

struct queue_state;

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	// ==============================================================================
	//! Lock free queue wrapper class, not to be used directly, as it's not type safe
	// ==============================================================================
	class LockFreeQueueVoid {
	public:
		LockFreeQueueVoid( int size );
		~LockFreeQueueVoid();

		bool Produce( void *data );
		bool ProduceSafe( void *data );
		bool Consume( void **data );

	private:
		struct queue_state *queueState;
	};

	// ==============================================================================
	//! Lock free queue for multi producer multi consumer scenarios
	// ==============================================================================
	template<typename type>
	class LockFreeQueue {
	public:
		// ==============================================================================
		//! Default constructor
		//!
		//! @param	size	The maximum number of elements which can be present in the queue.
		// ==============================================================================
		LockFreeQueue( int size=100 ) : queue(size) {}

		// ==============================================================================
		//! Add an entry to the queue
		//!
		//! @param	data	A pointer to the object
		// ==============================================================================
		bool Produce( type *data ) {
			return queue.ProduceSafe( (void *)data );
		}

		// ==============================================================================
		//! Get the next entry in the queue and remove it from the queue
		//!
		//! @return	NULL if the queue is empty, otherwise the next entry
		// ==============================================================================
		type *Consume( void ) {
			void *p;
			if( queue.Consume(&p) ) {
				return (type *)p;
			}
			return NULL;
		}
		
	private:
		LockFreeQueueVoid queue;	//!< The queue
	};

	//! @}
}

#endif
