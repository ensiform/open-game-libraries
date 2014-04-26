/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Ensiform
Purpose: Command Args for Console Commands
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

#include <og/Common.h>

namespace og {

/*
==============================================================================

  CmdArgs

==============================================================================
*/

/*
============
CmdArgs::Argv
============
*/
const char *CmdArgs::Argv( int i ) const {
	OG_ASSERT( i>=0 && i<num );
	return list[i]->c_str();
}

/*
============
CmdArgs::Args
============
*/
void CmdArgs::Args( String &result, int start, int end, bool escape ) const {
	if ( end < 0 || end >= num )
		end = num - 1;

	result.Clear();

	for ( int i=start; i<=end; i++ ) {
		if ( i > 0 ) {
			if ( i > start )
				result += " ";
			if ( escape )
				result += "\"";
		}
		result += list[i]->c_str();
		if ( i > 0 ) {
			if ( escape )
				result += "\"";
		}
	}
}

/*
============
CmdArgs::TokenizeString
============
*/
void CmdArgs::TokenizeString( const char *str ) {
	int bufPos = 0;
	Clear();

	if ( str == OG_NULL || *str == '\0' )
		return;

	char lookForQuote = 0;
	char tokBuffer[MAX_CMD_STRING] = {0};

	for( ; *str != '\0'; str++ ) {
		if ( lookForQuote ) {
			if ( *str == lookForQuote ) {
				tokBuffer[bufPos] = '\0';
				Append( tokBuffer );
				bufPos = 0;
				lookForQuote = 0;
				continue;
			}
			tokBuffer[bufPos++] = *str;
			continue;
		} else if ( String::IsSpace(*str) ) {
			// skip rest of whitespaces
			while( String::IsSpace(*(str+1)) ) str++;

			if (bufPos) {
				tokBuffer[bufPos] = '\0';
				Append( tokBuffer );
				bufPos = 0;
			}
		} else if ( *str == '\"' || *str == '\'' ) {
			lookForQuote = *str;
			if (bufPos) {
				tokBuffer[bufPos] = '\0';
				Append( tokBuffer );
				bufPos = 0;
			}
			continue;
		} else {
			tokBuffer[bufPos++] = *str;
			continue;
		}
	}
	if ( lookForQuote || bufPos ) {
		tokBuffer[bufPos] = '\0';
		Append( tokBuffer );
	}
}

}
