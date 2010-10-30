/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Thread helper classes, using boost just until c++0x threads are ready to be used.
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
#if OG_HAVE_STD_THREAD
	#include <thread>
	#include <condition_variable>
	#include <mutex>
	namespace ogst { using namespace std; }
#else
	#include <boost/thread/thread.hpp>
	#include <boost/thread/condition_variable.hpp>
	#include <boost/thread/mutex.hpp>
	#include <boost/thread/locks.hpp>
	namespace ogst { using namespace boost; }
#endif

namespace og {
	class Condition {
	private:
		ogst::mutex						mutex;
		ogst::condition_variable_any	condition;

	public:
		void	Lock( void ) {
			mutex.lock();
		}
		void	Unlock( void ) {
			mutex.unlock();
		}
		void	Wait( void ) {
			condition.wait( mutex );
		}
		void	Wait( int ms ) {
#if OG_HAVE_STD_THREAD
			condition.wait_for(mutex, ms);
#else
			// Crazy boost guys don't seem to like simple 'wait for x ms' type functions.
			int secs = 0;
			if ( ms >= 1000 ) {
				secs = ms / 1000;
				ms -= 1000 * secs;
			}
			int count = static_cast<int>(double(ms)* (double(boost::posix_time::time_duration::ticks_per_second())/double(1000.0)));
			boost::posix_time::ptime cur = boost::posix_time::microsec_clock::universal_time();
			condition.timed_wait(mutex, cur +  boost::posix_time::time_duration(0, 0, secs, count));
#endif
		}
		void	Signal( void ) {
			ogst::lock_guard<ogst::mutex> lock2(mutex);
			condition.notify_one();
		}
	};

	class SharedMutex {
	private:
		ogst::mutex	mutex;
		bool	exclusive;
		int		numShared, exclusiveRequests;
		ogst::condition_variable exclusiveCond;
		ogst::condition_variable sharedCond;

	public:
		SharedMutex() : exclusive(false), exclusiveRequests(0), numShared(0) {}

		void lock_shared( void );
		void unlock_shared( void );
		void lock( void );
		void unlock( void );
	};

	class SharedLock {
	public:
		SharedLock( SharedMutex &mtx ) : mutex(mtx) {
			mutex.lock_shared();
		}
		~SharedLock() {
			mutex.unlock_shared();
		}
		
	private:
		SharedMutex &mutex;
	};

	class Thread {
	public:
		Thread();
		virtual ~Thread() {}
		uInt GetNativeId( void ) { return nativeId; }
		bool IsRunning( void ) { return isRunning; }

		bool			Start( const char *name, bool waitForInit=false );
		virtual void	Stop( bool blocking=true );
		virtual void	WakeUp( void ) { wakeUpEvent.Signal(); }

	protected:
		uInt		nativeId;
		ogst::thread thread;
		bool		isRunning, keepRunning;
		Condition	wakeUpEvent;	// Time to wake up and do stuff

	private:
		String		name;
		bool		initResult;
		bool		selfDestruct;
		void		RunThread( Condition *initCondition );
		void		PlatformInit( void );

	protected:
		virtual bool	Init( void ) { return true; }
		virtual void	Run( void ) = 0;
	};
}

#endif
