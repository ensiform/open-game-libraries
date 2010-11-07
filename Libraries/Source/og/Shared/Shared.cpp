/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
author	chongo <Landon Curt Noll> ( Fowler/Noll/Vo Hash algorithm )
see	http://www.isthe.com/chongo/tech/comp/fnv/
Purpose: Shared parts
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

#include <og/Shared.h>
#include <ctype.h>

#if OG_WIN32
	#include <windows.h>
#elif OG_LINUX
	#include <stdio.h>
	#include <unistd.h>
#elif OG_MACOS_X
	#warning "Need MacOS here FIXME"
#endif

namespace og {

/*
=================
Sleep
=================
*/
void Sleep( int msec ) {
#if OG_WIN32
	::Sleep(msec);
#else
	usleep(msec * 1000);
#endif
}

/*
=================
CreateErrorString
=================
*/
void CreateErrorString( ErrorId id, const char *msg, const char *param, String &result ) {
	// Todo: Throw an exception on the id's you think are important.
	switch( id ) {
		// Generic Errors:
		case ERR_SYSTEM_REQUIREMENTS:
			result = Format( "Your system is not suitable due to: $*." ) << msg;
			return;
		case ERR_BUFFER_OVERFLOW:
			result = Format( "Buffer overflow in $*, size:$*." ) << msg << param;
			return;
		case ERR_OUT_OF_MEMORY:
			result = Format( "'$*' returned NULL, size would be $*" ) << msg << param;
			return;

		// File Errors
		case ERR_BAD_FILE_FORMAT:
		case ERR_FILE_CORRUPT:
		case ERR_FILE_WRITEFAIL:
			result = Format( "$*: on file '$*'" ) << msg << param;
			return;

		// Lexer Problems
		case ERR_LEXER_WARNING:
			result = Format( "Lexer Warning('$*'): $*." ) << param << msg;
			return;
		case ERR_LEXER_FAILURE:
			result = Format( "Lexer Error('$*'): $*." ) << param << msg;
			return;

		// zLib Errors:
		case ERR_ZIP_CRC32:
			result = Format( "Crc32 corrupt for file: '$*' on zip: '$*'." ) << param << msg;
			return;

		// Filesystem Errors
		case ERR_FS_FILE_OPENREAD:
		case ERR_FS_FILE_OPENWRITE:
		case ERR_FS_MAKEPATH:
			result = Format( "$*: '$*'" ) << msg << param;
			return;

		// Console Errors
		case ERR_CVAR_INIT:
		case ERR_CVAR_LINK:
			result = Format( "$*: '$*'" ) << msg << param;
			return;

		// Audio Errors
		case ERR_AUDIO_INIT:
			result = Format( "Audio initialization failed: '$*'." ) << msg;
			return;
	}
	result = Format( "Unknown Error($*): '$*' : '$*'." ) << id << msg << (param ? param : "NULL");
}

/*
================
FNV32
================
*/
uInt FNV32( const char *value, bool caseSensitive ) {
	uInt hval = 0x811c9dc5;
	const byte *s = reinterpret_cast<const byte *>(value);
	if ( caseSensitive ) {
		while ( *s != '\0' ) {
			hval ^= static_cast<uInt>(*s++);
			hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
		}
	} else {
		while ( *s != '\0' ) {
			hval ^= static_cast<uInt>(tolower(*s++));
			hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
		}
	}
	return hval;
}

/*
================
FNV64
================
*/
uLongLong FNV64( const char *value, bool caseSensitive ) {
	uLongLong hval = ((uLongLong)0x84222325 << 32 | (uLongLong)0x1b3);
	const byte *s = reinterpret_cast<const byte *>(value);
	if ( caseSensitive ) {
		while ( *s != '\0' ) {
			hval ^= static_cast<uLongLong>(*s++);
			hval += (hval<<1) + (hval<<4) + (hval<<5) + (hval<<7) + (hval<<8) + (hval<<40);
		}
	} else {
		while ( *s != '\0' ) {
			hval ^= static_cast<uLongLong>(tolower(*s++));
			hval += (hval<<1) + (hval<<4) + (hval<<5) + (hval<<7) + (hval<<8) + (hval<<40);
		}
	}
	return hval;
}

#if OG_WIN32
#include <windows.h>
static wchar_t *SharedToWString( const char *string ) {
	uInt numBytes = String::ByteLength( string ) + 1;
	uInt size = Max( 1, String::ToWide( string, numBytes, NULL, 0 ) );
	wchar_t *buffer = new wchar_t[size];
	if ( size == 1 )
		buffer[0] = L'\0';
	else
		String::ToWide( string, numBytes, buffer, size );
	return buffer;
}
void MessageDialog( const char *message, const char *title ) {
	wchar_t *messageW = SharedToWString( message );
	wchar_t *titleW = SharedToWString( title );
	MessageBoxW( GetForegroundWindow(), messageW, titleW, MB_OK|MB_ICONWARNING );
	delete[] messageW;
	delete[] titleW;
}
void ErrorDialog( const char *message, const char *title ) {
	wchar_t *messageW = SharedToWString( message );
	wchar_t *titleW = SharedToWString( title );
	MessageBoxW( GetForegroundWindow(), messageW, titleW, MB_OK|MB_ICONERROR );
	delete[] messageW;
	delete[] titleW;
}
#elif OG_LINUX
#warning "Need Linux here FIXME"
void MessageDialog( const char *message, const char *title ) {
	//! @todo	is there a nice way to do a messagebox/alert dialog on linux ?
	printf( "%s\n", message );			//! @todo	does linux printf support direct UTF-8 ?
}
void ErrorDialog( const char *message, const char *title ) {
	fprintf( stderr, "%s\n", message );	//! @todo	does linux fprintf support direct UTF-8 ?
}
#elif OG_MACOS_X
#warning "Need MacOS here FIXME"
void MessageDialog( const char *message, const char *title ) {
	//! @todo	is there a nice way to do a messagebox/alert dialog on mac ?
	printf( "%s\n", message );			//! @todo	does mac printf support direct UTF-8 ?
}
void ErrorDialog( const char *message, const char *title ) {
	fprintf( stderr, "%s\n", message );	//! @todo	does mac fprintf support direct UTF-8 ?
}
#endif

}
