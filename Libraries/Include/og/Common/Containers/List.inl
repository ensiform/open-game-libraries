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

/*
================
List::List
================
*/
template<class T>
OG_INLINE List<T>::List( int _granularity ) {
	list		= NULL;
	granularity = _granularity;
	Clear();
}

/*
================
List::List
================
*/
template<class T>
OG_INLINE List<T>::List( const List<T> &other ) {
	list = NULL;
	granularity = other.granularity;
	Copy(other);
}

/*
================
List::~List
================
*/
template<class T>
OG_INLINE List<T>::~List() {
	Clear();
}

/*
================
List::Clear
================
*/
template<class T>
void List<T>::Clear( void ) {
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
template<class T>
bool List<T>::IsEmpty( void ) const {
	return num == 0 || list == NULL;
}

/*
================
List::Num
================
*/
template<class T>
int List<T>::Num( void ) const {
	return num;
}

/*
================
List::AllocSize
================
*/
template<class T>
int List<T>::AllocSize( void ) const {
	return size;
}

/*
================
List::SetGranularity
================
*/
template<class T>
void List<T>::SetGranularity( int granularity ) {
	this->granularity = granularity;
}

/*
================
List::GetGranularity
================
*/
template<class T>
int List<T>::GetGranularity( void ) const {
	return granularity;
}

/*
================
List::operator[]
================
*/
template<class T>
OG_INLINE const T& List<T>::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index <= num );
	return list[index];
}
template<class T>
OG_INLINE T& List<T>::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index <= num );
	return list[index];
}

/*
================
List::operator+=
================
*/
template<class T>
void List<T>::operator+=( T value ) {
	Alloc() = value;
}

/*
================
List::Copy

Clear the list and
copy all items from the other list.
================
*/
template<class T>
OG_INLINE void List<T>::Copy( const List<T> &other ) {
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
template<class T>
OG_INLINE List<T> &List<T>::operator=( const List<T> &other ) {
	Copy(other);
	return *this;
}

/*
================
List::CheckSize

Calls Resize if needed
================
*/
template<class T>
OG_INLINE void List<T>::CheckSize( int newSize, bool keepContent ) {
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
template<class T>
T& List<T>::Alloc( void ) {
	CheckSize( num+1 );
	num++;
	return list[num-1];
}

/*
================
List::Append
================
*/
template<class T>
void List<T>::Append( T value ) {
	Alloc() = value;
}

/*
================
List::Remove

Removes items from the list.
The list size won't shrink.
================
*/
template<class T>
void List<T>::Remove( int index ) {
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
template<class T>
int List<T>::Find( T value ) const {
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
template<class T>
void List<T>::Sort( cmpFunc_t compare, bool removeDupes ) {
	if ( num < 2 )
		return;

	CompareData cmpData( compare );
	QuickSort( list, num, sizeof( T * ), &cmpData, CompareCallback );

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
template<class T>
void List<T>::SortEx( cmpFuncEx_t compare, void *param, bool removeDupes ) {
	if ( num < 2 )
		return;

	CompareDataEx cmpData( compare, param );
	QuickSort( list, num, sizeof( T * ), &cmpData, CompareCallbackEx );

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
template<class T>
int List<T>::CompareCallback( void *context, const void *a, const void *b ) {
	CompareData *cmpData = reinterpret_cast<CompareData *>(context);
	return cmpData->func( *(const T *)a, *(const T *)b );
}

/*
================
ListEx::CompareCallbackEx
================
*/
template<class T>
int List<T>::CompareCallbackEx( void *context, const void *a, const void *b ) {
	CompareDataEx *cmpData = reinterpret_cast<CompareDataEx *>(context);
	return cmpData->func( *(const T *)a, *(const T *)b, cmpData->param );
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
template<class T>
void List<T>::Resize( int newSize, bool keepContent ) {
	OG_ASSERT( newSize > 0 );
	size = static_cast<int>( ceil( static_cast<float>(newSize)/static_cast<float>(granularity) ) ) * granularity;

	T *newList = new T[ size ];

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

/*
================
ListEx::ListEx
================
*/
template<class T>
OG_INLINE ListEx<T>::ListEx( int _granularity ) {
	list		= NULL;
	granularity = _granularity;
	Clear();
}

/*
================
ListEx::ListEx
================
*/
template<class T>
OG_INLINE ListEx<T>::ListEx( const ListEx<T> &other ) {
	list = NULL;
	granularity = other.granularity;
	Copy(other);
}

/*
================
ListEx::~ListEx
================
*/
template<class T>
OG_INLINE ListEx<T>::~ListEx() {
	Clear();
}

/*
================
ListEx::Clear

Clears the list and the items allocated.
================
*/
template<class T>
void ListEx<T>::Clear( void ) {
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
template<class T>
bool ListEx<T>::IsEmpty( void ) const {
	return num == 0 || *list == NULL;
}

/*
================
ListEx::Num
================
*/
template<class T>
int ListEx<T>::Num( void ) const {
	return num;
}

/*
================
ListEx::AllocSize
================
*/
template<class T>
int ListEx<T>::AllocSize( void ) const {
	return size;
}

/*
================
ListEx::SetGranularity
================
*/
template<class T>
void ListEx<T>::SetGranularity( int granularity ) {
	this->granularity = granularity;
}

/*
================
List::GetGranularity
================
*/
template<class T>
int ListEx<T>::GetGranularity( void ) const {
	return granularity;
}

/*
================
ListEx::operator[]
================
*/
template<class T>
OG_INLINE const T& ListEx<T>::operator[]( int index ) const {
	OG_ASSERT( index >= 0 && index <= num );
	return *list[index];
}
template<class T>
OG_INLINE T& ListEx<T>::operator[]( int index ) {
	OG_ASSERT( index >= 0 && index <= num );
	return *list[index];
}

/*
================
ListEx::operator+=
================
*/
template<class T>
void ListEx<T>::operator+=( const T &value ) {
	Alloc() = value;
}

/*
================
ListEx::Copy

Clear the list and
copy all items from the other list.
================
*/
template<class T>
OG_INLINE void ListEx<T>::Copy( const ListEx<T> &other ) {
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
template<class T>
OG_INLINE ListEx<T> &ListEx<T>::operator=( const ListEx<T> &other ) {
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
template<class T>
OG_INLINE void ListEx<T>::CheckSize( int newSize, bool keepContent ) {
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
template<class T>
T& ListEx<T>::Alloc( void ) {
	CheckSize( num+1 );
	list[num] = new T;
	return *list[num++];
}

/*
================
ListEx::Append
================
*/
template<class T>
void ListEx<T>::Append( const T &value ) {
	Alloc() = value;
}

/*
================
ListEx::Remove

Removes items from the list.
The list size won't shrink.
But it frees the actual item data.
================
*/
template<class T>
void ListEx<T>::Remove( int index ) {
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
template<class T>
int ListEx<T>::Find( const T &value ) const {
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
template<class T>
void ListEx<T>::Sort( cmpFunc_t compare, bool removeDupes ) {
	if ( num < 2 )
		return;

	CompareData cmpData( compare );
	QuickSort( list, num, sizeof( T * ), &cmpData, CompareCallback );

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
template<class T>
void ListEx<T>::SortEx( cmpFuncEx_t compare, void *param, bool removeDupes ) {
	if ( num < 2 )
		return;

	CompareDataEx cmpData( compare, param );
	QuickSort( list, num, sizeof( T * ), &cmpData, CompareCallbackEx );

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
template<class T>
int ListEx<T>::CompareCallback( void *context, const void *a, const void *b ) {
	const T *pa = *(const T **)a;
	const T *pb = *(const T **)b;
	CompareData *cmpData = reinterpret_cast<CompareData *>(context);
	return cmpData->func( *pa, *pb );
}

/*
================
ListEx::CompareCallbackEx
================
*/
template<class T>
int ListEx<T>::CompareCallbackEx( void *context, const void *a, const void *b ) {
	const T *pa = *(const T **)a;
	const T *pb = *(const T **)b;
	CompareDataEx *cmpData = reinterpret_cast<CompareDataEx *>(context);
	return cmpData->func( *pa, *pb, cmpData->param );
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
template<class T>
void ListEx<T>::Resize( int newSize, bool keepContent ) {
	OG_ASSERT( newSize > 0 );
	size = static_cast<int>( ceil( static_cast<float>(newSize)/static_cast<float>(granularity) ) ) * granularity;

	T **newList = new T *[ size ];

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
StringList::Append
================
*/
OG_INLINE void StringList::Append( const String &value ) {
	Alloc() = value;
}
OG_INLINE void StringList::Append( const char *value ) {
	Alloc() = value;
}
OG_INLINE void StringList::Append( const char *value, int byteLen, int len ) {
	Alloc().SetData( value, byteLen, len );
}

/*
================
StringList::operator+=
================
*/
OG_INLINE void StringList::operator+=( const char *value ) {
	Alloc() = value;
}

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
