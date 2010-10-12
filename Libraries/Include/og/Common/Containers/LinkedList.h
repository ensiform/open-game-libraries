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

#ifndef __OG_LINKEDLIST_H__
#define __OG_LINKEDLIST_H__

namespace og {
	/*
	==============================================================================

	  Node

	==============================================================================
	*/
	template<class type>
	class Node {
	private:
		Node<type> *prev;
		Node<type> *next;
		template< class t > friend class LinkedList;

	public:
		Node<type> *GetNext( void ) const { return next; }
		Node<type> *GetPrev( void ) const { return prev; }

		type		value;
	};

	/*
	==============================================================================

	  LinkedList

	==============================================================================
	*/
	template<class type>
	class LinkedList {
	public:
		LinkedList( void );
		LinkedList( const LinkedList<type> &other );
		~LinkedList();

		typedef Node<type> nodeType;
		typedef int ( *cmpFunc_t )( const type &, const type & );
		typedef int ( *cmpFuncEx_t )( const type &, const type &, void *param );

		void		Clear( void );							// Clear All Entries

		bool		IsEmpty( void ) const;					// Checks to see if list is empty

		int			Num( void ) const { return num; }		// Number of entries

		void		AddToStart( const type &value );
		void		AddToEnd( const type &value );
		void		InsertBefore( nodeType *node, const type &value );
		void		InsertAfter( nodeType *node, const type &value );

		type		&Alloc( nodeType *baseNode=NULL, bool before=false );		// Create a new node
		void		Remove( nodeType *node );						// Remove a node

		void		Copy( const LinkedList<type> &other );			// Copy data from an existing list

		nodeType *	Find( const type &value ) const;				// Find a node
		nodeType *	FindByAddress( const type *value ) const;		// Find a node by its values address
		void		Sort( cmpFunc_t compare, bool removeDupes );	// Sort the list and (optional) remove duplicates
		void		SortEx( cmpFuncEx_t compare, void *param, bool removeDupes ); // Sort the list and (optional) remove duplicates

		// operators
		LinkedList<type> &operator=( const LinkedList<type> &other );	// Copy data from an existing list

		nodeType *	GetFirstNode( void ) const { return start; }	// Get the first node
		nodeType *	GetLastNode( void ) const { return end; }		// Get the last node

	protected:
		int			num;				// Number of items
		nodeType *	start;				// Pointer to the list start
		nodeType *	end;				// Pointer to the list end

		static int	CompareCallback( const void *a, const void *b );
		static int	CompareCallbackEx( const void *a, const void *b );
		
		struct cmpData_t {
			cmpFunc_t	func;
			cmpFuncEx_t	funcEx;
			void		*param;
		};
		static TLS<cmpData_t> cmpData;
	};
}

#endif
