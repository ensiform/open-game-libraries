/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Text Lexer
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

#include <og/Common/Common.h>
#include <stdarg.h>

namespace og {
extern FileSystemCore *commonFS;

/*
==============================================================================

  LexerError

==============================================================================
*/

/*
================
LexerError::LexerError
================
*/
LexerError::LexerError( ErrorType type, int line, const char *expected, const char *found ) {
	this->type = type;
	this->line = line;
	if ( expected != NULL )
		this->expected = expected;
	if ( found != NULL )
		this->found = found;
}

/*
================
LexerError::ToString
================
*/
void LexerError::ToString( String &result ) {
	switch( type ) {
		case CUSTOM:
			result = Format("Error '$*' on line $*") << expected << line;
			return;
		case BAD_TOKEN:
			result = Format("Expected token of type '$*', found '$*' on line $*") << expected << found << line;
			return;
		case MISSING_TOKEN:
			result = Format("Expected token '$*', found '$*' on line $*") << expected << found << line;
			return;
		case END_OF_FILE:
			result = "Unexpected End of File";
			return;
		default:
			result = "Unknown lexer error";
			return;
	}
}

/*
==============================================================================

  Token

==============================================================================
*/

/*
================
Token::Token
================
*/
Token::Token( const char *data ) {
	string = data;
}

/*
================
Token::GetString
================
*/
const char *Token::GetString( void ) const {
	return string;
}

/*
================
Token::GetDouble
================
*/
double Token::GetDouble( void ) const {
	return String::ToDouble(string);
}

/*
================
Token::GetFloat
================
*/
float Token::GetFloat( void ) const {
	return String::ToFloat(string);
}

/*
================
Token::GetInteger
================
*/
int Token::GetInteger( void ) const {
	return String::ToInt(string);
}

/*
==============================================================================

  Lexer

==============================================================================
*/

/*
================
Lexer::Lexer
================
*/
Lexer::Lexer( int _flags ) : token(tokenBuffer) {
	tokenBuffer[0] = '\0';
	buffer = NULL;
	bufferIsFile = false;
	flags = _flags;
	warningTriggered = false;
	hasByteOrderMark = false;
	tokIsUnread = true;
	tokPos = -1;

	useEscapeChars = true;
	SetSingleTokenChars( "{}(),;$=/" );
	SetCommentStrings( "//", "/*", "*/" );
}

/*
================
Lexer::~Lexer
================
*/
Lexer::~Lexer() {
	if ( buffer ) {
		if ( !bufferIsFile )
			delete[] buffer;
		else {
			//! @todo	error
			if ( commonFS != NULL )
				commonFS->FreeFile(buffer);
		}
	}
}

/*
================
Lexer::SetSingleTokenChars
================
*/
void Lexer::SetSingleTokenChars( const char *chars ) {
	singleTokenChars = chars;
}

/*
================
Lexer::SetCommentStrings
================
*/
void Lexer::SetCommentStrings( const char *line, const char *blockStart, const char *blockEnd ) {
	lineComment = line;
	blockComment[0] = blockStart;
	blockComment[1] = blockEnd;
}

/*
================
Lexer::Error
================
*/
void Lexer::Error( const char *message ) {
	throw LexerError( LexerError::CUSTOM, token.line, message );
}

/*
================
Lexer::Warning
================
*/
void Lexer::Warning( const char *message ) {
	warningTriggered = true;
	if ( flags & LEXER_NOWARNINGS )
		return;

	User::Error(ERR_LEXER_WARNING, message, Format("file $*, line $*" ) << name << token.line );
}

/*
================
Lexer::ReadToken
================
*/
const Token *Lexer::ReadToken( void ) {
	if ( tokPos == -1 )
		tokPos = 0;
	else if ( tokIsUnread ) {
		tokIsUnread = false;
		return &token;
	}
	while ( bufPos < bufSize ) {
		// Single line comments
		if ( !lineComment.IsEmpty() &&
			String::CmpPrefix((const char *)(buffer+bufPos), lineComment.c_str()) == 0 ) {
			if ( !(flags & LEXER_FULL_LINES) && tokPos > 0 ) {
				FinishToken();
				return &token;
			}
			bufPos += 2;
			if ( tokPos > 0 ) {
				SkipRestOfLine();
				return &token;
			}
			SkipRestOfLine();

			continue;
		}
		// Multiline comments
		else if ( !blockComment[0].IsEmpty() &&
			String::CmpPrefix((const char *)(buffer+bufPos), blockComment[0].c_str()) == 0 ) {
				if ( !(flags & LEXER_FULL_LINES) && tokPos > 0 ) {
					FinishToken();
					return &token;
				}
				bufPos += 2;
				int l = line;
				SkipToCommentEnd(); 
				// If changed line during comment, finish the line.
				if ( (flags & LEXER_FULL_LINES) && l != line && tokPos > 0 ) {
					FinishToken();
					return &token;
				}
				continue;
		}

		switch( buffer[bufPos] ) {
			// Ignore whitespaces
			case ' ':
			case '\t':
				SkipWhiteSpaces();
				if ( flags & LEXER_FULL_LINES )
					AddToToken( ' ' );
				else if ( tokPos > 0 ) {
					FinishToken();
					return &token;
				}
				continue;

			// Newlines
			case '\n':
			case '\r':
				if ( !tokPos ) {
					FinishLine();
					continue;
				}
				FinishLine();
				return &token;

			// Quoted tokens
			case '\"':
			case '\'':
				if ( flags & LEXER_FULL_LINES ) {
					ParseQuoted();
					continue;
				}
				if ( tokPos > 0 ) {
					FinishToken();
					return &token;
				}
				ParseQuoted();
				return &token;

			default:
				// Check for single char tokens
				if ( (flags & LEXER_FULL_LINES) == 0 && !singleTokenChars.IsEmpty() ) {
					char c = buffer[bufPos];
					const char *chars = singleTokenChars.c_str();
					int num = singleTokenChars.Length();
					for( int i=0; i<num; i++ ) {
						if ( c != chars[i] )
							continue;
						if ( tokPos == 0 ) {
							AddToToken( buffer[bufPos] );
							bufPos++;
						}
						FinishToken();
						return &token;
					}
				}
				// Add this char to the token
				AddToToken( buffer[bufPos] );
				bufPos++;
				continue;
		}
	}
	if ( tokPos > 0 ) {
		FinishToken();
		return &token;
	}
	line = 0;

	return NULL;
}

/*
================
Lexer::UnreadToken
================
*/
void Lexer::UnreadToken( void ) {
	OG_ASSERT( !tokIsUnread );
	tokIsUnread = true;
}

/*
================
Lexer::ExpectToken
================
*/
void Lexer::ExpectToken( const char *string ) {
	if ( !ReadToken() )
		throw LexerError( LexerError::END_OF_FILE );
	if ( String::Icmp( token.GetString(), string ) != 0 )
		throw LexerError( LexerError::MISSING_TOKEN, token.line, string, token.GetString() );
}

/*
================
Lexer::CheckToken
================
*/
bool Lexer::CheckToken( const char *string ) {
	if ( !ReadToken() )
		throw LexerError( LexerError::END_OF_FILE );
	if ( String::Icmp( token.GetString(), string ) != 0 ) {
		UnreadToken();
		return false;
	}
	return true;
}

/*
================
Lexer::ReadInt
================
*/
int Lexer::ReadInt( void ) {
	if ( !ReadToken() )
		throw LexerError( LexerError::END_OF_FILE );
	if ( !String::IsNumeric( token.GetString() ) )
		throw LexerError( LexerError::BAD_TOKEN, token.line, "integer", token.GetString() );
	return String::ToInt( token.GetString() );
}

/*
================
Lexer::ReadFloat
================
*/
float Lexer::ReadFloat( void ) {
	if ( !ReadToken() )
		throw LexerError( LexerError::END_OF_FILE );
	if ( !String::IsNumeric( token.GetString() ) )
		throw LexerError( LexerError::BAD_TOKEN, token.line, "float", token.GetString() );
	return String::ToFloat( token.GetString() );
}

/*
================
Lexer::ReadString
================
*/
const char *Lexer::ReadString( void ) {
	if ( !ReadToken() )
		throw LexerError( LexerError::END_OF_FILE );
	return token.GetString();
}

/*
================
Lexer::GetNextLine
================
*/
bool Lexer::GotoNextLine( void ) {
	int oldLine = token.line;
	while( ReadToken() ) {
		if ( token.line > oldLine ) {
			UnreadToken();
			return true;
		}
	}
	return false;
}

/*
================
Lexer::FindToken
================
*/
void Lexer::FindToken( const char *str ) {
	while ( ReadToken() ) {
		if ( String::Icmp( token.GetString(), str ) == 0 )
			return;
	}
	throw LexerError( LexerError::MISSING_TOKEN, token.line, str, "[EOF]" );
}

/*
================
Lexer::LoadFile
================
*/
bool Lexer::LoadFile( const char *filename ) {
	if ( commonFS == NULL )
		return false;

	buffer = NULL;
	name = filename;
	bufSize = commonFS->LoadFile( filename, &buffer );
	if ( bufSize == -1 )
		return false;

	byte bom[] = { 0xEF, 0xBB, 0xBF };
	hasByteOrderMark = memcmp( buffer, bom, 3) == 0;
	if ( hasByteOrderMark )
		bufPos = 3;
	else {
		bufPos = 0;
		if ( !(flags & LEXER_NO_BOM_WARNING) )
			User::Warning( Format("File missing BOM: '$*'" ) << filename );
	}

	bufferIsFile = true;

	tokPos = -1;
	line = 1;
	return true;
}

/*
================
Lexer::LoadData
================
*/
bool Lexer::LoadData( const char *dataName, const byte *data, int size ) {
	if ( !dataName || !data || size < 1 )
		return false;

	byte bom[] = { 0xEF, 0xBB, 0xBF };
	hasByteOrderMark = memcmp( buffer, bom, 3) == 0;
	if ( hasByteOrderMark )
		bufPos = 3;
	else {
		bufPos = 0;
		if ( !(flags & LEXER_NO_BOM_WARNING) )
			User::Warning( Format("File missing BOM: '$*'" ) << dataName );
	}

	buffer = new byte[size];
	memcpy(buffer, data, size);
	name = dataName;
	bufSize = size;

	bufferIsFile = false;
	tokPos = -1;
	line = 1;
	return true;
}

/*
================
Lexer::ParseQuoted
================
*/
void Lexer::ParseQuoted( void ) {
	char c = buffer[bufPos];
	if ( flags & LEXER_FULL_LINES )
		AddToToken( buffer[bufPos] );

	bufPos++;

	while ( bufPos < bufSize ) {
		// Check for newlines
		if ( buffer[bufPos] == '\n' || buffer[bufPos] == '\r' )
			throw LexerError( LexerError::BAD_TOKEN, token.line, Format( "quote end: '\\$*'" ) << c, "[EOL]" );

		// Check for escape characters
		if ( useEscapeChars && buffer[bufPos] == '\\' ) {
			if ( buffer[bufPos+1] == 'n' ) {
				AddToToken( '\n' );
				bufPos += 2;
				continue;
			} else if ( buffer[bufPos+1] == 't' ) {
				AddToToken( '\t' );
				bufPos += 2;
				continue;
			} else if ( buffer[bufPos+1] == '\"' || buffer[bufPos+1] == '\'' ) {
				AddToToken( buffer[bufPos+1] );
				bufPos += 2;
				continue;
			}
		}
		// Check if this is the quote end we are looking for.
		else if ( buffer[bufPos] == c ) {
			if ( flags & LEXER_FULL_LINES )
				AddToToken( buffer[bufPos] );
			else
				FinishToken( true );
			bufPos++;
			return;
		}

		AddToToken( buffer[bufPos] );
		bufPos++;
	}
	throw LexerError( LexerError::BAD_TOKEN, token.line, Format("quote end: '\\$*'" ) << c, "[EOF]" );
}

/*
================
Lexer::SkipToCommentEnd

For multi line comments
================
*/
void Lexer::SkipToCommentEnd( void ) {
	while ( bufPos < bufSize ) {
		if ( String::CmpPrefix((const char *)(buffer+bufPos), blockComment[1].c_str()) == 0 ) {
			bufPos += blockComment[1].Length();
			return;
		}
		if ( buffer[bufPos] == '\n' )
			line++;
		bufPos++;
	}
}

/*
================
Lexer::SkipRestOfLine
================
*/
void Lexer::SkipRestOfLine( void ) {
	while ( bufPos < bufSize ) {
		if ( buffer[bufPos] == '\n' || buffer[bufPos] == '\r' ) {
			FinishLine();
			return;
		}
		bufPos++;
	}
	// End of file
}

/*
================
Lexer::SkipWhiteSpaces
================
*/
void Lexer::SkipWhiteSpaces( void ) {
	bufPos++;
	while ( bufPos < bufSize && (buffer[bufPos] == ' ' || buffer[bufPos] == '\t') )
		bufPos++;
}

/*
================
Lexer::FinishLine
================
*/
void Lexer::FinishLine( void ) {
	if ( buffer[bufPos] == '\r' )
		bufPos += 2; // skip \n too
	else
		bufPos++;

	FinishToken();
	line++;
}

/*
================
Lexer::FinishToken
================
*/
void Lexer::FinishToken( bool allowEmpty ) {
	if ( tokPos || allowEmpty ) {
		tokenBuffer[tokPos] = '\0';
		token.line = line;
		tokPos = 0;
		tokIsUnread = false;
	}
}

/*
================
Lexer::AddToToken
================
*/
void Lexer::AddToToken( char c ) {
	if ( !hasByteOrderMark && c < 0 )
		tokenBuffer[tokPos] = '?';
	else
		tokenBuffer[tokPos] = c; 
	tokPos++;
}

}
