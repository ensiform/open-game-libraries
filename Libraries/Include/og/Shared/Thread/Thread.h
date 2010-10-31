// ==============================================================================
//! @file
//! @brief	Thread helper classes, using boost just until c++0x threads are ready to be used
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
	namespace ogst { 
		using namespace boost;
		using namespace ogst::this_thread;
	}
#endif

//! Open Game Libraries
namespace og {
//! @defgroup Shared Shared (Library)
//! @{

	// ==============================================================================
	//! Condition together with a mutex
	// ==============================================================================
	class Condition {
	private:
		ogst::mutex						mutex;		//!< The mutex
		ogst::condition_variable_any	condition;	//!< The condition

	public:
		// ==============================================================================
		//! Lock the mutex
		// ==============================================================================
		void	Lock( void ) {
			mutex.lock();
		}
		// ==============================================================================
		//! Unlock the mutex
		// ==============================================================================
		void	Unlock( void ) {
			mutex.unlock();
		}

		// ==============================================================================
		//! Wait for the condition signal
		// ==============================================================================
		void	Wait( void ) {
			condition.wait( mutex );
		}

		// ==============================================================================
		//! Wait x ms for the condition signal
		//!
		//! @param	ms	The milliseconds
		// ==============================================================================
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

		// ==============================================================================
		//! Lock the mutex and signal the condition ( notify_one )
		// ==============================================================================
		void	Signal( void ) {
			ogst::lock_guard<ogst::mutex> lock2(mutex);
			condition.notify_one();
		}
	};

	// ==============================================================================
	//! Shared mutex ( single writer multi reader )
	// ==============================================================================
	class SharedMutex {
	private:
		ogst::mutex	mutex;						//!< The mutex
		bool		exclusive;					//!< Exclusively locked
		int			exclusiveRequests;			//!< Number of exclusive locks requested
												//! (waiting for the shared locks to be done)
		int			numShared;					//!< Number of shared locks
		ogst::condition_variable exclusiveCond;	//!< The exclusive condition
		ogst::condition_variable sharedCond;	//!< The shared condition

	public:
		// ==============================================================================
		//! Default constructor
		// ==============================================================================
		SharedMutex() : exclusive(false), exclusiveRequests(0), numShared(0) {}

		// ==============================================================================
		//! Lock shared ( for reading )
		// ==============================================================================
		void lock_shared( void );

		// ==============================================================================
		//! Unlock shared ( for reading )
		// ==============================================================================
		void unlock_shared( void );

		// ==============================================================================
		//! Lock exclusively ( for writing )
		// ==============================================================================
		void lock( void );

		// ==============================================================================
		//! Unlock exclusively ( for writing )
		// ==============================================================================
		void unlock( void );
	};

	// ==============================================================================
	//! Auto locks/unlocks on construction/destruction
	// ==============================================================================
	class SharedLock {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	mtx	The mutex
		// ==============================================================================
		SharedLock( SharedMutex &mtx ) : mutex(mtx) {
			mutex.lock_shared();
		}

		// ==============================================================================
		//! Destructor
		// ==============================================================================
		~SharedLock() {
			mutex.unlock_shared();
		}
		
	private:
		SharedMutex &mutex;	//!< The mutex
	};

	// ==============================================================================
	//! Thread
	// ==============================================================================
	class Thread {
	public:
		// ==============================================================================
		//! Default constructor
		// ==============================================================================
		Thread();

		// ==============================================================================
		//! Virtual destructor
		// ==============================================================================
		virtual ~Thread() {}

		// ==============================================================================
		//! Get the native thread id
		//!
		//! @return	The native thread id
		// ==============================================================================
		#warning FixMe!! What if someone invokes GetNativeId before Start() happens ?
		uInt GetNativeId( void ) { return nativeId; }
		
		// ==============================================================================
		//! Get the result of Init()
		//!
		//! @param	result	Where to store the result
		//!
		//! @return	true if Init() has finished
		// ==============================================================================
		bool GetInitResult( bool &result ) {
			if ( initIsDone )
				result = initResult;
			return initIsDone;
		}

		// ==============================================================================
		//! Find out if the thread is still running
		//!
		//! @return	true if running, false if not
		// ==============================================================================
		bool IsRunning( void ) { return isRunning; }

		// ==============================================================================
		//! Starts the thread
		//!
		//! @param	name		The thread name ( for debugging purposes )
		//! @param	waitForInit	true to wait for Init() to finish
		//!
		//! @return	if waitForInit is true, this returns the result of Init(), otherwise true
		//!
		//! @note	Linux only supports 15 bytes long names, it will automaticly be truncated
		// ==============================================================================
		bool			Start( const char *name, bool waitForInit=false );

		// ==============================================================================
		//! Stop the thread
		//!
		//! @param	blocking	true to wait for the thread to be done
		//!
		//! @note	This will cause the Thread object to be deleted when it's done
		//! @note	Sets keepRunning to false and calls WakeUp()
		//!			the thread then needs to stop itself properly
		//! @note	Once done, the thread will cause all belonging TLS data to be freed
		// ==============================================================================
		virtual void	Stop( bool blocking=true );

		// ==============================================================================
		//! Wake up from hibernation to do handle new events ( signals wakeUpEvent )
		// ==============================================================================
		virtual void	WakeUp( void ) { wakeUpEvent.Signal(); }

	protected:
		uInt		nativeId;		//!< The native thread id
		ogst::thread thread;		//!< The thread object
		bool		isRunning;		//!< true if the thread is still running
		bool		keepRunning;	//!< when this is false, the thread needs to break out of its loop
		Condition	wakeUpEvent;	//!< Time to wake up and do stuff

	private:
		String		name;			//!< The thread name
		bool		initResult;		//!< The result of Init()
		bool		initIsDone;		//!< true when Init() has finished
		bool		selfDestruct;	//!< Set to true if it should selfdestruct when done

		// ==============================================================================
		//! Initialize the thread, run it and clean up afterwards
		//!
		//! @param	initCondition	If non-NULL, the init condition
		// ==============================================================================
		void		RunThread( Condition *initCondition );

		// ==============================================================================
		//! Platform initialization ( set thread name & get native id )
		// ==============================================================================
		void		PlatformInit( void );

	protected:
		// ==============================================================================
		//! User thread initialization
		//!
		//! @return	true if it succeeds, false if it fails 
		//!
		//! @note	This does not affect the call of Run().. maybe it should ?
		// ==============================================================================
		virtual bool	Init( void ) { return true; }

		// ==============================================================================
		//! The main function, called after Init()
		// ==============================================================================
		virtual void	Run( void ) = 0;
	};
	//! @}
}

#endif
