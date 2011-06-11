/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: The actual FileSystem class
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

#ifndef __OG_FILESYSTEM_EX_H__
#define __OG_FILESYSTEM_EX_H__

#include <string>
#include <zlib/zlib.h>
#include <og/Common.h>
#include <og/Common/Thread/EventQueue.h>
#include <og/FileSystem.h>
#include "FileEx.h"
#include "PakFileEx.h"
#include "Utilities.h"

namespace og {

	/*
	==============================================================================

	  FileTrackEvent

	==============================================================================
	*/
	class FileTrackEvent : public QueuedEvent {
	public:
		FileTrackEvent( FileEx *f, bool add ) : file(f), doAdd(add) {}
		void	Execute( void );

		static void	ClearAll( void );
	private:
		FileEx	*	file;
		bool		doAdd;

		static LinkedList<FileEx *> list;	// Keeps track of opened filed to close them later
	};

	/*
	==============================================================================

	  LoadTrackEvent

	==============================================================================
	*/
	class LoadTrackEvent : public QueuedEvent {
	public:
		LoadTrackEvent( byte *b, bool add ) : buffer(b), doAdd(add) {}
		void	Execute( void );

		static void	ClearAll( void );
	private:
		byte	*	buffer;
		bool		doAdd;
		static List<byte *> list;	// Same as above for file buffers
	};

	/*
	==============================================================================

	  FileSystemEx

	==============================================================================
	*/
	class FileSystemEx : public FileSystem, public Thread {
	public:
		// ---------------------- Public FileSystem Interface -------------------

		// Retrieve the mod list
		ModList *GetModList( void );
		void	FreeModList( ModList *list );

		// add and remove extensions that can be loaded without being in a pak file in pure mode.
		void	AddPureExtension( const char *ext );
		void	RemovePureExtension( const char *ext );

		// File Access
		File *	OpenRead( const char *filename, bool pure=true, bool buffered=false );
		File *	OpenWrite( const char *filename, bool pure=true );
		int		FileSize( const char *filename, bool pure=true );
		bool	FileExists( const char *filename, bool pure=true );
		time_t	FileTime( const char *filename, bool pure=true );

		int		LoadFile( const char *path, byte **buffer, bool pure=true );
		void	FreeFile( byte *buffer );
		bool	MakePath( const char *path, bool pure=true );

		// Retrieve file lists
		FileList *GetFileList( const char *dir, const char *extension, int flags=LF_DEFAULT );
		void	FreeFileList( FileList *list );

		// ---------------------- Internal FileSystemEx Members -------------------

	public:
		FileSystemEx();

		// moved from fileeventthread
		void	AddFileEvent( QueuedEvent *evt ) { eventQueue.Add( evt ); WakeUp(); }

	protected:
		void	Run( void );

	private:
		friend class FileSystem;

		// Pak File List enum
		enum pfListId { PFLIST_BASE, PFLIST_MOD, PFLIST_NUM };

		bool	Init( const char *pakExtension, const char *basePath, const char *userPath, const char *baseDir );	// Init the filesystem

		bool	ChangeMod( const char *dir );		// Set the active mod directory
		void	InitModList( void );				// Init the modlist

		void	AddResourceDir( const char *name, pfListId listId );	// Add a resource dir to the filesystem

		void	SetPureMode( bool enable );			// Set Pure Mode (restriced file access)
		bool	IsDir( const char *path );			// Is path a directory?
		bool	MakeDir( const char *path );		// Create a directory

		FileEx *OpenLocalFileRead( const char *filename, int *size=NULL ); // Open a local file for reading.
		int		GetArchivedFileList( const char *dir, const char *extension, StringList &files, int flags=LF_DEFAULT ); // Get all Files with this extension in the specified dir.

		bool	GetModDescription( const char *filename, String &name ); // Read the mods description.txt

	private:
		static TLS<bool> notFoundWarning;

		SharedMutex		sharedMutex;
		String			pakExtension;				// Pakfile extension, for example "gpk"
		String			basePath;					// Base path, normally the working directory.
		String			userPath;					// User path(for storing configs, downloads, ..)
		String			modDir;						// baseDir or mod-folder
		String			baseDir;					// Base Mod directory, for example "base"

		StringList		searchPaths;				// includes all the different paths we want to search (basepath & userpath)
		StringList		resourceDirs;				// Name of all directories that have been added with AddResourceDir()
		List<PakFileEx *>pakFiles[PFLIST_NUM];		// All Open Base & Mod PakFiles to search.

		bool			pureMode;					// Pure mode enabled (like sv_pure in quake3)
		StringList		pureExtensions;				// Extensions allowed when pure mode is enabled

		EventQueue				eventQueue;			// File event queue
	};
}

#endif
