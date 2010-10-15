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

#ifndef __OG_DICT_H__
#define __OG_DICT_H__


namespace og {
	/*
	==============================================================================

	  KeyValue

	==============================================================================
	*/
	class KeyValue {
	public:
		const String &GetKey( void ) const { return *key; }
		const String &GetValue( void ) const { return *value; }

	private:
		friend class Dict;
		const PoolString *key;
		const PoolString *value;
	};

	/*
	==============================================================================

	  Dict

	==============================================================================
	*/
	class Dict {
	public:
		Dict();

		void		Clear( void );							// Clear All Entries

		void		SetGranularity( int granularity );		// Set the granularity

		int			Num( void ) const;						// Number of used entries
		void		Remove( const char *key );				// Remove an item (list won't be resized).

		void		Copy( const Dict &other );				// Clear dict and copy from all keyvalues from other
		void		Append( const Dict &other, bool overWrite ); // Copy from all keyvalues from other, overwriting existing or leaving them

		const KeyValue *GetKeyValue( int index ) const;

		// String
		void		Set( const char *key, const char *value );
		void		Set( const char *key, const Format &value );
		void		Set( const char *key, const String &value );
		template<class T> void Set( const char *key, const T &value );
		template<class T> bool Get( const char *key, const char *defaultValue, T &value ) const;
		StringType	Get( const char *key, const char *defaultValue ) const;

		// Find functions
		int			Find( const char *key ) const;
		int			MatchPrefix( const char *prefix, int length, int prevMatch=-1 ) const;

		void		ReadFromFile( File *file );
		void		WriteToFile( File *file ) const;

		// operators
		Dict &		operator=( const Dict &other );
		StringType	operator[]( const char *key ) const;
		StringType	operator[]( int index ) const;

	private:
		static TLS<StringPool>	keyPool;
		static TLS<StringPool>	valuePool;

		ListEx<KeyValue> entries;
		HashIndex			hashIndex;
	};


	/*
	==============================================================================

	  DictEx

	==============================================================================
	*/
	template<class type>
	class DictEx {
	public:
		DictEx();
		virtual ~DictEx() {}

		void		Clear( void );							// Clear All Entries

		void		SetGranularity( int granularity );		// Set the granularity

		int			Num( void ) const;						// Number of used entries
		void		Remove( const char *key );				// Remove an item (list won't be resized).
		void		Remove( int index );					// Remove an item (list won't be resized).
		
		void		Copy( const DictEx<type> &other );		// Clear dict and copy from all keyvalues from other
		void		Append( const DictEx<type> &other, bool overWrite );	// Copy from all keyvalues from other, overwriting existing or leaving them

		const String &GetKey( int index ) const;				// Get they key of an item

		// Find functions
		int			Find( const char *key ) const;
		int			FindByValue( const type &value ) const;
		int			MatchPrefix( const char *prefix, int length, int prevMatch=-1 ) const;

		// operators
		const type &operator[]( const char *key ) const;	// Get the specified item
		type &		operator[]( const char *key );			// Get the specified item
		const type &operator[]( int index ) const;			// Get the specified item
		type &		operator[]( int index );				// Get the specified item

		DictEx<type> &operator=( const DictEx<type> &other );

	protected:
		ListEx<type>	entries;
		StringList		names;
		HashIndex		hashIndex;
	};
}

#endif
