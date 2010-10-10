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
		void		SetString( const char *key, const char *value );
		const char *GetString( const char *key, const char *defaultValue = "" ) const;
		bool		GetString( const char *key, const char *defaultValue, const char **out ) const;
		bool		GetString( const char *key, const char *defaultValue, String &out ) const;

		// Bool
		void		SetBool( const char *key, bool value );
		bool		GetBool( const char *key, const char *defaultValue = "0" ) const;
		bool		GetBool( const char *key, const char *defaultValue, bool &out ) const;

		// Integer
		void		SetInt( const char *key, int value );
		int			GetInt( const char *key, const char *defaultValue = "0" ) const;
		bool		GetInt( const char *key, const char *defaultValue, int &out ) const;

		// Float
		void		SetFloat( const char *key, float value );
		float		GetFloat( const char *key, const char *defaultValue = "0" ) const;
		bool		GetFloat( const char *key, const char *defaultValue, float &out ) const;

		// Vec2
		void		SetVec2( const char *key, const Vec2 &value );
		void		SetVec2( const char *key, float x, float y );
		Vec2		GetVec2( const char *key, const char *defaultValue = "0 0" ) const;
		bool		GetVec2( const char *key, const char *defaultValue, Vec2 &out ) const;

		// Vec3
		void		SetVec3( const char *key, const Vec3 &value );
		void		SetVec3( const char *key, float x, float y, float z );
		Vec3		GetVec3( const char *key, const char *defaultValue = "0 0 0" ) const;
		bool		GetVec3( const char *key, const char *defaultValue, Vec3 &out ) const;

		// Vec4
		void		SetVec4( const char *key, const Vec4 &value );
		void		SetVec4( const char *key, float x, float y, float z, float w );
		Vec4		GetVec4( const char *key, const char *defaultValue = "0 0 0 0" ) const;
		bool		GetVec4( const char *key, const char *defaultValue, Vec4 &out ) const;

		// ogRect
		void		SetRect( const char *key, const Rect &value );
		void		SetRect( const char *key, float x, float y, float w, float h );
		Rect		GetRect( const char *key, const char *defaultValue = "0 0 0 0" ) const;
		bool		GetRect( const char *key, const char *defaultValue, Rect &out ) const;

		// Color
		void		SetColor( const char *key, const Color &value );
		void		SetColor( const char *key, float r, float g, float b, float a );
		Color		GetColor( const char *key, const char *defaultValue = "1 1 1 1" ) const;
		bool		GetColor( const char *key, const char *defaultValue, Color &out ) const;

		// Angles
		void		SetAngles( const char *key, const Angles &value );
		void		SetAngles( const char *key, float pitch, float yaw, float roll );
		Angles		GetAngles( const char *key, const char *defaultValue = "0 0 0" ) const;
		bool		GetAngles( const char *key, const char *defaultValue, Angles &out ) const;

		// Mat2
		void		SetMat2( const char *key, const Mat2 &value );
		void		SetMat2( const char *key, float xx, float xy, float yx, float yy );
		Mat2		GetMat2( const char *key, const char *defaultValue = "1 0 0 1" ) const;
		bool		GetMat2( const char *key, const char *defaultValue, Mat2 &out ) const;

		// Mat3
		void		SetMat3( const char *key, const Mat3 &value );
		void		SetMat3( const char *key, float xx, float xy, float xz, float yx, float yy, float yz, float zx, float zy, float zz );
		Mat3		GetMat3( const char *key, const char *defaultValue = "1 0 0 0 1 0 0 0 1" ) const;
		bool		GetMat3( const char *key, const char *defaultValue, Mat3 &out ) const;

		// Find functions
		int			Find( const char *key ) const;
		int			MatchPrefix( const char *prefix, int length, int prevMatch=-1 ) const;

		void		ReadFromFile( File *file );
		void		WriteToFile( File *file ) const;

		// operators
		Dict &		operator=( const Dict &other );

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

	/*
	==============================================================================

	  LanguageDict

	==============================================================================
	*/
	//! @todo	make inheritance protected
	//! @todo	add SetString, LoadFromFile, SaveToFile functions
	class LanguageDict : public DictEx<String> {
	public:
		const char *GetString( const char *key );
	};
}

#endif
