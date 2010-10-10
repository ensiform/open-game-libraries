/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Thread Local Storage
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

#ifndef __OG_THREAD_LOCAL_STORAGE_H__
#define __OG_THREAD_LOCAL_STORAGE_H__


namespace og {
#if defined(OG_WIN32)
	#define OG_TLS_OUT_OF_INDEXES ((uLong)0xFFFFFFFF)

	uLong ogTlsAlloc( void );
	void ogTlsFree( uLong index );
	void *ogTlsGetValue( uLong index );
	void ogTlsSetValue( uLong index, void *data );
#endif

	struct TLS_Data {
		TLS_Data( uLong index ) : previous(NULL), tlsIndex(index) { ogTlsSetValue( index, this ); }
		virtual ~TLS_Data() { ogTlsSetValue( tlsIndex, NULL ); if ( previous ) delete previous; }
		TLS_Data *previous;
		uLong tlsIndex;
	};

	void RegisterTLS( TLS_Data *data );
	void CleanupTLS( void );

	template <typename type>
	class TLS {
	public:
		struct TLS_DataEx : public TLS_Data {
			TLS_DataEx( uLong index, type initialValue ) : TLS_Data(index), value(initialValue) {}
			type value;
		};
		TLS( type initial ) : initialValue(initial) {
			index = ogTlsAlloc();
			/*if ( index == OG_TLS_OUT_OF_INDEXES ) 
				User::Error("TlsAlloc failed");*/
		}
		TLS() {
			index = ogTlsAlloc();
			/*if ( index == OG_TLS_OUT_OF_INDEXES ) 
				User::Error("TlsAlloc failed");*/
		}
		~TLS() {
			CleanupTLS();
			if ( index != OG_TLS_OUT_OF_INDEXES )
				ogTlsFree( index );
		}
		type *Get( void ) const {
			OG_ASSERT( index != OG_TLS_OUT_OF_INDEXES );
			if ( index != OG_TLS_OUT_OF_INDEXES ) {
				TLS_DataEx *data = static_cast<TLS_DataEx *>( ogTlsGetValue( index ) );
				if ( data == NULL ) {
					data = new TLS_DataEx( index, initialValue );
					RegisterTLS( data );
				}
				return &data->value;
			}
			return NULL;
		}
		type *operator->( void ) const { return Get(); }
		type &operator*() const { return *Get(); }

	private:
		type	initialValue;
		uLong	index;
	};
}

#endif
