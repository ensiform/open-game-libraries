/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Dictionary
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

#ifndef __OG_DICT_INL__
#define __OG_DICT_INL__

namespace og {

/*
==============================================================================

  Dict

==============================================================================
*/

/*
================
Dict::Dict
================
*/
OG_INLINE Dict::Dict() {
	SetGranularity(16);
}

/*
================
Dict::SetGranularity
================
*/
OG_INLINE void Dict::SetGranularity( int granularity ) {
	entries.SetGranularity( granularity );
}

/*
================
Dict::Num
================
*/
OG_INLINE int Dict::Num( void ) const {
	return entries.Num();
}

/*
================
Dict::GetKeyValue
================
*/
OG_INLINE const KeyValue *Dict::GetKeyValue( int index ) const {
	OG_ASSERT( index >= 0 && index <= entries.Num() );
	return &entries[index];
}

/*
================
Dict::SetBool
================
*/
OG_INLINE void Dict::SetBool( const char *key, bool value ) {
	SetString( key, value?"1":"0" );
}

/*
================
Dict::GetBool
================
*/
OG_INLINE bool Dict::GetBool( const char *key, const char *defaultValue ) const {
	return String::ToInt( GetString(key, defaultValue) ) != 0;
}
OG_INLINE bool Dict::GetBool( const char *key, const char *defaultValue, bool &out ) const {
	const char *s;
	bool found = GetString( key, defaultValue, &s );
	out = String::ToInt( s ) != 0;
	return found;
}

/*
================
Dict::SetInt
================
*/
OG_INLINE void Dict::SetInt( const char *key, int value ) {
	SetString( key, Format() << value );
}

/*
================
Dict::GetInt
================
*/
OG_INLINE int Dict::GetInt( const char *key, const char *defaultValue ) const {
	return String::ToInt( GetString(key, defaultValue) );
}
OG_INLINE bool Dict::GetInt( const char *key, const char *defaultValue, int &out ) const {
	const char *s;
	bool found = GetString( key, defaultValue, &s );
	out = String::ToInt( s );
	return found;
}

/*
================
Dict::SetFloat
================
*/
OG_INLINE void Dict::SetFloat( const char *key, float value ) {
	SetString( key, Format() << value );
}

/*
================
Dict::GetFloat
================
*/
OG_INLINE float Dict::GetFloat( const char *key, const char *defaultValue ) const {
	return String::ToFloat( GetString(key, defaultValue) );
}
OG_INLINE bool Dict::GetFloat( const char *key, const char *defaultValue, float &out ) const {
	const char *s;
	bool found = GetString( key, defaultValue, &s );
	out = String::ToFloat( s );
	return found;
}

/*
================
Dict::SetVec2
================
*/
OG_INLINE void Dict::SetVec2( const char *key, const Vec2 &value ) {
	SetString( key, Format() << value );
}
OG_INLINE void Dict::SetVec2( const char *key, float x, float y ) {
	SetVec2( key, Vec2( x, y ) );
}

/*
================
Dict::GetVec2
================
*/
OG_INLINE Vec2 Dict::GetVec2( const char *key, const char *defaultValue ) const {
	Vec2 out;
	GetVec2(key, defaultValue, out);
	return out;
}
OG_INLINE bool Dict::GetVec2( const char *key, const char *defaultValue, Vec2 &out ) const {
	if ( !defaultValue )
		defaultValue = "0 0";

	const char *s;
	bool found = GetString( key, defaultValue, &s );
	out.Zero();
	String::ToFloatArray( s, &out.x, 2 );
	return found;
}

/*
================
Dict::SetVec3
================
*/
OG_INLINE void Dict::SetVec3( const char *key, const Vec3 &value ) {
	SetString( key, Format() << value );
}
OG_INLINE void Dict::SetVec3( const char *key, float x, float y, float z ) {
	SetVec3( key, Vec3( x, y, z ) );
}

/*
================
Dict::GetVec3
================
*/
OG_INLINE Vec3 Dict::GetVec3( const char *key, const char *defaultValue ) const {
	Vec3 out;
	GetVec3(key, defaultValue, out);
	return out;
}
OG_INLINE bool Dict::GetVec3( const char *key, const char *defaultValue, Vec3 &out ) const {
	if ( !defaultValue )
		defaultValue = "0 0 0";

	const char *s;
	bool found = GetString( key, defaultValue, &s );
	out.Zero();
	String::ToFloatArray( s, &out.x, 3 );
	return found;
}

/*
================
Dict::SetVec4
================
*/
OG_INLINE void Dict::SetVec4( const char *key, const Vec4 &value ) {
	SetString( key, Format() << value );
}
OG_INLINE void Dict::SetVec4( const char *key, float x, float y, float z, float w ) {
	SetVec4( key, Vec4( x, y, z, w ) );
}

/*
================
Dict::GetVec4
================
*/
OG_INLINE Vec4 Dict::GetVec4( const char *key, const char *defaultValue ) const {
	Vec4 out;
	GetVec4(key, defaultValue, out);
	return out;
}
OG_INLINE bool Dict::GetVec4( const char *key, const char *defaultValue, Vec4 &out ) const {
	if ( !defaultValue )
		defaultValue = "0 0 0 0";

	const char *s;
	bool found = GetString( key, defaultValue, &s );
	out.Zero();
	String::ToFloatArray( s, &out.x, 4 );
	return found;
}

/*
================
Dict::SetRect
================
*/
OG_INLINE void Dict::SetRect( const char *key, const Rect &value ) {
	SetString( key, Format() << value );
}
OG_INLINE void Dict::SetRect( const char *key, float x, float y, float w, float h ) {
	SetRect( key, Rect( x, y, w, h ) );
}

/*
================
Dict::GetRect
================
*/
OG_INLINE Rect Dict::GetRect( const char *key, const char *defaultValue ) const {
	Rect out;
	GetRect(key, defaultValue, out);
	return out;
}
OG_INLINE bool Dict::GetRect( const char *key, const char *defaultValue, Rect &out ) const {
	if ( !defaultValue )
		defaultValue = "0 0 0 0";

	const char *s;
	bool found = GetString( key, defaultValue, &s );
	out.Zero();
	String::ToFloatArray( s, &out.x, 4 );
	return found;
}

/*
================
Dict::SetColor
================
*/
OG_INLINE void Dict::SetColor( const char *key, const Color &value ) {
	SetString( key, Format() << value );
}
OG_INLINE void Dict::SetColor( const char *key, float r, float g, float b, float a ) {
	SetColor( key, Color( r, g, b, a ) );
}

/*
================
Dict::GetColor
================
*/
OG_INLINE Color Dict::GetColor( const char *key, const char *defaultValue ) const {
	Color out;
	GetColor( key, defaultValue, out );
	return out;
}
OG_INLINE bool Dict::GetColor( const char *key, const char *defaultValue, Color &out ) const {
	if ( !defaultValue )
		defaultValue = "1 1 1 1";

	const char *s;
	bool found = GetString( key, defaultValue, &s );
	out = c_color::white;
	String::ToFloatArray( s, &out.r, 4 );
	return found;
}

/*
================
Dict::SetAngles
================
*/
OG_INLINE void Dict::SetAngles( const char *key, const Angles &value ) {
	SetString( key, Format() << value );
}
OG_INLINE void Dict::SetAngles( const char *key, float pitch, float yaw, float roll ) {
	SetAngles( key, Angles( pitch, yaw, roll ) );
}

/*
================
Dict::GetAngles
================
*/
OG_INLINE Angles Dict::GetAngles( const char *key, const char *defaultValue ) const {
	Angles out;
	GetAngles(key, defaultValue, out);
	return out;
}
OG_INLINE bool Dict::GetAngles( const char *key, const char *defaultValue, Angles &out ) const {
	if ( !defaultValue )
		defaultValue = "0 0 0";

	const char *s;
	bool found = GetString( key, defaultValue, &s );
	out.Zero();
	String::ToFloatArray( s, &out.pitch, 3 );
	return found;
}

/*
================
Dict::SetMat2
================
*/
OG_INLINE void Dict::SetMat2( const char *key, const Mat2 &value ) {
	SetString( key, Format() << value );
}
OG_INLINE void Dict::SetMat2( const char *key, float xx, float xy, float yx, float yy ) {
	SetMat2( key, Mat2( xx, xy, yx, yy ) );
}

/*
================
Dict::GetMat2
================
*/
OG_INLINE Mat2 Dict::GetMat2( const char *key, const char *defaultValue ) const {
	Mat2 out;
	GetMat2(key, defaultValue, out);
	return out;
}
OG_INLINE bool Dict::GetMat2( const char *key, const char *defaultValue, Mat2 &out ) const {
	if ( !defaultValue )
		defaultValue = "1 0 0 1";

	const char *s;
	bool found = GetString( key, defaultValue, &s );
	out.Identity();
	String::ToMat2( s, out );
	return found;
}

/*
================
Dict::SetMat3
================
*/
OG_INLINE void Dict::SetMat3( const char *key, const Mat3 &value ) {
	SetString( key, Format() << value );
}
OG_INLINE void Dict::SetMat3( const char *key, float xx, float xy, float xz, float yx, float yy, float yz, float zx, float zy, float zz ) {
	SetMat3( key, Mat3( xx, xy, xz, yx, yy, yz, zx, zy, zz ) );
}

/*
================
Dict::GetMat3
================
*/
OG_INLINE Mat3 Dict::GetMat3( const char *key, const char *defaultValue ) const {
	Mat3 out;
	GetMat3(key, defaultValue, out);
	return out;
}
OG_INLINE bool Dict::GetMat3( const char *key, const char *defaultValue, Mat3 &out ) const {
	if ( !defaultValue )
		defaultValue = "1 0 0 0 1 0 0 0 1";

	const char *s;
	bool found = GetString( key, defaultValue, &s );
	out.Identity();
	String::ToMat3( s, out );
	return found;
}

/*
================
Dict::operator=
================
*/
OG_INLINE Dict &Dict::operator=( const Dict &other ) {
	Copy( other );
	return *this;
}


/*
==============================================================================

  DictEx

==============================================================================
*/

/*
================
DictEx::DictEx
================
*/
template<class type>
OG_INLINE DictEx<type>::DictEx() {
	SetGranularity(16);
}

/*
================
DictEx::Clear
================
*/
template<class type>
void DictEx<type>::Clear( void ) {
	entries.Clear();
	names.Clear();
	hashIndex.Clear();
}

/*
================
DictEx::SetGranularity
================
*/
template<class type>
OG_INLINE void DictEx<type>::SetGranularity( int granularity ) {
	entries.SetGranularity( granularity );
	names.SetGranularity( granularity );
}

/*
================
DictEx::Num
================
*/
template<class type>
OG_INLINE int DictEx<type>::Num( void ) const {
	return entries.Num();
}

/*
================
DictEx::Remove
================
*/
template<class type>
void DictEx<type>::Remove( const char *key ) {
	int hash = hashIndex.GenerateKey( key, false );
	for( int i=hashIndex.First(hash); i!=-1; i=hashIndex.Next() ) {
		if ( names[i].Icmp( key ) == 0 ) {
			entries.Remove( i );
			names.Remove( i );
			hashIndex.Remove( hash, i );
			return;
		}
	}
}

/*
================
DictEx::Remove
================
*/
template<class type>
void DictEx<type>::Remove( int index ) {
	OG_ASSERT( index >= 0 && index <= entries.Num() );

	hashIndex.Remove( hashIndex.GenerateKey( names[index].c_str(), false ), index );
	entries.Remove( index );
	names.Remove( index );
}

/*
================
DictEx::Copy
================
*/
template<class type>
void DictEx<type>::Copy( const DictEx<type> &other ) {
	Clear();
	int num = other.entries.Num();
	for ( int i = 0; i < num; i++ ) {
		const String &key = other.GetKey(i);
		hashIndex.Add( hashIndex.GenerateKey( key.c_str(), false ), entries.Num() );
		names.Append( key );
		entries.Append( other[i] );
	}
}

/*
================
DictEx::Append
================
*/
template<class type>
void DictEx<type>::Append( const DictEx<type> &other, bool overWrite ) {
	const char *key;
	int index;
	int num = other.entries.Num();
	for ( int i = 0; i < num; i++ ) {
		const String &key = other.GetKey(i);
		index = Find( key.c_str() );
		if ( index != -1 ) {
			if ( overWrite )
				entries[index] = other[i];
			continue;
		}
		hashIndex.Add( hashIndex.GenerateKey( key.c_str(), false ), entries.Num() );
		names.Append( key );
		entries.Append( other[i] );
	}
}

/*
================
DictEx::GetKey
================
*/
template<class type>
OG_INLINE const String &DictEx<type>::GetKey( int index ) const {
	OG_ASSERT( index >= 0 && index <= entries.Num() );
	return names[index];
}

/*
================
DictEx::Find
================
*/
template<class type>
int DictEx<type>::Find( const char *key ) const {
	if ( key == NULL || key[0] == '\0' )
		return -1;

	int hash = hashIndex.GenerateKey( key, false );
	for ( int i = hashIndex.First( hash ); i != -1; i = hashIndex.Next() ) {
		if ( names[i].Icmp( key ) == 0 )
			return i;
	}

	return -1;
}

/*
================
DictEx::FindByValue
================
*/
template<class type>
int DictEx<type>::FindByValue( const type &value ) const {
	return entries.Find( value );
}

/*
================
DictEx::MatchPrefix
================
*/
template<class type>
int DictEx<type>::MatchPrefix( const char *prefix, int length, int prevMatch ) const {
	if ( prefix == NULL || prefix[0] == '\0' )
		return -1;

	int num = entries.Num();
	for( int i=prevMatch+1; i<num; i++ ) {
		if ( !names[i].Icmpn( prefix, length ) )
			return i;
	}
	return -1;
}

/*
================
DictEx::operator[const char *]
================
*/
template<class type>
OG_INLINE const type &DictEx<type>::operator[]( const char *key ) const {
	int index = Find( key );
	if ( index != -1 )
		return entries[index];

	// Not found, create one
	hashIndex.Add( hashIndex.GenerateKey( key, false ), entries.Num() );
	names.Append(key);
	return entries.Alloc();
}
template<class type>
OG_INLINE type &DictEx<type>::operator[]( const char *key ) {
	int index = Find( key );
	if ( index != -1 )
		return entries[index];

	// Not found, create one
	hashIndex.Add( hashIndex.GenerateKey( key, false ), entries.Num() );
	names.Append(key);
	return entries.Alloc();
}

/*
================
DictEx::operator[int]
================
*/
template<class type>
OG_INLINE const type &DictEx<type>::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index <= entries.Num() );
	return entries[index];
}
template<class type>
OG_INLINE type &DictEx<type>::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index <= entries.Num() );
	return entries[index];
}

/*
================
DictEx::operator=
================
*/
template<class type>
OG_INLINE DictEx<type> &DictEx<type>::operator=( const DictEx<type> &other ) {
	Copy( other );
	return *this;
}


/*
==============================================================================

  LanguageDict

==============================================================================
*/

/*
================
LanguageDict::GetString
================
*/
OG_INLINE const char *LanguageDict::GetString( const char *key ) {
	int index = Find( key );
	if ( index != -1 )
		return entries[index].c_str();

	// Not found, create new entry and return it
	hashIndex.Add( hashIndex.GenerateKey( key, false ), entries.Num() );
	names.Append(key);
	String &entry = entries.Alloc();
	entry = key;

	return entry.c_str();
}

}

#endif
