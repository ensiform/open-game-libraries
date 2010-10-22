/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Win32 Thread & Mutex classes, just until c++0x threads are ready to be used.
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

#if OG_WIN32

#include <windows.h>
#include <process.h>

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
	return TlsAlloc();
}

/*
================
ogTlsFree
================
*/
void ogTlsFree( uLong index ) {
	TlsFree( index );
}

/*
================
ogTlsGetValue
================
*/
void *ogTlsGetValue( uLong index ) {
	return TlsGetValue( index );
}

/*
================
ogTlsSetValue
================
*/
void ogTlsSetValue( uLong index, void *data ) {
	TlsSetValue( index, data );
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
	CRITICAL_SECTION *pCrit = new CRITICAL_SECTION;
	data = pCrit;
	OG_ASSERT( (int)data != 0xcdcdcdcd );
	::InitializeCriticalSection( pCrit );
}

/*
================
Mutex::~Mutex
================
*/
Mutex::~Mutex() {
	CRITICAL_SECTION *pCrit = static_cast<CRITICAL_SECTION *>(data);
	::DeleteCriticalSection( pCrit );
	delete pCrit;
}

/*
================
Mutex::Lock
================
*/
void Mutex::Lock( void ) {
	::EnterCriticalSection( static_cast<CRITICAL_SECTION *>(data) );
}

/*
================
Mutex::Unlock
================
*/
void Mutex::Unlock( void ) {
	::LeaveCriticalSection( static_cast<CRITICAL_SECTION *>(data) );
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
	data = ::CreateEvent( NULL, FALSE, FALSE, NULL );
}

/*
================
Condition::~Condition
================
*/
Condition::~Condition() {
	if ( data )
		::CloseHandle( data );
}

/*
================
Condition::Signal
================
*/
void Condition::Signal( void ) {
	SetEvent( data );
}

/*
================
Condition::Wait
================
*/
bool Condition::Wait( int ms ) {
	return ::WaitForSingleObject( data, ms ) == WAIT_OBJECT_0;
}


/*
==============================================================================

  SetThreadName

==============================================================================
*/
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
	id				= 0;
	handle			= 0;
	initEvent		= NULL;
	initResult		= false;
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

	handle = (HANDLE)::_beginthreadex( 0, 0, (unsigned (__stdcall *)(void *))RunThread, this, 0, &id );
	if( !handle ) {
		User::Error( ERR_SYSTEM_REQUIREMENTS, Format("Couldn't create thread '$*'") << name );
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
	_endthread();
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

		if ( blocking )
			WaitForSingleObject( handle, INFINITE );
	}
}

}

#endif
