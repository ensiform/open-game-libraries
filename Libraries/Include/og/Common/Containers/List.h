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

#ifndef __OG_LIST_H__
#define __OG_LIST_H__

namespace og {
	/*
	==============================================================================

	  List -> use for small items

	  Note: Everytime the list gets resized, it will create a new array of items
	  and delete the old one. This is very inefficient for bigger items.

	  Warning: If you decide to use List with a class/struct, which contains
	  dynamic memory, make sure the class has a copy constructor.

	==============================================================================
	*/
	template< class type >
	class List {
	public:
		List( int granularity = 16 );
		List( const List<type> &other );
		virtual ~List();

		typedef int ( *cmpFunc_t )( const type &, const type & );
		typedef int ( *cmpFuncEx_t )( const type &, const type &, void *param );

		void		Clear( void );							// Clear All Entries

		bool		IsEmpty( void ) const;					// Checks to see if list is empty

		int			Num( void ) const { return num; }		// Number of used entries
		int			AllocSize( void ) const { return size; } // Number of allocated items

		void		SetGranularity( int granularity ) { this->granularity = granularity; } // Set the granularity
		int			GetGranularity( void ) { return granularity; } // Get the granularity

#if 0 // disabled until we need them and know how we want them to act
		size_t		Allocated( void ) const;				// Returns total size of allocated memory
		size_t		Size( void ) const;						// Returns total size of allocated memory including size of list type
		size_t		MemoryUsed( void ) const;				// Returns size of the used elements in the list
#endif

		type&		Alloc( void );							// Get the next unused item, resize the list if needed.
		void		Append( type value ) { Alloc() = value; } // Add a new item, resize the list if needed.
		void		Remove( int index );					// Remove an item (list won't be resized).
		void		Copy( const List<type> &other );		// Copy data from an existing list

		int			Find( const type value ) const;			// Find an item in the list
		void		Sort( cmpFunc_t compare, bool removeDupes ); // Sort the list and (optional) remove duplicates
		void		SortEx( cmpFuncEx_t compare, void *param, bool removeDupes ); // Sort the list and (optional) remove duplicates

		// operators
		const type &operator[]( int index ) const;			// Get the specified item
		type &		operator[]( int index );				// Get the specified item
		List<type> &operator=( const List<type> &other );	// Copy data from an existing list
		void		operator+=( type value ) { Alloc() = value; } // Add a new item, resize the list if needed.

	protected:
		int			granularity;							// by how much to grow the list everytime it needs resizing.
		int			num;									// Number of used items
		int			size;									// Total number of items
		type *		list;									// Pointer to the list

		void		Resize( int newSize, bool keepContent ); // Resize to the size and keep content

		static int	CompareCallback( const void *a, const void *b );
		static int	CompareCallbackEx( const void *a, const void *b );

		struct cmpData_t {
			cmpFunc_t	func;
			cmpFuncEx_t	funcEx;
			void		*param;
		};
		static TLS<cmpData_t> cmpData;

	public:
		void		CheckSize( int newSize, bool keepContent=true ); // Check if it needs resizing, and do it.
	};


	/*
	==============================================================================

	  ListEx -> use for bigger items, which need fast resize and sort functions

	  Note: Everytime the list gets resized, it will create a new array of
	  pointers to items and delete the old one. Since it just copies the pointer
	  values and not the actual data, it is very efficient for big items.

	==============================================================================
	*/
	template< class type >
	class ListEx {
	public:
		ListEx( int granularity = 16 );
		ListEx( const ListEx<type> &other );
		virtual ~ListEx();

		typedef int ( *cmpFunc_t )( const type &, const type & );
		typedef int ( *cmpFuncEx_t )( const type &, const type &, void *param );

		void		Clear( void );							// Clear all entries

		bool		IsEmpty( void ) const;					// Checks to see if list is empty

		int			Num( void ) const { return num; }		// Number of used entries
		int			AllocSize( void ) const { return size; } // Number of allocated item pointers

		void		SetGranularity( int granularity ) { this->granularity = granularity; } // Set the granularity
		int			GetGranularity( void ) { return granularity; } // Get the granularity

#if 0 // disabled until we need them and know how we want them to act
		size_t		Allocated( void ) const;				// Returns total size of allocated memory
		size_t		Size( void ) const;						// Returns total size of allocated memory including size of list type
		size_t		MemoryUsed( void ) const;				// Returns size of the used elements in the list
#endif

		type&		Alloc( void );							// Get the next unused item, resize the list if needed.
		void		Append( const type &value ) { Alloc() = value; } // Add a new item, resize the list if needed.
		void		Remove( int index );					// Remove an item (list won't be resized).
		void		Copy( const ListEx<type> &other );	// Copy data from an existing list

		int			Find( const type &value ) const;		// Find an item in the list
		void		Sort( cmpFunc_t compare, bool removeDupes ); // Sort the list and (optional) remove duplicates
		void		SortEx( cmpFuncEx_t compare, void *param, bool removeDupes ); // Sort the list and (optional) remove duplicates

		// operators
		const type &operator[]( int index ) const;			// Get the specified item
		type &		operator[]( int index );				// Get the specified item
		ListEx<type> &operator=( const ListEx<type> &other );	// Copy data from an existing list
		void		operator+=( const type &value ) { Alloc() = value; } // Add a new item, resize the list if needed.

	protected:
		int			granularity;							// by how much to grow the list everytime it needs resizing.
		int			num;									// Number of used items
		int			size;									// Total number of item pointers
		type **		list;									// Pointer to the list

		void		Resize( int newSize, bool keepContent ); // Resize to the size and keep content

		static int	CompareCallback( const void *a, const void *b );
		static int	CompareCallbackEx( const void *a, const void *b );
		
		struct cmpData_t {
			cmpFunc_t	func;
			cmpFuncEx_t	funcEx;
			void		*param;
		};
		static TLS<cmpData_t> cmpData;

	public:
		void		CheckSize( int newSize, bool keepContent=true );	// Check if it needs resizing, and do it.
	};


	/*
	==============================================================================

	  StringList -> just adding some more append functions to ListEx

	==============================================================================
	*/
	class StringList : public ListEx<String> {
	public:
		StringList( int granularity = 16 ) : ListEx<String>(granularity) {}
		virtual ~StringList() {}

		void		Append( const String &value ) { Alloc() = value; } // Add a new string, resize the list if needed.
		void		Append( const char *value ) { Alloc() = value; } // Add a new string, resize the list if needed.
		void		Append( const char *value, int byteLen, int len ) { Alloc().SetData( value, byteLen, len ); } // Add a new string (only first n characters), resize the list if needed.

		int			Find( const char *value ) const;				// Find an item in the list
		int			IFind( const char *value ) const;				// Find an item in the list

		// operators
		void		operator+=( const char *value ) { Alloc() = value; } // Add a new string, resize the list if needed
	};
}
#endif
