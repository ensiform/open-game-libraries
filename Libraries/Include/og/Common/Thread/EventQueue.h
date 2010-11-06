// ==============================================================================
//! @file
//! @brief	Event Queue
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

#ifndef __OG_EVENT_QUEUE_H__
#define __OG_EVENT_QUEUE_H__

#include <og/Common/Thread/LockFreeQueue.h>

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	// ==============================================================================
	//! A queued event
	// ==============================================================================
	class QueuedEvent {
	public:
		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~QueuedEvent() {}

		// ==============================================================================
		//! Execute your event in here
		// ==============================================================================
		virtual void Execute( void ) = 0;
	};

	// ==============================================================================
	//! An event queue
	// ==============================================================================
	class EventQueue {
	public:
		// ==============================================================================
		//! Destructor
		// ==============================================================================
		~EventQueue() {
			DeleteAll();
		}

		// ==============================================================================
		//! Adds a new event to the queue
		//!
		//! @param	evt	The event to add
		// ==============================================================================
		void Add( QueuedEvent *evt ) {
			queue.Produce( evt );
		}

		// ==============================================================================
		//! Process all events ( calls Execute on the event )
		// ==============================================================================
		void ProcessAll( void ) {
			QueuedEvent *evt;
			while( (evt=queue.Consume()) != NULL ) {
				evt->Execute();
				delete evt;
			}
		}

		// ==============================================================================
		//! Delete all remaining events without calling Execute
		// ==============================================================================
		void DeleteAll( void ) {
			QueuedEvent *evt;
			while( (evt=queue.Consume()) != NULL )
				delete evt;
		}

	private:
		LowLockQueue<QueuedEvent> queue;	//!< The queue
	};
//! @}
}

#endif
