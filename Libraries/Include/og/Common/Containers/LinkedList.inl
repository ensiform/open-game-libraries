/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Linked Lists :P
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

#ifndef __OG_LINKEDLIST_INL__
#define __OG_LINKEDLIST_INL__

namespace og {

/*
==============================================================================

  LinkedList

==============================================================================
*/
/*
================
LinkedList::LinkedList
================
*/
template<class T>
OG_INLINE LinkedList<T>::LinkedList( void ) {
	start		= OG_NULL;
	end			= OG_NULL;
	num			= 0;
}

/*
================
LinkedList::LinkedList
================
*/
template<class T>
OG_INLINE LinkedList<T>::LinkedList( const LinkedList<T> &other ) {
	start		= OG_NULL;
	end			= OG_NULL;
	Copy(other);
}

/*
================
LinkedList::~LinkedList
================
*/
template<class T>
OG_INLINE LinkedList<T>::~LinkedList() {
	Clear();
}

/*
================
LinkedList::Clear
================
*/
template<class T>
void LinkedList<T>::Clear( void ) {
	if ( start ) {
		nodeType *node = start;
		nodeType *nextNode;
		while( node ) {
			nextNode = node->next;
			delete node;
			node = nextNode;
		}
		start = OG_NULL;
		end = OG_NULL;
	}
	num		= 0;
}

/*
================
LinkedList::IsEmpty

Checks to see if list is empty
================
*/
template<class T>
bool LinkedList<T>::IsEmpty( void ) const {
	return num == 0;
}

/*
================
LinkedList::Num
================
*/
template<class T>
int LinkedList<T>::Num( void ) const {
	return num;
}

/*
================
LinkedList::Copy

Clear the list and
copy all items from the other list.
================
*/
template<class T>
OG_INLINE void LinkedList<T>::Copy( const LinkedList<T> &other ) {
	Clear();
	for( nodeType *node = other.start; node != OG_NULL; node = node->next )
		AddToEnd(node->value);
}

/*
================
LinkedList::operator=

Synonym for Copy
================
*/
template<class T>
OG_INLINE LinkedList<T> &LinkedList<T>::operator=( const LinkedList<T> &other ) {
	Copy(other);
	return *this;
}

/*
================
LinkedList::AddToStart

Insert an Item to the start of the list.
================
*/
template<class T>
void LinkedList<T>::AddToStart( const T &value ) {
	Alloc( OG_NULL, true ) = value;
}

/*
================
LinkedList::AddToEnd

Insert an Item to the end of the list.
================
*/
template<class T>
void LinkedList<T>::AddToEnd( const T &value ) {
	Alloc() = value;
}

/*
================
LinkedList::InsertBefore

Insert an Item right before the specified one.
================
*/
template<class T>
void LinkedList<T>::InsertBefore( nodeType *node, const T &value ) {
	Alloc( node, true ) = value;
}

/*
================
LinkedList::InsertAfter

Insert an item right after the specified one.
================
*/
template<class T>
void LinkedList<T>::InsertAfter( nodeType *node, const T &value ) {
	Alloc( node, false ) = value;
}

/*
================
LinkedList::Alloc

Allocate an item at the specified position.
================
*/
template<class T>
T &LinkedList<T>::Alloc( nodeType *baseNode, bool before ) {
	nodeType *newNode = new nodeType;
	if ( start == OG_NULL ) {
		start = end = newNode;
		start->prev = OG_NULL;
		start->next = OG_NULL;
	} else {
		if ( baseNode == OG_NULL )
			baseNode = before ? start : end;

		if ( before ) {
			newNode->prev = baseNode->prev;
			newNode->next = baseNode;
			baseNode->prev = newNode;
			if ( baseNode == start )
				start = newNode;
		} else {
			newNode->prev = baseNode;
			newNode->next = baseNode->next;
			baseNode->next = newNode;
			if ( baseNode == end )
				end = newNode;
		}
	}
	num++;
	return newNode->value;
}

/*
================
LinkedList::Remove

Removes an item from the list.
================
*/
template<class T>
void LinkedList<T>::Remove( nodeType *node ) {
	OG_ASSERT( node != OG_NULL );

	if ( node == start ) {
		if ( node == end )
			start = end = OG_NULL;
		else {
			start = node->next;
			start->prev = OG_NULL;
		}
	} else if ( node == end ) {
		end = node->prev;
		end->next = OG_NULL;
	} else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	delete node;
	num--;
}

/*
================
LinkedList::Find

Returns the index of the searched item.
-1 if no item with this value was found.
================
*/
template<class T>
Node<T> *LinkedList<T>::Find( const T &value ) const {
	nodeType *node = start;
	for( int i=0; node != OG_NULL; i++ ) {
		if ( node->value == value )
			return node;
		node = node->next;
	}
	return OG_NULL;
}

/*
================
LinkedList::FindByAddress

Returns the index of the searched item.
-1 if no item with this value was found.
================
*/
template<class T>
Node<T> *LinkedList<T>::FindByAddress( const T *value ) const {
	nodeType *node = start;
	for( int i=0; node != OG_NULL; i++ ) {
		if ( &node->value == value )
			return node;
		node = node->next;
	}
	return OG_NULL;
}

/*
================
LinkedList::Sort

Sorts the list and removes duplicates.
================
*/
template<class T>
void LinkedList<T>::Sort( cmpFunc_t compare, bool removeDupes ) {
	if ( num < 2 )
		return;
	
	nodeType **list = new nodeType*[num];
	int i = 0;
	for( nodeType *node=start; node; node=node->next, i++ )
		list[i] = node;

	CompareData cmpData( this, compare );
	QuickSort( list, num, sizeof( nodeType * ), &cmpData, CompareCallback );

	for( i=1; i<num; i++ ) {
		list[i]->prev = list[i-1];
		list[i-1]->next = list[i];
	}

	start = list[0];
	end = list[num-1];

	start->prev = OG_NULL;
	end->next = OG_NULL;

	delete[] list;
	list = OG_NULL;

	if ( removeDupes ) {
		for( nodeType *node=start; node->next; node=node->next ) {
			if ( compare( node->value, node->next->value ) == 0 )
				Remove( node->next );
		}
	}
}

/*
================
LinkedList::SortEx

Sorts the list and removes duplicates.
================
*/
template<class T>
void LinkedList<T>::SortEx( cmpFuncEx_t compare, void *param, bool removeDupes ) {
	if ( num < 2 )
		return;
	
	nodeType **list = new nodeType*[num];
	int i = 0;
	for( nodeType *node=start; node; node=node->next, i++ )
		list[i] = node;

	CompareDataEx cmpData( this, compare, param );
	QuickSort( list, num, sizeof( nodeType * ), &cmpData, CompareCallbackEx );

	for( i=1; i<num; i++ ) {
		list[i]->prev = list[i-1];
		list[i-1]->next = list[i];
	}

	start = list[0];
	end = list[num-1];

	start->prev = OG_NULL;
	end->next = OG_NULL;

	delete[] list;
	list = OG_NULL;

	if ( removeDupes ) {
		for( nodeType *node=start; node->next; node=node->next ) {
			if ( compare( node->value, node->next->value ) == 0 )
				Remove( node->next );
		}
	}
}

/*
================
LinkedList::CompareCallback
================
*/
template<class T>
int LinkedList<T>::CompareCallback( void *context, const void *a, const void *b ) {
	const nodeType *pa = *(const nodeType **)a;
	const nodeType *pb = *(const nodeType **)b;
	CompareData *cmpData = reinterpret_cast<CompareData *>(context);
	return cmpData->func( pa->value, pb->value );
}

/*
================
LinkedList::CompareCallbackEx
================
*/
template<class T>
int LinkedList<T>::CompareCallbackEx( void *context, const void *a, const void *b ) {
	const nodeType *pa = *(const nodeType **)a;
	const nodeType *pb = *(const nodeType **)b;
	CompareDataEx *cmpData = reinterpret_cast<CompareDataEx *>(context);
	return cmpData->func( pa->value, pb->value, cmpData->param );
}

}

#endif
