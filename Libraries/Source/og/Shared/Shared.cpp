/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
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
Shared::Sleep
=================
*/
void Shared::Sleep( int msec ) {
#ifdef OG_WIN32
	::Sleep(msec);
#else
	usleep(msec * 1000);
#endif
}

/*
=================
Shared::CreateErrorString
=================
*/
char *Shared::CreateErrorString( ErrorId id, const char *msg, const char *param ) {
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
Shared::FreeErrorString
=================
*/
void Shared::FreeErrorString( char *str ) {
	free( str );
}

}
