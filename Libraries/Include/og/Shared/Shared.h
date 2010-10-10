// ==============================================================================
//! @file
//! @brief	Shared Parts (used by all Open Game Libraries)
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

#ifndef __OG_SHARED_H__
#define __OG_SHARED_H__

#include <og/Setup.h>
#include <og/Shared/SysInfo.h>

//! Open Game
namespace og {
//! @defgroup UserCallbacks User Callbacks
//! Functions the user must supply.
//! @{

	// ==============================================================================
	//! Error Numbers
	//!
	//! When User::Error is called, these are the possible id's and what the msg param would contain in that case.
	//! If not mentioned here, param is always NULL
	// ==============================================================================
	enum ErrorId {
		// Generic Errors:
		ERR_SYSTEM_REQUIREMENTS,	//!< Requirements not met.
									//! \a msg contains the error message
		ERR_BUFFER_OVERFLOW,		//!< More Data passed than expected.
									//! \a msg contains the function name that failed
									//! \a param contains the size of the needed memory, -1 if unknown
		ERR_OUT_OF_MEMORY,			//!< Could not allocate needed memory.
									//! \a msg contains the allocation function name that failed
									//! \a param contains the size of the requested memory, -1 if unknown
		ERR_BAD_FILE_FORMAT,		//!< Unexpected file format error.
									//! \a msg contains error message
									//! \a dataName contains the filepath

		// File Errors
		ERR_FILE_CORRUPT,			//!< Couldn't read from file.
									//! \a msg contains the filetype + error message.
									//! \a dataName contains the filepath
		ERR_FILE_WRITEFAIL,			//!< Couldn't write to file.
									//! \a msg contains the filetype + error message
									//! \a dataName contains the filepath

		// Lexer Problems
		ERR_LEXER_WARNING,			//!< Lexer Warning.
									//! \a msg contains line number and problem
									//! \a dataName contains the filepath
		ERR_LEXER_FAILURE,			//!< Lexer Failure.
									//! \a msg contains line number and problem
									//! \a dataName contains the filepath

		// zLib Errors:
		ERR_ZIP_CRC32,				//!< Crc32 failed when extraction of a file was finished.
									//! \a msg contains zip's filename
									//! \a dataName contains the filepath

		// Filesystem Errors
		ERR_FS_FILE_OPENREAD,		//!< Could not open file for reading.
									//! \a msg is always the same ("Can't open file for reading")
									//! \a dataName contains the filepath
		ERR_FS_FILE_OPENWRITE,		//!< Could not open file for writing.
									//! \a msg is always the same ("Can't open file for writing")
									//! \a dataName contains the filepath
		ERR_FS_MAKEPATH,			//!< Could not create path.
									//! \a msg is always the same ("Can't create path")
									//! \a dataName contains the path

		// Console Errors
		ERR_CVAR_INIT,				//!< You created a cvar with bad data.
									//! \a msg contains the error message
									//! \a dataName contains the cvar name
		ERR_CVAR_LINK,				//!< Problem (un-)linking a cvar
									//! \a msg contains the error message
									//! \a dataName contains the cvar name

		// Audio Errors
		ERR_AUDIO_INIT,				//!< Audio init failed.
									//! \a msg contains error message
	};

	//! Functions the user must supply
	namespace User {
	//! @addtogroup UserCallbacks
	//! @{

		// ==============================================================================
		//! Called whenever an error occurs the user needs to know about.
		//!
		//! @param	id		The error id
		//! @param	msg		Depends on ErrorId
		//! @param	param	Depends on ErrorId
		//!
		//! @see	ErrorId
		// ==============================================================================
		void	Error( ErrorId id, const char *msg, const char *param=NULL );

		// ==============================================================================
		//! Called whenever a warning should be printed
		//!
		//! @param	msg	The message. 
		// ==============================================================================
		void	Warning( const char *msg );

		// ==============================================================================
		//! Called whenever an assert failed.
		//!
		//! @param	code		The expression that failed
		//! @param	function	The function name in which it failed
		// ==============================================================================
		void	AssertFailed( const char *code, const char *function );
	//! @}
	}

	//! Shared
	namespace Shared {
	//! @addtogroup Shared
	//! @{
		OG_INLINE bool	Init( void ) { return SysInfo::Init(); }
		OG_INLINE void	Shutdown( void );
	//! @}
	}
//! @}
}

#endif
