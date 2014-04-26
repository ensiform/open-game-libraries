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

#include <og/Common/Thread/Thread.h>
#include <og/Common/Thread/ThreadLocalStorage.h>

namespace og {
TLS_Index * lastTlsIndex = OG_NULL;

/*
================
TLS_AtExit
================
*/
void TLS_AtExit( void ) {
	if ( lastTlsIndex ) {
		TLS_Data::Cleanup();
		delete lastTlsIndex;
		lastTlsIndex = OG_NULL;
	}
}

/*
==============================================================================

  TLS_Data

==============================================================================
*/
/*
================
TLS_Data::TLS_Data
================
*/
TLS_Data::TLS_Data( const TLS_Index *index ) : previous(OG_NULL), tlsIndex(index) {
	tlsIndex->SetValue(this);
	Register(this);
}

/*
================
TLS_Data::~TLS_Data
================
*/
TLS_Data::~TLS_Data() {
	tlsIndex->SetValue(OG_NULL);
	delete previous;
}

/*
================
TLS_Data::Register
================
*/
void TLS_Data::Register( TLS_Data *data ) {
	if ( !lastTlsIndex ) {
		lastTlsIndex = new TLS_Index;
		OG_ASSERT( lastTlsIndex->IsValid() );
		atexit( TLS_AtExit );
	}

	TLS_Data *lastTLS = static_cast<TLS_Data *>( lastTlsIndex->GetValue() );
	data->previous = lastTLS;
	lastTlsIndex->SetValue( data );
}

/*
================
TLS_Data::Cleanup
================
*/
void TLS_Data::Cleanup( void ) {
	if ( lastTlsIndex == OG_NULL )
		return;

	TLS_Data *lastTLS = static_cast<TLS_Data *>( lastTlsIndex->GetValue() );
	if ( lastTLS != OG_NULL ) {
		delete lastTLS;
#if OG_LINUX
	#error "SegFault Here - FixMe"
#endif
		lastTlsIndex->SetValue( OG_NULL );
	}
}

/*
==============================================================================

  SharedMutex

==============================================================================
*/
/*
================
SharedMutex::lock_shared
================
*/
void SharedMutex::lock_shared( void ) {
	ogst::unique_lock<ogst::mutex> lock(mutex);
	while( exclusiveRequests )
		sharedCond.wait(lock);
	numShared++;
}

/*
================
SharedMutex::unlock_shared
================
*/
void SharedMutex::unlock_shared( void ) {
	mutex.lock();
	numShared--;
	if ( exclusiveRequests && numShared == 0 )
		exclusiveCond.notify_one();
	mutex.unlock();
}

/*
================
SharedMutex::lock
================
*/
void SharedMutex::lock( void ) {
	ogst::unique_lock<ogst::mutex> lock(mutex);
	exclusiveRequests++;
	while( exclusive )
		exclusiveCond.wait(lock);
	exclusiveRequests--;
	exclusive = true;
	lock.release();
}

/*
================
SharedMutex::unlock
================
*/
void SharedMutex::unlock( void ) {
	if ( exclusiveRequests > 0 )
		exclusiveCond.notify_one();
	else
		sharedCond.notify_all();
	mutex.unlock();
}

/*
==============================================================================

  Thread

==============================================================================
*/

/*
================
Thread::Thread
================
*/
Thread::Thread() {
	selfDestruct	= false;
	initResult		= false;
	isRunning		= false;
	keepRunning		= true;
	nativeId		= 0;
}

/*
================
Thread::Start
================
*/
bool Thread::Start( const char *_name, bool waitForInit ) {
	name = _name;
	if ( !waitForInit ) {
		thread = ogst::thread( &Thread::RunThread, this, (Condition *)OG_NULL );
		return true;
	}
	Condition initCondition;
	initCondition.Lock();
	thread = ogst::thread( &Thread::RunThread, this, &initCondition );
	initCondition.Wait();
	initCondition.Unlock();
	return initResult;
}

/*
================
Thread::RunThread
================
*/
void Thread::RunThread( Condition *initCondition ) {
	isRunning = true;

	PlatformInit();

	initResult = Init();
	if ( initCondition )
		initCondition->Signal();

	Run();

	isRunning = false;

	TLS_Data::Cleanup();

	if ( selfDestruct )
		delete this;
	else
		isRunning = false;
}

/*
================
Thread::Stop
================
*/
void Thread::Stop( bool blocking ) {
	if ( !isRunning )
		delete this;
	else {
		keepRunning = false;
		selfDestruct = !blocking;
		WakeUp();

		if ( blocking ) {
			thread.join();
			delete this;
		}
	}
}

}

