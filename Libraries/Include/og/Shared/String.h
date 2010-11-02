// ==============================================================================
//! @file
//! @brief	String interaction (UTF-8)
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

#ifndef __OG_STRING_H__
#define __OG_STRING_H__

#include <time.h>

//! Open Game Libraries
namespace og {
//! @defgroup FileSystem FileSystem (Library)
//! @{

	class File;

	// ==============================================================================
	//! String class ( UTF-8 )
	//!
	//! @todo	test extension, filename and filepath functions. 
	//! @todo	Use MultiByteToWideChar/mbstowcs rather at some points ?
	//! @todo	Add Method Verify() (check for utf-8 correctness)
	//! @todo	strip filename stuff and put it into a derived class, where extension and path will be saved differently.
	// ==============================================================================
	class String {
	public:
		static const int INVALID_POSITION	= -1;	//!< Name for an invalid position ( returned by Find )
		static const int HARDBUFFER_SIZE	= 20;	//!< Size of the string hardbuffer ( strings below this bytelength don't need dynamic memory )
		static const int GRANULARITY		= 16;	//!< The string granularity
		static const int FILE_MAX_BYTES		= 65534;//!< The maximum bytelength of a string in a file

		// ==============================================================================
		//! Default constructor
		// ==============================================================================
		String();

		// ==============================================================================
		//! Constructor
		//!
		//! @param	text	The initial value
		// ==============================================================================
		String( const char *text );

		// ==============================================================================
		//! Constructor
		//!
		//! @param	str		The initial value
		// ==============================================================================
		String( const String &str );

		// ==============================================================================
		//! Virtual destructor
		// ==============================================================================
		virtual ~String();

		// ==============================================================================
		//! Frees dynamic memory and sets the hardbuffer to ""
		//!
		//! @see	Empty
		// ==============================================================================
		void			Clear( void );

		// ==============================================================================
		//! Returns the C-String
		//!
		//! @return	The current string
		// ==============================================================================
		const char*		c_str( void ) const;

		// ==============================================================================
		//! Raw pointer, use very careful, string information might get invalid.
		//!
		//! @return	the current string
		// ==============================================================================
		char*			raw_ptr( void ) const;

		// ==============================================================================
		//! The string length
		//!
		//! @return	Number of characters in this string
		// ==============================================================================
		int				Length( void ) const;

		// ==============================================================================
		//! The string byte length
		//!
		//! @return	Number of bytes used ( not counting the termination byte )
		// ==============================================================================
		int				ByteLength( void ) const;

		// ==============================================================================
		//! Find out if the string is empty
		//!
		//! @return	true if length is 0
		// ==============================================================================
		bool			IsEmpty( void ) const;

		// ==============================================================================
		//! Find out if the string is a positive or negative floating point or integral value
		//!
		//! @return	true if numeric, false if not
		// ==============================================================================
		bool			IsNumeric( void ) const;

		// ==============================================================================
		//! Find out if the string only consists of the characters '0'-'9', 'a'-'z', 'A'-'Z', '_'
		//!
		//! @return	true if it's a word, false if not
		// ==============================================================================
		bool			IsWord( void ) const;


		// fixme: how to not double the documentation with the non-static functions ?
		static bool		IsNumeric( const char *text );
		static bool		IsWord( const char *text );

		// ==============================================================================
		//! Find out if a character is a digit ( '0'-'9' )
		//!
		//! @param	c	The character
		//!
		//! @return	true if c is a digit, false if not
		// ==============================================================================
		static bool		IsDigit( char c );

		// ==============================================================================
		//! Find out if a character is an alpha character ( 'a'-'z', 'A'-'Z' )
		//!
		//! @param	c	The character
		//!
		//! @return	true if c is an alpha char, false if not
		// ==============================================================================
		static bool		IsAlpha( char c );

		// ==============================================================================
		//! Find out if a character is a whitespace ( ' ', '\f', '\n', '\r', '\t', '\v', 0x0D, 0x0A )
		//!
		//! @param	c	The character
		//!
		//! @return	true if it is a whitespace, false if not
		// ==============================================================================
		static bool		IsSpace( char c );

		// ==============================================================================
		//! Find out if a character is a newline ( '\n', '\r', '\v' )
		//!
		//! @param	c	The character
		//!
		//! @return	true if it is a newline, false if not
		// ==============================================================================
		static bool		IsNewLine( char c );

		// ==============================================================================
		//! Clear the string without deleting dynamic memory
		//!
		//! @see	Clear
		// ==============================================================================
		void			Empty( void );

		// ==============================================================================
		//! Cap the string length to n characters
		//!
		//! @param	len		The number of characters ( not bytes! )
		//! @param	elipsis	Set to true to add an elipsis when capped
		// ==============================================================================
		void			CapLength( int len, bool elipsis=false );

		// ==============================================================================
		//! Convert this string to lower case characters
		// ==============================================================================
		void			ToLower( void );

		// ==============================================================================
		//! Convert this string to upper case characters
		// ==============================================================================
		void			ToUpper( void );

		// ==============================================================================
		//! Convert all slashes to forward slashes
		// ==============================================================================
		void			ToForwardSlashes( void );

		// ==============================================================================
		//! Convert all slashes to backward slashes
		// ==============================================================================
		void			ToBackwardSlashes( void );

		// ==============================================================================
		//! Format Number of Bytes styles
		// ==============================================================================
		enum fnbStyle {
			FNB_INT,		//!< No fractional digits
			FNB_FLOAT,		//!< Use Fractional digits when appropriate
			FNB_FLOATEX,	//!< Same as FNB_FLOAT, but does not count the '.' to the digits
		};

		// ==============================================================================
		//! Format number of bytes
		//!
		//! @param	bytes	The number of bytes
		//! @param	digits	How many digits to display
		//! @param	style	The style to use
		//!
		//! @see	fnbStyle
		// ==============================================================================
		void			FormatNumBytes( uLongLong bytes, int digits=4, fnbStyle style=FNB_INT );

		// ==============================================================================
		//! Initialize from a wide string
		//!
		//! @param	in	The unicode string to convert.
		// ==============================================================================
		void			FromWide( const wchar_t *in );

		// ==============================================================================
		//! Convert this string to a wide string
		//!
		//! @param	in			The input string 
		//! @param	numBytes	Number of bytes of the input string
		//! @param	out			If not NULL, the output buffer
		//! @param	outSize		Size of the output buffer
		//!
		//! @return	The output buffer size needed to store this string as a widestring ( including the termination )
		//!
		//! @note	Call once with out=NULL to get the size and once to convert it
		// ==============================================================================
		static int		ToWide( const char *in, uInt numBytes, wchar_t *out, uInt outSize );

		// ==============================================================================
		//! Get a string from bitflags set
		//!
		//! @param	flags		The flags 
		//! @param	flagNames	The flag names
		//!
		//! @todo	Add parameter for the number of entries in flagNames
		// ==============================================================================
		void			FromBitFlags( int flags, const char **flagNames );

		// ==============================================================================
		//! Create a string version of the time
		//!
		//! @param	format	Describes the format to use
		//! @param	time	The time to use
		//!
		//! @see	http://www.cplusplus.com/reference/clibrary/ctime/strftime/
		//!
		//! @todo	Make unicode safe
		// ==============================================================================
		void			FromDateTime( const char *format, const tm *time );

	// Search & Replace
		// ==============================================================================
		//! Find a string
		//!
		//! @param	text			The string to look for
		//! @param	caseSensitive	true to use case sensitive searching
		//! @param	start			The starting position ( in characters, not bytes )
		//!
		//! @return	The position of the first occurence, INVALID_POSITION if not found
		//!
		//! @see	ReverseFind
		//! @see	FindOneOf
		//! @see	Replace
		// ==============================================================================
		int				Find( const char *text, bool caseSensitive=true, int start=0 ) const;

		// ==============================================================================
		//! Find a string ( static method )
		//!
		//! @param	str				The string to search in
		//! @param	text			The string to look for
		//! @param	caseSensitive	true to use case sensitive searching
		//! @param	start			The starting position ( in characters, not bytes )
		//!
		//! @return	The position of the first occurence, INVALID_POSITION if not found
		//!
		//! @see	ReverseFind
		//! @see	FindOneOf
		//! @see	Replace
		// ==============================================================================
		static int		Find( const char *str, const char *text, bool caseSensitive=true, int start=0 );

		// ==============================================================================
		//! Find the first occurence of one of the specified characters
		//!
		//! @param	text			The characters to look for
		//! @param	caseSensitive	true to use case sensitive searching
		//! @param	start			The starting position ( in characters, not bytes )
		//!
		//! @return	The position of the first occurence, INVALID_POSITION if not found
		//!
		//! @see	Find
		//! @see	ReverseFind
		//! @see	Replace
		// ==============================================================================
		int				FindOneOf( const char *text, bool caseSensitive=true, int start=0 ) const;

		// ==============================================================================
		//! Find the first occurence of one of the specified characters ( static method )
		//!
		//! @param	str				The string to search in
		//! @param	text			The characters to look for
		//! @param	caseSensitive	true to use case sensitive searching
		//! @param	start			The starting position ( in characters, not bytes )
		//!
		//! @return	The position of the first occurence, INVALID_POSITION if not found
		//!
		//! @see	Find
		//! @see	ReverseFind
		//! @see	Replace
		// ==============================================================================
		static int		FindOneOf( const char *str, const char *text, bool caseSensitive=true, int start=0 );

		// ==============================================================================
		//! Find a string in reverse mode
		//!
		//! @param	text			The string to look for
		//! @param	caseSensitive	true to use case sensitive searching
		//!
		//! @return	The position of the last occurence, INVALID_POSITION if not found
		//!
		//! @see	Find
		//! @see	FindOneOf
		//! @see	Replace
		// ==============================================================================
		int				ReverseFind( const char *text, bool caseSensitive=true ) const;
		
		// ==============================================================================
		//! Find a string ( static method )
		//!
		//! @param	str				The string to search in
		//! @param	text			The string to look for
		//! @param	caseSensitive	true to use case sensitive searching
		//!
		//! @return	The position of the last occurence, INVALID_POSITION if not found
		//!
		//! @see	Find
		//! @see	FindOneOf
		//! @see	Replace
		// ==============================================================================
		static int		ReverseFind( const char *str, const char *text, bool caseSensitive=true );

		// ==============================================================================
		//! Replaces a with b in this string
		//!
		//! @param	a		What to look for
		//! @param	b		The replacement
		//! @param	start			The starting position ( in characters, not bytes )
		//!
		//! @return	. 
		// ==============================================================================
		int				Replace( const char *a, const char *b, int start=0 );

	// Strip left
		// ==============================================================================
		//! Strip text from the beginning of this string as often as it's possible
		//!
		//! @param	text	The text to strip from this string
		//!
		//! @return	The number of removals
		//!
		//! @see	StripLeadingOnce
		//! @see	StripLeadingWhitespaces
		//! @see	StripTrailing
		//! @see	StripTrailingOnce
		//! @see	StripTrailingWhitespaces
		// ==============================================================================
		int				StripLeading( const char *text );

		// ==============================================================================
		//! Strip text from the beginning of this string just once
		//!
		//! @param	text	The text to strip from this string
		//!
		//! @return	true if it stripped the text, otherwise false
		//!
		//! @see	StripLeading
		//! @see	StripLeadingWhitespaces
		//! @see	StripTrailing
		//! @see	StripTrailingOnce
		//! @see	StripTrailingWhitespaces
		// ==============================================================================
		bool			StripLeadingOnce( const char *text );

		// ==============================================================================
		//! Strip leading whitespaces
		//!
		//! @return	The number of whitespace bytes/characters removed
		//!
		//! @see	StripLeading
		//! @see	StripLeadingOnce
		//! @see	StripTrailing
		//! @see	StripTrailingOnce
		//! @see	StripTrailingWhitespaces
		// ==============================================================================
		int				StripLeadingWhitespaces( void );

	// Strip right
		// ==============================================================================
		//! Strip text from the end of this string as often as it's possible
		//!
		//! @param	text	The text to strip from this string
		//!
		//! @return	The number of removals
		//!
		//! @see	StripLeading
		//! @see	StripLeadingOnce
		//! @see	StripLeadingWhitespaces
		//! @see	StripTrailingOnce
		//! @see	StripTrailingWhitespaces
		// ==============================================================================
		int				StripTrailing( const char *text );

		// ==============================================================================
		//! Strip text from the end of this string just once
		//!
		//! @param	text	The text to strip from this string
		//!
		//! @return	true if it stripped the text, otherwise false
		//!
		//! @see	StripLeading
		//! @see	StripLeadingOnce
		//! @see	StripLeadingWhitespaces
		//! @see	StripTrailing
		//! @see	StripTrailingWhitespaces
		// ==============================================================================
		bool			StripTrailingOnce( const char *text );

		// ==============================================================================
		//! Strip trailing whitespaces
		//!
		//! @return	The number of whitespace bytes/characters removed
		//!
		//! @see	StripLeading
		//! @see	StripLeadingOnce
		//! @see	StripLeadingWhitespaces
		//! @see	StripTrailing
		//! @see	StripTrailingOnce
		// ==============================================================================
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
		char			hardBuffer[HARDBUFFER_SIZE];

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
	//! @}
}

#endif
