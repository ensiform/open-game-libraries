/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Reading Data from local and archived files
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

#ifndef __OG_FILE_EX_H__
#define __OG_FILE_EX_H__

namespace og {
	struct CentralDirEntry;
	class PakFileEx;
	const uLong UNZ_BUFSIZE = 16384;

	/*
	==============================================================================

	  FileEx
	  - add comments for all functions

	==============================================================================
	*/
	class FileEx : public File {
	public:
		// ---------------------- Public File Interface -------------------

		long			Size( void ) { return size; }						// Returns the filesize

		const char *	GetFileName( void ) { return filename; }			// Returns the filename without path
		const char *	GetFullPath( void ) { return fullpath.c_str(); }	// Returns the full filepath
		time_t			GetTime( void ) { return time; }					// Returns the modification date/time
		virtual void	Close( void );										// Close the file

		// ---------------------- Internal FileEx Members -------------------

	public:
		FileEx() { filename = fullpath.c_str(); }
		virtual ~FileEx() {}

	protected:
		LinkedList<FileEx *>::nodeType *node;
		bool		writeMode;		// Reading or Writing ?

		time_t		time;			// file modification date/time
		long		size;			// file size
		const char *filename;		// filename only
		String		fullpath;		// filename including path

		friend class FileTrackEvent;
		friend class PakFileEx;
		friend class FileEventThread;
	};

	/*
	==============================================================================

	  FileLocal

	==============================================================================
	*/
	class FileLocal : public FileEx {
	public:
		// ---------------------- Public File Interface -------------------

		void	Seek( long offset, int origin );			// Jump to a position
		long	Tell( void ) { return ftell( file ); }		// Returns read/write position
		void	Rewind( void ) { rewind( file ); }			// Rewind to the beginning
		void	Flush( void );								// Buffered write data will be written to the file.
		bool	Eof( void ) { return ftell( file )>=size; }	// Tests for end-of-file

		void	Read( void *buffer, uInt len );				// Read data
		void	Write( const void *buffer, uInt len );		// Write data

		// ---------------------- Internal FileLocal Members -------------------

	public:
		~FileLocal() { fclose( file ); }

		static FileLocal *Create( FILE *f );

	protected:
		friend class FileSystemEx;
		FILE*	file;
	};

	/*
	==============================================================================

	  FileBuffered

	==============================================================================
	*/
	class FileBuffered : public FileEx {
	public:
		// ---------------------- Public File Interface -------------------

		void	Seek( long offset, int origin );			// Jump to a position
		long	Tell( void ) { return position; }			// Returns read/write position
		void	Rewind( void ) { position = 0; }			// Rewind to the beginning
		void	Flush( void ) { OG_DEBUG_BREAK(); }			// Buffered write data will be written to the file.
		bool	Eof( void ) { return position >= size; }	// Tests for end-of-file

		void	Read( void *buffer, uInt len );				// Read data
		void	Write( const void *buffer, uInt len ) { OG_DEBUG_BREAK(); }	// Write data

		void	Close( void );								// Close the file

		// ---------------------- Internal FileBuffered Members -------------------

	public:
		static FileBuffered *Create( byte *buffer );

	protected:
		friend class FileSystemEx;
		uLong	position;
		byte*	data;
	};

	/*
	==============================================================================

	  FileInPak

	==============================================================================
	*/
	class FileInPak : public FileEx {
	public:
		// ---------------------- Public File Interface -------------------

		void	Seek( long offset, int origin );					// Jump to a position
		long	Tell( void ) { return static_cast<long>(stream.total_out); }	// Returns read/write position
		void	Rewind( void );										// Rewind to the beginning
		void	Flush( void ) { OG_DEBUG_BREAK(); }					// Buffered write data will be written to the file.
		bool	Eof( void ) { return atEOF; }						// Tests for end-of-file

		void	Read( void *buffer, uInt len );						// Read data
		void	Write( const void *buffer, uInt len ) { OG_DEBUG_BREAK(); }	// Write data

		// ---------------------- Internal FileInPak Members -------------------

	public:
		FileInPak();
		~FileInPak();

		static FileInPak *Create( PakFileEx *pakFile, CentralDirEntry *cde );

	protected:
		friend class FileSystemEx;
		CentralDirEntry *cde;
		PakFileEx *pakFile;					// Pointer to the parent PakFileEx
		FILE*	file;						// Filepointer to the zipfile this file is stored in.
		int		posInCD;					// Position of the file in the central dir

		char	readBuffer[UNZ_BUFSIZE];	// Buffer for compressed data
		z_stream stream;					// zLib stream structure for inflate

		uLong	posInZipfile;				// Position in byte on the zipfile, for fseek
		bool	compressedData;				// Flag set if streaming data out of a compressed zipfile

		uLong	crc32Value;					// Crc32 of all data uncompressed
		uLong	crc32Wait;					// Crc32 we must obtain after decompress all
		uLong	remainingArchivedSize;		// Number of raw bytes remaining to be read from the archive
		uLong	remainingFinalSize;			// Number of bytes to be obtained after extraction
		bool	atEOF;
	};
}

#endif
