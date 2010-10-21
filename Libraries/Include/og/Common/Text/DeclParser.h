// ==============================================================================
//! @file
//! @brief	Decl File Parser
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

#ifndef __OG_DECLPARSER_H__
#define __OG_DECLPARSER_H__

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	// ==============================================================================
	//! Decl type
	//!
	//! Used to read all dicts of one type
	// ==============================================================================
	class DeclType {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	typeName	Name to look for in the .decl file
		// ==============================================================================
		DeclType( const char *typeName );

		// ==============================================================================
		//! Solve inheritance
		//!
		//! @see	DeclParser::SolveInheritance
		// ==============================================================================
		void			SolveInheritance( void );

		String			name;		//!< Type name
		DictEx<Dict>	declList;	//!< All dictionaries found

	private:
		// ==============================================================================
		//! Do the inheritance for the specified declList entry
		//!
		//! @param	index	The index for the declList
		//!
		//! @see	SolveInheritance
		// ==============================================================================
		void			DoInheritance( int index );

		bool *			inheritanceSolved;	//!< Temp var to see what dictionary has already been solved
		StringList		inheritancePast;	//!< History of all inheritances solved for this dict already,
											//! so we can avoid infinite inheritance
	};

	// ==============================================================================
	//! .Decl file parser
	// ==============================================================================
	class DeclParser {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	flags	Flags passed to the lexer
		//! @note	LEXER_FULL_LINES will be ignored
		// ==============================================================================
		DeclParser( int flags=0 );

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
		bool		LexData( const char *dataName, const byte *data, int size );

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
		//! Adds a decl type
		//!
		//! @param	declType	The DeclType to add
		//!
		//! When parsing a .decl file, it looks for all DeclTypes names
		//! and adds the found dictionary to the DeclType matching the name
		// ==============================================================================
		void		AddDeclType( DeclType *declType );

		// ==============================================================================
		//! Solve all inheritance
		//!
		//! This is called once all wanted files have been lexed
		// ==============================================================================
		void		SolveInheritance( void );

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

		int					lexerFlags;	//!< Flags passed to the lexer
		DictEx<DeclType *>	declTypes;	//!< All DeclTypes to look for / fill with data
	};
//! @}
}

#endif
