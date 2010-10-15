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

#ifndef __OG_STRING_INL__
#define __OG_STRING_INL__

namespace og {

/*
==============================================================================

  String

==============================================================================
*/

/*
================
String::c_str
================
*/
OG_INLINE const char *String::c_str( void ) const {
	return data;
}

/*
================
String::raw_ptr
================
*/
OG_INLINE char *String::raw_ptr( void ) const {
	return data;
}

/*
================
String::Length
================
*/
OG_INLINE int String::Length( void ) const {
	return length;
}

/*
================
String::ByteLength
================
*/
OG_INLINE int String::ByteLength( void ) const {
	return byteLength;
}

/*
================
String::IsEmpty
================
*/
OG_INLINE bool String::IsEmpty( void ) const {
	return (length == 0);
}

/*
================
String::IsNumeric
================
*/
OG_INLINE bool String::IsNumeric( void ) const {
	return IsNumeric( data );
}

/*
================
String::IsWord
================
*/
OG_INLINE bool String::IsWord( void ) const {
	return IsWord( data );
}

/*
================
String::IsDigit
================
*/
OG_INLINE bool String::IsDigit( char c ) {
	return c >= '0' && c <= '9';
}

/*
================
String::IsAlpha
================
*/
OG_INLINE bool String::IsAlpha( char c ) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/*
================
String::IsSpace
================
*/
OG_INLINE bool String::IsSpace( char c ) {
	return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == (unsigned char)0x0D || c == (unsigned char)0x0A;
}

/*
================
String::IsNewLine
================
*/
OG_INLINE bool String::IsNewLine( char c ) {
	return c == '\n' || c == '\r' || c == '\v';
}

/*
================
String::Empty
================
*/
OG_INLINE void String::Empty( void ) {
	data[0] = '\0';
	length = 0;
	byteLength = 0;
}

/*
================
String::Find
================
*/
OG_INLINE int String::Find( const char *text, bool caseSensitive, int start ) const {
	return Find( data, text, caseSensitive, start );
}

/*
================
String::FindOneOf
================
*/
OG_INLINE int String::FindOneOf( const char *text, bool caseSensitive, int start ) const {
	return FindOneOf( data, text, caseSensitive, start );
}

/*
================
String::ReverseFind
================
*/
OG_INLINE int String::ReverseFind( const char *text, bool caseSensitive ) const {
	return ReverseFind( data, text, caseSensitive );
}

/*
================
String::ToForwardSlashes
================
*/
OG_INLINE void String::ToForwardSlashes( void ) {
	for( int i=0; data[i] != '\0'; i++ ) {
		if ( data[i] == '\\' )
			data[i] = '/';
	}
}

/*
================
String::ToBackwardSlashes
================
*/
OG_INLINE void String::ToBackwardSlashes( void ) {
	for( int i=0; data[i] != '\0'; i++ ) {
		if ( data[i] == '/' )
			data[i] = '\\';
	}
}

/*
================
String::GetFileExtension
================
*/
OG_INLINE void String::GetFileExtension( String &str ) const {
	GetFileExtension( data, byteLength, str );
}
OG_INLINE String String::GetFileExtension( void ) const {
	return GetFileExtension( data, byteLength );
}

/*
================
String::Cmp
================
*/
OG_INLINE int String::Cmp( const char *text ) const {
	return Cmp( data, text );
}

/*
================
String::Cmpn
================
*/
OG_INLINE int String::Cmpn( const char *text, int len ) const {
	return Cmpn( data, text, len );
}

/*
================
String::CmpPrefix
================
*/
OG_INLINE int String::CmpPrefix( const char *text ) const {
	return Cmpn( data, text, ByteLength(text) );
}

/*
================
String::Icmp
================
*/
OG_INLINE int String::Icmp( const char *text ) const {
	return String::Icmp( data, text );
}

/*
================
String::Icmpn
================
*/
OG_INLINE int String::Icmpn( const char *text, int len ) const {
	return String::Icmpn( data, text, len );
}

/*
================
String::IcmpPrefix
================
*/
OG_INLINE int String::IcmpPrefix( const char *text ) const {
	return String::Icmpn( data, text, Length(text) );
}
OG_INLINE int String::IcmpPrefix( const char *text1, const char *text2 ) {
	return String::Icmpn( text1, text2, Length(text2) );
}

/*
================
String::Free
================
*/
OG_INLINE void String::Free( void ) {
	if ( data && data != hardBuffer )
		delete[] data;
}

/*
================
String::CheckSize
================
*/
OG_INLINE void String::CheckSize( int newSize, bool keepContent ) {
	if ( newSize > size )
		Resize( newSize, keepContent );
}

/*
================
String::AppendData
================
*/
OG_INLINE void String::AppendData( const char *text, int byteLen, int len ) {
	if ( text && len ) {
		CheckSize( byteLength + byteLen + 1 );
		memcpy( data+byteLength, text, byteLen );
		byteLength += byteLen;
		length += len;
		data[byteLength] = '\0';
	}
}

/*
================
String::operator==
================
*/
OG_INLINE bool String::operator==( const String &other ) const {
	return ( Cmp( data, other.data ) == 0 );
}
OG_INLINE bool String::operator==( const char *text ) const {
	return ( Cmp( data, text ) == 0 );
}
OG_INLINE bool operator==( const char *text, const String &str ) {
	return ( String::Cmp( str.data, text ) == 0 );
}

/*
================
String::operator!=
================
*/
OG_INLINE bool String::operator!=( const String &other ) const {
	return ( Cmp( data, other.data ) != 0 );
}
OG_INLINE bool String::operator!=( const char *text ) const {
	return ( Cmp( data, text ) != 0 );
}
OG_INLINE bool operator!=( const char *text, const String &str ) {
	return ( String::Cmp( str.data, text ) != 0 );
}

/*
================
String::operator=
================
*/
OG_INLINE String &String::operator=( const char *text ) {
	size_t byteLen, len;
	BothLengths( text, &byteLen, &len );
	SetData(text, byteLen, len );
	return *this;
}
OG_INLINE String &String::operator=( const String &str ) {
	SetData( str.data, str.byteLength, str.length );
	return *this;
}

/*
================
String::operator+=
================
*/
OG_INLINE void String::operator+=( const char *text ) {
	size_t byteLen, len;
	BothLengths( text, &byteLen, &len );
	AppendData( text, byteLen, len );
}
OG_INLINE void String::operator+=( const String &str ) {
	AppendData( str.data, str.byteLength, str.length );
}

/*
================
String::operator+
================
*/
OG_INLINE String String::operator+( const String &str ) {
	String ret(*this);
	ret += str;
	return ret;
}
OG_INLINE String String::operator+( const char *text ) {
	String ret(*this);
	ret += text;
	return ret;
}
OG_INLINE String operator+( const char *text, const String &str ) {
	String ret(text);
	ret += str;
	return ret;
}

}

#endif
