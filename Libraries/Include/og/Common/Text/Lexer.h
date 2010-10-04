// ==============================================================================
//! @file
//! @brief	Text Lexer
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

#ifndef __OG_LEXER_H__
#define __OG_LEXER_H__

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

//! @defgroup CommonText Text
//! @{

	class Lexer;

	// ==============================================================================
	//! Lexer Error
	//!
	//! Thrown when something unexpected happened while parsing
	// ==============================================================================
	class LexerError {
	public:
		// ==============================================================================
		//! The error type ids
		// ==============================================================================
		enum ErrorType {
			CUSTOM,				//!< Set if you use Lexer.Error()Do not use manually
			BAD_TOKEN,			//!< Tried to read a token of a different type
			MISSING_TOKEN,		//!< Token was expected, but not found
			END_OF_FILE			//!< Unexpected end of file
		};
		ErrorType	type;		//!< The current error type
		int			line;		//!< The line where it occured
		String		expected;	//!< What was expected
		String		found;		//!< What was found instead

		// ==============================================================================
		//! Constructor
		//!
		//! @param	type		The current error type
		//! @param	line		The line where it occured
		//! @param	expected	What was expected
		//! @param	found		What was found instead
		// ==============================================================================
		LexerError( ErrorType type, int line=0, const char *expected=NULL, const char *found=NULL );

		// ==============================================================================
		//! Get a string representation of the error type
		//!
		//! @param	result	Reference to a String to store the error message
		// ==============================================================================
		void ToString( String &result );
	};

	// ==============================================================================
	//! Token
	// ==============================================================================
	class Token {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	data	The string data
		// ==============================================================================
		Token( const char *data );

		// ==============================================================================
		//! Get as string
		//!
		//! @return	NULL if it fails, else the string
		// ==============================================================================
		const char *	GetString( void ) const;

		// ==============================================================================
		//! Get as double
		//!
		//! @return	The double
		// ==============================================================================
		double			GetDouble( void ) const;

		// ==============================================================================
		//! Get as float
		//!
		//! @return	The float
		// ==============================================================================
		float			GetFloat( void ) const;

		// ==============================================================================
		//! Get as integer
		//!
		//! @return	The integer
		// ==============================================================================
		int				GetInteger( void ) const;

	private:
		friend class Lexer;

		const char *string;	//!< The string data
		int			line;	//!< The line number
	};

	// ==============================================================================
	//! Flags (options) to pass to the lexer
	// ==============================================================================
	enum LexerFlag {
		LEXER_FULL_LINES		= BIT(0),	//!< Split the file into full lines
		LEXER_NOWARNINGS		= BIT(1),	//!< Do not show warnings
		LEXER_NO_BOM_WARNING	= BIT(2)	//!< Do not show Byte Order Mark warnings
	};

	// ==============================================================================
	//! Lexer
	//!
	//! Parses a text (utf-8) file
	// ==============================================================================
	class Lexer {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	flags	The flags
		// ==============================================================================
		Lexer( int flags = 0 );

		// ==============================================================================
		//! Destructor
		// ==============================================================================
		~Lexer();

#ifdef OG_COMMON_USE_FS
		// ==============================================================================
		//! Loads a file into buffer
		//!
		//! @param	filename	The (relative) file path
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		bool		LoadFile( const char *filename );
#endif

		// ==============================================================================
		//! Loads a data
		//!
		//! @param	dataName	Name of the data ( for error messages )
		//! @param	data		Raw data (text)
		//! @param	size		The size of the data
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		bool		LoadData( const char *dataName, const byte *data, int size );

		// ==============================================================================
		//! Reads the next token
		//!
		//! @return	NULL if it fails, else the token
		// ==============================================================================
		const Token *ReadToken( void );

		// ==============================================================================
		//! Unread the last token
		//!
		//! This only works once until ReadToken was called again
		// ==============================================================================
		void		UnreadToken( void );

		// ==============================================================================
		//! Expect a token
		//!
		//! @param	string	The expected string
		// ==============================================================================
		void		ExpectToken( const char *string );

		// ==============================================================================
		//! Check for a token
		//!
		//! @param	string	The string to check for
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		bool		CheckToken( const char *string );

		// ==============================================================================
		//! Reads an integer
		//!
		//! @return	The int
		// ==============================================================================
		int			ReadInt( void );

		// ==============================================================================
		//! Reads a float
		//!
		//! @return	The float
		// ==============================================================================
		float		ReadFloat( void );

		// ==============================================================================
		//! Reads a string
		//!
		//! @return	The string
		// ==============================================================================
		const char *ReadString( void );

		// ==============================================================================
		//! Skip all tokens on this line
		//!
		//! @return	false if there is no next line, otherwise true
		// ==============================================================================
		bool		GotoNextLine( void );

		// ==============================================================================
		//! Skip all tokens until the string is found
		//!
		//! @param	str	The string
		// ==============================================================================
		void		FindToken( const char *str );

		// ==============================================================================
		//! Triggered warning
		//!
		//! @return	true if a warning has been triggered, otherwise false
		// ==============================================================================
		bool		TriggeredWarning( void ) { return warningTriggered; }

		// ==============================================================================
		//! Throws a custom lexer error
		//!
		//! @param	message	The error message
		// ==============================================================================
		void		Error( const char *message );

		// ==============================================================================
		//! Show a warning message
		//!
		//! @param	message	The message
		// ==============================================================================
		void		Warning( const char *message );

		// ==============================================================================
		//! Get the current line
		//!
		//! @return	The line of the current token
		// ==============================================================================
		int			GetLine( void ) { return token.line; }

		// ==============================================================================
		//! Enable or disable the use of escape characters
		//!
		//! @param	enable	true to enable, false to disable
		//!
		//! @todo	should rather take a key/value pair list
		//! @note	It's enabled by default
		// ==============================================================================
		void		SetEscapeCharacters( bool enable ) { useEscapeChars = enable; }

		// ==============================================================================
		//! Set the characters that should each be handled as a single token
		//!
		//! @param	chars	The characters
		// ==============================================================================
		void		SetSingleTokenChars( const char *chars );

		// ==============================================================================
		//! Set How comments start and end
		//!
		//! @param	line		The string for line comments
		//! @param	blockStart	The string for block comments start
		//! @param	blockEnd	The string for block comments end
		// ==============================================================================
		void		SetCommentStrings( const char *line, const char *blockStart, const char *blockEnd );

	private:

		// ==============================================================================
		//! Parse all characters until the quote end is reached
		// ==============================================================================
		void		ParseQuoted( void );

		// ==============================================================================
		//! Skip all characters until the comment end is reached
		// ==============================================================================
		void		SkipToCommentEnd( void );

		// ==============================================================================
		//! Skip all characters until the line end is reached
		// ==============================================================================
		void		SkipRestOfLine( void );

		// ==============================================================================
		//! Skip all white spaces
		// ==============================================================================
		void		SkipWhiteSpaces( void );

		// ==============================================================================
		//! Handle a line end
		// ==============================================================================
		void		FinishLine( void );

		// ==============================================================================
		//! Finish the current token
		//!
		//! @param	allowEmpty	true to allow empty tokens
		// ==============================================================================
		void		FinishToken( bool allowEmpty=false );

		// ==============================================================================
		//! Add a character to the token
		//!
		//! @param	c	The character
		// ==============================================================================
		void		AddToToken( char c );

		String		name;				//!< The file or data name
		int			flags;				//!< The lexer flags

		bool		warningTriggered;	//!< Set if a warning has been triggered during the lexing process
		bool		hasByteOrderMark;	//!< True if the file has an utf-8 Byte Order Mark

		// Used while parsing:
		char		tokenBuffer[4096];	//!< Buffer for the token
		Token		token;				//!< The current token
		byte *		buffer;				//!< The file buffer
#ifdef OG_COMMON_USE_FS
		bool		bufferIsFile;		//!< Set if this data was loaded from a file
#endif
		bool		tokIsUnread;		//!< Set if a token was unread
		int			bufSize;			//!< Size of the buffer
		int			bufPos;				//!< The buffer position
		int			tokPos;				//!< The token position
		int			line;				//!< The line the parser is on (not the current token line)

		bool		useEscapeChars;		//!< true to use escape characters
		String		singleTokenChars;	//!< The single token characters
		String		lineComment;		//!< The line comment string
		String		blockComment[2];	//!< The block comment strings
	};

//! @}
//! @}
}

#endif
