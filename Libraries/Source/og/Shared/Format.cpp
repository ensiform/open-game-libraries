/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Text Formatting
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

#include <og/Common/Common.h>
#include <stdarg.h>

namespace og {

/*
================
vsnPrintf
================
*/
int vsnPrintf( char *dest, int size, const char *fmt, va_list list ) {
	int ret = vsnprintf(dest, size, fmt, list);
	dest[size-1] = '\0';
	return (ret >= 0 && ret < size) ? ret : -1;
}

/*
================
Format::Format
================
*/
Format::Format( const char *fmt ) {
	firstFormatEntry = NULL;
	lastFormatEntry = NULL;
	numFormatEntries = 0;
	Reset( false, fmt );
}
/*
================
Format::~Format
================
*/
Format::~Format() {
	DeleteFormatEntries();
}

/*
================
Format::DeleteFormatEntries
================
*/
void Format::DeleteFormatEntries( void ){
	FormatEntry *temp;
	while( firstFormatEntry ) {
		temp = firstFormatEntry;
		firstFormatEntry = firstFormatEntry->next;
		delete temp;
	}
	numFormatEntries = 0;
}

/*
================
Format::Reset
================
*/
void Format::Reset( bool keep, const char *fmt ) {
	floatPrecision = -1;
	fillChar = ' ';

	if ( keep ) {
		if ( !hasFormat ) {
			offset = 0;
			paramCount = -1;
		} else {
			offset = strlen( fmtBuffer );
			memcpy( buffer, fmtBuffer, offset );
			if ( paramCount > 0 )
				paramCount = 0;

			FormatEntry *entry = firstFormatEntry;
			while( entry ) {
				if ( entry->takeInput == 1 )
					entry->takeInput = 2;
				entry = entry->next;
			}
		}
		buffer[offset] = '\0';
		return;
	}
	DeleteFormatEntries();

	hasFormat = (fmt != NULL);
	if ( !hasFormat ) {
		offset = 0;
		paramCount = -1;
		fmtBuffer[0] = '\0';
	} else {
		strcpy( fmtBuffer, fmt );
		fmtBuffer[255] = '\0';

		int end;
		offset = -1;
		for( int r=0, w=0; fmtBuffer[r] != '\0'; r++, w++ ) {
			if ( fmtBuffer[r] == '$' ) {
				if ( fmtBuffer[r+1] == '$' )
					r++;
				else {
					FormatEntry *entry = new FormatEntry;
					if ( firstFormatEntry == NULL )
						firstFormatEntry = lastFormatEntry = entry;
					else
						lastFormatEntry = lastFormatEntry->next = entry;
					numFormatEntries++;

					entry->fieldWidth = 0;
					end = r;
					if ( fmtBuffer[r+1] == '?' ) {
						entry->takeInput = 2;
						r++;
					} else if ( fmtBuffer[r+1] == '+' || fmtBuffer[r+1] == '-' ) {
						bool negative = fmtBuffer[r+1] == '-';
						entry->takeInput = 0;
						for ( r += 2; String::IsDigit(fmtBuffer[r]); r++ )
							entry->fieldWidth = 10 * entry->fieldWidth + (fmtBuffer[r] - '0');
						if ( negative )
							entry->fieldWidth *= -1;
						r--;
					}
					if ( fmtBuffer[r+1] == '*' ) {
						fmtBuffer[w] = '\0';
						if ( offset == -1 ) {
							memcpy( buffer, fmtBuffer, r );
							offset = end;
						}
						r++;
						entry->append = fmtBuffer + r + 1;
						w = r;
						continue;
					}
					OG_DEBUG_BREAK()
					//! @todo	error ?
				}
			}
			fmtBuffer[w] = fmtBuffer[r];
		}
		// This assert gets triggered when you have no parameters specified in the format.
		//! @todo	add an error here ?
		OG_ASSERT( offset != -1 );
		if ( offset != -1 ) {
			paramCount = 0;
		} else {
			// no params specified, act like this is the first parameter
			offset = strlen( fmtBuffer );
			memcpy( buffer, fmtBuffer, offset );
			paramCount = -1;
		}
	}
	buffer[offset] = '\0';
}

/*
================
Format::GetFormatEntry
================
*/
Format::FormatEntry *Format::GetFormatEntry( int index ) {
	//! @todo: do this nicer
	FormatEntry *entry = firstFormatEntry;
	for( int i=0; i<index && entry; i++ )
		entry = entry->next;
	return entry;
}

/*
================
Format::TryPrint
================
*/
void Format::TryPrint( const char *fmt, ... ) {
	int size = bufferSize - offset;
	if ( size > 1 ) {
		va_list	list;
		va_start(list, fmt);
		int ret = vsnPrintf( buffer+offset, size, fmt, list );
		if ( ret > 0 ) {
			int fieldWidth = 0;
			if ( paramCount != -1 && paramCount < numFormatEntries ) {
				FormatEntry *entry = GetFormatEntry( paramCount );
				if ( entry )
					fieldWidth = entry->fieldWidth;
			}
			bool alignLeft = true;
			if ( fieldWidth < 0 ) {
				fieldWidth *= -1;
				alignLeft = false;
			}
			if ( ret >= fieldWidth )
				offset += ret;
			else {
				if ( !alignLeft ) {
					int max = Min( offset + fieldWidth - ret, bufferSize-1 );
					for( ; offset < max; offset++ )
						buffer[offset] = fillChar;
					size = bufferSize - offset;
					if ( size > 1 ) {
						ret = vsnPrintf( buffer+offset, size, fmt, list );
						if ( ret > 0 )
							offset += ret;
					}
				} else {
					offset += ret;
					int max = Min( offset + fieldWidth - ret, bufferSize-1 );
					for( ; offset < max; offset++ )
						buffer[offset] = fillChar;
					buffer[offset] = '\0';
				}
			}
		}
		va_end(list);
		if ( ret == -1 )
			User::Error( ERR_BUFFER_OVERFLOW, "Format::TryPrint", Format() << size );
	}
}

/*
================
Format::OnAppend
================
*/
void Format::OnAppend( void ) {
	// This assert gets triggered when you add more parameters than where specified in the format.
	//! @todo	add an error here ?
	OG_ASSERT( paramCount < numFormatEntries );
	if ( paramCount < numFormatEntries ) {
		FormatEntry *entry = GetFormatEntry( paramCount++ );
		if ( entry )
			TryPrint( "%s", entry->append );
	}
}

/*
================
Format::CheckVariableInput
================
*/
bool Format::CheckVariableInput( void ) {
	// This assert gets triggered when you add more parameters than where specified in the format.
	//! @todo	add an error here ?
	OG_ASSERT( paramCount != -1 && paramCount < numFormatEntries );
	if ( paramCount < numFormatEntries ) {
		FormatEntry *entry = GetFormatEntry( paramCount );
		if ( entry && entry->takeInput == 2 ) {
			entry->takeInput = 1;
			return true;
		}
	}
	return false;
}

/*
================
Format::operator <<
================
*/
Format &Format::operator << ( int value ) {
	if ( paramCount != -1 && CheckVariableInput() ) {
		FormatEntry *entry = GetFormatEntry( paramCount );
		if ( entry )
			entry->fieldWidth = value;
		return *this;
	}
	TryPrint( "%d", value );
	return Finish();
}

Format &Format::operator << ( uInt value ) {
	if ( paramCount != -1 && CheckVariableInput() ) {
		FormatEntry *entry = GetFormatEntry( paramCount );
		if ( entry )
			entry->fieldWidth = value;
		return *this;
	}
	TryPrint( "%u", value );
	return Finish();
}

Format &Format::operator << ( short value ) {
	if ( paramCount != -1 && CheckVariableInput() ) {
		FormatEntry *entry = GetFormatEntry( paramCount );
		if ( entry )
			entry->fieldWidth = value;
		return *this;
	}
	TryPrint( "%d", value );
	return Finish();
}

Format &Format::operator << ( uShort value ) {
	if ( paramCount != -1 && CheckVariableInput() ) {
		FormatEntry *entry = GetFormatEntry( paramCount );
		if ( entry )
			entry->fieldWidth = value;
		return *this;
	}
	TryPrint( "%u", value );
	return Finish();
}

Format &Format::operator << ( char value ) {
	TryPrint( "%c", value );
	return Finish();
}

Format &Format::operator << ( byte value ) {
	TryPrint( "%u", value );
	return Finish();
}

Format &Format::operator << ( float value ) {
	if ( floatPrecision >= 0 )
		TryPrint( "%.*f", floatPrecision, value );
	else
		TryPrint( "%f", value );
	return Finish();
}

Format &Format::operator << ( const char *value ) {
	TryPrint( "%s", value );
	return Finish();
}

Format &Format::operator << ( const String &value ) {
	TryPrint( "%s", value.c_str() );
	return Finish();
}

}
