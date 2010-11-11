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

#include "FileSystemEx.h"

#if OG_WIN32
	#include <windows.h>
#elif OG_LINUX || OG_MACOS_X
	#include <iconv.h>
	#include <limits.h>
#endif

// Check for correct short and long sizes
#if USHRT_MAX != 0xffff || ULONG_MAX != 0xffffffffUL
	#error "short must be 2 bytes and long must be 4 bytes to work correctly.."
#endif

namespace og {

// Structure sizes
const int MAX_COMMENT			= 0xffff;
const int SIZE_CENTRALDIREND	= 0x16;
const int SIZE_CENTRALDIRITEM	= 0x2e;
const int SIZE_ZIPLOCALHEADER	= 0x1e;

// Maximum length of a filename stored in a zipfile
const int UNZ_MAXFILENAMEINZIP	= 256;

// Signatures
const uLong SIGNATURE_LOC_FH	= 0x04034b50;
const uLong SIGNATURE_CD_FH		= 0x02014b50;
const uLong SIGNATURE_CD_END	= 0x06054b50;

// Flags
const short FH_FLAG_BIT3		= BIT(3);

const byte	MASKBITS	= 0x3F;
const byte	MASKBYTE	= 0x80;
const byte	MASK2BYTES	= 0xC0;

/*
================
LocalFileHeader
================
*/
#pragma pack(1)
struct LocalFileHeader {
	uLong	signature;			// Local file header signature (SIGNATURE_LOC_FH)

	short	versionNeeded;		// Version needed to extract
	short	flag;				// General purpose bit flag
	short	compressionMethod;	// Compression method
	short	dosTime;			// Last mod file time
	short	dosDate;			// Last mod file date

	uLong	crc32Value;			// Crc-32
	uLong	compressedSize;		// Compressed size
	uLong	unCompressedSize;	// Uncompressed size

	short	filenameLength;		// filename length
	short	extraFieldLength;	// Extra field length
};

/*
================
FileHeader
================
*/
struct FileHeader {
	uLong	signature;			// Central file header signature (SIGNATURE_CD_FH)

	short	version;			// Version made by
	short	versionNeeded;		// Version needed to extract
	short	flag;				// General purpose bit flag
	short	compressionMethod;	// Compression method
	short	dosTime;			// Last mod file time
	short	dosDate;			// Last mod file date

	uLong	crc32Value;			// Crc-32
	uLong	compressedSize;		// Compressed size
	uLong	unCompressedSize;	// Uncompressed size

	short	filenameLength;		// filename length
	short	extraFieldLength;	// Extra field length
	short	fileCommentLength;	// File comment length

	short	diskNumStart;		// Disk number start
	short	internalFa;			// Internal file attributes
	uLong	externalFa;			// External file attributes
	uLong	localHeaderOffset;	// Relative offset of local header
};

/*
================
CentralDirEnd
================
*/
struct CentralDirEnd {
	uLong	signature;			// End of central dir signature (SIGNATURE_CD_END)

	short	numDisk;			// Number of this disk
	short	numDiskWithCD;		// Number of the disk with the start of the central dir
	/*
	The disk variables are used for spanning (splitting on multiple diskette media)
	We don't support it, so both must be 0
	*/
	short	numEntries;			// Number entries in the central dir on this disk
	short	numEntriesCD;		// Number entries in the central dir (same as above on nospan)

	uLong	centralDirSize;		// Size of the central directory
	uLong	centralDirOffset;	// Offset of start of central directory
								// with respect to the starting disk number

	short	commentLength;		// Comment length
};

#pragma pack()

/*
================
ConvertDosTime

Convert Dos Time & Date to time_t
================
*/
time_t ConvertDosTime( short DosTime, short DosDate ) {
	time_t clock = time(NULL);
	struct tm *t = localtime(&clock);
	t->tm_isdst = -1;
	t->tm_sec  = (DosTime <<  1) & 0x3e;
	t->tm_min  = (DosTime >>  5) & 0x3f;
	t->tm_hour = (DosTime >> 11) & 0x1f;
	t->tm_mday = DosDate & 0x1f;
	t->tm_mon  = (DosDate >>  5) & 0x0f;
	t->tm_year = ((DosDate >>  9) & 0x7f) + 1980;

	return mktime(t);
}

/*
==============================================================================

  Ocp2Utf8 ( OEM Codepage 437 to UTF-8 Conversion )

==============================================================================
*/
class Ocp2Utf8 {
public:
	Ocp2Utf8() {}
	~Ocp2Utf8() {
#if OG_LINUX || OG_MACOS_X
		if ( cd != (iconv_t)-1 )
			iconv_close(cd);
#endif
	}

	bool Init( void ) {
#if OG_WIN32
		return true;
#elif OG_LINUX || OG_MACOS_X
		return cd != (iconv_t)-1;
#endif
	}

	const char *Convert( char *input, int numBytes ) {
#if OG_WIN32
		// Convert from OEM codepage to unicode first
		int length = MultiByteToWideChar( 437, 0, input, numBytes, NULL, 0 );
		wideBuffer.CheckSize( length + 1 );
		wideBuffer.data[0] = L'\0';
		MultiByteToWideChar( 437, 0,  input, numBytes, wideBuffer.data, length+1 );

		// Convert unicode to UTF-8
		utf8Buffer.FromWide( wideBuffer.data );
		return utf8Buffer.c_str();
#elif OG_LINUX || OG_MACOS_X
		size_t inBytes = numBytes;
		utf8Buffer.CheckSize( inBytes*6 );
		size_t outBytes = inBytes*6;

		char *inchar =  input ;
		char *outchar = utf8Buffer.data ;
		if ( iconv(cd, &inchar, &inBytes, &outchar, &outBytes) == -1 )
			return NULL;
		utf8Buffer.data[numBytes-1] = '\0'; //fixme: wrong, numBytes is for input, not output
		return utf8Buffer.data;
#endif
}

private:
#if OG_WIN32
        DynBuffer<wchar_t>	wideBuffer;
        String				utf8Buffer;
#elif OG_LINUX || OG_MACOS_X
        iconv_t				cd;
        DynBuffer<char>		utf8Buffer;
#endif
};

/*
==============================================================================

  PakFileEx

==============================================================================
*/
/*
================
PakFileEx::OpenFile

Open the specified file in the zipfile for reading data.
returns NULL if the file does not exist, otherwise it
returns a new File Object.
================
*/
File *PakFileEx::OpenFile( const char *filename ) {
	if ( FS == NULL )
		return NULL;

	int index = centralDir.Find( filename );
	if ( index != -1 ) {
		// Create a new object
		FileInPak *fileEx = FileInPak::Create( this, &centralDir[index] );
		if ( fileEx == NULL )
			return NULL;
		fileEx->writeMode = false;
		fileEx->size = centralDir[index].unCompressedSize;
		fileEx->fullpath = centralDir.GetKey( index );
		int i = fileEx->fullpath.ReverseFind("/");
		fileEx->filename = fileEx->fullpath.c_str() + ((i == -1) ? 0 : i+1);

		static_cast<FileSystemEx *>(FS)->AddFileEvent( new FileTrackEvent( fileEx, true ) );
		return fileEx;
	}
	return NULL;
}

/*
================
PakFileEx::CompareFileHeader

Read the local file header and compare it
with the filehader of the central dir
================
*/
int PakFileEx::CompareFileHeader( FILE *file, uLong zipfileOffset, FileHeader *pFH, uLong *pPosInZip ) {
	// Jump to the local fileheader
	if ( fseek( file, zipfileOffset + pFH->localHeaderOffset, SEEK_SET ) != 0 )
		return UNZ_ERRNO;

	// Read in the local file header
	LocalFileHeader localFH;

	if ( fread( &localFH, sizeof(localFH), 1, file ) != 1 )
		return UNZ_ERRNO;

	// Check the signature
	if ( localFH.signature != SIGNATURE_LOC_FH )
		return UNZ_BADZIPFILE;

	// Compare
	if ( localFH.compressionMethod != pFH->compressionMethod ||
		localFH.filenameLength != pFH->filenameLength )
		return UNZ_BADZIPFILE;

	// If bit 3 is set, the crc-32, compressed and uncompressed size
	// are set to zero in the local header, so no need to compare.
	if ( !(localFH.flag & FH_FLAG_BIT3) ) {
		if ( localFH.crc32Value != pFH->crc32Value ||
			localFH.compressedSize != pFH->compressedSize ||
			localFH.unCompressedSize != pFH->unCompressedSize )
			return UNZ_BADZIPFILE;
	}

	// Calculate the position of the file in the zipfile
	*pPosInZip = zipfileOffset + pFH->localHeaderOffset + SIZE_ZIPLOCALHEADER +
		localFH.filenameLength + localFH.extraFieldLength;

	return UNZ_OK;
}


/*
================
PakFileEx::ReadCentralDir

Read all entries in the Central Dir
================
*/
int PakFileEx::ReadCentralDir( FILE *file, uLong zipfileOffset, uLong Offset, int TotalEntries ) {
	if ( TotalEntries <= 0 )
		return UNZ_ERRNO;

	// Get the first file in the pakfile.
	uLong posInCentralDir = Offset;

	DynBuffer<char> filenameBuf;
	const char *pszFilename;
	bool isDir;			// Is a directory
	uLong PosInZip;		// Position of the current file in the zipfile

	FileHeader fh;		// Storrage for the current fileheader

	Ocp2Utf8 converter;
	if ( !converter.Init() )
		return UNZ_ERRNO;

	// Read all entries
	for ( int i=0; i<TotalEntries; i++ ) {
		// Jump to the current file header in the central dir.
		if ( fseek( file, posInCentralDir + zipfileOffset, SEEK_SET ) != 0 )
			return UNZ_ERRNO;

		// Read the current file header
		if ( fread( &fh, sizeof(fh), 1, file ) != 1 )
			return UNZ_ERRNO;

		// Check the signature
		if ( fh.signature != SIGNATURE_CD_FH )
			return UNZ_BADZIPFILE;

		// We do only support no compression or deflated compression at the moment.
		if ( fh.compressionMethod != 0 && fh.compressionMethod != Z_DEFLATED )
			return UNZ_BADZIPFILE;

		// Files without name don't exist.
		if ( fh.filenameLength <= 0 )
			return UNZ_BADZIPFILE;

		filenameBuf.CheckSize( fh.filenameLength+1 );

		// Read the filename
		if ( fread( filenameBuf.data, fh.filenameLength, 1, file ) != 1 )
			return UNZ_ERRNO;
		filenameBuf.data[fh.filenameLength] = '\0';

		// Check if it's a directory and remove the last slash if it is
		isDir = filenameBuf.data[fh.filenameLength-1] == '/';
		if ( isDir )
			filenameBuf.data[fh.filenameLength-1] = '\0';

		// Is the file UTF-8 encoded?
		if ( fh.flag & BIT(11) )
			pszFilename = filenameBuf.data;
		else {
			// Convert from CP437 to UTF-8
			pszFilename = converter.Convert( filenameBuf.data, fh.filenameLength+1 );
			if ( pszFilename == NULL )
				return UNZ_ERRNO;
		}

		// Compare file headers
		if ( CompareFileHeader( file, zipfileOffset, &fh, &PosInZip ) != UNZ_OK )
			return UNZ_BADZIPFILE;

		// Add it to the list
		CentralDirEntry &cde	= centralDir[pszFilename];
		cde.isDir = isDir;
		cde.compressionMethod	= fh.compressionMethod;
		cde.unCompressedSize	= fh.unCompressedSize;
		cde.compressedSize		= fh.compressedSize;
		cde.posInZipfile		= PosInZip;
		cde.crc32Value			= fh.crc32Value;
		cde.time				= ConvertDosTime( fh.dosTime, fh.dosDate );

		// Increase position in central dir.
		posInCentralDir += SIZE_CENTRALDIRITEM + fh.filenameLength +
				fh.extraFieldLength + fh.fileCommentLength;
	}
	return UNZ_OK;
}

/*
================
PakFileEx::FindCentralDir

Locate the Central directory signature
(at the end of the zipfile, just before the global comment)
================
*/
uLong PakFileEx::FindCentralDir( FILE *file ) {
	// Get filesize
	if ( fseek( file, 0, SEEK_END ) != 0 )
		return 0;

	uLong fileSize = ftell( file );
	if ( fileSize == -1 )
		return 0;

	// Don't try to read more than we can get;
	uLong maxRead = Min( static_cast<uLong>(SIZE_CENTRALDIREND + MAX_COMMENT),fileSize );

	// Jump to read start position
	uLong startSearch =fileSize-maxRead;
	if ( fseek( file, startSearch, SEEK_SET ) != 0 )
		return 0;

	// Read into the buffer
	DynBuffer<byte> buffer( maxRead );
	if ( fread( buffer.data, maxRead, 1, file ) != 1 )
		return 0;

	// Do a backwards search
	for ( uLong i=maxRead-SIZE_CENTRALDIREND; i >=0 ; i-- ) {
		// Look for the 'end of central dir' signature (SIGNATURE_CD_END)
		if ( buffer.data[i] == 0x50 && buffer.data[i+1] == 0x4b && buffer.data[i+2] == 0x05 && buffer.data[i+3] == 0x06 )
			return startSearch + i;
	}
	return 0;
}

/*
================
PakFileEx::OpenZip

Open a Zip file. Path needs to be relative to the main executable.
If the zipfile cannot be opened (file don't exist or is not valid),
the return value is NULL.
Else, the return value is a new PakFileEx Object.
You can use it to search through the zipfile or extract data.
================
*/
PakFileEx *PakFileEx::OpenZip( const char *path ) {
	// Create a new zipfile object
	PakFileEx *pakFile = new PakFileEx;

	// Try to open the file
	pakFile->pakFileName = path;
	pakFile->pakFileName.ToForwardSlashes();
	FILE *file = fopen(path, "rb");

	if ( file == NULL ) {
		// Couldn't be opened, so delete the object again
		delete pakFile;
		User::Error( ERR_FS_FILE_OPENREAD, "Can't open file for reading", path );
		return NULL;
	}

	// The structure to read in the Central directory end..
	CentralDirEnd cde;

	// Get the position of the beginning of the central dir
	uLong central_pos = pakFile->FindCentralDir( file );

	// Check if data is valid
	bool failed = true;
	if ( central_pos != 0 &&
		// Jump to central_pos
		fseek( file, central_pos, SEEK_SET ) == 0 &&

		// Load the central dir end
		fread( &cde, sizeof(cde), 1, file ) == 1 &&

		// Do not add empty or splitted files.
		cde.numEntries > 0 && cde.numEntriesCD == cde.numEntries && cde.numDiskWithCD == 0 && cde.numDisk == 0 &&

		// If central_pos is wrong
		central_pos >= (cde.centralDirOffset + cde.centralDirSize) )
	{
		// Set the byte before zip
		uLong zipfileOffset = central_pos - (cde.centralDirOffset + cde.centralDirSize);
		// Read the central dir
		failed = ( pakFile->ReadCentralDir( file, zipfileOffset, cde.centralDirOffset, cde.numEntries ) != UNZ_OK );
	}

	fclose( file );

	if ( failed ) {
		// Some error happened, so close the zipfile and return NULL
		PakFileEx::CloseZip( pakFile );
		User::Error( ERR_FILE_CORRUPT, "ZIP: Central dir defect", path );
		return NULL;
	}

	return pakFile;
}

/*
================
PakFileEx::CloseZip

Close a ZipFile opened with OpenZip.
If there are files inside the .Zip opened with OpenCurrentFile,
these files MUST be closed before calling CloseZip.
================
*/
void PakFileEx::CloseZip( PakFileEx *pakFile ) {
	OG_ASSERT( pakFile != NULL );
	delete pakFile;
}

}
