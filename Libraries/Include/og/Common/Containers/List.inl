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

#ifndef __OG_LIST_INL__
#define __OG_LIST_INL__

namespace og {

/*
==============================================================================

  List

==============================================================================
*/

template< class type > TLS<typename List<type>::cmpData_t> List<type>::cmpData;

/*
================
List::List
================
*/
template< class type >
OG_INLINE List<type>::List( int _granularity ) {
	list		= NULL;
	granularity = _granularity;
	Clear();
}

/*
================
List::List
================
*/
template< class type >
OG_INLINE List<type>::List( const List<type> &other ) {
	list = NULL;
	granularity = other.granularity;
	Copy(other);
}

/*
================
List::~List
================
*/
template< class type >
OG_INLINE List<type>::~List() {
	Clear();
}

/*
================
List::Clear
================
*/
template< class type >
void List<type>::Clear( void ) {
	if ( list ) {
		delete[] list;
		list = NULL;
	}
	num		= 0;
	size	= 0;
}

/*
================
List::IsEmpty

Checks to see if list is empty
================
*/
template< class type >
bool List<type>::IsEmpty( void ) const {
	return num == 0 || list == NULL;
}

#if 0 // disabled until we need them and know how we want them to act
/*
================
List::Allocated
================
*/
template< class type >
OG_INLINE size_t List<type>::Allocated( void ) const {
	return size * sizeof( type );
}

/*
================
List::Size
================
*/
template< class type >
OG_INLINE size_t List<type>::Size( void ) const {
	return sizeof( List<type> ) + Allocated();
}

/*
================
List::MemoryUsed
================
*/
template< class type >
OG_INLINE size_t List<type>::MemoryUsed( void ) const {
	return num * sizeof( *list );
}
#endif

/*
================
List::operator[]
================
*/
template< class type >
OG_INLINE const type& List<type>::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index <= num );
	return list[index];
}
template< class type >
OG_INLINE type& List<type>::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index <= num );
	return list[index];
}

/*
================
List::Copy

Clear the list and
copy all items from the other list.
================
*/
template< class type >
OG_INLINE void List<type>::Copy( const List<type> &other ) {
	Clear();
	for( int i=0; i<other.num; i++ )
		Alloc() = other[i];
}

/*
================
List::operator=

Synonym for Copy
================
*/
template< class type >
OG_INLINE List<type> &List<type>::operator=( const List<type> &other ) {
	Copy(other);
	return *this;
}

/*
================
List::CheckSize

Calls Resize if needed
================
*/
template< class type >
OG_INLINE void List<type>::CheckSize( int newSize, bool keepContent ) {
	if ( newSize > size )
		Resize( newSize, keepContent );
}

/*
================
List::Alloc

Returns the first free item
in the list and resizes if needed.
================
*/
template< class type >
type& List<type>::Alloc( void ) {
	CheckSize( num+1 );
	num++;
	return list[num-1];
}

/*
================
List::Remove

Removes items from the list.
The list size won't shrink.
================
*/
template< class type >
void List<type>::Remove( int index ) {
	OG_ASSERT( index >= 0 && index < num );

	for ( int i=index; i<num; i++ ) {
		if ( (i+1) < num )
			list[i] = list[i+1];
	}
	num--;
}

/*
================
List::Find

Returns the index of the searched item.
-1 if no item with this value was found.
================
*/
template< class type >
int List<type>::Find( type value ) const {
	for ( int i=0; i<num; i++ ) {
		if ( list[i] == value )
			return i;
	}
	return -1;
}

/*
================
List::Sort

Sorts the list and removes duplicates.
================
*/
template< class type >
void List<type>::Sort( cmpFunc_t compare, bool removeDupes ) {
	if ( num < 2 )
		return;

	cmpData->func = compare;
	qsort( list, num, sizeof( type * ), CompareCallback );

	if ( removeDupes ) {
		for ( int i=num-2; i>=0; i-- ) {
			if ( compare( list[i], list[i+1] ) == 0 )
				Remove(i+1);
		}
	}
}

/*
================
List::SortEx

Sorts the list and removes duplicates.
================
*/
template< class type >
void List<type>::SortEx( cmpFuncEx_t compare, void *param, bool removeDupes ) {
	if ( num < 2 )
		return;

	cmpData->funcEx = compare;
	cmpData->param = param;
	qsort( list, num, sizeof( type ), CompareCallbackEx );

	if ( removeDupes ) {
		for ( int i=num-2; i>=0; i-- ) {
			if ( compare( list[i], list[i+1], param ) == 0 )
				Remove(i+1);
		}
	}
}

/*
================
List::CompareCallback
================
*/
template< class type >
int List<type>::CompareCallback( const void *a, const void *b ) {
	return cmpData->func( *(const type *)a, *(const type *)b );
}

/*
================
ListEx::CompareCallbackEx
================
*/
template< class type >
int List<type>::CompareCallbackEx( const void *a, const void *b ) {
	return cmpData->funcEx( *(const type *)a, *(const type *)b, cmpData->param );
}

/*
================
List::Resize

Create a new list with n items.
(Checks for granularity)
Copies all of the content if wanted.
Old list will be freed.
================
*/
template< class type >
void List<type>::Resize( int newSize, bool keepContent ) {
	OG_ASSERT( newSize > 0 );
	size = Math::FtoiFast( ceil( static_cast<float>(newSize)/static_cast<float>(granularity) ) ) * granularity;

	type *newList = new type[ size ];

	if ( list ) {
		if ( keepContent ) {
			for ( int i=0; i<num; i++ )
				newList[i] = list[i];
		}
		else
			num = 0;
		delete[] list;
	}
	list = newList;
}

/*
==============================================================================

  ListEx

==============================================================================
*/

template< class type > TLS<typename ListEx<type>::cmpData_t> ListEx<type>::cmpData;

/*
================
ListEx::ListEx
================
*/
template< class type >
OG_INLINE ListEx<type>::ListEx( int _granularity ) {
	list		= NULL;
	granularity = _granularity;
	Clear();
}

/*
================
ListEx::ListEx
================
*/
template< class type >
OG_INLINE ListEx<type>::ListEx( const ListEx<type> &other ) {
	list = NULL;
	granularity = other.granularity;
	Copy(other);
}

/*
================
ListEx::~ListEx
================
*/
template< class type >
OG_INLINE ListEx<type>::~ListEx() {
	Clear();
}

/*
================
ListEx::Clear

Clears the list and the items allocated.
================
*/
template< class type >
void ListEx<type>::Clear( void ) {
	if ( list ) {
		for ( int i=0; i<num; i++ )
			delete list[i];
		delete[] list;
		list = NULL;
	}
	num		= 0;
	size	= 0;
}

/*
================
ListEx::IsEmpty

Checks to see if list is empty
================
*/
template< class type >
bool ListEx<type>::IsEmpty( void ) const {
	return num == 0 || *list == NULL;
}

#if 0 // disabled until we need them and know how we want them to act
/*
================
ListEx::Allocated
================
*/
template< class type >
OG_INLINE size_t ListEx<type>::Allocated( void ) const {
	return size * sizeof( type );
}

/*
================
ListEx::Size
================
*/
template< class type >
OG_INLINE size_t ListEx<type>::Size( void ) const {
	return sizeof( ListEx<type> ) + Allocated();
}

/*
================
ListEx::MemoryUsed
================
*/
template< class type >
OG_INLINE size_t ListEx<type>::MemoryUsed( void ) const {
	return num * sizeof( **list );
}
#endif

/*
================
ListEx::operator[]
================
*/
template< class type >
OG_INLINE const type& ListEx<type>::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index <= num );
	return *list[index];
}
template< class type >
OG_INLINE type& ListEx<type>::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index <= num );
	return *list[index];
}

/*
================
ListEx::Copy

Clear the list and
copy all items from the other list.
================
*/
template< class type >
OG_INLINE void ListEx<type>::Copy( const ListEx<type> &other ) {
	Clear();
	for( int i=0; i<other.num; i++ )
		Alloc() = other[i];
}

/*
================
ListEx::operator=

Synonym for Copy
================
*/
template< class type >
OG_INLINE ListEx<type> &ListEx<type>::operator=( const ListEx<type> &other ) {
	Clear();
	for( int i=0; i<other.num; i++ )
		Alloc() = other[i];

	return *this;
}

/*
================
ListEx::CheckSize

Calls Resize if needed
================
*/
template< class type >
OG_INLINE void ListEx<type>::CheckSize( int newSize, bool keepContent ) {
	if ( newSize > size )
		Resize( newSize, keepContent );
}

/*
================
ListEx::Alloc

Returns the first free item
in the list and resizes if needed.
================
*/
template< class type >
type& ListEx<type>::Alloc( void ) {
	CheckSize( num+1 );
	list[num] = new type;
	return *list[num++];
}

/*
================
ListEx::Remove

Removes items from the list.
The list size won't shrink.
But it frees the actual item data.
================
*/
template< class type >
void ListEx<type>::Remove( int index ) {
	OG_ASSERT( index >= 0 && index < num );

	delete list[index];
	for ( int i=index; i<num; i++ ) {
		if ( (i+1) < num )
			list[i] = list[i+1];
	}
	list[--num] = NULL;
}

/*
================
ListEx::Find

Returns the index of the searched item.
-1 if no item with this value was found.
================
*/
template< class type >
int ListEx<type>::Find( const type &value ) const {
	for ( int i=0; i<num; i++ ) {
		if ( *list[i] == value )
			return i;
	}
	return -1;
}

/*
================
ListEx::Sort

Sorts the list and removes duplicates.
================
*/
template< class type >
void ListEx<type>::Sort( cmpFunc_t compare, bool removeDupes ) {
	if ( num < 2 )
		return;

	cmpData->func = compare;
	qsort( list, num, sizeof( type * ), CompareCallback );

	if ( removeDupes ) {
		for ( int i=num-2; i>=0; i-- ) {
			if ( compare( *list[i], *list[i+1] ) == 0 )
				Remove(i+1);
		}
	}
}

/*
================
ListEx::SortEx

Sorts the list and removes duplicates.
================
*/
template< class type >
void ListEx<type>::SortEx( cmpFuncEx_t compare, void *param, bool removeDupes ) {
	if ( num < 2 )
		return;

	cmpData->funcEx = compare;
	cmpData->param = param;
	qsort( list, num, sizeof( type * ), CompareCallbackEx );

	if ( removeDupes ) {
		for ( int i=num-2; i>=0; i-- ) {
			if ( compare( *list[i], *list[i+1], param ) == 0 )
				Remove(i+1);
		}
	}
}

/*
================
ListEx::CompareCallback
================
*/
template< class type >
int ListEx<type>::CompareCallback( const void *a, const void *b ) {
	const type *pa = *(const type **)a;
	const type *pb = *(const type **)b;
	return cmpData->func( *pa, *pb );
}

/*
================
ListEx::CompareCallbackEx
================
*/
template< class type >
int ListEx<type>::CompareCallbackEx( const void *a, const void *b ) {
	const type *pa = *(const type **)a;
	const type *pb = *(const type **)b;
	return cmpData->funcEx( *pa, *pb, cmpData->param );
}

/*
================
ListEx::Resize

Create a new list with n item pointers.
(Checks for granularity)
Copies all pointer values if wanted.
Otherwise the old data will be freed
Old pointer list will be freed.
================
*/
template< class type >
void ListEx<type>::Resize( int newSize, bool keepContent ) {
	OG_ASSERT( newSize > 0 );
	size = static_cast<int>( ceil( static_cast<float>(newSize)/static_cast<float>(granularity) ) ) * granularity;

	type **newList = new type *[ size ];

	if ( list ) {
		if ( keepContent ) {
			for ( int i=0; i<num; i++ )
				newList[i] = list[i];
		}
		else {
			for ( int i=0; i<num; i++ )
				delete list[i];
			num = 0;
		}
		delete[] list;
	}
	list = newList;
}

/*
==============================================================================

  StringList

==============================================================================
*/

/*
================
StringListCmp
================
*/
OG_INLINE int StringListCmp( const String &a, const String &b ) {
	return a.Cmp(b.c_str());
}

/*
================
StringListICmp
================
*/
OG_INLINE int StringListICmp( const String &a, const String &b ) {
	return a.Icmp(b.c_str());
}

}

#endif
