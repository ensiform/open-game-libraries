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

#include <og/Common/Thread/Thread.h>
#include <og/Common/Thread/ThreadLocalStorage.h>

#if OG_WIN32
#include <windows.h>

namespace og {

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
TLS_Index::TLS_Index() : data(OG_NULL) {
	uLong index = TlsAlloc();
	OG_ASSERT( index != TLS_OUT_OF_INDEXES );

	if ( index != TLS_OUT_OF_INDEXES )
		data = new uLong(index);
}

/*
================
TLS_Index::~TLS_Index
================
*/
TLS_Index::~TLS_Index() {
	if ( data ) {
		uLong *index = static_cast<uLong *>(data);
		TlsFree( *index );
		delete index;
	}
}

/*
================
TLS_Index::GetValue
================
*/
void *TLS_Index::GetValue( void ) const {
	OG_ASSERT( data != OG_NULL );
	return TlsGetValue( *static_cast<uLong *>(data) );
}

/*
================
TLS_Index::SetValue
================
*/
void TLS_Index::SetValue( void *value ) const {
	OG_ASSERT( data != OG_NULL );
	TlsSetValue( *static_cast<uLong *>(data), value );
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
	nativeId = GetCurrentThreadId();

	//! @todo   Not sure if this is microsoft specific or if just mingw needs something different
#ifdef __MINGW32__
	#warning "Need MinGW here FIXME"
#else
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
#endif
}


}

#endif
