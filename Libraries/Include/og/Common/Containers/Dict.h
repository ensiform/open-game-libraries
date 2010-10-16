// ==============================================================================
//! @file
//! @brief	Dictionaries
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

#ifndef __OG_DICT_H__
#define __OG_DICT_H__


//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

//! @defgroup CommonContainers Containers
//! @{

	// ==============================================================================
	//! Key/Value for a Dict
	// ==============================================================================
	class KeyValue {
	public:
		// ==============================================================================
		//! Get the key string
		//!
		//! @return	The key string
		// ==============================================================================
		const String &GetKey( void ) const { return *key; }

		// ==============================================================================
		//! Get the value string
		//!
		//! @return	A temporary object to use for conversion to other types
		// ==============================================================================
		StringType GetValue( void ) const { return StringType( value->c_str() ); }

		// ==============================================================================
		//! Get the value string
		//!
		//! @return	The value string
		// ==============================================================================
		const String &GetStringValue( void ) const { return *value; }

	private:
		friend class Dict;
		const PoolString *key;		//!< Pointer to the key PoolString
		const PoolString *value;	//!< Pointer to the value PoolString
	};

	// ==============================================================================
	//! Dictionary
	//!
	//! Accesses values via a key string, stores values in string format
	// ==============================================================================
	class Dict {
	public:
		// ==============================================================================
		//! Default constructor
		// ==============================================================================
		Dict();

		// ==============================================================================
		//! Clear all entries
		// ==============================================================================
		void		Clear( void );

		// ==============================================================================
		//! Set the granularity
		//!
		//! @param	granularity	The new granularity
		// ==============================================================================
		void		SetGranularity( int granularity );

		// ==============================================================================
		//! Get the number of entries
		//!
		//! @return	The current number of entries
		// ==============================================================================
		int			Num( void ) const;

		// ==============================================================================
		//! Remove the entry with the specified key
		//!
		//! @param	key	The key string
		// ==============================================================================
		void		Remove( const char *key );

		// ==============================================================================
		//! Copy all KeyValues from another Dict ( clears existing entries )
		//!
		//! @param	other	Another instance to copy. 
		// ==============================================================================
		void		Copy( const Dict &other );

		// ==============================================================================
		//! Copy all KeyValues from another Dict ( keeps existing entries )
		//!
		//! @param	other		The other dictionary
		//! @param	overWrite	true to overwrite values of existing keys
		// ==============================================================================
		void		Append( const Dict &other, bool overWrite );

		// ==============================================================================
		//! Get the KeyValue with the specified index
		//!
		//! @param	index	Zero-based index
		//!
		//! @return	Pointer to the KeyValue object
		// ==============================================================================
		const KeyValue *GetKeyValue( int index ) const;

		// ==============================================================================
		//! Insert / overwrite an entry
		//!
		//! @param	key		The key
		//! @param	value	The value
		// ==============================================================================
		void		Set( const char *key, const char *value );
		void		Set( const char *key, const Format &value );
		void		Set( const char *key, const String &value );

		template<class T> void Set( const char *key, const T &value );

		// ==============================================================================
		//! Get the value for the specified key
		//!
		//! @param	key				The key
		//! @param	defaultValue	Default value to return if the key does not exist
		//!
		//! @return	A temporary object to use for conversion to other types
		// ==============================================================================
		StringType	Get( const char *key, const char *defaultValue ) const;

		// ==============================================================================
		//! Get the value for the specified key and find out if the key existed
		//!
		//! @param	key				The key
		//! @param	defaultValue	Default value to return if the key does not exist
		//! @param	value			A reference to the value you want the result to be stored in
		//!
		//! @return	true if the key existed
		// ==============================================================================
		template<class T> bool Get( const char *key, const char *defaultValue, T &value ) const;

		// ==============================================================================
		//! Find the index of the specified key
		//!
		//! @param	key	The key
		//!
		//! @return	Zero-based index of the key, -1 if not found.
		// ==============================================================================
		int			Find( const char *key ) const;

		// ==============================================================================
		//! Find all entries that start with a prefix
		//!
		//! @param	prefix		The prefix
		//! @param	length		The number of characters to compare
		//! @param	prevMatch	The previous match index
		//!
		//! @return	The first index matching prefix, -1 if no more entries were found.
		// ==============================================================================
		int			MatchPrefix( const char *prefix, int length, int prevMatch=-1 ) const;

		// ==============================================================================
		//! Read the dictionary from a file ( clears existing entries )
		//!
		//! @param file	The file to read from
		// ==============================================================================
		void		ReadFromFile( File *file );

		// ==============================================================================
		//! Write the dictionary to a file
		//!
		//! @param file	The file to write to
		// ==============================================================================
		void		WriteToFile( File *file ) const;

	// operators
		// ==============================================================================
		//! Copy all entries from the other dictionary
		//!
		//! @param	other	The other dictionary
		//!
		//! @return	A reference to this object
		// ==============================================================================
		Dict &		operator=( const Dict &other );

		// ==============================================================================
		//! Short version of Get without the option to set the default value
		//!
		//! @param	key	The key
		//!
		//! @return	A temporary object to use for conversion to other types
		// ==============================================================================
		StringType	operator[]( const char *key ) const;

		// ==============================================================================
		//! Get a value by its index
		//!
		//! @param	key	The key
		//!
		//! @return	A temporary object to use for conversion to other types
		// ==============================================================================
		StringType	operator[]( int index ) const;

	private:
		static TLS<StringPool>	keyPool;	//!< The key pool ( thread local )
		static TLS<StringPool>	valuePool;	//!< The value pool ( thread local )

		ListEx<KeyValue>	entries;		//!< The entries list
		HashIndex			hashIndex;		//!< Hash index for faster key access
	};

	// ==============================================================================
	//! Custom format dictionary
	//!
	//! Accesses values via a key string
	// ==============================================================================
	template<class type>
	class DictEx {
	public:
		// ==============================================================================
		//! Default constructor
		// ==============================================================================
		DictEx();


		// ==============================================================================
		//! Clear all entries
		// ==============================================================================
		void		Clear( void );

		// ==============================================================================
		//! Set the granularity
		//!
		//! @param	granularity	The new granularity
		// ==============================================================================
		void		SetGranularity( int granularity );

		// ==============================================================================
		//! Get the number of entries
		//!
		//! @return	The current number of entries
		// ==============================================================================
		int			Num( void ) const;

		// ==============================================================================
		//! Remove the entry with the specified key
		//!
		//! @param	key	The key string
		// ==============================================================================
		void		Remove( const char *key );

		// ==============================================================================
		//! Remove the entry with the specified index
		//!
		//! @param	index	Zero-based index
		// ==============================================================================
		void		Remove( int index );

		// ==============================================================================
		//! Copy all entries from another DictEx ( clears existing entries )
		//!
		//! @param	other	Another instance to copy. 
		// ==============================================================================
		void		Copy( const DictEx<type> &other );

		// ==============================================================================
		//! Copy all KeyValues from another DictEx ( keeps existing entries )
		//!
		//! @param	other		The other dictionary
		//! @param	overWrite	true to overwrite values of existing keys
		// ==============================================================================
		void		Append( const DictEx<type> &other, bool overWrite );

		// ==============================================================================
		//! Get the key string of the specified index
		//!
		//! @return	The key string
		// ==============================================================================
		const String &GetKey( int index ) const;

		// ==============================================================================
		//! Find the index of the specified key
		//!
		//! @param	key	The key
		//!
		//! @return	Zero-based index of the key, -1 if not found.
		// ==============================================================================
		int			Find( const char *key ) const;
		
		// ==============================================================================
		//! Find the first index of the entry where the value matches the specified one
		//!
		//! @param	value	The value to look for
		//!
		//! @return	Zero-based index of the value, -1 if not found.
		// ==============================================================================
		int			FindByValue( const type &value ) const;

		// ==============================================================================
		//! Find all entries that start with a prefix
		//!
		//! @param	prefix		The prefix
		//! @param	length		The number of characters to compare
		//! @param	prevMatch	The previous match index
		//!
		//! @return	The first index matching prefix, -1 if no more entries were found.
		// ==============================================================================
		int			MatchPrefix( const char *prefix, int length, int prevMatch=-1 ) const;

	// operators
		// ==============================================================================
		//! Copy all entries from the other dictionary
		//!
		//! @param	other	The other dictionary
		//!
		//! @return	A reference to this object
		// ==============================================================================
		DictEx<type> &operator=( const DictEx<type> &other );

		// ==============================================================================
		//! Get a value by its key ( creates one if it does not exist )
		//!
		//! @param	key	The key
		//!
		//! @return	A reference to the object
		// ==============================================================================
		const type &operator[]( const char *key ) const;
		type &		operator[]( const char *key );

		// ==============================================================================
		//! Get a value by its index
		//!
		//! @param	index	Zero-based index
		//!
		//! @return	A reference to the object
		// ==============================================================================
		const type &operator[]( int index ) const;
		type &		operator[]( int index );

	protected:
		ListEx<type>	entries;	//!< The entries list
		StringList		names;		//!< The key names list
		HashIndex		hashIndex;	//!< Hash index for faster key access
	};
}

#endif
