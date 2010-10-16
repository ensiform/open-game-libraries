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

#include <og/Shared/Thread/Thread.h>
#include <og/Shared/Thread/ThreadLocalStorage.h>

#if defined(OG_WIN32)
	#include <windows.h>
	#include <process.h>
#else
	#include <pthread.h>
#endif

namespace og {
uLong lastTlsIndex = OG_TLS_OUT_OF_INDEXES;

void RegisterTLS( TLS_Data *data ) {
	if ( lastTlsIndex == OG_TLS_OUT_OF_INDEXES )
		lastTlsIndex = ogTlsAlloc();
	OG_ASSERT( lastTlsIndex != OG_TLS_OUT_OF_INDEXES );

	TLS_Data *lastTLS = static_cast<TLS_Data *>( ogTlsGetValue( lastTlsIndex ) );
	data->previous = lastTLS;
	ogTlsSetValue( lastTlsIndex, data );
}

void CleanupTLS( void ) {
	if ( lastTlsIndex == OG_TLS_OUT_OF_INDEXES )
		return;

	TLS_Data *lastTLS = static_cast<TLS_Data *>( ogTlsGetValue( lastTlsIndex ) );
	if ( lastTLS != NULL ) {
		delete lastTLS;
		ogTlsSetValue( lastTlsIndex, NULL );
	}
}

uLong ogTlsAlloc( void ) {
#ifdef OG_WIN32
	return TlsAlloc();
#else
	pthread_key_t key;
	if ( pthread_key_create( &key, NULL ) == 0 )
		return static_cast<uLong>( key );
	return OG_TLS_OUT_OF_INDEXES;
#endif
}

void ogTlsFree( uLong index ) {
#ifdef OG_WIN32
	TlsFree( index );
#else
	pthread_key_delete( static_cast<pthread_key_t>(index) );
#endif
}

void *ogTlsGetValue( uLong index ) {
#ifdef OG_WIN32
	return TlsGetValue( index );
#else
	return pthread_getspecific( static_cast<pthread_key_t>(index) )
#endif
}

void ogTlsSetValue( uLong index, void *data ) {
#ifdef OG_WIN32
	TlsSetValue( index, data );
#else
	pthread_setspecific( static_cast<pthread_key_t>(index), data );
#endif
}

#if defined(OG_WIN32)

Mutex::Mutex() {
	CRITICAL_SECTION *pCrit = new CRITICAL_SECTION;
	data = pCrit;
	OG_ASSERT( (int)data != 0xcdcdcdcd );
	::InitializeCriticalSection( pCrit );
}
Mutex::~Mutex() {
	CRITICAL_SECTION *pCrit = static_cast<CRITICAL_SECTION *>(data);
	::DeleteCriticalSection( pCrit );
	delete pCrit;
}
void Mutex::Lock( void ) {
	::EnterCriticalSection( static_cast<CRITICAL_SECTION *>(data) );
}
void Mutex::Unlock( void ) {
	::LeaveCriticalSection( static_cast<CRITICAL_SECTION *>(data) );
}

Condition::Condition() {
	data = ::CreateEvent( NULL, FALSE, FALSE, NULL );
}
Condition::~Condition() {
	if ( data )
		::CloseHandle( data );
}
void Condition::Signal( void ) {
	SetEvent( data );
}
bool Condition::Wait( int ms ) {
	return ::WaitForSingleObject( data, ms ) == WAIT_OBJECT_0;
}

class SetThreadName {
public:
	SetThreadName( LPCSTR name, DWORD id ) : dwType(0x1000), szName(name), dwThreadID(id), dwFlags(0) {
		__try { RaiseException( 0x406D1388, 0, sizeof(*this)/sizeof(DWORD), (DWORD*)this ); }
		__except( EXCEPTION_CONTINUE_EXECUTION ) { }
	}

private:
	DWORD	dwType;
	LPCSTR	szName;
	DWORD	dwThreadID;
	DWORD	dwFlags;
};

Thread::Thread() {
	selfDestruct	= false;
	id				= 0;
	handle			= 0;
	initEvent		= NULL;
	initResult		= false;
}

bool Thread::Start( const char *name, bool waitForInit ) {
	if ( handle )
		return true;
	if ( waitForInit )
		initEvent = new Condition();

	handle = (HANDLE)::_beginthreadex( 0, 0, (unsigned (__stdcall *)(void *))RunThread, this, 0, &id );
	if( !handle ) {
		User::Error( ERR_SYSTEM_REQUIREMENTS, "Couldn't create message thread" );
		return false;
	}

	SetThreadName( name, id );
	if ( waitForInit ) {
		initEvent->Wait( OG_INFINITE );
		delete initEvent;
		initEvent = NULL;
		return initResult;
	}
	return true;
}

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
	_endthread();
}

void Thread::Stop( bool blocking ) {
	if ( handle == NULL )
		delete this;
	else {
		selfDestruct = true;
		WakeUp();

		if ( blocking )
			WaitForSingleObject( handle, INFINITE );
	}
}

void SingleWriterMultiReader::LockRead( void ) {
	mtx.Lock();
	if ( writeRequest ) {
		Waiter *waiter = new Waiter;
		if ( firstWaiter == NULL )
			firstWaiter = lastWaiter = waiter;
		else
			lastWaiter = lastWaiter->next = waiter;
		mtx.Unlock();
		waiter->Wait(INFINITE);
		LockRead();
		return;
	}
	readers++;
	mtx.Unlock();
}
void SingleWriterMultiReader::UnlockRead( void ) {
	mtx.Lock();
	readers--;
	if ( writeRequest )
		unlockedRead.Signal();
	mtx.Unlock();
}
void SingleWriterMultiReader::LockWrite( void ) {
	mtx.Lock();
	writeRequest = true;
	mtx.Unlock();
	while( readers > 0 )
		unlockedRead.Wait(200);

	mtx.Lock();
	writeRequest = false;
}
void SingleWriterMultiReader::UnlockWrite( void ) {
	Waiter *temp;
	while( firstWaiter != NULL ) {
		firstWaiter->Signal();
		temp = firstWaiter;
		firstWaiter = firstWaiter->next;
		delete temp;
	}
	lastWaiter = NULL;
	mtx.Unlock();
}

#endif

}

