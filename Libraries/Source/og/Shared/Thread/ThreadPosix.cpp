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
==============================================================================

  TLS_Index

==============================================================================
*/
/*
================
TLS_Index::TLS_Index
================
*/
TLS_Index::TLS_Index() : data(NULL) {
	pthread_key_t key;
	int result = pthread_key_create( &key, NULL ) == 0;
	OG_ASSERT( result == 0 );

	if ( result == 0 )
		data = new pthread_key_t(key);
}

/*
================
TLS_Index::~TLS_Index
================
*/
TLS_Index::~TLS_Index() {
	if ( data ) {
		pthread_key_t *key = static_cast<pthread_key_t *>(data);
		pthread_key_delete( *key );
		delete key;
	}
}

/*
================
TLS_Index::GetValue
================
*/
void *TLS_Index::GetValue( void ) const {
	OG_ASSERT( data != NULL );
	return pthread_getspecific( *static_cast<pthread_key_t *>(data) );
}

/*
================
TLS_Index::SetValue
================
*/
void TLS_Index::SetValue( void *value ) const {
	OG_ASSERT( data != NULL );
	pthread_setspecific( *static_cast<pthread_key_t *>(data), data );
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
