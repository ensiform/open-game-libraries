// ==============================================================================
//! @file
//! @brief	FileSystem Interface
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

#ifndef __OG_FILESYSTEM_H__
#define __OG_FILESYSTEM_H__

#include <time.h>
#include <og/Shared/Shared.h>
#include <og/Shared/File.h>
#include <og/Shared/FileSystemCore.h>

//! Open Game Libraries
namespace og {
//! @defgroup FileSystem FileSystem (Library)
//! You need to include <og/FileSystem/FileSystem.h>
//! @see	UserCallbacks, UserFileSystem
//! @todo	Test everything, and on all systems.
//! @todo	Interface to return searchpaths
//! @todo	Ensure all filenames are always stored with forward slashes
//! @{

	class FileSystem;

	// ==============================================================================
	//! Pak file
	//!
	//! passed to User::IsPakFileAllowed
	// ==============================================================================
	class PakFile {
	public:
		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~PakFile() {}

		// ==============================================================================
		//! Get the filename
		//!
		//! @return	The filename
		// ==============================================================================
		virtual const char *GetFilename( void ) = 0;

		// ==============================================================================
		//! Checks if a file with the given filename exists
		//!
		//! @param	filename	The file path
		//!
		//! @return	true if it exists, false otherwise
		// ==============================================================================
		virtual bool		FileExists( const char *filename ) = 0;

		// ==============================================================================
		//! Open a file for reading
		//!
		//! @param	filename	The file path
		//!
		//! @return	NULL if it does not exist, otherwise a File pointer
		// ==============================================================================
		virtual File *		OpenFile( const char *filename ) = 0;
	};

	// ==============================================================================
	//! List of mods
	//!
	//! returned by FS->GetModList()
	// ==============================================================================
	class ModList {
	public:
		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~ModList() {}

		// ==============================================================================
		//! Get the next mod
		//!
		//! @return	false if there are no more mods, otherwise true
		// ==============================================================================
		virtual bool		GetNext( void ) = 0;

		// ==============================================================================
		//! Resets the file index.
		// ==============================================================================
		virtual void		Reset( void ) = 0;

		// ==============================================================================
		//! Get the (relative) mod path of the current index
		//!
		//! @return	The path
		// ==============================================================================
		virtual const char *GetPath( void ) = 0;

		// ==============================================================================
		//! Get Number of files
		//!
		//! @return	number of mods
		// ==============================================================================
		virtual int			Num( void ) = 0;

		// ==============================================================================
		//! Get current mod directory index
		//!
		//! @return	index
		// ==============================================================================
		virtual int			GetCurrentModIndex( void ) = 0;
	};

	// ==============================================================================
	//! FileSystem interface.
	//!
	//! Global file access
	//! @note	Thread safety class: multiple
	// ==============================================================================
	class FileSystem : public FileSystemCore {
	public:
	// Static Interface
	//! @addtogroup FileSystem
	//! @{

		// ==============================================================================
		//! Initializes the filesystem
		//!
		//! @param	pakExtension	The extension of your zip files including the '.' ( for example ".gpk" )
		//! @param	basePath		First searchpath, usually the working directory of the exe
		//! @param	userPath		Second searchpath, and also the path where to store files in
		//!							( for example a directory in the users home directory )
		//! @param	baseDir			The name of the base resources folder ( for example "base" )
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		static bool		Init( const char *pakExtension, const char *basePath, const char *userPath, const char *baseDir );

		// ==============================================================================
		//! Shuts down the filesystem and frees all resources.
		//!
		//! Clears all open files and unloads all pakfiles
		// ==============================================================================
		static void		Shutdown( void );

		// ==============================================================================
		//! Set Pure Mode (restricted file access)
		//!
		//! @param	enable	true to enable, false to disable
		//!
		//! If enabled only the allowed file extensions can be loaded from outside pak files 
		// ==============================================================================
		static void		SetPureMode( bool enable );

		// ==============================================================================
		//! Set the active mod directory
		//!
		//! @param	dir	The mod directory, empty string if none
		//!
		//! @return	true if the mod directory exists, otherwise false
		// ==============================================================================
		static bool		ChangeMod( const char *dir );
	//! @}

	// Object Interface
		// Retrieve the mod list
		// ==============================================================================
		//! Find all mods and their description
		//!
		//! @return	the mod list
		// ==============================================================================
		virtual ModList *GetModList( void ) = 0;

		// ==============================================================================
		//! Free a mod list previously created by GetModList
		//!
		//! @param	list	The list to free
		// ==============================================================================
		virtual void	FreeModList( ModList *list ) = 0;

		// ==============================================================================
		//! Add an extension to the pure list
		//!
		//! @param	ext		The extension
		//!
		//! @see	SetPureMode
		// ==============================================================================
		virtual void	AddPureExtension( const char *ext ) = 0;

		// ==============================================================================
		//! Remove an extension from the pure list
		//!
		//! @param	ext		The extension
		//!
		//! @see	AddPureExtension
		// ==============================================================================
		virtual void	RemovePureExtension( const char *ext ) = 0;
	};
	// ==============================================================================
	//! FileSystem interface pointer (can be passed to the game module)
	//!
	//! @note	This pointer is automatically available in the executable linking to this library,\n
	//!			but needs to be created (and assigned the value of the original) for other modules that need access to it.
	// ==============================================================================
	extern FileSystem *FS;

//! @}

	//! Functions the user must supply
	namespace User {
	//! @addtogroup UserCallbacks
	//! @{
		//! @defgroup UserFileSystem FileSystem (User)
		//!	Functions the user must supply, if using the FileSystem library.
		//! @{

			// ==============================================================================
			//! Called whenever a pakfile is getting loaded
			//!
			//! @param pakFile				Pointer to the pakfile
			//!
			//! @return 				true if you want to allow this pakfile, otherwise false
			//!
			//! In this function, you can use the pakFile pointer to check the pakfile for validity.\n
			//! You could for example check the crc sum against a whitelist,
			//! or read a config file to see if it's currently needed.
			// ==============================================================================
			bool		IsPakFileAllowed( PakFile *pakFile );
		//! @}
	//! @}
	}
}
#endif
