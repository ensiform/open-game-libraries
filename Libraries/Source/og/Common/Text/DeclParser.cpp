/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Decl File Parser
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

namespace og {

#define DECL_DESCRIPTOR_STR "BinDecl"
const int DECL_DESCRIPTOR_LENGTH = 7;

/*
==============================================================================

  DeclType

==============================================================================
*/

/*
================
DeclType::DeclType
================
*/
DeclType::DeclType( const char *typeName ) {
	name = typeName;
	inheritanceSolved = NULL;
}

/*
================
DeclType::SolveInheritance
================
*/
void DeclType::SolveInheritance( void ) {
	int num = declList.Num();
	if ( num == 0 )
		return;

	DynBuffer<bool> buffer(num);
	inheritanceSolved = buffer.data;
	memset( inheritanceSolved, 0, num * sizeof(bool) );
	for( int i=0; i<num; i++ ) {
		inheritancePast.Clear();
		DoInheritance( i );
	}
	inheritanceSolved = NULL;
}

/*
================
DeclType::DoInheritance
================
*/
void DeclType::DoInheritance( int index ) {
	const char *value;
	int inheritFrom;
	for( int i = declList[index].MatchPrefix( "inherit", 7 ); i != -1; i = declList[index].MatchPrefix( "inherit", 7, i ) ) {
		value = declList[index].GetKeyValue(i)->GetValue().c_str();
		if ( inheritancePast.IFind( value ) != -1 ) {
			User::Error(ERR_LEXER_FAILURE, "Inheritance Problem: declaration is inheriting from itself", value );
			return;
		}

		inheritancePast.Append( value );

		inheritFrom = declList.Find( value );
		if ( inheritFrom == -1 ) { 
			User::Error(ERR_LEXER_WARNING, Format("Could not find inheritance '$*' of type $*" ) << value << name, declList.GetKey(index).c_str() );
			continue;
		}

		if ( !inheritanceSolved[inheritFrom] )
			DoInheritance( inheritFrom );

		declList[index].Append( declList[inheritFrom], false );
		inheritancePast.Remove( inheritancePast.IFind( value ) );
	}
	inheritanceSolved[index] = true;
}

/*
==============================================================================

  DeclParser

==============================================================================
*/

/*
================
DeclParser::DeclParser
================
*/
DeclParser::DeclParser( int flags ) {
	lexerFlags = flags;
	if ( lexerFlags & LEXER_FULL_LINES )
		lexerFlags &= ~LEXER_FULL_LINES;
}

/*
================
DeclParser::LoadFile

Loads either the binary or the text version, depending on which is newer.
================
*/
bool DeclParser::LoadFile( const char *filename ) {
	if ( commonFS == NULL )
		return false;
	String binFilename(filename);
	binFilename += ".bin";
	bool binExists = commonFS->FileExists( binFilename.c_str() );
	bool txtExists = commonFS->FileExists( filename );
	if ( binExists && txtExists ) {
		if ( commonFS->FileTime( binFilename.c_str() ) > commonFS->FileTime( filename ) )
			return BinaryFile( binFilename.c_str() );
		return LexFile(filename);
	}
	else if ( binExists )
		return BinaryFile( binFilename.c_str() );
	else if ( txtExists )
		return LexFile( filename );
	return false;
}

/*
================
DeclParser::LexFile
================
*/
bool DeclParser::LexFile( const char *filename ) {
	Lexer lexer(lexerFlags);
	if ( !lexer.LoadFile( filename ) )
		return false;

	try {
		Parse(lexer);
		return true;
	}
	catch( LexerError err ) {
		String errStr;
		err.ToString( errStr );
		User::Error( ERR_LEXER_FAILURE, errStr.c_str(), filename );
		return false;
	}
}

/*
================
DeclParser::LexData
================
*/
bool DeclParser::LexData( const char *dataName, const byte *data, int size ) {
	Lexer lexer(lexerFlags);
	if ( !lexer.LoadData( dataName, data, size ) )
		return false;

	try {
		Parse(lexer);
		return true;
	}
	catch( LexerError err ) {
		String errStr;
		err.ToString( errStr );
		User::Error( ERR_LEXER_FAILURE, errStr.c_str(), dataName );
		return false;
	}
}

/*
================
DeclParser::Parse
================
*/
void DeclParser::Parse( Lexer &lexer ) {
	Dict *resultDict = NULL;

	bool getKeyValue = false;

	int index;
	const Token *token;
	String key, value;

	const char *p;
	while ( (token = lexer.ReadToken()) != NULL ) {
		p = token->GetString();
		if ( p ) {
			if ( *p == '\0' )
				lexer.Error("Unexpected Empty Token");
			if ( !getKeyValue ) {
				index = declTypes.Find(p);
				if ( index != -1 ) {
					value = lexer.ReadString();

					if ( value.IsEmpty() )
						lexer.Error("Empty name!");

					DictEx<Dict> &result = declTypes[index]->declList;
					index = result.Find(p);
					if ( index == -1 ) 
						resultDict = &result[p];
					else {
						resultDict = &result[index];
						resultDict->Clear();
					}
				} else {
					resultDict = NULL;
					lexer.Warning( Format("Unknown decl Type '$*'") << p );
				}
				lexer.ExpectToken("{");
				getKeyValue = true;
			} else {
				if ( *p == '}' )
					getKeyValue = false;
				else if ( resultDict ) {
					key = p;
					(*resultDict).Set( key.c_str(), lexer.ReadString() );
				}
			}
		}
	}
	if ( getKeyValue )
		throw LexerError( LexerError::END_OF_FILE );
}

/*
================
DeclParser::BinaryFile
================
*/
bool DeclParser::BinaryFile( const char *filename ) {
	if ( commonFS == NULL )
		return false;
	File *file = commonFS->OpenRead( filename );
	if ( !file )
		return false;

	try {
		bool ret = BinaryFile( file );
		file->Close();
		return ret;
	}
	catch( FileReadWriteError err ) {
		file->Close();
		User::Error( ERR_FILE_CORRUPT, Format("Decl: $*" ) << err.ToString(), filename );
		return false;
	}
}

/*
================
DeclParser::BinaryFile
================
*/
bool DeclParser::BinaryFile( File *file ) {
	char descriptor[DECL_DESCRIPTOR_LENGTH];
	file->Read( descriptor, DECL_DESCRIPTOR_LENGTH );
	if ( String::Cmpn( descriptor, DECL_DESCRIPTOR_STR, DECL_DESCRIPTOR_LENGTH ) != 0 )
		return false;

	return false;
}

/*
================
DeclParser::AddDeclType
================
*/
void DeclParser::AddDeclType( DeclType *declType ) {
	declTypes[declType->name.c_str()] =  declType;
}

/*
================
DeclParser::SolveInheritance
================
*/
void DeclParser::SolveInheritance( void ) {
	int num = declTypes.Num();
	for( int i=0; i<num; i++ )
		declTypes[i]->SolveInheritance();
}

/*
================
DeclParser::MakeBinary
================
*/
bool DeclParser::MakeBinary( const char *filename ) {
	if ( commonFS == NULL )
		return false;

	Lexer lexer;
	if ( !lexer.LoadFile( filename ) )
		return false;

	File *f = commonFS->OpenWrite( Format( "$*.bin" ) << filename );
	if ( !f )
		return false;

	try {
		f->Write( DECL_DESCRIPTOR_STR, DECL_DESCRIPTOR_LENGTH );

		Dict dict;
		bool getKeyValue = false;

		const Token *token;
		String key, value;

		String str;
		const char *p;
		while ( (token = lexer.ReadToken()) != NULL ) {
			//! @todo	maybe token should be stored as String, so we don't have to recalc len/bytelen
			p = token->GetString();
			if ( p ) {
				if ( *p == '\0' ) {
					lexer.Error("Unexpected Empty Token");
					return false;
				}
				if ( !getKeyValue ) {
					str = p;
					str.WriteToFile( f );
					value = lexer.ReadString();

					if ( value.IsEmpty() ) {
						lexer.Error("Empty name!");
						return false;
					}
					value.WriteToFile( f );

					lexer.ExpectToken("{");
					getKeyValue = true;
				} else {
					if ( *p == '}' ) {
						getKeyValue = false;
						dict.WriteToFile( f );
						dict.Clear();
					}
					else {
						key = p;
						dict.Set( key.c_str(), lexer.ReadString() );
					}
				}
			}
		}
		if ( getKeyValue ) {
			lexer.Error("Unexpected End Of File");
			return false;
		}
		return true;
	}
	catch( FileReadWriteError err ) {
		f->Close();
		User::Error( ERR_FILE_WRITEFAIL, Format("Binary Decl: $*.bin" ) << err.ToString(), filename );
		return false;
	}
	catch( LexerError err ) {
		f->Close();
		String errStr;
		err.ToString( errStr );
		User::Error( ERR_LEXER_FAILURE, errStr.c_str(), filename );
		return false;
	}
}

}
