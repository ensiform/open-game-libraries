/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: String (UTF-8) interaction
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
#include <ctype.h>
#include <stdarg.h>

#ifdef OG_WIN32
	#include<windows.h>
#endif

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
================
Utf8ToWChar
================
*/
wchar_t Utf8ToWChar( const char *data, int *numBytes ) {
	if( data[0] < MASK1BIT && data[0] >= 0 ) {
		*numBytes = 1;
		return data[0];
	} else if((data[0] & MASK3BIT) == MASK2BIT) {
		*numBytes = 2;
		return ((data[0] & 0x1F) << 6) | (data[1] & MASKBITS);
	} else if( (data[0] & MASK4BIT) == MASK3BIT ) {
		*numBytes = 3;
		return ((data[0] & 0x0F) << 12) | ( (data[1] & MASKBITS) << 6) | (data[2] & MASKBITS);
	}
#if 0 //! @todo	if sizof(wchar_t) == 4
	else if( (data[0] & MASK5BIT) == MASK4BIT ) {
		*numBytes = 4;
		return ((data[0] & 0x07) << 18) | ((data[1] & MASKBITS) << 12) |
				((data[2] & MASKBITS) << 6) | (data[3] & MASKBITS);
	} else if( (data[0] & MASK6BIT) == MASK5BIT ) {
		*numBytes = 5;
		return ((data[0] & 0x03) << 24) | ((data[1] & MASKBITS) << 18) | ((data[2] & MASKBITS) << 12) |
				((data[3] & MASKBITS) << 6) | (data[4] & MASKBITS);
	} else if( (data[0] & MASK7BIT) == MASK6BIT ) {
		*numBytes = 6;
		return ((data[0] & 0x01) << 30) | ((data[1] & MASKBITS) << 24) | ((data[2] & MASKBITS) << 18) |
				((data[3] & MASKBITS) << 12) | ((data[4] & MASKBITS) << 6) | (data[5] & MASKBITS);
	}
#endif
	*numBytes = 1;
	return L'?';
}

/*
================
WCharToUtf8
================
*/
void WCharToUtf8( wchar_t ch, char *dest ) {
	if( ch < 0x80 ) {
		dest[0] = (char)ch;
		dest[1] = '\0';
	} else if( ch < 0x800 ) {
		dest[0] = (char)(MASK2BIT | ch >> 6);
		dest[1] = (char)(MASK1BIT | ch & MASKBITS);
		dest[2] = '\0';
	} else if( ch < 0x10000 ) {
		dest[0] = (char)(MASK3BIT | ch >> 12);
		dest[1] = (char)(MASK1BIT | ch >> 6 & MASKBITS);
		dest[2] = (char)(MASK1BIT | ch & MASKBITS);
		dest[3] = '\0';
	}
#if 0 //! @todo	if sizof(wchar_t) == 4
	else if( ch < 0x200000 ) {
		dest[0] = (char)(MASK4BIT | ch >> 18);
		dest[1] = (char)(MASK1BIT | ch >> 12 & MASKBITS);
		dest[2] = (char)(MASK1BIT | ch >> 6 & MASKBITS);
		dest[3] = (char)(MASK1BIT | ch & MASKBITS);
		dest[4] = '\0';
	} else if( ch < 0x200000 ) {
		dest[0] = (char)(MASK5BIT | ch >> 24);
		dest[1] = (char)(MASK1BIT | ch >> 18 & MASKBITS);
		dest[2] = (char)(MASK1BIT | ch >> 12 & MASKBITS);
		dest[3] = (char)(MASK1BIT | ch >> 6 & MASKBITS);
		dest[4] = (char)(MASK1BIT | ch & MASKBITS);
		dest[5] = '\0';
	} else if( ch < 0x200000 ) {
		dest[0] = (char)(MASK6BIT | ch >> 30);
		dest[1] = (char)(MASK1BIT | ch >> 24 & MASKBITS);
		dest[2] = (char)(MASK1BIT | ch >> 18 & MASKBITS);
		dest[3] = (char)(MASK1BIT | ch >> 12 & MASKBITS);
		dest[4] = (char)(MASK1BIT | ch >> 6 & MASKBITS);
		dest[5] = (char)(MASK1BIT | ch & MASKBITS);
		dest[6] = '\0';
	}
#endif
	else {
		dest[0] = '?';
		dest[1] = '\0';
	}
}

/*
================
Utf8ToLowerWide
================
*/
OG_INLINE uInt Utf8ToLowerWide( const char *ch, int *len ) {
	// Check if text1[i] is utf-8
	if ( (ch[0] & MASK2BIT) == MASK2BIT )
		return towlower( Utf8ToWChar( ch, len ) );

	// we assume it's ascii..
	// OG_ASSERT( ch[0] < MASK1BIT && ch[0] >= 0 );
	*len = 1;
	return tolower( ch[0] );
}

/*
================
countBytesForLength
================
*/
int countBytesForLength( const char *str, int len, int byteLen ) {
	int i = 0;
	for( int pos=0; str[i] != '\0'; i++ ) {
		if( (str[i] & MASK2BIT) != MASK1BIT ) {
			if ( pos == len )
				return i;
			pos++;
		}
	}
	return i;
}

/*
================
countBytesForLengthReverse
================
*/
int countBytesForLengthReverse( const char *str, int len, int byteLen ) {
	const char *str2 = str + byteLen-1;
	int i = 0;
	for( int pos=0; str2 >= str; i++, str2-- ) {
		if( (str2[i] & MASK2BIT) != MASK1BIT ) {
			if ( pos == len )
				return i;
			pos++;
		}
	}
	return i;
}

/*
================
SingleHexToFloat
================
*/
static float SingleHexToFloat( char c ) {
	if ( c <= '9' && c >= '0' )
		return static_cast<float>( c - '0' ) / 15.0f;
	else if ( c <= 'f' && c >= 'a' )
		return static_cast<float>( c - 'a' + 10 ) / 15.0f;
	else if ( c <= 'F' && c >= 'A' )
		return static_cast<float>( c - 'A' + 10 ) / 15.0f;
	return 1.0f;
}

/*
==============================================================================

  String

==============================================================================
*/

/*
================
String::String
================
*/
String::String() {
	Init();
}
String::String( const char *text ) {
	Init();
	size_t byteLen, len;
	BothLengths( text, &byteLen, &len );
	SetData( text, byteLen, len );
}
String::String( const String &str ) {
	Init();
	SetData( str.data, str.byteLength, str.length );
}

/*
================
String::~String
================
*/
String::~String() {
	Free();
}

/*
================
String::Clear
================
*/
void String::Clear( void ) {
	Free();
	Init();
}

/*
================
String::Init
================
*/
void String::Init( void ) {
	length = 0;
	byteLength = 0;
	size = OG_STR_HARDBUFFER_SIZE;
	data = hardBuffer;
	data[0] = '\0';
}

/*
================
String::Resize
================
*/
void String::Resize( int newSize, bool keepContent ) {
	OG_ASSERT( newSize > 0 );
	size = Math::FtoiFast( ceil( static_cast<float>(newSize)/static_cast<float>(OG_STR_GRANULARITY) ) ) * OG_STR_GRANULARITY;

	char *newData = new char[ size ];

	if ( data ) {
		if ( keepContent )
			memcpy( newData, data, byteLength+1 );
		else {
			length = 0;
			byteLength = 0;
		}
		if ( data != hardBuffer )
			delete[] data;
	}
	data = newData;
}

/*
================
String::IsNumeric
================
*/
bool String::IsNumeric( const char *text ) {
	if ( !text || text[0] == '\0' )
		return false;

	if ( text[0] == '-' || text[0] == '+' )
		text++;

	bool hasDot = false;
	for ( int i=0; text[i] != '\0'; i++ ) {
		if ( text[i] == '.' && !hasDot )
			hasDot = true;
		else if ( !IsDigit(text[i]) )
			return false;
	}
	return true;
}

/*
================
String::IsWord
================
*/
bool String::IsWord( const char *text ) {
	if ( !text || text[0] == '\0' )
		return false;

	for ( int i=0; text[i] != '\0'; i++ ) {
		if ( !IsDigit(text[i]) && !IsAlpha(text[i]) && text[i] != '_' )
			return false;
	}
	return true;
}

/*
================
String::ToLower
================
*/
void String::ToLower( void ) {
	char utf8[7];
	wchar_t ch;
	int len;
	String old(*this);
	Empty();
	for( int i=0; i<old.byteLength; i++ ) {
		ch = Utf8ToWChar( old.data+i, &len );
		WCharToUtf8( towlower(ch), utf8 );
		AppendData( utf8, ByteLength(utf8), 1 );
	}
}

/*
================
String::ToUpper
================
*/
void String::ToUpper( void ) {
	char utf8[7];
	wchar_t ch;
	int len;
	String old(*this);
	Empty();
	for( int i=0; i<old.byteLength; i++ ) {
		ch = Utf8ToWChar( old.data+i, &len );
		WCharToUtf8( towupper(ch), utf8 );
		AppendData( utf8, ByteLength(utf8), 1 );
	}
}

/*
================
String::CapLength
================
*/
void String::CapLength( int len, bool elipsis ) {
	if ( len < length ) {
		if ( elipsis )
			len = Math::Max(len-3, 0);

		for( int i=0, pos=0; data[i] != '\0'; i++ ) {
			if( (data[i] & MASK2BIT) != MASK1BIT ) {
				if ( pos == len ) {
					length = len;
					byteLength = i;
					data[byteLength] = '\0';
					if ( elipsis )
						AppendData("...", 3, 3);
					return;
				}
				pos++;
			}
		}
	}
}

/*
================
String::Find
================
*/
int String::Find( const char *str, const char *text, bool caseSensitive, int start ) {
	//! @todo	test this..
	int findLen = Length(text);
	if ( caseSensitive ) {
		for( int i=0; str[i] != 0; i++ ) {
			if ( Cmpn(str+i, text, findLen ) == 0 )
				return i;
		}
	} else {
		for( int i=0; str[i] != 0; i++ ) {
			if ( Icmpn(str+i, text, findLen ) == 0 )
				return i;
		}
	}
	return -1;
}

/*
================
String::FindOneOf
================
*/
int String::FindOneOf( const char *str, const char *text, bool caseSensitive, int start ) {
	//! @todo	unicode
	return -1;
}

/*
================
String::ReverseFind
================
*/
int String::ReverseFind( const char *str, const char *text, bool caseSensitive ) {
	//! @todo	test this..
	int byteLength = ByteLength(str);
	int findLen = Length(text);
	if ( caseSensitive ) {
		for( int i=byteLength-findLen; i >= 0; i-- ) {
			if ( Cmpn(str+i, text, findLen ) == 0 )
				return i;
		}
	} else {
		for( int i=byteLength-findLen; i >= 0; i-- ) {
			if ( Icmpn(str+i, text, findLen ) == 0 )
				return i;
		}
	}
	return -1;
}

/*
================
String::Replace
================
*/
int String::Replace( const char *a, const char *b, int start ) {
	int lenA = ByteLength(a);
	if ( lenA == 0 )
		return 0;

	int lenB = ByteLength(b);
	int max = byteLength - lenA;
	if ( max >= start ) {
		List<int> positions;
		int i;
		for ( i=start; i<=max; i++ ) {
			if ( Cmpn( data+i, a, lenA ) == 0 )
				positions.Append(i);
		}
		int count = positions.Num();
		if ( count ) {
			char *oldData;
			int oldByteLength = byteLength;
			int oldLength = length;
			int oldSize = size;
			if ( data == hardBuffer ) {
				oldData = new char[byteLength+1];
				memcpy( oldData, data, byteLength+1 );
			} else {
				oldData = data;
				data = hardBuffer;
				size = OG_STR_HARDBUFFER_SIZE;
			}
			length = 0;
			byteLength = 0;

			int p = 0;
			int j;
			CheckSize( oldByteLength + ((lenB-lenA)*count) + 1, false );
			for ( i=start; i< oldByteLength; i++ ) {
				if ( p < count && i == positions[p] ) {
					p++;
					i += lenA-1;
					for ( j=0; j<lenB; j++ )
						data[byteLength++] = b[j];
				}
				else
					data[byteLength++] = oldData[i];
			}
			data[byteLength] = '\0';
			length = oldLength + (Length(b) - Length(a)) * count;

			delete[] oldData;
		}
		return count;
	}
	return 0;
}

/*
================
String::StripLeading
================
*/
int String::StripLeading( const char *text ) {
	size_t byteLen, len;
	BothLengths( text, &byteLen, &len );
	if ( byteLen == 0 )
		return 0;
	int count = 0;
	int pos = 0;
	while ( Cmpn(data+pos, text, byteLen) == 0 ) {
		pos += byteLen;
		length -= len;
		count++;
	}

	if ( pos > 0 ) {
		byteLength -= pos;
		if ( byteLength == 0 )
			data[0] = '\0';
		else
			memmove( data, data + pos, byteLength+1 );
	}
	return count;
}

/*
================
String::StripLeadingOnce
================
*/
bool String::StripLeadingOnce( const char *text ) {
	size_t byteLen, len;
	BothLengths( text, &byteLen, &len );
	if ( byteLen == 0 )
		return false;
	if ( Cmpn(text, byteLen) == 0 ) {
		length -= len;
		byteLength -= byteLen;

		if ( byteLength == 0 )
			data[0] = '\0';
		else
			memmove( data, data + byteLen, byteLength+1 );
		return true;
	}
	return false;
}

/*
================
String::StripLeadingWhitespaces
================
*/
int String::StripLeadingWhitespaces( void ) {
	int pos = 0;
	while ( IsSpace(data[pos]) ) pos++;

	if ( pos > 0 ) {
		length -= pos;
		byteLength -= pos;
		if ( byteLength == 0 )
			data[0] = '\0';
		else
			memmove( data, data + pos, byteLength+1 );
	}
	return pos;
}

/*
================
String::StripTrailing
================
*/
int String::StripTrailing( const char *text ) {
	int len = ByteLength(text);
	int pos = byteLength - len;
	int count = 0;
	while ( pos >= 0 && Cmpn(data+pos, text, len) == 0 ) {
		pos -= len;
		count++;
	}

	pos += len;
	if ( pos != byteLength ) {
		length -= Length( data+pos );
		byteLength = pos;
		data[byteLength] = '\0';
	}
	return count;
}

/*
================
String::StripTrailingOnce
================
*/
bool String::StripTrailingOnce( const char *text ) {
	int len = ByteLength(text);
	int pos = byteLength - len;
	if ( pos >= 0 && Cmpn( data+pos, text, len ) == 0 ) {
		length -= Length(text);
		byteLength = pos;
		data[byteLength] = '\0';
		return true;
	}
	return false;
}

/*
================
String::StripTrailingWhitespaces
================
*/
int String::StripTrailingWhitespaces( void ) {
	int pos = byteLength-1;
	int count = 0;
	while ( IsSpace(data[pos--]) ) count++;

	pos++;
	if ( pos != byteLength ) {
		length -= byteLength - pos;
		byteLength = pos;
		data[byteLength] = '\0';
	}
	return count;
}

/*
================
String::StripFileExtension
================
*/
void String::StripFileExtension( void ) {
	for ( int i=byteLength-1; i>=0; i-- ) {
		if ( data[i] == '/' || data[i] == '\\' )
			return;
		if ( data[i] == '.' ) {
			length -= Length( data+i );
			byteLength = i;
			data[byteLength] = '\0';
			return;
		}
	}
}

/*
================
String::SetFileExtension
================
*/
void String::SetFileExtension( const char *ext ) {
	StripFileExtension();
	if ( *ext != '.' )
		AppendData(".", 1, 1);
	size_t byteLen, len;
	BothLengths( ext, &byteLen, &len );
	AppendData( ext, byteLen, len );
}

/*
================
String::DefaultFileExtension
================
*/
void String::DefaultFileExtension( const char *ext ) {
	for ( int i=byteLength-1; i>=0; i-- ) {
		if ( data[i] == '/' || data[i] == '\\' )
			break;
		if ( data[i] == '.' )
			return;
	}
	if ( *ext != '.' )
		AppendData(".", 1, 1);
	size_t byteLen, len;
	BothLengths( ext, &byteLen, &len );
	AppendData( ext, byteLen, len );
}

/*
================
String::CheckFileExtension
================
*/
bool String::CheckFileExtension( const char *ext ) const {
	int len = Length( ext );
	if ( len > length )
		return false;
	else {
		uInt byteLen = countBytesForLengthReverse( data, len, byteLength );
		return  Icmp( ext, data+byteLength-byteLen ) == 0;
	}
}

/*
================
String::GetFileExtension
================
*/
void String::GetFileExtension( const char *text, int byteLen, String &str ) {
	for ( int i=byteLen-1; i>=0; i-- ) {
		if ( text[i] == '/' || text[i] == '\\' ) {
			str.Empty();
			return;
		}
		if ( text[i] == '.' ) {
			str = text+i+1;
			return;
		}
	}
}
String String::GetFileExtension( const char *text, int byteLen ) {
	String str;
	for ( int i=byteLen-1; i>=0; i-- ) {
		if ( text[i] == '/' || text[i] == '\\' ) {
			return str;
		}
		if ( text[i] == '.' ) {
			str = text+i+1;
			return str;
		}
	}
	return str;
}

/*
================
String::StripFilename
================
*/
void String::StripFilename( void ) {
	for ( int i=byteLength-1; i>0; i-- ) {
		if ( data[i-1] == '/' || data[i-1] == '\\' ) {
			length -= Length( data+i );
			byteLength = i;
			data[byteLength] = '\0';
			return;
		}
	}
	Clear();
}

/*
================
String::GetFilename
================
*/
void String::GetFilename( String &str ) const {
	for ( int i=byteLength-1; i>0; i-- ) {
		if ( data[i-1] == '/' || data[i-1] == '\\' ) {
			str = data+i;
			return;
		}
	}
	str = data;
}
String String::GetFilename( void ) const {
	for ( int i=byteLength-1; i>0; i-- ) {
		if ( data[i-1] == '/' || data[i-1] == '\\' ) {
			return (data+i);
		}
	}
	return *this;
}

/*
================
String::StripPath
================
*/
void String::StripPath( void ) {
	int len = 0;
	for ( int i=byteLength-1; i>0; i-- ) {
		if( (data[i] & MASK2BIT) != MASK1BIT )
			len++;
		if ( data[i-1] == '/' || data[i-1] == '\\' ) {
			SetData(data+i, byteLength-i, len );
			return;
		}
	}
}

/*
================
String::GetPath
================
*/
void String::GetPath( String &str ) const {
	int len = 0;
	for ( int i=byteLength-1; i>0; i-- ) {
		if( (data[i] & MASK2BIT) != MASK1BIT )
			len++;
		if ( data[i-1] == '/' || data[i-1] == '\\' ) {
			str.SetData(data, i, len);
			return;
		}
	}
	str.Empty();
}
String String::GetPath( void ) const {
	int len = 0;
	String str;
	for ( int i=byteLength-1; i>0; i-- ) {
		if( (data[i] & MASK2BIT) != MASK1BIT )
			len++;
		if ( data[i-1] == '/' || data[i-1] == '\\' ) {
			str.SetData(data, i, len);
			return str;
		}
	}
	return str;
}

/*
================
String::Left
================
*/
String String::Left( int len ) const {
	if ( len > length )
		return *this;
	else {
		uInt byteLen = countBytesForLength( data, len, byteLength );
		String ret;
		ret.SetData(data, byteLen, len);
		return ret;
	}
}

/*
================
String::Left
================
*/
void String::Left( int len, String &str ) const {
	str.Clear();
	if ( len > length )
		str = *this;
	else {
		uInt byteLen = countBytesForLength( data, len, byteLength );
		str.SetData(data, byteLen, len);
	}
}

/*
================
String::Right
================
*/
String String::Right( int len ) const {
	if ( len > length )
		return *this;
	else {
		uInt byteLen = countBytesForLengthReverse( data, len, byteLength );
		String ret;
		ret.SetData( data+byteLength-byteLen, byteLen, len );
		return ret;
	}
}

/*
================
String::Right
================
*/
void String::Right( int len, String &str ) const {
	str.Clear();
	if ( len > length )
		str = *this;
	else {
		uInt byteLen = countBytesForLengthReverse( data, len, byteLength );
		str.SetData( data+byteLength-byteLen, byteLen, len );
	}
}

/*
================
String::Mid
================
*/
String String::Mid( int start, int len ) const {
	if ( (start + len) > length ) {
		start = countBytesForLength( data, start, byteLength );
		if ( start < byteLength ) {
			uInt byteLen = byteLength - start;
			String ret;
			ret.SetData(data+start, byteLen, len);
			return ret;
		}
		return "";
	}
	start = countBytesForLength( data, start, byteLength );
	uInt byteLen = countBytesForLength( data+start, len, byteLength );
	String ret;
	ret.SetData(data+start, byteLen, len);
	return ret;
}

/*
================
String::Mid
================
*/
void String::Mid( int start, int len, String &str ) const {
	str.Clear();
	if ( (start + len) > length ) {
		start = countBytesForLength( data, start, byteLength );
		if ( start < byteLength ) {
			uInt byteLen = byteLength - start;
			str.SetData(data+start, byteLen, len);
		}
		return;
	}
	start = countBytesForLength( data, start, byteLength );
	uInt byteLen = countBytesForLength( data+start, len, byteLength );
	str.SetData(data+start, byteLen, len);
}

/*
================
String::Split
================
*/
void String::Split( const char *text, StringList& list, const char *delimiter ) {
	if ( !text || !text[0] )
		return;
	int pos1 = 0;
	int pos2 = 0;
	int byteLen, len;

	do {
		pos2 = Find( text, delimiter, false, pos1 );
		if ( pos2 == INVALID_POSITION )
			list.Append( text + pos1 );
		else if ( pos2 == pos1 )
			list.Append( "" ); // same pos, so just add an empty string
		else {
			byteLen = pos2-pos1;
			// Count number of characters
			len = 0;
			for( int i=pos1; text[i] != '\0' && i<pos2; i++ ) {
				if( (text[i] & MASK2BIT) != MASK1BIT )
					len++;
			}
			list.Append( text+pos1, byteLen, len );
		}
		pos1 = pos2+1;
	} while ( pos2 != INVALID_POSITION );
}

/*
================
String::Cmp
================
*/
int String::Cmp( const char *text1, const char *text2 ) {
	if ( text1 == NULL ) {
		if ( text2 == NULL )
			return 0;
		else
			return -1;
	}
	else if ( text2 == NULL )
		return 1;

	int d;
	for( int i=0; text1[i] || text2[i]; i++) {
		d = text1[i] - text2[i];
		if ( d != 0 )
			return d;
	}

	return 0;
}

/*
================
String::Cmpn
================
*/
int String::Cmpn( const char *text1, const char *text2, int len ) {
	if ( text1 == NULL ) {
		if ( text2 == NULL )
			return 0;
		else
			return -1;
	}
	else if ( text2 == NULL )
		return 1;

	int d;
	for( int i=0; text1[i] || text2[i]; i++) {
		if ( len <= 0 )
			return 0;
		if( (text1[i] & MASK2BIT) != MASK1BIT )
			len--;

		d = text1[i] - text2[i];
		if ( d != 0 )
			return d;
	}

	return 0;
}

/*
================
String::Icmp
================
*/
int String::Icmp( const char *text1, const char *text2 ) {
	if ( text1 == NULL ) {
		if ( text2 == NULL )
			return 0;
		else
			return -1;
	}
	else if ( text2 == NULL )
		return 1;

	int numB1, numB2, d;
	for( int i=0,j=0; text1[i] || text2[i]; i += numB1, j += numB2 ) {
		d = Utf8ToLowerWide( text1+i, &numB1 ) - Utf8ToLowerWide( text2+j, &numB2 );
		if ( d != 0 )
			return d;
	}
	return 0;
}

/*
================
String::Icmpn
================
*/
int String::Icmpn( const char *text1, const char *text2, int len ) {
	if ( text1 == NULL ) {
		if ( text2 == NULL )
			return 0;
		else
			return -1;
	}
	else if ( text2 == NULL )
		return 1;

	int numB1, numB2, d;
	for( int i=0,j=0; text1[i] || text2[i]; i += numB1, j += numB2 ) {
		if ( len <= 0 )
			return 0;
		if( (text1[i] & MASK2BIT) != MASK1BIT )
			len--;

		d = Utf8ToLowerWide( text1+i, &numB1 ) - Utf8ToLowerWide( text2+j, &numB2 );
		if ( d != 0 )
			return d;
	}

	return 0;
}

/*
================
String::CmpPrefix
================
*/
int String::CmpPrefix( const char *text1, const char *text2 ) {
	return Cmpn( text1, text2, ByteLength(text2) );
}

/*
=============
String::Length
=============
*/
size_t String::Length( const char *text ) {
	if ( !text ) // users could send a NULL string (no point in getting to loop if it is NULL)
		return 0;

	size_t pos = 0;
	for( int i=0; text[i] != '\0'; i++ ) {
		if( (text[i] & MASK2BIT) != MASK1BIT )
			pos++;
	}
	return pos;
}

/*
=============
String::ByteLength
=============
*/
size_t String::ByteLength( const char *text ) {
	if ( !text ) // users could send a NULL string (no point in getting to loop if it is NULL)
		return 0;

	size_t i;
	for ( i = 0; text[i] != '\0'; i++ );
	return i;
}

/*
=============
String::BothLengths
=============
*/
void String::BothLengths( const char *text, size_t *byteLength, size_t *length ) {
	*byteLength = 0;
	*length = 0;
	if ( !text ) // users could send a NULL string (no point in getting to loop if it is NULL)
		return;

	for( ; text[*byteLength] != '\0'; (*byteLength)++ ) {
		if( (text[*byteLength] & MASK2BIT) != MASK1BIT )
			(*length)++;
	}
}

/*
================
String::GetEscapeColor
================
*/
int String::GetEscapeColor( const char *str, Color &destColor, const Color &defaultColor ) {
	if ( *str == '\0' )
		return -1;

	if ( *str == 'c' ) {
		if ( str[1] == '\0' || str[2] == '\0' || str[3] == '\0' )
			return -1;
		destColor.r = SingleHexToFloat( str[1] );
		destColor.g = SingleHexToFloat( str[2] );
		destColor.b = SingleHexToFloat( str[3] );
		return 4;
	}
	switch( *str ) {
		default:
		case '0':
			destColor = defaultColor;
			break;
		case '1':
			destColor = c_color::red;
			break;
		case '2':
			destColor = c_color::green;
			break;
		case '3':
			destColor = c_color::yellow;
			break;
		case '4':
			destColor = c_color::blue;
			break;
		case '5':
			destColor = c_color::cyan;
			break;
		case '6':
			destColor = c_color::magenta;
			break;
		case '7':
			destColor = c_color::white;
			break;
		case '8':
			destColor = c_color::gray;
			break;
		case '9':
			destColor = c_color::black;
			break;
	}
	return 1;
}

/*
================
String::GetEscapeColorLength
================
*/
int String::GetEscapeColorLength( const char *str ) {
	if ( str[0] != '^' )
		return 0;
	if ( IsDigit(str[1]) )
		return 2;
	if ( str[1] != 'c')
		return 0;
	
	for( int i=2; i<5; i++ ) {
		if ( !IsDigit(str[i]) && !( str[i] <= 'f' && str[i] >= 'a' ) && !( str[i] <= 'F' && str[i] >= 'A' ) )
			return 0;
	}
	return 5;
}

/*
================
String::StripEscapeColor
================
*/
size_t String::StripEscapeColor( char *str ) {
	uInt escapeLength, numRemoved = 0;
	int skip=0;
	char *writePos = str;
	char *readPos = str;
	while( *readPos != '\0' ) {
		if ( skip ) {
			skip--;
			numRemoved++;
		} else {
			escapeLength = GetEscapeColorLength( readPos );
			if ( escapeLength ) {
				skip = escapeLength-1;
				numRemoved += escapeLength;
			} else {
				writePos[0] = readPos[0];
				writePos++;
			}
		}
		readPos++;
	}
	*writePos = '\0';
	return numRemoved;
}

/*
================
String::StripEscapeColor
================
*/
void String::StripEscapeColor( void ) {
	int removed = StripEscapeColor( data );
	byteLength -= removed;
	length -= removed;
}

/*
================
String::SetData
================
*/
void String::SetData( const char *text, int byteLen, int len ) {
	//! @todo	rather just an assert ?
	if ( text == NULL ) {
		// don't boom on NULL text
		data[0] = '\0';
		length = 0;
		byteLength = 0;
		return;
	}

	CheckSize( byteLen + 1, false );
	memcpy( data, text, byteLen );
	data[len] = '\0';
	byteLength = byteLen;
	length = len;
}

#ifdef OG_COMMON_USE_FS
/*
================
String::ReadFromFile
================
*/
void String::ReadFromFile( File *f ) {
	uShort byteLen = f->ReadUshort();
	uShort len = f->ReadUshort();
	if ( byteLen > OG_STR_FILE_MAX_BYTES )
		byteLen = OG_STR_FILE_MAX_BYTES;

	CheckSize( byteLen + 1, false );
	f->Read( data, len );
	data[len] = '\0';
	length = len;
	byteLength = byteLen;
}

/*
================
String::WriteToFile
================
*/
void String::WriteToFile( File *f ) const {
	int byteLen = Math::Min( byteLength, OG_STR_FILE_MAX_BYTES );
	f->WriteUshort(static_cast<uShort>( byteLen ));
	f->WriteUshort(static_cast<uShort>(length));
	f->Write( data, byteLen );
}
#endif

/*
================
String::ToInt

faster than atoi..
================
*/
int String::ToInt( const char *str ) {
	int val = 0;
	int i = 0;
	int m = (str[0] == '-' ) ? -1 : 1;

	if ( str[0] == '-'  || str[0] == '+' )
		i++;

	int max = 10 + i;
	for ( ; IsDigit(str[i]) && i<max; i++ )
		val = 10 * val + (str[i] - '0');

	return val * m;
}

/*
================
String::ToLong

faster than atol..
================
*/
long String::ToLong( const char *str ) {
	long val = 0;
	int i = 0;
	int m = (str[0] == '-' ) ? -1 : 1;

	if ( str[0] == '-'  || str[0] == '+' )
		i++;

	int max = 10 + i;
	for ( ; IsDigit(str[i]) && i<max; i++ )
		val = 10 * val + (str[i] - '0');

	return val * m;
}

/*
================
String::ToFloat

faster than atof and sscanf
================
*/
float String::ToFloat( const char *str ) {
	uLong val;
	float m = (str[0] == '-' ) ? -1.0f : 1.0f;
	int i = 0;

	if ( str[0] == '-'  || str[0] == '+' )
		i++;
	int max = 8 + i;
	for ( val=0; IsDigit(str[i]) && i<max; i++ )
		val = 10 * val + (str[i] - '0');

	if ( str[i] == '.' ) {
		for ( i++; IsDigit(str[i]) && i<max; i++ ) {
			val = 10 * val + (str[i] - '0');
			m *= 0.1f;
		}
	}
	return val * m;
}

/*
================
String::ToFloatArray

faster than sscanf
================
*/
bool String::ToFloatArray( const char *str, float *fp, int dim ) {
	uLong val;
	float m;
	int i = 0;
	int j, max;

	for ( j=0; j < dim; j++ ) {
		if ( j && str[i-1] != ' ' )
			break;

		m = (str[i] == '-' ) ? -1.0f : 1.0f;

		if ( str[i] == '-'  || str[i] == '+' )
			i++;

		if ( !IsDigit(str[i]) )
			break;

		max = 8 + i;
		for ( val=0; IsDigit(str[i]) && i<max; i++ )
			val = 10 * val + (str[i] - '0');

		//! @todo	fix this nicer
		// skip remaining digits we where not able to read.
		for ( ; IsDigit(str[i]); i++ )
			continue;

		if ( str[i] == '.' ) {
			for ( i++; IsDigit(str[i]) && i<max; i++ ) {
				val = 10 * val + (str[i] - '0');
				m *= 0.1f;
			}
			//! @todo	fix this nicer
			// skip remaining digits we where not able to read.
			for ( ; IsDigit(str[i]); i++ )
				continue;
		}
		fp[j] = val * m;

		i++;
	}
	if ( j != dim ) {
		while(j<dim)
			fp[j++] = 0.0f;
		return false;
	}
	return true;
}

/*
================
String::ToDouble

faster than atof and sscanf
================
*/
double String::ToDouble( const char *str ) {
	uLongLong val;
	double m = (str[0] == '-' ) ? -1.0 : 1.0;
	int i = 0;

	if ( str[0] == '-'  || str[0] == '+' )
		i++;
	int max = 17 + i;
	for ( val=0; IsDigit(str[i]) && i<max; i++ )
		val = 10 * val + (str[i] - '0');

	if ( str[i] == '.' ) {
		for ( i++; IsDigit(str[i]) && i<max; i++ ) {
			val = 10 * val + (str[i] - '0');
			m *= 0.1;
		}
	}
	return val * m;
}

/*
================
String::ToDoubleArray

faster than sscanf
================
*/
bool String::ToDoubleArray( const char *str, double *dp, int dim ) {
	uLongLong val;
	double m;
	int i = 0;
	int j, max;

	for ( j=0; j < dim; j++ ) {
		if ( j && str[i-1] != ' ' )
			break;

		m = (str[i] == '-' ) ? -1.0 : 1.0;

		if ( str[i] == '-'  || str[i] == '+' )
			i++;

		if ( !IsDigit(str[i]) )
			break;

		max = 17 + i;
		for ( val=0; IsDigit(str[i]) && i<max; i++ )
			val = 10 * val + (str[i] - '0');

		if ( str[i] == '.' ) {
			for ( i++; IsDigit(str[i]) && i<max; i++ ) {
				val = 10 * val + (str[i] - '0');
				m *= 0.1;
			}
		}
		dp[j] = val * m;

		i++;
	}
	if ( j != dim ) {
		while(j<dim)
			dp[j++] = 0.0;
		return false;
	}
	return true;
}

/*
================
String::FormatNumBytes
================
*/
void String::FormatNumBytes( uLongLong bytes, int digits, fnbStyle style ) {
	static const char *units[] = {
		"Byte",
		"KByte",
		"MByte",
		"GByte",
		"TByte",
		"PByte"
	};
	static const int total_units = sizeof(units) / sizeof(units[0]);

	double bytesD = static_cast<double>(bytes);
	int limit = Math::FtoiFast(Math::Pow(10.0f, static_cast<float>(digits)));
	double value = bytesD;
	const double factor = 1.0f/1024.0f;
	int floorValue;
	for( int i = 1; i < total_units; ++i ) {
		value *= factor;
		floorValue = static_cast<int>(floor(value));
		if( floorValue < limit ) {
			switch(style) {
				case FNB_FLOAT:
					digits--;
				case FNB_FLOATEX:
					if ( floorValue != value ) {
						*this = TS( "$* $*") << SetPrecision( digits-Math::Digits(floorValue) ) << static_cast<float>(value) << units[i];
						return;
					}
				default:
					*this = TS("$* $*") << floorValue << units[i];
					return;
			}
		}
	}

	// Should never get here
	value = Math::FtoiFast(static_cast<float>(bytesD / Math::Pow(1024.0, static_cast<float>(total_units-1))));
	*this = TS("$* $*") << static_cast<float>(value) << units[total_units-1];
}

/*
================
String::FromWide
================
*/
void String::FromWide( const wchar_t *in ) {
#ifdef OG_WIN32
	int inLen = wcslen(in)+1;
	int outLen = WideCharToMultiByte( CP_UTF8, 0, in, inLen, NULL, 0, NULL, NULL );
	CheckSize( outLen, false );
	WideCharToMultiByte( CP_UTF8, 0,  in, inLen, data, outLen, NULL, NULL );
	
	length = inLen;
	byteLength = outLen;
#else
	//! @todo	other platforms
	#error "sorry, I was lazy"
#endif
}

/*
================
String::ToWide
================
*/
int String::ToWide( const char *in, uInt numBytes, wchar_t *out, uInt outSize ) {
#ifdef OG_WIN32
	if ( out == NULL )
		return MultiByteToWideChar( CP_UTF8, 0, in, numBytes, NULL, 0 );

	out[0] = L'\0';
	return MultiByteToWideChar( CP_UTF8, 0,  in, numBytes, out, outSize );
#else
	//! @todo	other platforms
	#error "sorry, I was lazy"
#endif
}
int String::ToWide( const char *in, DynBuffer<wchar_t> &buffer ) {
	uInt numBytes = String::ByteLength( in ) + 1;
	uInt size = Math::Max( 1, String::ToWide( in, numBytes, NULL, 0 ) );
	buffer.CheckSize( size );
	if ( size == 1 )
		buffer.data[0] = '\0';
	else
		String::ToWide( in, numBytes, buffer.data, buffer.size );
	return size;
}

/*
================
String::FromBitFlags
================
*/
void String::FromBitFlags( int flags, const char **flagNames ) {
	if ( flags == -1 )
		*this = "ALL";
	else {
		*this = "";
		for( int i=0; i<32 && flagNames[i]; i++ ) {
			if ( ( flags & BIT(i) ) != 0 ) {
				*this += flagNames[i];
				*this += "|";
			}
		}
		StripTrailingOnce("|");
	}
}

}
