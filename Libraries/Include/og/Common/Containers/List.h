// ==============================================================================
//! @file
//! @brief	Storing Data in Variable Arrays
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

#ifndef __OG_LIST_H__
#define __OG_LIST_H__


//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	// ==============================================================================
	//! List -> use for small items
	//!
	//! @note	Everytime the list gets resized, it will create a new array of items
	//!			and delete the old one. This is very inefficient for bigger items.
	//!	@warning	If you decide to use List with a class/struct, which contains
	//!				dynamic memory, make sure the class has a copy constructor.
	// ==============================================================================
	template<class T>
	class List {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	granularity	The granularity
		// ==============================================================================
		List( int granularity = 16 );

		// ==============================================================================
		//! Copy Constructor
		//!
		//! @param	other	The list to copy from
		// ==============================================================================
		List( const List<T> &other );

		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~List();

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
		//! Get the number of allocated items
		//!
		//! @return	The current number of allocated items
		// ==============================================================================
		int			AllocSize( void ) const;

		// ==============================================================================
		//! Set the granularity
		//!
		//! @param	granularity	The new granularity
		// ==============================================================================
		void		SetGranularity( int granularity );

		// ==============================================================================
		//! Get the granularity
		//!
		//! @return	The current granularity
		// ==============================================================================
		int			GetGranularity( void ) const;

		// ==============================================================================
		//! Get the next unused item, resize the list if needed
		//!
		//! @return	a reference to the new object
		// ==============================================================================
		T &			Alloc( void );

		// ==============================================================================
		//! Add a new item, resize the list if needed
		//!
		//! @param	value	The value to append
		// ==============================================================================
		void		Append( T value );

		// ==============================================================================
		//! Remove an item (list won't be resized)
		//!
		//! @param	index	The index to remove
		// ==============================================================================
		void		Remove( int index );

		// ==============================================================================
		//! Copy data from an existing list
		//!
		//! @param	other	The list to copy from
		// ==============================================================================
		void		Copy( const List<T> &other );

		// ==============================================================================
		//! Find an item in the list
		//!
		//! @param	value	The value to look for
		//!
		//! @return	the index of the first matching item
		// ==============================================================================
		int			Find( const T value ) const;

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

		// ==============================================================================
		//! Check if it needs resizing, and do it
		//!
		//! @param	newSize		The new list size
		//! @param	keepContent	true to keep the content
		// ==============================================================================
		void		CheckSize( int newSize, bool keepContent=true );

	// operators
		// ==============================================================================
		//! Copy all entries from the other list
		//!
		//! @param	other	The other list
		//!
		//! @return	A reference to this object
		// ==============================================================================
		List<T> &	operator=( const List<T> &other );

		// ==============================================================================
		//! Add a new item, resize the list if needed
		//!
		//! @param	value	The value to append
		// ==============================================================================
		void		operator+=( T value );

		// ==============================================================================
		//! Get a value by its index
		//!
		//! @param	index	Zero-based index
		//!
		//! @return	A reference to the object
		// ==============================================================================
		const T &	operator[]( int index ) const;
		T &			operator[]( int index );

	protected:
		int			granularity;	//<! By how much to grow the list everytime it needs resizing
		int			num;			//<! Number of used items
		int			size;			//<! Total number of items
		T *			list;			//<! Pointer to the list

		// ==============================================================================
		//! Resize to the size and (optional) keep the content
		//!
		//! @param	newSize		The new list size
		//! @param	keepContent	true to keep the content
		// ==============================================================================
		void		Resize( int newSize, bool keepContent );

		// ==============================================================================
		//! Comparison callback
		//!
		//! @param	a	A pointer to the first object
		//! @param	b	A pointer to the second object
		//!
		//! @return	0 if the objects match, otherwise positive or negative
		// ==============================================================================
		static int	CompareCallback( const void *a, const void *b );

		// ==============================================================================
		//! Comparison callback
		//!
		//! @param	a	A pointer to the first object
		//! @param	b	A pointer to the second object
		//!
		//! @return	0 if the objects match, otherwise positive or negative
		// ==============================================================================
		static int	CompareCallbackEx( const void *a, const void *b );

		// ==============================================================================
		//! Comparison data
		// ==============================================================================
		struct cmpData_t {
			cmpFunc_t	func;	//!< The normal comparison function
			cmpFuncEx_t	funcEx;	//!< The extended comparison function
			void		*param;	//!< The parameter ( for the extended comparison )
		};
		static TLS<cmpData_t> cmpData;	//!< Thread local comparison data
	};

	// ==============================================================================
	//! ListEx -> use for bigger items, which need fast resize and sort functions
	//!
	//! @note	Everytime the list gets resized, it will create a new array of
	//!			pointers to items and delete the old one. Since it just copies the pointer
	//!			values and not the actual data, it is more efficient on big items.
	// ==============================================================================
	template<class T>
	class ListEx {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	granularity	The granularity
		// ==============================================================================
		ListEx( int granularity = 16 );

		// ==============================================================================
		//! Copy Constructor
		//!
		//! @param	other	The list to copy from
		// ==============================================================================
		ListEx( const ListEx<T> &other );

		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~ListEx();

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
		//! Get the number of allocated items
		//!
		//! @return	The current number of allocated item pointers
		// ==============================================================================
		int			AllocSize( void ) const;

		// ==============================================================================
		//! Set the granularity
		//!
		//! @param	granularity	The new granularity
		// ==============================================================================
		void		SetGranularity( int granularity );

		// ==============================================================================
		//! Get the granularity
		//!
		//! @return	The current granularity
		// ==============================================================================
		int			GetGranularity( void ) const;

		// ==============================================================================
		//! Get the next unused item, resize the list if needed
		//!
		//! @return	a reference to the new object
		// ==============================================================================
		T &			Alloc( void );

		// ==============================================================================
		//! Add a new item, resize the list if needed
		//!
		//! @param	value	The value to append
		// ==============================================================================
		void		Append( const T &value );

		// ==============================================================================
		//! Remove an item (list won't be resized)
		//!
		//! @param	index	The index to remove
		// ==============================================================================
		void		Remove( int index );

		// ==============================================================================
		//! Copy data from an existing list
		//!
		//! @param	other	The list to copy from
		// ==============================================================================
		void		Copy( const ListEx<T> &other );

		// ==============================================================================
		//! Find an item in the list
		//!
		//! @param	value	The value to look for
		//!
		//! @return	the index of the first matching item
		// ==============================================================================
		int			Find( const T &value ) const;

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

		// ==============================================================================
		//! Check if it needs resizing, and do it
		//!
		//! @param	newSize		The new list size
		//! @param	keepContent	true to keep the content
		// ==============================================================================
		void		CheckSize( int newSize, bool keepContent=true );

	// operators
		// ==============================================================================
		//! Copy all entries from the other list
		//!
		//! @param	other	The other list
		//!
		//! @return	A reference to this object
		// ==============================================================================
		ListEx<T> &	operator=( const ListEx<T> &other );

		// ==============================================================================
		//! Add a new item, resize the list if needed
		//!
		//! @param	value	The value to append
		// ==============================================================================
		void		operator+=( const T &value );

		// ==============================================================================
		//! Get a value by its index
		//!
		//! @param	index	Zero-based index
		//!
		//! @return	A reference to the object
		// ==============================================================================
		const T &	operator[]( int index ) const;
		T &			operator[]( int index );

	protected:
		int			granularity;	//<! By how much to grow the list everytime it needs resizing
		int			num;			//<! Number of used items
		int			size;			//<! Total number of item pointers
		T **		list;			//<! Pointer to the list

		// ==============================================================================
		//! Resize to the size and (optional) keep the content
		//!
		//! @param	newSize		The new list size
		//! @param	keepContent	true to keep the content
		// ==============================================================================
		void		Resize( int newSize, bool keepContent );

		// ==============================================================================
		//! Comparison callback
		//!
		//! @param	a	A pointer to the first object
		//! @param	b	A pointer to the second object
		//!
		//! @return	0 if the objects match, otherwise positive or negative
		// ==============================================================================
		static int	CompareCallback( const void *a, const void *b );

		// ==============================================================================
		//! Comparison callback
		//!
		//! @param	a	A pointer to the first object
		//! @param	b	A pointer to the second object
		//!
		//! @return	0 if the objects match, otherwise positive or negative
		// ==============================================================================
		static int	CompareCallbackEx( const void *a, const void *b );
		
		// ==============================================================================
		//! Comparison data
		// ==============================================================================
		struct cmpData_t {
			cmpFunc_t	func;	//!< The normal comparison function
			cmpFuncEx_t	funcEx;	//!< The extended comparison function
			void		*param;	//!< The parameter ( for the extended comparison )
		};
		static TLS<cmpData_t> cmpData;	//!< Thread local comparison data
	};

	// ==============================================================================
	//!  StringList -> just adding some more append functions to ListEx
	// ==============================================================================
	class StringList : public ListEx<String> {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	granularity	The granularity
		// ==============================================================================
		StringList( int granularity = 16 ) : ListEx<String>(granularity) {}

		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~StringList() {}

		// ==============================================================================
		//! Add a new string, resize the list if needed
		//!
		//! @param	value	The value
		// ==============================================================================
		void		Append( const String &value );
		void		Append( const char *value );

		// ==============================================================================
		//! Add a new string (only first n characters), resize the list if needed
		//!
		//! @param	value	The value
		//! @param	byteLen	The number of bytes to use
		//! @param	len		The number of characters it would be
		// ==============================================================================
		void		Append( const char *value, int byteLen, int len );

		// ==============================================================================
		//! Find an item in the list
		//!
		//! @param	value	The value to look for
		//!
		//! @return	the index of the first matching item
		// ==============================================================================
		int			Find( const char *value ) const;
		int			IFind( const char *value ) const;

		// ==============================================================================
		//! Clear the list and split a String ito it
		//!
		//! @param	text		The string to split
		//! @param	delimiter	The delimiter to use
		// ==============================================================================
		void		SplitString( const char *text, const char * delimiter=";" );

	// operators
		// ==============================================================================
		//! Add a new item, resize the list if needed
		//!
		//! @param	value	The value to append
		// ==============================================================================
		void		operator+=( const char *value );
	};
//! @}
}
#endif
