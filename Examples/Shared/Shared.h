/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Shared stuff amongst the demo apps
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

OG_INLINE void getErrorString( og::ErrorId id, const char *msg, const char *param, og::String &result ) {
	// Todo: Throw an exception on the id's you think are important.
	switch( id ) {
		// Generic Errors:
		case og::ERR_SYSTEM_REQUIREMENTS:
			result = og::TS( "Your system is not suitable due to: $*." ) << msg;
			return;
		case og::ERR_BUFFER_OVERFLOW:
			result = og::TS( "Buffer overflow in $*, size:$*." ) << msg << param;
			return;
		case og::ERR_OUT_OF_MEMORY:
			result = og::TS( "'$*' returned NULL, size would be $*" ) << msg << param;
			return;

		// File Errors
		case og::ERR_BAD_FILE_FORMAT:
		case og::ERR_FILE_CORRUPT:
		case og::ERR_FILE_WRITEFAIL:
			result = og::TS( "$*: on file '$*'" ) << msg << param;
			return;

		// Lexer Problems
		case og::ERR_LEXER_WARNING:
			result = og::TS( "Lexer Warning('$*'): $*." ) << param << msg;
			return;
		case og::ERR_LEXER_FAILURE:
			result = og::TS( "Lexer Error('$*'): $*." ) << param << msg;
			return;

		// zLib Errors:
		case og::ERR_ZIP_CRC32:
			result = og::TS( "Crc32 corrupt for file: '$*' on zip: '$*'." ) << param << msg;
			return;

		// Filesystem Errors
		case og::ERR_FS_FILE_OPENREAD:
		case og::ERR_FS_FILE_OPENWRITE:
		case og::ERR_FS_MAKEPATH:
			result = og::TS( "$*: '$*'" ) << msg << param;
			return;

		// Console Errors
		case og::ERR_CVAR_INIT:
		case og::ERR_CVAR_LINK:
			result = og::TS( "$*: '$*'" ) << msg << param;
			return;

		// Audio Errors
		case og::ERR_AUDIO_INIT:
			result = og::TS( "Audio initialization failed: '$*'." ) << msg;
			return;
	}
	result = "Unknown";
}