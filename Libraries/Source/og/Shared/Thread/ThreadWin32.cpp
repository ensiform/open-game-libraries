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

  Thread

==============================================================================
*/

/*
================
Thread::PlatformInit
================
*/
void Thread::PlatformInit( void ) {
	// Get native id
	nativeId = GetThreadId( thread.native_handle() );

	// Set thread name
	struct tnData {
		DWORD	dwType;
		LPCSTR	szName;
		DWORD	dwThreadID;
		DWORD	dwFlags;
	};
	tnData data;
	data.dwType		= 0x1000;
	data.szName		= name.c_str();
	data.dwThreadID	= nativeId;
	data.dwFlags	= 0;
	__try { RaiseException( 0x406D1388, 0, sizeof(data)/sizeof(DWORD), (DWORD*)&data ); }
	__except( EXCEPTION_CONTINUE_EXECUTION ) { }
}


}

#endif
