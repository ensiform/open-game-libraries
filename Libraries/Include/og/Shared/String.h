/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: String (UTF-8) interaction
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

#ifndef __OG_STRING_H__
#define __OG_STRING_H__

#include <time.h>

namespace og {
	class File;
	const int OG_STR_HARDBUFFER_SIZE = 20;
	const int OG_STR_GRANULARITY = 16;
	const int OG_STR_FILE_MAX_BYTES = 65534;	//!< The maximum bytelength of a string in a file

	//! @todo	strip filename stuff and put it into a derived class, where extension and path will be saved differently.
	/*
	==============================================================================

	  String

	==============================================================================
	*/
	//! @todo	test extension, filename and filepath functions.
	//! @todo	Add Method Verify() (check for utf-8 correctness)
	//! @todo	Use MultiByteToWideChar/mbstowcs rather at some points ?
	class String {
	public:
		String();
		String( const char *text );
		String( const String &str );
		virtual ~String();

		static const int INVALID_POSITION = -1;

		void			Clear( void );

		const char*		c_str( void ) const;
		char*			raw_ptr( void ) const;						// Careful with this one.
		int				Length( void ) const;
		int				ByteLength( void ) const;
		bool			IsEmpty( void ) const;
		bool			IsNumeric( void ) const;
		bool			IsWord( void ) const;

		static bool		IsNumeric( const char *text );
		static bool		IsWord( const char *text );
		static bool		IsDigit( char c );
		static bool		IsAlpha( char c );
		static bool		IsSpace( char c );
		static bool		IsNewLine( char c );

		void			Empty( void );								// Clear without deleting memory
		void			CapLength( int len, bool elipsis=false );	// Cap length to n characters

		void			ToLower( void );
		void			ToUpper( void );							// Convert to upper case characters

		void			ToForwardSlashes( void );					// Convert all slashes to forward slashes
		void			ToBackwardSlashes( void );					// Convert all slashes to backward slashes

		// Format Number of Bytes styles
		enum fnbStyle {
			FNB_INT,
			FNB_FLOAT,
			FNB_FLOATEX, // does not count the '.' to the digits.
		};
		void			FormatNumBytes( uLongLong bytes, int digits=4, fnbStyle style=FNB_INT );

		void			FromWide( const wchar_t *in );
		static int		ToWide( const char *in, uInt numBytes, wchar_t *out, uInt outSize );
		void			FromBitFlags( int flags, const char **flagNames );

		// ==============================================================================
		//! Create a string version of the time
		//!
		//! @param	format	Describes the format to use
		//! @param	time	The time to use
		//!
		//! @see	http://www.cplusplus.com/reference/clibrary/ctime/strftime/
		// ==============================================================================
		void			FromDateTime( const char *format, const tm *time );

		// Search & Replace
		int				Find( const char *text, bool caseSensitive=true, int start=0 ) const;					// Find index of first occurrence of text
		static int		Find( const char *str, const char *text, bool caseSensitive=true, int start=0 );		// Find index of first occurrence of text in str
		int				FindOneOf( const char *text, bool caseSensitive=true, int start=0 ) const;				// Find index of first occurrence of one of the chars specified in text
		static int		FindOneOf( const char *str, const char *text, bool caseSensitive=true, int start=0 );	// Find index of first occurrence of one of the chars specified in text in str
		int				ReverseFind( const char *text, bool caseSensitive=true ) const;							// Find index of last occurrence of c
		static int		ReverseFind( const char *str, const char *text, bool caseSensitive=true );				// Find index of last occurrence of c in str
		int				Replace( const char *a, const char *b, int start=0 );									// Replace a with b

		// Strip left
		int				StripLeading( const char *text );
		bool			StripLeadingOnce( const char *text );
		int				StripLeadingWhitespaces( void );

		// Strip right
		int				StripTrailing( const char *text );
		bool			StripTrailingOnce( const char *text );
		int				StripTrailingWhitespaces( void );

		// File Extension
		void			StripFileExtension( void );
		void			SetFileExtension( const char *ext );
		void			DefaultFileExtension( const char *ext );
		bool			CheckFileExtension( const char *ext ) const;
		void			GetFileExtension( String &str ) const;
		String			GetFileExtension( void ) const;
		static void		GetFileExtension( const char *text, int byteLen, String &str );
		static String	GetFileExtension( const char *text, int byteLen );

		// Filename & Path
		void			StripFilename( void );
		void			GetFilename( String &str ) const;
		String			GetFilename( void ) const;
		void			StripPath( void );
		void			GetPath( String &str ) const;
		String			GetPath( void ) const;

		// Partial Strings
		String			Left( int len ) const;
		void			Left( int len, String &str ) const;

		String			Right( int len ) const;
		void			Right( int len, String &str ) const;

		String			Mid( int start, int len ) const;
		void			Mid( int start, int len, String &str ) const;

		// String Comparison (len is considered character count, not byte count)
		int				Cmp( const char *text ) const;
		int				Cmpn( const char *text, int len ) const;
		int				CmpPrefix( const char *text ) const;
		int				Icmp( const char *text ) const;
		int				Icmpn( const char *text, int len ) const;
		int				IcmpPrefix( const char *text ) const;

		// Static ones :P
		static int		Cmp( const char *text1, const char *text2 );
		static int		Cmpn( const char *text1, const char *text2, int len );
		static int		CmpPrefix( const char *text1, const char *text2 );
		static int		Icmp( const char *text1, const char *text2 );
		static int		Icmpn( const char *text1, const char *text2, int len );
		static int		IcmpPrefix( const char *text1, const char *text2 );

		static size_t	Length( const char *text );
		static size_t	ByteLength( const char *text );
		static void		BothLengths( const char *text, size_t *byteLength, size_t *length );

		// Escape Colors
		static int		GetEscapeColorLength( const char *str );
		static size_t	StripEscapeColor( char *str );	// Returns how many bytes have been removed.
		void			StripEscapeColor( void );

		// Moved here from Utilities.h
		static int		ToInt( const char *str );
		static long		ToLong( const char *str );
		static float	ToFloat( const char *str );
		static bool		ToFloatArray( const char *str, float *fp, int dim );
		static double	ToDouble( const char *str );
		static bool		ToDoubleArray( const char *str, double *dp, int dim );

		// operators
		bool			operator==( const String &other ) const;
		bool			operator==( const char *text ) const;
		friend bool		operator==( const char *text, const String &str );

		bool			operator!=( const String &other ) const;
		bool			operator!=( const char *text ) const;
		friend bool		operator!=( const char *text, const String &str );

		String &		operator=( const char *text );
		String &		operator=( const String &str );

		void			operator+=( const char *text );
		void			operator+=( const String &str );

		String			operator+( const String &str );
		String			operator+( const char *text );
		friend String	operator+( const char *text, const String &str );

		void			ReadFromFile( File *f );
		void			WriteToFile( File *f ) const;

	protected:
		friend class	StringList;

		int				size;
		int				length;
		int				byteLength;
		char *			data;
		char			hardBuffer[OG_STR_HARDBUFFER_SIZE];

		void			Init( void );
		void			Free( void );
		void			Resize( int newSize, bool keepContent );
		void			CheckSize( int newSize, bool keepContent=true );

		void			SetData( const char *text, int byteLength, int length );
		void			AppendData( const char *text, int byteLength, int length );
	};

	/*
	==============================================================================

	  StringType

	  Helper for dict return value conversion

	==============================================================================
	*/
	class StringType {
	private:
		StringType( const char *val ) : value(val) {}
		StringType( const StringType & ) {} // not copyable
		const char *value;

		friend class Dict;
		friend class KeyValue;
	public:

		// Conversion operators
		operator const char *() const { return value; }
		operator bool	() const { return String::ToInt( value ) != 0; }
		operator int	() const { return String::ToInt( value ); }
		operator uInt	() const { return String::ToInt( value ); }
		operator long	() const { return String::ToInt( value ); }
		operator uLong	() const { return String::ToInt( value ); }
		operator float	() const { return String::ToFloat( value ); }
	};
}

#endif
