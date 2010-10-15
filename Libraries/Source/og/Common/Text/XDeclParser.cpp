/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Extended ( Nested ) Decl File Parser
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

#define XDECL_DESCRIPTOR_STR "BinXDecl"
const int XDECL_DESCRIPTOR_LENGTH = 8;

/*
==============================================================================

  XDeclNode

==============================================================================
*/

/*
================
XDeclNode::XDeclNode
================
*/
XDeclNode::XDeclNode() {
	numChildren = 0;
	firstChild = NULL;
	lastChild = NULL;
	next = NULL;
}

/*
================
XDeclNode::GetFirstChildByName

return first child that has this name.
================
*/
const XDeclNode *XDeclNode::GetFirstChildByName( const char *name ) const {
	XDeclNode *child = firstChild;
	while( child ) {
		if ( child->name.Icmp(name) == 0 )
			return child;
		child = child->next;
	}
	return NULL;
}

/*
================
XDeclNode::GetNextByName

return next that has the same name.
================
*/
const XDeclNode *XDeclNode::GetNextByName( void ) const {
	XDeclNode *sibling = next;
	while( sibling ) {
		if ( sibling->name.Icmp(name.c_str()) == 0 )
			return sibling;
		sibling = sibling->next;
	}
	return NULL;
}

/*
==============================================================================

  XDeclParser

==============================================================================
*/

/*
================
XDeclParser::LoadFile

Loads either the binary or the text version, depending on which is newer.
================
*/
bool XDeclParser::LoadFile( const char *filename ) {
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
XDeclParser::LexFile
================
*/
bool XDeclParser::LexFile( const char *filename ) {
	Lexer lexer;
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
XDeclParser::LexData
================
*/
bool XDeclParser::LexData( const char *dataName, byte *data, int size ) {
	Lexer lexer;
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
XDeclParser::Parse
================
*/
void XDeclParser::Parse( Lexer &lexer ) {
	const Token *token;
	String name, value;
	XDeclNode *currentNode = &rootNode;
	XDeclNode *newNode;
	bool getKeyValue = false;
	bool addNode;
	Stack<XDeclNode *> nodeStack;

	const char *p;
	while ( (token = lexer.ReadToken()) != NULL ) {
		p = token->GetString();
		if ( p ) {
			if ( *p == '\0' )
				lexer.Error("Unexpected Empty Token");

			if ( !getKeyValue ) {
				if ( String::Cmp( p, "}" ) == 0 ) {
					currentNode = nodeStack.Top();
					nodeStack.Pop();
				} else {
					name = p;
					addNode = false;
					if ( lexer.CheckToken("=") ) {
						currentNode->dict.Set( name.c_str(), lexer.ReadString() );
						lexer.ExpectToken(";");
					} else if ( lexer.CheckToken("(") ) {
						addNode = true;
						getKeyValue = true;
					} else if ( lexer.CheckToken("{") ) {
						addNode = true;
					} else {
						lexer.Error("Syntax Error, was expecting '(', '=' or '{'");
					}
					if ( addNode ) {
						newNode = allocator.Alloc();
						newNode->name = name;
						currentNode->numChildren++;
						if ( currentNode->firstChild == NULL )
							currentNode->firstChild = newNode;
						else
							currentNode->lastChild->next = newNode;
						currentNode->lastChild= newNode;
						nodeStack.Push( currentNode );
						currentNode = newNode;
					}
				}
			} else  {
				name = p;
				lexer.ExpectToken("=");
				currentNode->dict.Set( name.c_str(), lexer.ReadString() );
				if ( !lexer.CheckToken(",") ) {
					lexer.ExpectToken(")");
					if ( lexer.CheckToken(";") ) {
						currentNode = nodeStack.Top();
						nodeStack.Pop();
						getKeyValue = false;
					} else if ( lexer.CheckToken("{") ) {
						getKeyValue = false;
					} else {
						lexer.Error("Syntax Error, was expecting ';' or '{'");
					}
				}
			}
		}
	}
	if ( getKeyValue || nodeStack.Num() )
		throw LexerError( LexerError::END_OF_FILE );
}

/*
================
XDeclParser::BinaryFile
================
*/
bool XDeclParser::BinaryFile( const char *filename ) {
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
	catch( FileReadWriteError &err ) {
		file->Close();
		User::Error( ERR_FILE_CORRUPT, Format("XDecl: $*" ) << err.ToString(), filename );
		return false;
	}
}

/*
================
XDeclParser::BinaryFile
================
*/
bool XDeclParser::BinaryFile( File *file ) {
	char descriptor[XDECL_DESCRIPTOR_LENGTH];
	file->Read( descriptor, XDECL_DESCRIPTOR_LENGTH );
	if ( String::Cmpn( descriptor, XDECL_DESCRIPTOR_STR, XDECL_DESCRIPTOR_LENGTH ) != 0 )
		return false;

	ReadNodeAndChildren( &rootNode, file );
	return true;
}

/*
================
XDeclParser::ReadNodeAndChildren
================
*/
void XDeclParser::ReadNodeAndChildren( XDeclNode *node, File *f ) {
	XDeclNode *newNode;
	node->numChildren = f->ReadUint();
	for( int i=0; i<node->numChildren; i++ ) {
		newNode = allocator.Alloc();
		if ( node->firstChild == NULL )
			node->firstChild = newNode;
		else
			node->lastChild->next = newNode;
		node->lastChild= newNode;

		newNode->name.ReadFromFile( f );
		newNode->dict.ReadFromFile( f );
		ReadNodeAndChildren( newNode, f );
	}
}

/*
================
XDeclParser::MakeBinary
================
*/
bool XDeclParser::MakeBinary( const char *filename ) {
	if ( commonFS == NULL )
		return false;

	XDeclParser parser;
	if ( !parser.LexFile( filename ) )
		return false;
	
	File *f = commonFS->OpenWrite( Format( "$*.bin" ) << filename );
	if ( !f )
		return false;

	try {
		f->Write( XDECL_DESCRIPTOR_STR, XDECL_DESCRIPTOR_LENGTH );

		const XDeclNode *node = parser.GetFirstChild();
		f->WriteUint( parser.rootNode.numChildren );
		if ( node->numChildren )
			WriteNodeAndChildren( node, f );

		f->Close();
		return true;
	}
	catch( FileReadWriteError &err ) {
		f->Close();
		User::Error( ERR_FILE_WRITEFAIL, Format("Binary XDecl: $*" ) << err.ToString(), filename );
		return false;
	}
}

/*
================
XDeclParser::WriteNodeAndChildren
================
*/
void XDeclParser::WriteNodeAndChildren( const XDeclNode *node, File *f ) {
	while( node ) {
		node->name.WriteToFile( f );
		node->GetDict().WriteToFile( f );
		f->WriteUint( node->numChildren );
		if ( node->numChildren )
			WriteNodeAndChildren( node->GetFirstChild(), f );
		node = node->GetNext();
	}
}

}
