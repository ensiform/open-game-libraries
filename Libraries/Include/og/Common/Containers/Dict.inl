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
Dict::Set
================
*/
OG_INLINE void Dict::Set( const char *key, const String &value ) {
	Set( key, value.c_str() );
}
OG_INLINE void Dict::Set( const char *key, const Format &value ) {
	Set( key, value.c_str() );
}
template<class T>
void Dict::Set( const char *key, const T &value ) {
	Set( key, (Format() << value).c_str() );
}
template<class T>
bool Dict::Get( const char *key, const char *defaultValue, T &out ) const {
	int index = Find( key );
	if ( index == -1 ) {
		out = StringType( defaultValue );
		return false;
	}
	out = StringType( entries[index].GetValue().c_str() );
	return true;
}

/*
================
Dict::operator[]
================
*/
OG_INLINE StringType Dict::operator[]( const char *key ) const {
	return Get(key, "");
}
OG_INLINE StringType Dict::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index <= entries.Num() );
	return StringType( entries[index].GetValue().c_str() );
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

}

#endif
