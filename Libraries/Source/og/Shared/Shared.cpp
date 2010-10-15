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

#include <og/Shared/Shared.h>

#ifdef OG_WIN32
	#include <windows.h>
#endif

namespace og {

/*
=================
Sleep
=================
*/
void Sleep( int msec ) {
#ifdef OG_WIN32
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
char *CreateErrorString( ErrorId id, const char *msg, const char *param ) {
	// Todo: Throw an exception on the id's you think are important.
	switch( id ) {
		// Generic Errors:
		case ERR_SYSTEM_REQUIREMENTS:
			return strdup( Format( "Your system is not suitable due to: $*." ) << msg );
		case ERR_BUFFER_OVERFLOW:
			return strdup( Format( "Buffer overflow in $*, size:$*." ) << msg << param );
		case ERR_OUT_OF_MEMORY:
			return strdup( Format( "'$*' returned NULL, size would be $*" ) << msg << param );

		// File Errors
		case ERR_BAD_FILE_FORMAT:
		case ERR_FILE_CORRUPT:
		case ERR_FILE_WRITEFAIL:
			return strdup( Format( "$*: on file '$*'" ) << msg << param );

		// Lexer Problems
		case ERR_LEXER_WARNING:
			return strdup( Format( "Lexer Warning('$*'): $*." ) << param << msg );
		case ERR_LEXER_FAILURE:
			return strdup( Format( "Lexer Error('$*'): $*." ) << param << msg );

		// zLib Errors:
		case ERR_ZIP_CRC32:
			return strdup( Format( "Crc32 corrupt for file: '$*' on zip: '$*'." ) << param << msg );

		// Filesystem Errors
		case ERR_FS_FILE_OPENREAD:
		case ERR_FS_FILE_OPENWRITE:
		case ERR_FS_MAKEPATH:
			return strdup( Format( "$*: '$*'" ) << msg << param );

		// Console Errors
		case ERR_CVAR_INIT:
		case ERR_CVAR_LINK:
			return strdup( Format( "$*: '$*'" ) << msg << param );

		// Audio Errors
		case ERR_AUDIO_INIT:
			return strdup( Format( "Audio initialization failed: '$*'." ) << msg );
	}
	return strdup( Format( "Unknown Error($*): '$*' : '$*'." ) << id << msg << (param ? param : "NULL") );
}

/*
=================
FreeErrorString
=================
*/
void FreeErrorString( char *str ) {
	free( str );
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
}
