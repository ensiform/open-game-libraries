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
//! @defgroup Shared Shared (Library)
//! @{

	class File;

	// ==============================================================================
	//! String class ( UTF-8 )
	//!
	//! @todo	test extension, filename and filepath functions. 
	//! @todo	Use MultiByteToWideChar/mbstowcs rather at some points ?
	//! @todo	Add Method Verify() (check for utf-8 correctness)
	//! @todo	strip filename stuff and put it into a derived class, where extension and path will be saved differently.
	//! @todo	Decide on size_t/uInt, etc.
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
		//! @overload
		//!
		//! @param	text	The string to check
		// ==============================================================================
		static size_t	Length( const char *text );

		// ==============================================================================
		//! The string byte length
		//!
		//! @return	Number of bytes used ( not counting the termination byte )
		// ==============================================================================
		int				ByteLength( void ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @param	text	The string to check
		// ==============================================================================
		static size_t	ByteLength( const char *text );

		// ==============================================================================
		//! Get both the string length as well as the byte length
		//!
		//! @param	text		The string to check
		//! @param	byteLength	Where to store the byte length result
		//! @param	length		Where to store the string length result
		// ==============================================================================
		static void		BothLengths( const char *text, size_t *byteLength, size_t *length );

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
		//! @overload
		//!
		//! @param	text	The string to check
		// ==============================================================================
		static bool		IsNumeric( const char *text );

		// ==============================================================================
		//! Find out if the string only consists of the characters '0'-'9', 'a'-'z', 'A'-'Z', '_'
		//!
		//! @return	true if it's a word, false if not
		// ==============================================================================
		bool			IsWord( void ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @param	text	The string to check
		// ==============================================================================
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
		//! Find out if a character is a whitespace ( ' ', '\\f', '\\n', '\\r', '\\t', '\\v', 0x0D, 0x0A )
		//!
		//! @param	c	The character
		//!
		//! @return	true if it is a whitespace, false if not
		// ==============================================================================
		static bool		IsSpace( char c );

		// ==============================================================================
		//! Find out if a character is a newline ( '\\n', '\\r', '\\v' )
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
		//!
		//! @todo	Linux / Mac Support
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
		//!
		//! @todo	Linux / Mac Support
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
		//!
		//! @todo	test this..
		// ==============================================================================
		int				Find( const char *text, bool caseSensitive=true, int start=0 ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @param	str		The string to search in
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
		//!
		//! @todo	Needs to be written for UTF-8 support
		// ==============================================================================
		int				FindOneOf( const char *text, bool caseSensitive=true, int start=0 ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @param	str		The string to search in
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
		//!
		//! @todo	test this..
		// ==============================================================================
		int				ReverseFind( const char *text, bool caseSensitive=true ) const;
		
		// ==============================================================================
		//! @overload
		//!
		//! @param	str		The string to search in
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
		// ==============================================================================
		//! Strip file extension
		//!
		//! Searches in reverse for a slash or a dot
		//! If a dot was found before(after) a slash, everything after ( and including ) the dot will be removed
		// ==============================================================================
		void			StripFileExtension( void );

		// ==============================================================================
		//! Change the file extension ( calls StripFileExtension and then adds the new one )
		//!
		//! @param	ext	The new file extension ( should include the dot, but will be automaticly added if not )
		// ==============================================================================
		void			SetFileExtension( const char *ext );

		// ==============================================================================
		//! Add a file extension if none is present
		//!
		//! @param	ext	The new file extension ( should include the dot, but will be automaticly added if not )
		// ==============================================================================
		void			DefaultFileExtension( const char *ext );

		// ==============================================================================
		//! Check for a file extension ( case insensitive )
		//!
		//! @param	ext	The file extension ( should include the dot! )
		//!
		//! @return	true if it was found, false if not
		// ==============================================================================
		bool			CheckFileExtension( const char *ext ) const;

		// ==============================================================================
		//! Get the file extension of this string
		//!
		//! @param	str	The string to store the file extension in
		// ==============================================================================
		void			GetFileExtension( String &str ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @return	The file extension
		// ==============================================================================
		String			GetFileExtension( void ) const;

		// ==============================================================================
		//! Get the file extension from the specified string
		//!
		//! @param	text		The string to search for the file extension
		//! @param	byteLen		Byte length of text
		//! @param	str	The string to store the file extension in
		// ==============================================================================
		static void		GetFileExtension( const char *text, int byteLen, String &str );

		// ==============================================================================
		//! @overload
		//!
		//! @return	The file extension
		// ==============================================================================
		static String	GetFileExtension( const char *text, int byteLen );

	// Filename & Path
		// ==============================================================================
		//! Strip filename from this string
		// ==============================================================================
		void			StripFilename( void );

		// ==============================================================================
		//! Get the filename without it's path
		//!
		//! @param	str	The string to store the filename in
		// ==============================================================================
		void			GetFilename( String &str ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @return	The filename
		// ==============================================================================
		String			GetFilename( void ) const;

		// ==============================================================================
		//! Strip the path from this string
		// ==============================================================================
		void			StripPath( void );

		// ==============================================================================
		//! Gets the path from this string
		//!
		//! @param	str	The string to store the path in
		// ==============================================================================
		void			GetPath( String &str ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @return	The path
		// ==============================================================================
		String			GetPath( void ) const;

	// Partial Strings
		// ==============================================================================
		//! Get the first n characters as a string
		//!
		//! @param	len	The number of characters to get
		//! @param	str	The string to store the first n characters in
		// ==============================================================================
		void			Left( int len, String &str ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @return	The string consisting of the first n characters
		// ==============================================================================
		String			Left( int len ) const;

		// ==============================================================================
		//! Get the last n characters as a string
		//!
		//! @param	len	The number of characters to get
		//! @param	str	The string to store the last n characters in
		// ==============================================================================
		void			Right( int len, String &str ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @return	The string consisting of the last n characters
		// ==============================================================================
		String			Right( int len ) const;

		// ==============================================================================
		//! Get a sub string
		//!
		//! @param	start	The starting position ( characters, not bytes )
		//! @param	len	The number of characters to get
		//! @param	str	The string to store the result in
		// ==============================================================================
		void			Mid( int start, int len, String &str ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @return	The sub string
		// ==============================================================================
		String			Mid( int start, int len ) const;

	// String Comparison (len is considered character count, not byte count)
		// ==============================================================================
		//! Compare this string to another (case sensitive)
		//!
		//! @param	text	The string to compare to
		//!
		//! @return	<0 if text is less than this, 0 if equal, >0 if text is greater than this
		// ==============================================================================
		int				Cmp( const char *text ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @param	text1	The base string
		// ==============================================================================
		static int		Cmp( const char *text1, const char *text2 );

		// ==============================================================================
		//! Compare the first n characters of this string to another (case sensitive)
		//!
		//! @param	text	The string to compare to
		//! @param	len		The number of characters to compare
		//!
		//! @return	<0 if text is less than this, 0 if equal, >0 if text is greater than this
		// ==============================================================================
		int				Cmpn( const char *text, int len ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @param	text1	The base string
		// ==============================================================================
		static int		Cmpn( const char *text1, const char *text2, int len );

		// ==============================================================================
		//! Compare the first Length(text) characters of this string to another (case sensitive)
		//!
		//! @param	text	The string to compare to
		//!
		//! @return	<0 if text is less than this, 0 if equal, >0 if text is greater than this
		// ==============================================================================
		int				CmpPrefix( const char *prefix ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @param	text1	The base string
		// ==============================================================================
		static int		CmpPrefix( const char *text1, const char *text2 );

		// ==============================================================================
		//! Compare this string to another (case insensitive)
		//!
		//! @param	text	The string to compare to
		//!
		//! @return	<0 if text is less than this, 0 if equal, >0 if text is greater than this
		// ==============================================================================
		int				Icmp( const char *text ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @param	text1	The base string
		// ==============================================================================
		static int		Icmp( const char *text1, const char *text2 );

		// ==============================================================================
		//! Compare the first n characters of this string to another (case insensitive)
		//!
		//! @param	text	The string to compare to
		//! @param	len		The number of characters to compare
		//!
		//! @return	<0 if text is less than this, 0 if equal, >0 if text is greater than this
		// ==============================================================================
		int				Icmpn( const char *text, int len ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @param	text1	The base string
		// ==============================================================================
		static int		Icmpn( const char *text1, const char *text2, int len );

		// ==============================================================================
		//! Compare the first Length(text) characters of this string to another (case insensitive)
		//!
		//! @param	text	The string to compare to
		//!
		//! @return	<0 if text is less than this, 0 if equal, >0 if text is greater than this
		// ==============================================================================
		int				IcmpPrefix( const char *text ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @param	text1	The base string
		// ==============================================================================
		static int		IcmpPrefix( const char *text1, const char *text2 );

	// Escape Colors
		// ==============================================================================
		//! Get the length of an escape color sequence
		//!
		//! @param	str	The start of the escape color ( first char is '^' )
		//!
		//! @return	The number of bytes/characters this color escape sequence is long, 0 if it's an invalid sequence
		// ==============================================================================
		static int		GetEscapeColorLength( const char *str );

		// ==============================================================================
		//! Strip all escape color sequences from the string
		//!
		//! @return	Number of bytes/characters removed
		// ==============================================================================
		size_t			StripEscapeColor( void );

		// ==============================================================================
		//! @overload
		//!
		//! @param	str	The string to clean
		// ==============================================================================
		static size_t	StripEscapeColor( char *str );

	// Moved here from Utilities.h
		// ==============================================================================
		//! Convert a string to an integer
		//!
		//! @param	str	The string
		//!
		//! @return	The string converted to an integer
		// ==============================================================================
		static int		ToInt( const char *str );

		// ==============================================================================
		//! Convert a string to a long
		//!
		//! @param	str	The string
		//!
		//! @return	The string converted to a long
		// ==============================================================================
		static long		ToLong( const char *str );

		// ==============================================================================
		//! Convert a string to a floating point value
		//!
		//! @param	str	The string
		//!
		//! @return	The string converted to a floating point value
		// ==============================================================================
		static float	ToFloat( const char *str );

		// ==============================================================================
		//! Convert a string to a floating point array
		//!
		//! @param	str	The string
		//! @param	fp	The float array
		//! @param	dim	The number of entries in fp
		//!
		//! @return	true on success, otherwise false
		// ==============================================================================
		static bool		ToFloatArray( const char *str, float *fp, int dim );

		// ==============================================================================
		//! Convert a string to a double
		//!
		//! @param	str	The string
		//!
		//! @return	The string converted to a double
		// ==============================================================================
		static double	ToDouble( const char *str );

		// ==============================================================================
		//! Convert a string to a double array
		//!
		//! @param	str	The string
		//! @param	dp	The double array
		//! @param	dim	The number of entries in dp
		//!
		//! @return	true on success, otherwise false
		// ==============================================================================
		static bool		ToDoubleArray( const char *str, double *dp, int dim );

	// operators
		// ==============================================================================
		//! Compare one string to another ( case sensitive )
		//!
		//! @param	other	The other string
		//!
		//! @return	true if this string is equal to the other one
		// ==============================================================================
		bool			operator==( const String &other ) const;

		// ==============================================================================
		//! @overload
		// ==============================================================================
		bool			operator==( const char *text ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @param	str		The base string
		// ==============================================================================
		friend bool		operator==( const char *text, const String &str );

		// ==============================================================================
		//! Compare one string to another ( case sensitive )
		//!
		//! @param	other	The other string
		//!
		//! @return	false if this string is equal to the other one
		// ==============================================================================
		bool			operator!=( const String &other ) const;

		// ==============================================================================
		//! @overload
		// ==============================================================================
		bool			operator!=( const char *text ) const;

		// ==============================================================================
		//! @overload
		//!
		//! @param	str		The base string
		// ==============================================================================
		friend bool		operator!=( const char *text, const String &str );

		// ==============================================================================
		//! Initialize from another string
		//!
		//! @param	text	The string to copy
		//!
		//! @return	A shallow copy of this object. 
		// ==============================================================================
		String &		operator=( const char *text );

		// ==============================================================================
		//! @overload
		// ==============================================================================
		String &		operator=( const String &str );

		// ==============================================================================
		//! Append a string to this one
		//!
		//! @param	text	The string to append
		// ==============================================================================
		void			operator+=( const char *text );

		// ==============================================================================
		//! @overload
		// ==============================================================================
		void			operator+=( const String &str );

		// ==============================================================================
		//! Concatenate this string with another one
		//!
		//! @param	str	The second string
		//!
		//! @return	Both strings concatenated
		// ==============================================================================
		String			operator+( const String &str );

		// ==============================================================================
		//! @overload
		// ==============================================================================
		String			operator+( const char *text );

		// ==============================================================================
		//! @overload
		//!
		//! @param	str		The base string
		// ==============================================================================
		friend String	operator+( const char *text, const String &str );

		// ==============================================================================
		//! Read the string data from a binary file
		//!
		//! @param	file	The file to read from
		// ==============================================================================
		void			ReadFromFile( File *file );

		// ==============================================================================
		//! Write the string data to a binary file
		//!
		//! @param	file	The file to write to
		// ==============================================================================
		void			WriteToFile( File *file ) const;

	protected:
		friend class	StringList;

		int				size;							//!< The current buffer size
		int				length;							//!< How many characters the string consists of ( excluding the termination )
		int				byteLength;						//!< How many bytes the string consists of ( excluding the termination )
		char *			data;							//!< The pointer to the buffer ( can point to hardBuffer )
		char			hardBuffer[HARDBUFFER_SIZE];	//!< Hard buffer: if string is below HARDBUFFER_SIZE it will use the hardbuffer
														//!  Otherwise dynamic memory will be allocated.

		// ==============================================================================
		//! Initialize the string with the hardBuffer
		// ==============================================================================
		void			Init( void );

		// ==============================================================================
		//! Free the dynamic memory used
		// ==============================================================================
		void			Free( void );

		// ==============================================================================
		//! Resize the buffer to the new size ( must be greater than the current size )
		//!
		//! @param	newSize		The new size
		//! @param	keepContent	true to keep the old content
		// ==============================================================================
		void			Resize( int newSize, bool keepContent );

		// ==============================================================================
		//! Make sure the buffer is at least the specified size
		//!
		//! @param	newSize		The size needed
		//! @param	keepContent	true to keep content
		// ==============================================================================
		void			CheckSize( int newSize, bool keepContent=true );

		// ==============================================================================
		//! Copy data from the specified input
		//!
		//! @param	text		The text to copy
		//! @param	byteLength	The bytelength of the string
		//! @param	length		The length of the string
		// ==============================================================================
		void			SetData( const char *text, int byteLength, int length );

		// ==============================================================================
		//! Append data from the specified input
		//!
		//! @param	text		The text to append
		//! @param	byteLength	The bytelength of the string
		//! @param	length		The length of the string
		// ==============================================================================
		void			AppendData( const char *text, int byteLength, int length );
	};

	// ==============================================================================
	//! StringType - A helper for Dict return value conversion
	// ==============================================================================
	class StringType {
	private:

		// ==============================================================================
		//! Constructor
		//!
		//! @param	val	The return value as c-string
		// ==============================================================================
		StringType( const char *val ) : value(val) {}

		// ==============================================================================
		//! Can not be copied
		// ==============================================================================
		StringType( const StringType & ) {}

		const char *value;	//!< The value, might be deleted once out of scope, so do not store the raw pointer

		friend class Dict;
		friend class KeyValue;
	public:

	// Conversion operators
		// ==============================================================================
		//! Returns the raw string, do not store as const char *, rather copy the string
		//!
		//! @return	The string value
		//!
		//! @warning	The pointer might get invalid once out of scope, so do not store the raw pointer.
		// ==============================================================================
		operator const char *() const { return value; }

		// ==============================================================================
		//! Convert to a boolean value
		// ==============================================================================
		operator bool	() const { return String::ToInt( value ) != 0; }

		// ==============================================================================
		//! Convert to an integer
		// ==============================================================================
		operator int	() const { return String::ToInt( value ); }

		// ==============================================================================
		//! Convert to an unsigned integer
		// ==============================================================================
		operator uInt	() const { return String::ToInt( value ); }

		// ==============================================================================
		//! Convert to a long
		// ==============================================================================
		operator long	() const { return String::ToInt( value ); }

		// ==============================================================================
		//! Convert to an unsigned long
		// ==============================================================================
		operator uLong	() const { return String::ToInt( value ); }

		// ==============================================================================
		//! Convert to a floating point value
		// ==============================================================================
		operator float	() const { return String::ToFloat( value ); }
	};
	//! @}
}

#endif
