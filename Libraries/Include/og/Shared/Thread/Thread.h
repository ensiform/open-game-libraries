/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Thread & Mutex classes, just until c++0x threads are ready to be used.
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

/*
Thread Safety Classes:
Unknown		= Needs to be verified
None		= Not thread safe (i.e. there is still some shared data between multiple objects
			  that will cause problems when using multiple threads, even if the objects are used in their threads only)
Multiple	= This object can be used from multiple threads at the same time
Single		= One object will not interfere with objects or shared data in other threads,
			  but can not be used by multiple threads at the same time without extra code.

If not specified otherwise, the object has thread safety class single.
*/

#ifndef __OG_THREAD_H__
#define __OG_THREAD_H__

#include <og/Shared/Shared.h>

namespace og {
	const int OG_INFINITE = 0xFFFFFFFF;

	// Mutex management (thread data protection)
	class Mutex {
	private:
		void *data;

	public:
		Mutex();
		~Mutex();
		void	Lock( void );
		void	Unlock( void );
	};

	class Condition {
	private:
		void *data;

	public:
		Condition();
		~Condition();
		void Signal( void );
		bool Wait( int ms );
	};

	class Waiter : public Condition {
	public:
		Waiter() : next(NULL) {}
		Waiter *next;
	};

	class SingleWriterMultiReader {
	private:
		Mutex	mtx;
		bool	writeRequest;
		int		readers;
		Waiter *firstWaiter;
		Waiter *lastWaiter;
		Condition	unlockedRead;

	public:
		SingleWriterMultiReader() : writeRequest(false), readers(0), firstWaiter(NULL), lastWaiter(NULL) {}

		void LockRead( void );
		void UnlockRead( void );
		void LockWrite( void );
		void UnlockWrite( void );
	};

	class Thread {
	public:
		Thread();
		virtual ~Thread() {}
		uInt GetId( void ) { return id; }
		bool IsRunning( void ) { return handle != NULL; }

		bool			Start( const char *name, bool waitForInit=false );
		virtual void	Stop( bool blocking=true );
		virtual void	WakeUp( void ) { wakeUpEvent.Signal(); }

	protected:
		bool		selfDestruct;
		Condition	wakeUpEvent;	// Time to wake up and do stuff
		void		*handle;
		uInt		id;

	private:
		Condition	*initEvent;
		bool		initResult;

	protected:
		virtual bool	Init( void ) { return true; }
		virtual void	Run( void ) = 0;
		static void		RunThread( void *param );
	};
}

#endif
