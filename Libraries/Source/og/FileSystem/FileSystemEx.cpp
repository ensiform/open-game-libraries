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

#include <sys/stat.h>
#include <algorithm>

#include "FileSystemEx.h"

namespace og {

FileSystem *FS = NULL;
TLS<bool> FileSystemEx::notFoundWarning(true);

/*
==============================================================================

  ModListEx

==============================================================================
*/
class ModListEx : public ModList {
public:
	// ---------------------- Public ModList Interface -------------------

	bool		GetNext( void ) {
		if ( (position+1) < paths.Num() ) {
			position++;
			return true;
		}
		return false;
	}
	void		Reset( void ) { position = 0; }
	const char *GetPath( void ) { return paths[position].c_str(); }
	int			Num( void ) { return paths.Num(); }
	int			GetCurrentModIndex( void ) { return currentModIndex; }

	// ---------------------- Internal ModListEx Members -------------------

public:
	ModListEx() { position = 0; currentModIndex = -1; }

	uInt			position;
	int				currentModIndex;
	StringList		paths;
};

/*
==============================================================================

  FileSystemEx

==============================================================================
*/

// These are here so we don't have to expose them to the public interface.
bool FileSystem::Init( const char *pakExtension, const char *basePath, const char *userPath, const char *baseDir ) {
	if ( FS != NULL )
		return false; //! @todo error

	FileSystemEx *fileSys = new FileSystemEx;
	fileSys->Start("FileSystemEx");
	FS = fileSys;
	if ( !fileSys->Init( pakExtension, basePath, userPath, baseDir ) ) {
		fileSys->Stop();
		FS = NULL;
		return false;
	}
	return true;
}
void FileSystem::Shutdown( void )
	{ static_cast<FileSystemEx *>(FS)->Stop(); FS = NULL; }
void FileSystem::SetPureMode( bool enable )
	{ static_cast<FileSystemEx *>(FS)->SetPureMode( enable ); }
bool FileSystem::ChangeMod( const char *dir )
	{ return static_cast<FileSystemEx *>(FS)->ChangeMod( dir ); }

/*
================
FileSystemEx::FileSystemEx
================
*/
FileSystemEx::FileSystemEx() {
	*notFoundWarning = true;
	pureMode = false;
}

/*
================
FileSystemEx::Init

Adds all pakfiles in the base directory.
================
*/
bool FileSystemEx::Init( const char *_pakExtension, const char *_basePath, const char *_userPath, const char *_baseDir ) {
	CommonSetFileSystem( this );

	pakExtension	= _pakExtension;
	basePath		= _basePath;
	userPath		= _userPath;
	baseDir			= _baseDir;
	modPath			= baseDir;

	basePath.ToForwardSlashes();
	userPath.ToForwardSlashes();

	searchPaths.Clear();
	searchPaths.Append( basePath );
	if ( userPath.Icmp( basePath.c_str() ) != 0 )
		searchPaths.Append( userPath );

	// Start up with basedir by default
	AddResourceDir( baseDir.c_str(), PFLIST_BASE );

	bool ret = !pakFiles[PFLIST_BASE].IsEmpty();
	return ret;
}

/*
================
FileSystemEx::ConsumeEvents
================
*/
void FileSystemEx::ConsumeEvents( void ) {
	int index;
	FileEx *file;
	FileBuffered *fileBuffered;
	byte *buffer;
	FileEvent *evt = NULL;
	while( (evt = eventQueue.Consume()) != NULL ) {
		switch( evt->fileAction ) {
			case FileEvent::OPEN:
				file = static_cast<FileEx *>(evt->fileParam);
				openFiles.AddToEnd( file );
				file->node = openFiles.GetLastNode();
				break;
			case FileEvent::CLOSE:
				file = static_cast<FileEx *>(evt->fileParam);
				openFiles.Remove( file->node );
				fileBuffered = dynamic_cast<FileBuffered *>(file);
				if ( fileBuffered ) {
					buffer = fileBuffered->data;
					index = openFilesLoaded.Find( buffer );
					OG_ASSERT( index != -1 );
					if ( index != -1 ) {
						delete[] buffer;
						openFilesLoaded.Remove( index );
					}
				}
				delete file;
				break;
			case FileEvent::BUFFER_LOAD:
				openFilesLoaded.Append( static_cast<byte *>(evt->fileParam) );
				break;
			case FileEvent::BUFFER_FREE:
				buffer = static_cast<byte *>(evt->fileParam);
				index = openFilesLoaded.Find( buffer );
				if ( index != -1 ) {
					delete[] buffer;
					openFilesLoaded.Remove( index );
				}
				break;
		}
		delete evt;
	}
}

/*
================
FileSystemEx::Run
================
*/
void FileSystemEx::Run( void ) {
	// All this does is watch the open/close file events and on shutdown clear all files that where still open.
	wakeUpEvent.Lock();
	while( keepRunning ) {
		ConsumeEvents();
		wakeUpEvent.Wait();
	}
	wakeUpEvent.Unlock();

	// Consume remaining events
	ConsumeEvents();

	// Elvis has left the building, clear all evidence
	CloseAllFiles();

	// This tape will selfdestruct in 0 seconds
	CommonSetFileSystem( NULL );

	// Remove all resource directories
	resourceDirs.Clear();

	// Remove all pak files
	int max;
	for ( int i=0; i<PFLIST_NUM; i++ ) {
		max = pakFiles[i].Num();
		for( int j=0; j<max; j++ )
			PakFileEx::CloseZip( pakFiles[i][j] );
		pakFiles[i].Clear();
	}
}

/*
================
FileSystemEx::CloseAllFiles
================
*/
void FileSystemEx::CloseAllFiles( void ) {
	//! @todo	The user should be notified if he left files open
	LinkedList<FileEx *>::nodeType *node = openFiles.GetFirstNode();
	while( node != NULL ) {
		delete node->value;
		node = node->GetNext();
	}
	for( int i=openFilesLoaded.Num()-1; i >= 0; i-- )
		delete[] openFilesLoaded[i];
}

/*
================
FileSystemEx::SetPureMode
================
*/
void FileSystemEx::SetPureMode( bool enable ) {
	pureMode = enable;
}

/*
================
FileSystemEx::IsDir
================
*/
bool FileSystemEx::IsDir( const char *path ) {
#if OG_WIN32
	DynBuffer<wchar_t> strPath;
	StringToWide( path, strPath );
	struct _stat stat_Info;
	if ( _wstat ( strPath.data, &stat_Info ) == -1 )
		return false;
	return (stat_Info.st_mode & S_IFDIR) != 0;
#else
	struct stat stat_Info;
	if ( stat ( path, &stat_Info ) == -1 )
		return false;
	return (stat_Info.st_mode & S_IFDIR) != 0;
#endif
}

/*
================
FileSystemEx::MakeDir
================
*/
bool FileSystemEx::MakeDir( const char *path ) {
	if ( IsDir( path ) )
		return true;
#if OG_WIN32
	DynBuffer<wchar_t> strPath;
	StringToWide( path, strPath );
	return _wmkdir( strPath.data ) == 0;
#else
	return mkdir( path, 0777 ) == 0;
#endif
}

/*
================
FileSystemEx::MakePath
================
*/
bool FileSystemEx::MakePath( const char *path, bool pure ) {
	if ( pure ) {
		SharedLock lock(sharedMutex);
		return MakePath( Format("$*/$*/$*" ) << userPath << modPath << path, false );
	}
	int len = String::ByteLength(path);
	DynBuffer<char> newPath(len+1);

	for( int i=0; i<len; i++ ) {
		if ( path[i] == '/' || path[i] == '\\' ) {
			newPath.data[i] = '\0';
			// Ignore root
#if OG_WIN32
			if ( i > 2  || (i == 2 && newPath.data[1] != ':') ) {
#elif OG_LINUX
			if ( i != 0 ) {
#elif OG_MACOS_X
    #warning "Need MacOS here FIXME"
#endif
				if ( !MakeDir( newPath.data ) ) {
					User::Error( ERR_FS_MAKEPATH, "Can't create path", path );
					return false;
				}
			}
		}
		newPath.data[i] = path[i];
	}
	return true;
}

/*
================
FileSystemEx::AddPureExtension
================
*/
void FileSystemEx::AddPureExtension( const char *ext ) {
	sharedMutex.lock();
	pureExtensions.Append( ext[0] == '.' ? ext+1 : ext );
	sharedMutex.unlock();
}

/*
================
FileSystemEx::RemovePureExtension
================
*/
void FileSystemEx::RemovePureExtension( const char *ext ) {
	sharedMutex.lock();
	int index = pureExtensions.Find( ext[0] == '.' ? ext+1 : ext );
	if ( index != -1 )
		pureExtensions.Remove( index );
	sharedMutex.unlock();
}

/*
================
FileSystemEx::AddResourceDir

Adds the specified resource directory to the resourceDirs list
and its pakfiles to the pakfile list
================
*/
void FileSystemEx::AddResourceDir( const char *name, pfListId listId ) {
	if ( resourceDirs.IFind(name) != -1 ) {
		User::Warning( Format("Trying to add resource dir '$*' twice" ) << name );
		return;
	}
	StringList files;
	// Add to the list of resourceDirs
	resourceDirs.Append( name );
	int max = searchPaths.Num();
	int max2;
	for( int i=0; i<max; i++ ) {
		// Do a simple search
		files.Clear();
		if ( !LocalFileSearch( searchPaths[i].c_str(), name, pakExtension.c_str(), &files, LF_FILES ) ) {
			User::Warning( Format("Searching resource dir '$*/$*' failed" ) << searchPaths[i] << name );
			continue;
		}

		if ( !files.IsEmpty() ) {
			// Add all files in alphabetic order
			files.Sort( StringListICmp, false );
			PakFileEx *pakFile;
			max2 = files.Num();
			for( int j=0; j<max2; j++ ) {
				// Load the pakfile
				pakFile = PakFileEx::OpenZip( files[j].c_str() );
				if ( !pakFile )
					continue;

				// Ask the user if this one is ok to add, otherwise close it
				if ( User::IsPakFileAllowed( pakFile ) )
					pakFiles[listId].Append(pakFile);
				else
					PakFileEx::CloseZip( pakFile );
			}
		}
	}
}

/*
================
FileSystemEx::ChangeMod

Frees up all mod directories
Calls AddResourceDir() if the new moddir is not an IsEmpty string
================
*/
bool FileSystemEx::ChangeMod( const char *dir ) {
	ogst::unique_lock<SharedMutex> lock(sharedMutex);
	if ( dir[0] == '\0' )
		modPath = baseDir;
	else {
		bool found = false;
		// Check if the mod exists
		ModList *mods = GetModList();
		if ( mods ) {
			do {
				// Found it ?
				if ( String::Icmp( mods->GetPath(), dir ) == 0 ) {
					modPath = mods->GetPath();
					found = true;
					break;
				}
			} while ( mods->GetNext() );

			FreeModList( mods );
			mods = NULL;
		}
		if ( !found )
			return false;
	}

	// Close all opened files
	CloseAllFiles();

	// Remove all resource directories, except the default(base) one
	resourceDirs.Clear();
	resourceDirs.Append( baseDir );

	// Remove all mod pak files
	int max = pakFiles[PFLIST_MOD].Num();
	for( int i=0; i<max; i++ )
		PakFileEx::CloseZip( pakFiles[PFLIST_MOD][i] );
	pakFiles[PFLIST_MOD].Clear();

	// If we have a mod, add its resources.
	if ( dir[0] != '\0' )
		AddResourceDir( modPath.c_str(), PFLIST_MOD );
	return true;
}

/*
===========
FileSystemEx::OpenLocalFileRead
===========
*/
FileEx *FileSystemEx::OpenLocalFileRead( const char *filename, int *size ) {
	// Try to open it.
	FILE *file = fopen( filename, "rb" );
	if ( file ) {
		FileLocal *fileEx = FileLocal::Create( file );
		if ( fileEx == NULL )
			return NULL;

		// Move to the end of the file to get the filesize
		if ( fseek( file, 0, SEEK_END ) == 0 ) {
			// Get filesize and jump to start again
			fileEx->size = ftell( file );
			if ( fileEx->size != -1 && fseek( file, 0, SEEK_SET ) == 0 ) {
				// Get modification date/time
				fileEx->time = FileTime( filename, false );
				fileEx->writeMode = false;
				fileEx->fullpath = filename;
				fileEx->fullpath.ToForwardSlashes();
				int i = fileEx->fullpath.ReverseFind("/");
				fileEx->filename = fileEx->fullpath.c_str() + ((i == -1) ? 0 : i);

				if ( size != NULL )
					*size = fileEx->size;
				
				AddFileEvent( new FileEvent( FileEvent::OPEN, fileEx ) );
				return fileEx;
			}
		}
		// Something along the way failed. No need for fclose(file) here,
		// since the FileLocal Destructor already does that.
		delete fileEx;
	}
	return NULL;
}

/*
===========
FileSystemEx::OpenRead

Finds a file in the pakfile list or if pure is false, on the disk
returns NULL if the file has not been found
otherwise a new File Object and the filesize
===========
*/
File *FileSystemEx::OpenRead( const char *filename, bool pure, bool buffered ) {
	if ( buffered ) {
		byte *buffer = NULL;
		int filesize = LoadFile( filename, &buffer, pure );
		if ( buffer == NULL )
			return NULL;
		
		FileBuffered *fileEx = FileBuffered::Create( buffer );
		fileEx->writeMode = false;
		fileEx->size = filesize;
		fileEx->time = FileTime(filename);
		fileEx->fullpath = filename;
		fileEx->fullpath.ToForwardSlashes();
		int i = fileEx->fullpath.ReverseFind("/");
		fileEx->filename = fileEx->fullpath.c_str() + ((i == -1) ? 0 : i);

		AddFileEvent( new FileEvent( FileEvent::OPEN, fileEx ) );
		return fileEx;
	}

	if ( !pure ) {
		// Try to open it directly
		FileEx *fileEx = OpenLocalFileRead( filename );
		if ( fileEx )
			return fileEx;
	} else {
		SharedLock lock(sharedMutex);

		// Can the extension be loaded in pure mode ?
		bool unpureFileAllowed = false;
		if ( pureMode && !pureExtensions.IsEmpty() ) {
			String strExt = String::GetFileExtension( filename, String::ByteLength(filename) );
			if ( pureExtensions.Find( strExt.c_str() ) != -1 )
				unpureFileAllowed = true;
		}

		// Check for local files.
		if ( !pureMode || unpureFileAllowed ) {
			Format path( "$*/$*/$*" );
			for( int i=searchPaths.Num()-1; i >= 0; i-- ) {
				// check in reverse order to get mod dir before base dir
				for( int j=resourceDirs.Num()-1; j >= 0; j-- ) {
					// Try to open it.
					FileEx *fileEx = OpenLocalFileRead( path << searchPaths[i] << resourceDirs[j] << filename );
					if ( fileEx )
						return fileEx;
					path.Reset();
				}
			}
		}

		// Search all pak files (in reverse order so
		// mod pakfiles come before base pakfiles)
		FileEx *fileEx;
		for ( int i=PFLIST_NUM-1; i >= 0; i-- ) {
			for( int j=pakFiles[i].Num()-1; j >= 0; j-- ) {
				// Search for the file in the pakfile.
				fileEx = static_cast<FileEx *>( pakFiles[i][j]->OpenFile( filename ) );
				// Found it!
				if ( fileEx != NULL )
					return fileEx;
			}
		}
	}

	if ( *notFoundWarning )
		User::Error( ERR_FS_FILE_OPENREAD, "Can't open file for reading", filename );
	return NULL;
}

/*
===========
FileSystemEx::OpenWrite
===========
*/
File *FileSystemEx::OpenWrite( const char *filename, bool pure ) {
	if ( pure ) {
		SharedLock lock(sharedMutex);
		return OpenWrite( Format( "$*/$*/$*" ) << userPath << modPath << filename, false );
	}

	// If the path doesn't exist and can not be created, fail.
	if ( !MakePath( filename, false ) )
		return NULL;

	// Try to open it.
	FILE *file = fopen( filename, "wb" );
	if ( !file ) {
		User::Error( ERR_FS_FILE_OPENWRITE, "Can't open file for writing", filename );
		return NULL;
	}

	FileLocal *fileEx = FileLocal::Create( file );
	if ( fileEx == NULL )
		return NULL;
	fileEx->writeMode = true;
	fileEx->size = 0;
	fileEx->time = time(NULL);
	fileEx->fullpath = filename;
	fileEx->fullpath.ToForwardSlashes();
	int i = fileEx->fullpath.ReverseFind("/");
	fileEx->filename = fileEx->fullpath.c_str() + ((i == -1) ? 0 : i);

	static_cast<FileSystemEx *>(FS)->AddFileEvent( new FileEvent( FileEvent::OPEN, fileEx ) );

	// Return the filehandle
	return fileEx;
}

/*
===========
FileSystemEx::FileSize

Gets Filesize of a file (inside or outside a pakfile)
===========
*/
int FileSystemEx::FileSize( const char *filename, bool pure ) {
	// Open the file to get its size and then close it again.
	File *file = OpenRead( filename, pure );
	if ( file ) {
		int size = file->Size();
		file->Close();
		return size;
	}
	return -1;
}

/*
===========
FileSystemEx::FileExists

Does the specified file exist ?
===========
*/
bool FileSystemEx::FileExists( const char *filename, bool pure ) {
	*notFoundWarning = false;
	File *file = OpenRead( filename, pure );
	if ( file )
		file->Close();
	*notFoundWarning = true;
	return file != NULL;
}

/*
===========
FileSystemEx::FileTime

Returns the file modification date/time
===========
*/
time_t FileSystemEx::FileTime( const char *filename, bool pure ) {
	if ( pure ) {
		*notFoundWarning = false;
		File *file = OpenRead( filename, pure );
		if ( !file )
			return 0;
		time_t time = file->GetTime();
		file->Close();
		*notFoundWarning = true;
		return time;
	}
#if OG_WIN32
	DynBuffer<wchar_t> strFilename;
	StringToWide( filename, strFilename );
	struct _stat fileStat;
	if ( _wstat ( strFilename.data, &fileStat ) == -1 )
		return false;
	return (fileStat.st_mode & S_IFDIR) != 0;
#else
	struct stat fileStat;
	if ( stat(filename, &fileStat) == -1 )
		return 0;
	return fileStat.st_mtime;
#endif
}

/*
============
FileSystemEx::LoadFile

Will load the whole file into a buffer
(use FreeFile to free the buffer again)
============
*/
int FileSystemEx::LoadFile( const char *path, byte **buffer, bool pure ) {
	OG_ASSERT( buffer != NULL );

	// Open the file
	File *file = OpenRead( path, pure );
	if ( !file ) {
		*buffer = NULL;
		return -1;
	}

	// Allocate enough memory for the whole file.
	int size = file->Size();
	*buffer = new byte[size+1];

	try {
		// Read the whole file into the buffer
		file->Read( *buffer, size );

		// Just in case we are reading a text file, terminate the buffer
		(*buffer)[size] = 0;

		AddFileEvent( new FileEvent( FileEvent::BUFFER_LOAD, *buffer ) );
		file->Close();
		return size;
	}
	catch( FileReadWriteError &err ) {
		delete[] *buffer;
		*buffer = NULL;
		file->Close();
		User::Error( ERR_FILE_CORRUPT, Format( "Unknown: $*" ) << err.ToString(), path );
		return -1;
	}
}

/*
============
FileSystemEx::FreeFile

Free a file that has been loaded with LoadFile()
============
*/
void FileSystemEx::FreeFile( byte *buffer ) {
	AddFileEvent( new FileEvent( FileEvent::BUFFER_FREE, buffer ) );
}

/*
================
FileSystemEx::GetFileList

Finds all files with the specified extension in the specified directory
See the LS_* flags in FileSystem.h for options
================
*/
FileList *FileSystemEx::GetFileList( const char *dir, const char *extension, int flags ) {
	sharedMutex.lock_shared();
	FileListEx *fileList = new FileListEx;
	if ( flags & LF_CHECK_LOCAL ) {
		// Can the extension be loaded in pure mode ?
		bool unpureFileAllowed = (flags & LF_CHECK_UNPURE) != 0;
		if ( !unpureFileAllowed && pureMode && !pureExtensions.IsEmpty() ) {
			if ( pureExtensions.Find( extension ) != -1 )
				unpureFileAllowed = true;
		}

		// Check for local files.
		if ( !pureMode || unpureFileAllowed ) {
			int max = searchPaths.Num();
			Format path( "$*/$*" );
			for( int i=0; i<max; i++ ) {
				for( int j=resourceDirs.Num()-1; j >= 0; j-- ) {
					LocalFileSearch( path << searchPaths[i] << resourceDirs[j], dir, extension, &fileList->files, flags );
					path.Reset();
				}
			}
		}
	}
	// Check for archived files.
	if ( flags & LF_CHECK_ARCHIVED )
		GetArchivedFileList( dir, extension, fileList->files, flags );

	sharedMutex.unlock_shared();

	// No files found, return
	if ( fileList->files.IsEmpty() ) {
		delete fileList;
		return NULL;
	}

	// If the user does not want the dir he provided inside the filenames, remove it.
	if ( *dir && (flags & LF_REMOVE_DIR) ) {
		int len = String::Length(dir);
		for ( int i=fileList->files.Num()-1; i >= 0; i-- )
			fileList->files[i] = fileList->files[i].Right( len );
	}

	// Remove double entries
	fileList->files.Sort( StringListICmp, true );
	return fileList;
}

/*
================
FileSystemEx::FreeFileList

Free a filelist created by GetFileList
================
*/
void FileSystemEx::FreeFileList( FileList *list ) {
	OG_ASSERT( list );
	delete static_cast<FileListEx *>(list);
}

/*
================
FileSystemEx::GetArchivedFileList

Finds all files with the specified extension in the specified directory
Searches for archived files only.
================
*/
int FileSystemEx::GetArchivedFileList( const char *dir, const char *extension, StringList &files, int flags ) {
	// Get number of existing entries.
	int oldsize = files.Num();
	int dirLength = String::Length( dir );
	int extLength = String::Length( extension );

	int lastslash;
	int length;
	PakFileEx *pakFile;

	String dirWithSlash = dir;
	dirWithSlash.ToForwardSlashes();
	dirWithSlash += "/";

	int max, max2;
	// Check all pak files
	for ( int i=0; i<PFLIST_NUM; i++ ) {
		max = pakFiles[i].Num();
		for ( int j=0; j<max; j++ ) {
			// Get the first file in the pakfile.
			pakFile = pakFiles[i][j];

			const CentralDir &cd = *pakFile->GetCentralDir();
			max2 = cd.Num();
			for( int k=0; k<max2; k++ ) {
				// Check for directory and file filter flags
				if ( cd[k].isDir ) {
					if ( !(flags & LF_DIRS) )
						continue;
				}
				else if ( !(flags & LF_FILES) )
					continue;

				const String &filename = cd.GetKey(k);
				length = filename.Length();

				// Find last /
				lastslash = filename.ReverseFind("/");

				// If searching in root
				if ( dirLength == 0 ) {
					// And we found a slash, and we are not diving into subdirs
					// then the file doesn't match
					if ( lastslash != -1 && !(flags & LF_CHECK_SUBDIRS) )
						continue;
				}
				else {
					// The File is in the root
					if ( lastslash == -1 )
						continue;

					if ( flags & LF_CHECK_SUBDIRS ) {
						// If the directory doesn't match, we don't want it.
						if ( filename.Icmpn( dirWithSlash.c_str(), dirLength+1 ) != 0 )
							continue;
					}
					else {
						// If the directory doesn't match, we don't want it.
						if ( lastslash != dirLength )
							continue;
						if ( filename.Icmpn( dirWithSlash.c_str(), dirLength+1 ) != 0 )
							continue;
					}
				}

				// If there is not a min. of 1 char between the path and the extension
				int pos = length - extLength;
				if ( pos <= lastslash + 1 )
					continue;

				// If the extension doesn't match
				if ( !filename.CheckFileExtension( extension ) )
					continue;

				files.Append( filename );
			}
		}
	}

	// Return the number of files added.
	return files.Num() - oldsize;
}

/*
================
FileSystemEx::GetModList
================
*/
ModList *FileSystemEx::GetModList( void ) {
	int max = searchPaths.Num();
	Format path( "$*/mods" );
	StringList modDirs;
	ModListEx *mods = new ModListEx;
	for( int i=0; i<max; i++ ) {
		LocalFileSearch( path << searchPaths[i], "", "", &mods->paths, LF_DIRS );
		path.Reset();
	}
	mods->paths.Sort( StringListICmp, true );

	max = mods->paths.Num();
	for( int i=0; i<max; i++ ) {
		if ( modPath.Icmp( mods->paths[i].c_str() ) == 0 )
			mods->currentModIndex = i;
	}

	return mods;
}

/*
================
FileSystemEx::FreeModList
================
*/
void FileSystemEx::FreeModList( ModList *list ) {
	OG_ASSERT( list );
	delete static_cast<ModListEx *>(list);
}

}