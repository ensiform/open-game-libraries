// ==============================================================================
//! @file
//! @brief	Command Args for Console Commands
//! @author	Ensiform
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

#ifndef __OG_ARGUMENTS_H__
#define __OG_ARGUMENTS_H__

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	// ==============================================================================
	//! Console command arguments
	// ==============================================================================
	class CmdArgs : public StringList {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	str	The string to tokenize
		// ==============================================================================
		CmdArgs( const char *str=NULL ) : StringList(16) { if ( str ) TokenizeString( str ); }

		// ==============================================================================
		//! Get the number of arguments
		//!
		//! @return	Number of arguments found
		// ==============================================================================
		int					Argc( void ) const { return num; }

		// ==============================================================================
		//! Get the nth argument
		//!
		//! @param	i	The index
		//!
		//! @return	NULL if it fails, else a pointer to the nth argument
		// ==============================================================================
		const char *		Argv( int i ) const;

		// ==============================================================================
		//! All arguments within the parameters concatened together to one string.
		//!
		//! @param	result	The string to store the result in
		//! @param	start	The start index
		//! @param	end		The last argument you want included, -1 for all
		//! @param	escape	true to add quotes around strings
		// ==============================================================================
		void				Args( String &result, int start = 1, int end = -1, bool escape = false ) const;

		// ==============================================================================
		//! Parse a string for arguments
		//!
		//! @param	str	The string to parse
		// ==============================================================================
		void				TokenizeString( const char *str );

	private:
		static const int	MAX_CMD_ARGUMENTS	= 64;	//!< The maximum number of command arguments
		static const int	MAX_CMD_STRING		= 2048;	//!< The maximum number of bytes one argument can hold
	};
//! @}
}

#endif
