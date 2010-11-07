/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Storing Data in Variable Arrays
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
const byte	MASKBITS	= 0x3F;
const byte	MASK1BIT	= 0x80;
const byte	MASK2BIT	= 0xC0;
const byte	MASK3BIT	= 0xE0;
const byte	MASK4BIT	= 0xF0;
const byte	MASK5BIT	= 0xF8;
const byte	MASK6BIT	= 0xFC;
const byte	MASK7BIT	= 0xFE;

/*
==============================================================================

  StringList

==============================================================================
*/

/*
================
StringList::Find

Returns the index of the searched item.
-1 if no item with this value was found.
================
*/
int StringList::Find( const char *value ) const {
	for ( int i=0; i<num; i++ ) {
		if ( list[i]->Cmp(value) == 0 )
			return i;
	}
	return -1;
}

/*
================
StringList::IFind

Returns the index of the searched item.
-1 if no item with this value was found.
================
*/
int StringList::IFind( const char *value ) const {
	for ( int i=0; i<num; i++ ) {
		if ( list[i]->Icmp(value) == 0 )
			return i;
	}
	return -1;
}

/*
================
StringList::SplitString
================
*/
void StringList::SplitString( const char *text, const char *delimiter ) {
	if ( !text || !text[0] )
		return;
	int pos1 = 0;
	int pos2 = 0;
	int byteLen, len;

	do {
		pos2 = String::Find( text, delimiter, false, pos1 );
		if ( pos2 == String::INVALID_POSITION )
			Append( text + pos1 );
		else if ( pos2 == pos1 )
			Append( "" ); // same pos, so just add an empty string
		else {
			byteLen = pos2-pos1;
			// Count number of characters
			len = 0;
			for( int i=pos1; text[i] != '\0' && i<pos2; i++ ) {
				if( (text[i] & MASK2BIT) != MASK1BIT )
					len++;
			}
			Append( text+pos1, byteLen, len );
		}
		pos1 = pos2+1;
	} while ( pos2 != String::INVALID_POSITION );
}

}
