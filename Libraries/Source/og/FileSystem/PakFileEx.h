/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Loading zip files
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

#ifndef __OG_PAKFILE_H__
#define __OG_PAKFILE_H__

namespace og {
	// The error codes
	enum {
		UNZ_OK			= 0,
		UNZ_ERRNO		= -1,
		UNZ_EOF			= -100,
		UNZ_BADZIPFILE	= -101,
		UNZ_CRCERROR	= -102
	};

	/*
	==============================================================================

	  CentralDir

	==============================================================================
	*/
	struct CentralDirEntry {
		bool		isDir;								// Is a directory
		short		compressionMethod;					// Compression method
		uLong		crc32Value;							// Crc-32
		uLong		compressedSize;						// Compressed size
		uLong		unCompressedSize;					// Uncompressed size
		uLong		posInZipfile;						// position in zipfile
		time_t		time;								// Modification date
	};
	typedef DictEx<CentralDirEntry> CentralDir;

	/*
	==============================================================================

	  PakFileEx

	==============================================================================
	*/
	struct FileHeader;
	class PakFileEx : public PakFile {
	public:
		// ---------------------- Public PakFile Interface -------------------

		const char *GetFilename( void ) { return pakFileName.c_str(); }
		bool		FileExists( const char *filename ) { return centralDir.Find(filename) != -1; }
		File *		OpenFile( const char *filename );

		// ---------------------- Internal PakFileEx Members -------------------

		const CentralDir *GetCentralDir( void ) { return &centralDir; }
		static PakFileEx *	OpenZip( const char *path );				// Open a new ZipFile
		static void			CloseZip( PakFileEx *pakFile );				// Close the ZipFile

	private:
		String		pakFileName;
		CentralDir	centralDir;											// One entry for each file in the pak

		int			CompareFileHeader( FILE *file, uLong zipfileOffset, FileHeader *pFH, uLong *pPosInZip ); // Compare local file header with the CD entry
		int			ReadCentralDir( FILE *file, uLong zipfileOffset, uLong Offset, int TotalEntries );	// Read All Central Dir Entries
		uLong		FindCentralDir( FILE *file );						// Find Central Dir signature
	};
}

#endif
