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
#include <og/Shared.h>
#include <og/FileSystem/File.h>

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
		//! @param	filename	The (relative) file path
		//!
		//! @return	true if it exists, false otherwise
		// ==============================================================================
		virtual bool		FileExists( const char *filename ) = 0;

		// ==============================================================================
		//! Open a file for reading
		//!
		//! @param	filename	The (relative) file path
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
	//! List of files
	//!
	//! returned by FS->GetFileList()
	// ==============================================================================
	class FileList {
	public:
		// ==============================================================================
		//! Get the next file
		//!
		//! @return	false if there are no more files, otherwise true
		// ==============================================================================
		virtual bool		GetNext( void ) = 0;

		// ==============================================================================
		//! Resets the file index.
		// ==============================================================================
		virtual void		Reset( void ) = 0;

		// ==============================================================================
		//! Get the filename of the current index
		//!
		//! @return	The filename
		// ==============================================================================
		virtual const char *GetName( void ) = 0;

		// ==============================================================================
		//! Get Number of files
		//!
		//! @return	number of files
		// ==============================================================================
		virtual int			Num( void ) = 0;
	};

	// ==============================================================================
	//! File list flags
	// ==============================================================================
	enum ListFlags {
		LF_DIRS				= BIT(0),	//!< List directories.
		LF_FILES			= BIT(1),	//!< List files.
		LF_CHECK_SUBDIRS	= BIT(2),	//!< Dive into subdirectories.
		LF_CHECK_LOCAL		= BIT(3),	//!< List local files.
		LF_CHECK_ARCHIVED	= BIT(4),	//!< List archived files.
		LF_CHECK_UNPURE		= BIT(5),	//!< List also unpure files.
		LF_REMOVE_DIR		= BIT(6),	//!< Removes only first directory, not sub dirs.

		LF_DEFAULT			= (LF_FILES | LF_CHECK_SUBDIRS | LF_CHECK_LOCAL
								| LF_CHECK_ARCHIVED), //!< Default flags
	};

	// ==============================================================================
	//! FileSystem interface.
	//!
	//! Global file access
	//! @note	Thread safety class: multiple
	// ==============================================================================
	class FileSystem {
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

		// Retrieve file lists
		// ==============================================================================
		//! Find files and or directories in the local filesystem
		//!
		//! @param	dir			The directory to search in
		//! @param	extension	The file extension to look for
		//! @param	flags		The flags to use
		//!
		//! @return	NULL if it fails, else the file list. 
		//!
		//! @see	ListFlags
		// ==============================================================================
		virtual FileList *GetFileList( const char *dir, const char *extension, int flags=LF_DEFAULT ) = 0;

		// ==============================================================================
		//! Free a file list previously created by GetFileList
		//!
		//! @param	list	The list to free
		// ==============================================================================
		virtual void	FreeFileList( FileList *list ) = 0;

		// File Access
		// ==============================================================================
		//! Open a file for reading
		//!
		//! @param	filename		The (relative) file path
		//!
		//! @return	Pointer to a new File object on success, otherwise NULL
		// ==============================================================================
		virtual File *	OpenFileRead( const char *filename ) = 0;
		
		// ==============================================================================
		//! Open a file for reading (buffered)
		//!
		//! @param	filename		The (relative) file path
		//!
		//! @return	Pointer to a new File object on success, otherwise NULL
		//!
		//! The file will be loaded completely into memory for faster parsing,
		//! so only use it on files with reasonable size.
		// ==============================================================================
		virtual File *	OpenFileReadBuffered( const char *filename ) = 0;
		
		// ==============================================================================
		//! Open a file for writing
		//!
		//! @param	filename		The (relative) file path
		//!
		//! @return	Pointer to a new File object on success, otherwise NULL
		// ==============================================================================
		virtual File *	OpenFileWrite( const char *filename ) = 0;

		// ==============================================================================
		//! Close a file previously opened by one of the OpenFile* functions
		//!
		//! @param	file	The file to close
		// ==============================================================================
		virtual void	CloseFile( File *file ) = 0;

		// ==============================================================================
		//! Get the size of a given file
		//!
		//! @param	filename		The (relative) file path
		//!
		//! @return	Size of the file if exists, otherwise -1
		// ==============================================================================
		virtual int		FileSize( const char *filename ) = 0;

		// ==============================================================================
		//! Find out if a given file exists
		//!
		//! @param	filename		The (relative) file path
		//!
		//! @return	true if it exists, otherwise false
		// ==============================================================================
		virtual bool	FileExists( const char *filename ) = 0;

		// ==============================================================================
		//! Get the file modification time
		//!
		//! @param	filename		The (relative) file path
		//!
		//! @return	The time in seconds (since 1.1.1970), when the file has been modified last
		// ==============================================================================
		virtual time_t	FileTime( const char *filename ) = 0;

		// ==============================================================================
		//! Load the file into a buffer
		//!
		//! @param	path		The (relative) file path
		//! @param	buffer		Pointer to a byte array, gets set by the function
		//!
		//! @return	The files size on success, otherwise -1
		//!
		//!	@note	You need to free it when you're done.
		// ==============================================================================
		virtual int		LoadFile( const char *path, byte **buffer ) = 0;

		// ==============================================================================
		//! Free a file buffer previously created by LoadFile
		//!
		//! @param	buffer	The buffer to free
		// ==============================================================================
		virtual void	FreeFile( byte *buffer ) = 0;

		// ==============================================================================
		//! Create a path, if it doesn't exist already
		//!
		//! @param	path		Path
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		virtual bool	MakePath( const char *path ) = 0;

	// Same as above, using an explicit OS path
		// ==============================================================================
		//! @copydoc OpenFileRead
		//!
		//! @note	An explicit/full (OS) path is expected
		// ==============================================================================
		virtual File *	OpenFileReadEx( const char *filename ) = 0;

		// ==============================================================================
		//! @copydoc OpenFileReadBuffered
		//!
		//! @note	An explicit/full (OS) path is expected
		// ==============================================================================
		virtual File *	OpenFileReadBufferedEx( const char *filename ) = 0;

		// ==============================================================================
		//! @copydoc OpenFileWrite
		//!
		//! @note	An explicit/full (OS) path is expected
		// ==============================================================================
		virtual File *	OpenFileWriteEx( const char *filename ) = 0;

		// ==============================================================================
		//! @copydoc FileSize
		//!
		//! @note	An explicit/full (OS) path is expected
		// ==============================================================================
		virtual int		FileSizeEx( const char *filename ) = 0;

		// ==============================================================================
		//! @copydoc FileExists
		//!
		//! @note	An explicit/full (OS) path is expected
		// ==============================================================================
		virtual bool	FileExistsEx( const char *filename) = 0;

		// ==============================================================================
		//! @copydoc FileTime
		//!
		//! @note	An explicit/full (OS) path is expected
		// ==============================================================================
		virtual time_t	FileTimeEx( const char *filename) = 0;

		// ==============================================================================
		//! @copydoc LoadFile
		//!
		//! @note	An explicit/full (OS) path is expected
		// ==============================================================================
		virtual int		LoadFileEx( const char *path, byte **buffer ) = 0;

		// ==============================================================================
		//! @copydoc MakePath
		//!
		//! @note	An explicit/full (OS) path is expected
		// ==============================================================================
		virtual bool	MakePathEx( const char *path ) = 0;
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
