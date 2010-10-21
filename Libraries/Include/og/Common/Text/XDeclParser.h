// ==============================================================================
//! @file
//! @brief	Extended ( Nested ) Decl File Parser
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

#ifndef __OG_XDECLPARSER_H__
#define __OG_XDECLPARSER_H__

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	// ==============================================================================
	//! XDecl node
	// ==============================================================================
	class XDeclNode {
	public:
		// ==============================================================================
		//! Default constructor
		// ==============================================================================
		XDeclNode();

		// ==============================================================================
		//! Get the name of this node
		//!
		//! @return	The name
		// ==============================================================================
		const char *	GetName( void ) const { return name.c_str(); }

		// ==============================================================================
		//! Get the dictionary of this node
		//!
		//! @return	Const reference to the dictionary
		// ==============================================================================
		const Dict &	GetDict( void ) const { return dict; }

		// ==============================================================================
		//! Get the first child of this node
		//!
		//! @return	NULL if this node has no child, else the first child
		// ==============================================================================
		const XDeclNode *GetFirstChild( void ) const { return firstChild; }

		// ==============================================================================
		//! Get the first child by name
		//!
		//! @param	name	The name of the node you are looking for
		//!
		//! @return	NULL if this node has no (direct) childs with this name, else the first child by name
		// ==============================================================================
		const XDeclNode *GetFirstChildByName( const char *name ) const; // return first child that has this name.

		// ==============================================================================
		//! Get the next sibling node
		//!
		//! @return	NULL if there are no nodes left, else the next node
		// ==============================================================================
		const XDeclNode *GetNext( void ) const { return next; }

		// ==============================================================================
		//! Get the next sibling node with the same name as this one
		//!
		//! @return	NULL if there are no nodes left, else the next node
		// ==============================================================================
		const XDeclNode *GetNextByName( void ) const;

	private:
		friend class XDeclParser;
		String			name;			//!< The name
		Dict			dict;			//!< The dictionary
		uInt			numChildren;	//!< Number of children
		XDeclNode *		firstChild;		//!< The first child
		XDeclNode *		lastChild;		//!< The last child
		XDeclNode *		next;			//!< The next sibling
	};

	// ==============================================================================
	//! .XDecl file parser
	//!
	//! @todo	add granularity to pass to allocator
	// ==============================================================================
	class XDeclParser {
	public:
		// ==============================================================================
		//! Loads a file and parses it
		//!
		//! @param	filename	The (relative) file path
		//!
		//! @return	true if it succeeds, false if it fails
		//!
		//! Loads either the binary or the text version, depending on which is newer
		// ==============================================================================
		bool		LoadFile( const char *filename );

		// ==============================================================================
		//! Lex a file
		//!
		//! @param	filename	The (relative) file path
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		bool		LexFile( const char *filename );

		// ==============================================================================
		//! Lex raw data
		//!
		//! @param	dataName	Name of the data ( for error messages )
		//! @param	data		Raw data (text)
		//! @param	size		The size of the data
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		bool		LexData( const char *dataName, byte *data, int size );

		// ==============================================================================
		//! Load from a binary file
		//!
		//! @param	filename	The (relative) file path
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		bool		BinaryFile( const char *filename );

		// ==============================================================================
		//! Load from a binary file
		//!
		//! @param	file	the file to read from
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		bool		BinaryFile( File *file );

		// ==============================================================================
		//! Get the first child of this item
		//!
		//! @return	NULL if it fails, else the first child
		// ==============================================================================
		const XDeclNode *GetFirstChild( void ) const { return rootNode.GetFirstChild(); }

		// ==============================================================================
		//! Get the first child by name
		//!
		//! @param	name	The name of the node you are looking for
		//!
		//! @return	NULL if this node has no (direct) childs with this name, else the first child by name
		// ==============================================================================
		const XDeclNode *GetFirstChildByName( const char *name ) const { return rootNode.GetFirstChildByName( name ); }

		// ==============================================================================
		//! Convert the file from text to binary format for faster parsing
		//!
		//! @param	filename	The (relative) file path
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		static bool	MakeBinary( const char *filename );

	private:

		// ==============================================================================
		//! Parses the file loaded in the lexer
		//!
		//! @param	lexer	The lexer to use
		// ==============================================================================
		void		Parse( Lexer &lexer );

		// ==============================================================================
		//! Read a node and its children from a binary file
		//!
		//! @param	node	The node
		//! @param	f		The file
		// ==============================================================================
		void		ReadNodeAndChildren( XDeclNode *node, File *f );

		// ==============================================================================
		//! Write a node and its children to a binary file
		//!
		//! @param	node	The node
		//! @param	f		The file
		// ==============================================================================
		static void	WriteNodeAndChildren( const XDeclNode *node, File *f );

		XDeclNode rootNode;				//!< The root node
		Allocator<XDeclNode> allocator;	//!< An allocator
	};
//! @}
}

#endif
