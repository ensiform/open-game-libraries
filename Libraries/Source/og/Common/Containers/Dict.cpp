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

#include <og/Common.h>

namespace og {

/*
==============================================================================

  Dict

==============================================================================
*/

TLS<StringPool> Dict::keyPool;
TLS<StringPool> Dict::valuePool;

/*
================
Dict::Clear
================
*/
void Dict::Clear( void ) {
	int num = entries.Num();
	for( int i=0; i<num; i++ ) {
		keyPool->Free(entries[i].key);
		valuePool->Free(entries[i].value);
	}
	entries.Clear();
	hashIndex.Clear();
}

/*
================
Dict::Remove
================
*/
void Dict::Remove( const char *key ) {
	int hash = hashIndex.GenerateKey( key, false );
	for( int i=hashIndex.First(hash); i!=-1; i=hashIndex.Next() ) {
		if ( entries[i].GetKey().Icmp( key ) == 0 ) {
			keyPool->Free( entries[i].key );
			valuePool->Free( entries[i].value );
			entries.Remove( i );
			hashIndex.Remove( hash, i );
			return;
		}
	}
}

/*
================
Dict::Copy
================
*/
void Dict::Copy( const Dict &other ) {
	Clear();
	int num = other.entries.Num();
	for ( int i = 0; i < num; i++ )
		Set( other.entries[i].key->c_str(), other.entries[i].value->c_str() );
}

/*
================
Dict::Append
================
*/
void Dict::Append( const Dict &other, bool overWrite ) {
	int num = other.entries.Num();
	for ( int i = 0; i < num; i++ ) {
		if ( overWrite || Find( other.entries[i].key->c_str() ) == -1 )
			Set( other.entries[i].key->c_str(), other.entries[i].value->c_str() );
	}
}

/*
================
Dict::Set
================
*/
void Dict::Set( const char *key, const char *value ) {
	if ( key == NULL || key[0] == '\0' )
		return;

	int i = Find( key );
	if ( i == -1 ) {
		KeyValue kv;
		kv.key = keyPool->Alloc( key );
		kv.value = valuePool->Alloc( value );
		hashIndex.Add( hashIndex.GenerateKey( kv.key->c_str(), false ), entries.Num() );
		entries.Append( kv );
	} else {
		const PoolString *oldValue = entries[i].value;
		entries[i].value = valuePool->Alloc( value );
		valuePool->Free( oldValue );
	}
}

/*
================
Dict::Get
================
*/
StringType Dict::Get( const char *key, const char *defaultValue ) const {
	int index = Find( key );
	if ( index == -1 )
		return StringType( defaultValue );
	return entries[index].GetValue();
}

/*
================
Dict::Find
================
*/
int Dict::Find( const char *key ) const {
	if ( key == NULL || key[0] == '\0' )
		return -1;

	int hash = hashIndex.GenerateKey( key, false );
	for ( int i = hashIndex.First( hash ); i != -1; i = hashIndex.Next() ) {
		if ( entries[i].key->Icmp( key ) == 0 )
			return i;
	}

	return -1;
}

/*
================
Dict::MatchPrefix
================
*/
int Dict::MatchPrefix( const char *prefix, int length, int prevMatch ) const {
	if ( prefix == NULL || prefix[0] == '\0' )
		return -1;

	int num = entries.Num();
	for( int i=prevMatch+1; i<num; i++ ) {
		if ( !entries[i].key->Icmpn( prefix, length ) )
			return i;
	}
	return -1;
}

/*
================
Dict::ReadFromFile
================
*/
void Dict::ReadFromFile( File *file ) {
	Clear();

	String key, val;
	int n = file->ReadInt();
	for ( int i = 0; i < n; i++ ) {
		key.ReadFromFile( file );
		val.ReadFromFile( file );
		Set( key.c_str(), val.c_str() );
	}
}

/*
================
Dict::WriteToFile
================
*/
void Dict::WriteToFile( File *file ) const {
	int n = entries.Num();
	file->WriteInt(n);
	for ( int i = 0; i < n; i++ ) {
		entries[i].key->WriteToFile( file );
		entries[i].value->WriteToFile( file );
	}
}

}
