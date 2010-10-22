/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  George Alysandratos (Muggen)
Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Posix Thread & Mutex classes, just until c++0x threads are ready to be used.
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

#include <og/Shared/Thread/Thread.h>
#include <og/Shared/Thread/ThreadLocalStorage.h>

#if OG_LINUX || OG_MACOS_X

#include <pthread.h>

namespace og {

/*
==============================================================================

  TLS Helpers

==============================================================================
*/

/*
================
ogTlsAlloc
================
*/
uLong ogTlsAlloc( void ) {
	pthread_key_t key;
	if ( pthread_key_create( &key, NULL ) == 0 )
		return static_cast<uLong>( key );
	return OG_TLS_OUT_OF_INDEXES;
}

/*
================
ogTlsFree
================
*/
void ogTlsFree( uLong index ) {
	pthread_key_delete( static_cast<pthread_key_t>(index) );
}

/*
================
ogTlsGetValue
================
*/
void *ogTlsGetValue( uLong index ) {
	return pthread_getspecific( static_cast<pthread_key_t>(index) );
}

/*
================
ogTlsSetValue
================
*/
void ogTlsSetValue( uLong index, void *data ) {
	pthread_setspecific( static_cast<pthread_key_t>(index), data );
}


/*
==============================================================================

  Mutex

==============================================================================
*/

/*
================
Mutex::Mutex
================
*/
Mutex::Mutex() {
    pthread_mutex_t *mutex = new pthread_mutex_t;
    data = mutex;
    pthread_mutex_init( mutex , NULL);
}

/*
================
Mutex::~Mutex
================
*/
Mutex::~Mutex() {
    pthread_mutex_t *mutex  = static_cast<pthread_mutex_t *>(data);
    pthread_mutex_destroy(mutex);
    delete mutex;
}

/*
================
Mutex::Lock
================
*/
void Mutex::Lock( void ) {
    pthread_mutex_lock( static_cast<pthread_mutex_t *>(data) );
}

/*
================
Mutex::Unlock
================
*/
void Mutex::Unlock( void ) {
    pthread_mutex_unlock( static_cast<pthread_mutex_t *>(data) );
}


/*
==============================================================================

  Condition

==============================================================================
*/

/*
================
Condition::Condition
================
*/
Condition::Condition() {
	pthread_cond_t * condition = new pthread_cond_t;
	pthread_cond_init (condition, NULL);
	data = condition;
}

/*
================
Condition::~Condition
================
*/
Condition::~Condition() {
	pthread_cond_t * condition = static_cast<pthread_cond_t *>(data);
	pthread_cond_destroy(condition);
	delete condition;
}

/*
================
Condition::Signal
================
*/
void Condition::Signal( void ) {
	pthread_cond_signal(static_cast<pthread_cond_t *>(data));
}

/*
================
Condition::Wait
================
*/
bool Condition::Wait( int ms ) {
	timespec temp;
	temp.tv_sec = 0;
	temp.tv_nsec = ms * 1000000; //1 Nanosec = 10^-6 mSec
	#warning ----FIXME----
	pthread_mutex_t mutex;
	if( ! pthread_cond_timedwait(static_cast<pthread_cond_t *>(data), &mutex, &temp))
		return true;
	else return false;
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
}

/*
================
Thread::Start
================
*/
bool Thread::Start( const char *name, bool waitForInit ) {
	if ( handle )
		return true;
	if ( waitForInit )
		initEvent = new Condition();

#warning "Need Linux here FIXME"
	// pthread_create?
	//handle = (HANDLE)::_beginthreadex( 0, 0, (unsigned (__stdcall *)(void *))RunThread, this, 0, &id );
	if( !handle ) {
		User::Error( ERR_SYSTEM_REQUIREMENTS, Format("Couldn't create thread '$*'") << name );
		return false;
	}

#warning "Need Linux here FIXME"
//	SetThreadName( name, id ); // prctl(PR_SET_NAME, name, 0, 0, 0, 0) ; ? ( max 16 bytes, needs to be set in the thread itself )

	if ( waitForInit ) {
		initEvent->Wait( OG_INFINITE );
		delete initEvent;
		initEvent = NULL;
		return initResult;
	}
	return true;
}

/*
================
Thread::RunThread
================
*/
void Thread::RunThread( void *param ) {
	Thread *thread = static_cast<Thread *>(param);

	thread->initResult = thread->Init();
	if ( thread->initEvent )
		thread->initEvent->Signal();

	thread->Run();

	if ( thread->selfDestruct )
		delete thread;
	else
		thread->handle = NULL;

	CleanupTLS();

#warning "Need Linux here FIXME"
	//_endthread(); // pthread_exit(NULL); ?
}

/*
================
Thread::Stop
================
*/
void Thread::Stop( bool blocking ) {
	if ( handle == NULL )
		delete this;
	else {
		selfDestruct = true;
		WakeUp();

#warning "Need Linux here FIXME"
		//if ( blocking )
		//	WaitForSingleObject( handle, INFINITE ); // pthread_join ?
	}
}

}

#endif
