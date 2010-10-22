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

namespace og {
uLong lastTlsIndex = OG_TLS_OUT_OF_INDEXES;

/*
================
RegisterTLS
================
*/
void RegisterTLS( TLS_Data *data ) {
	if ( lastTlsIndex == OG_TLS_OUT_OF_INDEXES )
		lastTlsIndex = ogTlsAlloc();
	OG_ASSERT( lastTlsIndex != OG_TLS_OUT_OF_INDEXES );

	TLS_Data *lastTLS = static_cast<TLS_Data *>( ogTlsGetValue( lastTlsIndex ) );
	data->previous = lastTLS;
	ogTlsSetValue( lastTlsIndex, data );
}

/*
================
CleanupTLS
================
*/
void CleanupTLS( void ) {
	if ( lastTlsIndex == OG_TLS_OUT_OF_INDEXES )
		return;

	TLS_Data *lastTLS = static_cast<TLS_Data *>( ogTlsGetValue( lastTlsIndex ) );
	if ( lastTLS != NULL ) {
		delete lastTLS;
		ogTlsSetValue( lastTlsIndex, NULL );
	}
}

/*
================
SingleWriterMultiReader::LockRead
================
*/
void SingleWriterMultiReader::LockRead( void ) {
	mtx.Lock();
	if ( writeRequest ) {
		Waiter *waiter = new Waiter;
		if ( firstWaiter == NULL )
			firstWaiter = lastWaiter = waiter;
		else
			lastWaiter = lastWaiter->next = waiter;
		mtx.Unlock();
		waiter->Wait(OG_INFINITE);
		LockRead();
		return;
	}
	readers++;
	mtx.Unlock();
}

/*
================
SingleWriterMultiReader::UnlockRead
================
*/
void SingleWriterMultiReader::UnlockRead( void ) {
	mtx.Lock();
	readers--;
	if ( writeRequest )
		unlockedRead.Signal();
	mtx.Unlock();
}

/*
================
SingleWriterMultiReader::LockWrite
================
*/
void SingleWriterMultiReader::LockWrite( void ) {
	mtx.Lock();
	writeRequest = true;
	mtx.Unlock();
	while( readers > 0 )
		unlockedRead.Wait(200);

	mtx.Lock();
	writeRequest = false;
}

/*
================
SingleWriterMultiReader::UnlockWrite
================
*/
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

}

