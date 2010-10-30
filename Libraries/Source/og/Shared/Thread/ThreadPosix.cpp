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

  Thread

==============================================================================
*/

/*
================
Thread::PlatformInit
================
*/
void Thread::PlatformInit( void ) {
#if OG_LINUX
#warning "Need Linux here FIXME"
	// Get native id ?
	nativeId = 0;

	// To Muggen: you'll need to test this
	// Set thread name ( may only be max 16 bytes total including the termination )
	for( int i=15; name.ByteLength() > 15; i-- )
		name.CapLength( i );

	prctl(PR_SET_NAME, threadName.c_str(), 0, 0, 0, 0) ;
#elif OG_MACOS_X
#warning "Need MacOS here FIXME"
#endif
}

}

#endif
