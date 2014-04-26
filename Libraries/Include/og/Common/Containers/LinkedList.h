// ==============================================================================
//! @file
//! @brief	Linked Lists :P
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2007-2010 Lusito Software
// ==============================================================================
//
// The Open Game Libraries.
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// ==============================================================================

#ifndef __OG_LINKEDLIST_H__
#define __OG_LINKEDLIST_H__

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	// ==============================================================================
	//! Node for the LinkedList
	// ==============================================================================
	template<class T>
	class Node {
	private:
		Node<T> *prev;	//!< The previous node
		Node<T> *next;	//!< The next node
		template<class> friend class LinkedList;

	public:
		// ==============================================================================
		//! Gets the next node
		//!
		//! @return	The node, NULL if this is the last node
		// ==============================================================================
		Node<T> *GetNext( void ) const { return next; }

		// ==============================================================================
		//! Gets the previous node
		//!
		//! @return	The node, NULL if this is the first node
		// ==============================================================================
		Node<T> *GetPrev( void ) const { return prev; }

		T		value;	//!< The value
	};

	// ==============================================================================
	//! Linked list
	// ==============================================================================
	template<class T>
	class LinkedList {
	public:
		// ==============================================================================
		//! Default constructor
		// ==============================================================================
		LinkedList();

		// ==============================================================================
		//! Copy Constructor
		//!
		//! @param	other	The list to copy from
		// ==============================================================================
		LinkedList( const LinkedList<T> &other );

		// ==============================================================================
		//! Destructor
		// ==============================================================================
		~LinkedList();

		typedef Node<T> nodeType; //!< The node type

		// ==============================================================================
		//! Definition for a comparision function without extra parameters
		// ==============================================================================
		typedef int ( *cmpFunc_t )( const T &a, const T &b );

		// ==============================================================================
		//! Definition for a comparision function with extra parameters
		// ==============================================================================
		typedef int ( *cmpFuncEx_t )( const T &a, const T &b, void *param );

		// ==============================================================================
		//! Clear all entries
		// ==============================================================================
		void		Clear( void );

		// ==============================================================================
		//! Checks to see if list is empty
		//!
		//! @return	true if empty, false if not
		// ==============================================================================
		bool		IsEmpty( void ) const;

		// ==============================================================================
		//! Get the number of entries
		//!
		//! @return	The current number of entries
		// ==============================================================================
		int			Num( void ) const;

		// ==============================================================================
		//! Add a new value to the start of the list
		//!
		//! @param	value	The value to add
		// ==============================================================================
		void		AddToStart( const T &value );

		// ==============================================================================
		//! Add a new value to the end of the list
		//!
		//! @param	value	The value to add
		// ==============================================================================
		void		AddToEnd( const T &value );

		// ==============================================================================
		//! Insert a value before the specified node
		//!
		//! @param	node	The node to insert before ( if NULL it behaves like AddtoStart )
		//! @param	value	The value to insert
		// ==============================================================================
		void		InsertBefore( nodeType *node, const T &value );

		// ==============================================================================
		//! Insert a value after the specified node
		//!
		//! @param	node	The node to insert after ( if NULL it behaves like AddtoEnd )
		//! @param	value	The value to insert
		// ==============================================================================
		void		InsertAfter( nodeType *node, const T &value );

		// ==============================================================================
		//! Create a new node at the specified node
		//!
		//! @param	baseNode	The base node, can be NULL
		//! @param	before		Set to true to insert it before the base node,
		//!						otherwise it will be inserted after the base node
		//!
		//! @return	A reference to the new value
		// ==============================================================================
		T &			Alloc( nodeType *baseNode=OG_NULL, bool before=false );

		// ==============================================================================
		//! Remove a node
		//!
		//! @param	node	The node to remove
		// ==============================================================================
		void		Remove( nodeType *node );

		// ==============================================================================
		//! Copy data from an existing list
		//!
		//! @param	other	The list to copy from
		// ==============================================================================
		void		Copy( const LinkedList<T> &other );

		// ==============================================================================
		//! Find a node by its value
		//!
		//! @param	value	The value
		//!
		//! @return	NULL if the value was not found, otherwise the node
		// ==============================================================================
		nodeType *	Find( const T &value ) const;

		// ==============================================================================
		//! Find a node by its value's address
		//!
		//! @param	value	The address of the value
		//!
		//! @return	NULL if the value was not found, otherwise the node
		// ==============================================================================
		nodeType *	FindByAddress( const T *value ) const;
		
		// ==============================================================================
		//! Sort the list and (optional) remove duplicates
		//!
		//! @param	compare		The comparision function
		//! @param	removeDupes	true to remove duplicates
		// ==============================================================================
		void		Sort( cmpFunc_t compare, bool removeDupes );

		// ==============================================================================
		//! Sort the list by specifying a parameter and (optional) remove duplicates
		//!
		//! @param	compare		The comparision function
		//! @param	param		The parameter to pass to the comparision function
		//! @param	removeDupes	true to remove duplicates
		// ==============================================================================
		void		SortEx( cmpFuncEx_t compare, void *param, bool removeDupes );

	// operators
		// ==============================================================================
		//! Copy all entries from the other list
		//!
		//! @param	other	The other list
		//!
		//! @return	A reference to this object
		// ==============================================================================
		LinkedList<T> &operator=( const LinkedList<T> &other );

		// ==============================================================================
		//! Get the first node
		//!
		//! @return	The first node, NULL if empty
		// ==============================================================================
		nodeType *	GetFirstNode( void ) const { return start; }

		// ==============================================================================
		//! Get the last node
		//!
		//! @return	The last node, NULL if empty
		// ==============================================================================
		nodeType *	GetLastNode( void ) const { return end; }

	protected:
		int			num;	//!< Number of items
		nodeType *	start;	//!< Pointer to the list start
		nodeType *	end;	//!< Pointer to the list end

		// ==============================================================================
		//! Comparison callback
		//!
		//! @param	context	A pointer to the CompareData object
		//! @param	a		A pointer to the first object
		//! @param	b		A pointer to the second object
		//!
		//! @return	0 if the objects match, otherwise positive or negative
		// ==============================================================================
		static int	CompareCallback( void *context, const void *a, const void *b );

		// ==============================================================================
		//! Comparison callback
		//!
		//! @param	context	A pointer to the CompareDataEx object
		//! @param	a	A pointer to the first object
		//! @param	b	A pointer to the second object
		//!
		//! @return	0 if the objects match, otherwise positive or negative
		// ==============================================================================
		static int	CompareCallbackEx( void *context, const void *a, const void *b );
		
		// ==============================================================================
		//! Comparison data
		// ==============================================================================
		class CompareData {
		public:
			CompareData( cmpFunc_t f ) : func(f) {}

			cmpFunc_t	func;		//!< The normal comparison function
		};

		// ==============================================================================
		//! Comparison data with parameter
		// ==============================================================================
		class CompareDataEx {
		public:
			CompareDataEx( cmpFuncEx_t f, void *p ) : func(f), param(p) {}

			cmpFuncEx_t	func;		//!< The extended comparison function
			void		*param;		//!< The parameter ( for the extended comparison )
		};
	};
//! @}
}

#endif
