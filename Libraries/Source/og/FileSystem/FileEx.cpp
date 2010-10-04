/*
===========================================================================
The Open Game Pak FileSystem.
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

#include "FileSystemEx.h"
#include <stdarg.h>

namespace og {
/////////////
// Little and Big Endians

OG_INLINE int SwapLong( byte *data ) {
	return static_cast<int>(data[0]<<24) + static_cast<int>(data[1]<<16) + static_cast<int>(data[2]<<8) + data[3];
}
OG_INLINE short SwapShort( byte *data ) {
	return static_cast<short>(data[0]<<8) + data[1];
}
OG_INLINE float SwapFloat( byte *data ) {
	byte newData[4];
	newData[0] = data[3];
	newData[1] = data[2];
	newData[2] = data[1];
	newData[3] = data[0];
	return *reinterpret_cast<float *>(newData);
}

#if OG_LITTLE_ENDIAN
	OG_INLINE int BigLong( byte *buf )			{ return SwapLong( buf ); }
	OG_INLINE int BigLong( int value )			{ return SwapLong( reinterpret_cast<byte *>(&value) ); }
	OG_INLINE int LittleLong( byte *buf )		{ return *reinterpret_cast<int *>(buf); }
	OG_INLINE int LittleLong( int value )		{ return value; }

	OG_INLINE short BigShort( byte *buf )		{ return SwapShort( buf ); }
	OG_INLINE short BigShort( short value )		{ return SwapShort( reinterpret_cast<byte *>(&value) ); }
	OG_INLINE short LittleShort( byte *buf )	{ return *reinterpret_cast<short *>(buf); }
	OG_INLINE short LittleShort( short value )	{ return value; }

	OG_INLINE float BigFloat( byte *buf )		{ return SwapFloat( buf ); }
	OG_INLINE float BigFloat( float value )		{ return SwapFloat( reinterpret_cast<byte *>(&value) ); }
	OG_INLINE float LittleFloat( byte *buf )	{ return *reinterpret_cast<float *>(buf); }
	OG_INLINE float LittleFloat( float value )	{ return value; }
#else
	OG_INLINE int BigLong( byte *buf )			{ return *reinterpret_cast<int *>(buf); }
	OG_INLINE int BigLong( int value )			{ return value; }
	OG_INLINE int LittleLong( byte *buf )		{ return SwapLong( buf ); }
	OG_INLINE int LittleLong( int value )		{ return SwapLong( reinterpret_cast<byte *>(&value) ); }

	OG_INLINE short BigShort( byte *buf )		{ return *reinterpret_cast<short *>(buf); }
	OG_INLINE short BigShort( short value )		{ return value; }
	OG_INLINE short LittleShort( byte *buf )	{ return SwapShort( buf ); }
	OG_INLINE short LittleShort( short value )	{ return SwapShort( reinterpret_cast<byte *>(&value) ); }

	OG_INLINE float BigFloat( byte *buf )		{ return *reinterpret_cast<float *>(buf); }
	OG_INLINE float BigFloat( float value )		{ return value; }
	OG_INLINE float LittleFloat( byte *buf )	{ return SwapFloat( buf ); }
	OG_INLINE float LittleFloat( float value )	{ return SwapFloat( reinterpret_cast<byte *>(&value) ); }
#endif

/*
==============================================================================

  FileEx

==============================================================================
*/
/*
================
FileEx::ReadInt
================
*/
int FileEx::ReadInt( void ) {
	OG_ASSERT( !writeMode );

	Read( endianBuf, 4 );
	return LittleLong(endianBuf);
}

/*
================
FileEx::ReadIntArray
================
*/
void FileEx::ReadIntArray( int *values, int num ) {
	for( int i=0; i<num; i++ )
		values[i] = ReadInt();
}

/*
================
FileEx::ReadUint
================
*/
uInt FileEx::ReadUint( void ) {
	OG_ASSERT( !writeMode );

	Read( endianBuf, 4 );
	return static_cast<uInt>(LittleLong(endianBuf));
}

/*
================
FileEx::ReadShort
================
*/
short FileEx::ReadShort( void ) {
	OG_ASSERT( !writeMode );

	Read( endianBuf, 2 );
	return LittleShort(endianBuf);
}

/*
================
FileEx::ReadShortArray
================
*/
void FileEx::ReadShortArray( short *values, int num ) {
	for( int i=0; i<num; i++ )
		values[i] = ReadShort();
}

/*
================
FileEx::ReadUshort
================
*/
uShort FileEx::ReadUshort( void ) {
	OG_ASSERT( !writeMode );

	Read( endianBuf, 2 );
	return static_cast<uShort>(LittleShort(endianBuf));
}

/*
================
FileEx::ReadChar
================
*/
char FileEx::ReadChar( void ) {
	OG_ASSERT( !writeMode );

	Read( endianBuf, 1 );
	return static_cast<char>(endianBuf[0]);
}

/*
================
FileEx::ReadByte
================
*/
byte FileEx::ReadByte( void ) {
	OG_ASSERT( !writeMode );

	Read( endianBuf, 1 );
	return endianBuf[0];
}

/*
================
FileEx::ReadFloat
================
*/
float FileEx::ReadFloat( void ) {
	OG_ASSERT( !writeMode );

	Read( endianBuf, 4 );
	return LittleFloat(endianBuf);
}

/*
================
FileEx::ReadFloatArray
================
*/
void FileEx::ReadFloatArray( float *values, int num ) {
	for( int i=0; i<num; i++ )
		values[i] = ReadFloat();
}

/*
================
FileEx::ReadBool
================
*/
bool FileEx::ReadBool( void ) {
	OG_ASSERT( !writeMode );

	Read( endianBuf, 1 );
	return (endianBuf[0] == 1);
}

/*
================
FileEx::WriteInt
================
*/
void FileEx::WriteInt( int value ) {
	OG_ASSERT( writeMode );

	*reinterpret_cast<int *>(endianBuf) = LittleLong(value);
	Write( endianBuf, 4 );
}

/*
================
FileEx::WriteIntArray
================
*/
void FileEx::WriteIntArray( const int *values, int num ) {
	for( int i=0; i<num; i++ )
		WriteInt( values[i] );
}

/*
================
FileEx::WriteUint
================
*/
void FileEx::WriteUint( uInt value ) {
	OG_ASSERT( writeMode );

	*reinterpret_cast<int *>(endianBuf) = LittleLong(value);
	Write( endianBuf, 4 );
}

/*
================
FileEx::WriteShort
================
*/
void FileEx::WriteShort( short value ) {
	OG_ASSERT( writeMode );

	*reinterpret_cast<short *>(endianBuf) = LittleShort(value);
	Write( endianBuf, 2 );
}

/*
================
FileEx::WriteUshort
================
*/
void FileEx::WriteUshort( uShort value ) {
	OG_ASSERT( writeMode );

	*reinterpret_cast<short *>(endianBuf) = LittleShort(value);
	Write( endianBuf, 2 );
}

/*
================
FileEx::WriteChar
================
*/
void FileEx::WriteChar( char value ) {
	OG_ASSERT( writeMode );

	Write( &value, 1 );
}

/*
================
FileEx::WriteByte
================
*/
void FileEx::WriteByte( byte value ) {
	OG_ASSERT( writeMode );

	Write( &value, 1 );
}

/*
================
FileEx::WriteFloat
================
*/
void FileEx::WriteFloat( float value ) {
	OG_ASSERT( writeMode );

	*reinterpret_cast<float *>(endianBuf) = LittleFloat(value);
	Write( endianBuf, 4 );
}

/*
================
FileEx::WriteFloatArray
================
*/
void FileEx::WriteFloatArray( const float *values, int num ) {
	for( int i=0; i<num; i++ )
		WriteFloat( values[i] );
}

/*
================
FileEx::WriteBool
================
*/
void FileEx::WriteBool( bool value ) {
	OG_ASSERT( writeMode );

	byte b = value ? 1 : 0;
	Write( &b, 1 );
}

/*
==============================================================================

  FileLocal

==============================================================================
*/
/*
================
FileLocal::Create
================
*/
FileLocal *FileLocal::Create( FILE *f ) {
	FileLocal *fileEx = new FileLocal;
	fileEx->file = f;
	return fileEx;
}

/*
================
FileLocal::Seek
================
*/
void FileLocal::Seek( long offset, int origin ) {
	if ( fseek( file, offset, origin ) != 0 )
		throw FileReadWriteError( FileReadWriteError::SEEK );
}

/*
================
FileLocal::Flush
================
*/
void FileLocal::Flush( void ) {
	OG_ASSERT( writeMode );
	if ( fflush( file ) != 0 )
		throw FileReadWriteError(FileReadWriteError::FLUSH);
}

/*
================
FileLocal::Read

Read a buffer from the file.
Be careful, this is not endian aligned reading.
================
*/
void FileLocal::Read( void *buffer, uInt len ) {
	OG_ASSERT( !writeMode );

	if ( fread( buffer, 1, len, file ) != len )
		throw FileReadWriteError(FileReadWriteError::READ);
}

/*
================
FileLocal::Write

Write a buffer to the file.
Be careful, this is not endian aligned writing.
================
*/
void FileLocal::Write( const void *buffer, uInt len ) {
	OG_ASSERT( writeMode );

	if ( fwrite( buffer, 1, len, file ) != len )
		throw FileReadWriteError(FileReadWriteError::WRITE);
}


/*
==============================================================================

  FileBuffered

==============================================================================
*/
/*
================
FileBuffered::Create
================
*/
FileBuffered *FileBuffered::Create( byte *buffer ) {
	FileBuffered *fileEx = new FileBuffered;
	fileEx->data = buffer;
	fileEx->position = 0;
	return fileEx;
}

/*
================
FileBuffered::Seek

Jump to a specified location in the current file.
Memory Seek is faster than other seeks.

If there is no error, the return value is UNZ_OK.
================
*/
void FileBuffered::Seek( long offset, int origin ) {
	if ( origin == SEEK_END ) {
		position = size;
		return;
	}
	if ( offset < 0 )
		throw FileReadWriteError(FileReadWriteError::SEEK);

	if ( origin == SEEK_SET )
		position = offset;
	else if ( origin == SEEK_CUR )
		position += offset;
	else
		throw FileReadWriteError(FileReadWriteError::SEEK);

	if ( position > size ) {
		position = size;
		throw FileReadWriteError(FileReadWriteError::SEEK);
	}
}

/*
================
FileBuffered::Read

Read a buffer from the file.
Be careful, this is not endian aligned reading.
================
*/
void FileBuffered::Read( void *buffer, uInt len ) {
	if ( position + len > size )
		throw FileReadWriteError(FileReadWriteError::READ);
	memcpy( buffer, data + position, len );
	position += len;
}

/*
==============================================================================

  FileInPak

==============================================================================
*/
/*
================
FileInPak::FileInPak
================
*/
FileInPak::FileInPak() {
	file = NULL;
	compressedData = false;
	atEOF = false;
}

/*
================
FileInPak::~FileInPak

Close this file and check for crc32
================
*/
FileInPak::~FileInPak() {
	// Close decompressing process.
	if ( compressedData )
		inflateEnd(&stream);

	// At end of file, check if the crc32 is correct.
	if ( Eof() && crc32Value != crc32Wait )
		User::Error( ERR_ZIP_CRC32, pakFile->GetFilename(), filename );

	// close file
	if ( file )
		fclose( file );
}

/*
================
FileInPak::Create

Open the specified file from the zipfile for reading data.
================
*/
FileInPak *FileInPak::Create( PakFileEx *pakFile, CentralDirEntry *cde ) {
	FILE *file = fopen( pakFile->GetFilename(), "rb");
	if ( file == NULL )
		return NULL; //! @todo	error message

	FileInPak *fileEx = new FileInPak;
	fileEx->file = file;

	fileEx->cde = cde;
	fileEx->pakFile = pakFile;


	// Copy all relevant data from the current file
	fileEx->posInZipfile = cde->posInZipfile;
	fileEx->remainingArchivedSize = cde->compressedSize;
	fileEx->remainingFinalSize = fileEx->size = cde->unCompressedSize;
	fileEx->atEOF = (fileEx->remainingFinalSize <= 0);
	fileEx->time = cde->time;

	fileEx->crc32Wait = cde->crc32Value;
	fileEx->crc32Value = 0;
	fileEx->compressedData = false;

	fileEx->stream.total_out = 0;

	// If the file is compressed
	if ( cde->compressionMethod != 0 ) {
		fileEx->stream.avail_in = 0;
		fileEx->stream.next_in = Z_NULL;
		fileEx->stream.zalloc = Z_NULL;
		fileEx->stream.zfree = Z_NULL;
		fileEx->stream.opaque = Z_NULL;

		// Initialize decompressing process
		if ( inflateInit2( &fileEx->stream, -MAX_WBITS ) == Z_OK )
			fileEx->compressedData = true;
	}
	return fileEx;
}

/*
================
FileInPak::Seek

Jump to a specified location in the current file.
Seek can be extremely slow, so use it with care.

If there is no error, the return value is UNZ_OK.
================
*/
void FileInPak::Seek( long offset, int origin ) {
	if ( origin == SEEK_END ) {
		atEOF = true;
		stream.total_out = size;
		return;
	}
	if ( offset < 0 )
		throw FileReadWriteError(FileReadWriteError::SEEK);

	// Rewind
	if ( origin == SEEK_SET ) {
		// No need to rewind, if the wanted position is in front.
		if ( offset >= Tell() )
			offset -= Tell();
		else
			Rewind();
	}

	if ( offset == 0 )
		return;

	if ( atEOF )
		throw FileReadWriteError(FileReadWriteError::SEEK);

	// No need to read the data if this is an uncompressed file.
	if ( !compressedData ) {
		if ( fseek( file, (origin == SEEK_SET) * cde->posInZipfile + offset, origin ) != 0 )
			throw FileReadWriteError(FileReadWriteError::SEEK);
	}

	DynBuffer<byte> buffer(offset);
	Read( buffer.data, offset );
}

/*
================
FileInPak::Rewind

Jump to the beginning of the file.
If there is no error, the return value is UNZ_OK.
================
*/
void FileInPak::Rewind( void ) {
	// Copy all relevant data from the current file
	posInZipfile = cde->posInZipfile;
	remainingArchivedSize = cde->compressedSize;
	remainingFinalSize = cde->unCompressedSize;
	atEOF = (remainingFinalSize <= 0);

	crc32Value = 0;
	stream.total_out = 0;

	// If the file is compressed
	if ( compressedData ) {
		stream.avail_in = 0;
		stream.next_in = Z_NULL;
		stream.zalloc = Z_NULL;
		stream.zfree = Z_NULL;
		stream.opaque = Z_NULL;
		if ( inflateReset(&stream) != UNZ_OK )
			throw FileReadWriteError(FileReadWriteError::REWIND);
	}
}

/*
================
FileInPak::Read

Read bytes from the current file.
return the number of bytes copied
return 0 if the end of file was reached
return <0 with error code if there is an error
(UNZ_ERRNO for IO error, or zLib error for uncompress error)
================
*/
void FileInPak::Read( void *buffer, uInt len ) {
	OG_ASSERT( !writeMode );
	OG_ASSERT( buffer != NULL );
	OG_ASSERT( len > 0 );

	uInt totalRead = 0;

	// If no bytes left to read, return end of file
	if ( remainingFinalSize == 0 )
		throw FileReadWriteError(FileReadWriteError::END_OF_FILE);
	// Data is not compressed
	else if ( !compressedData ) {
		totalRead = Math::Min(static_cast<uLong>(len), remainingFinalSize);

		// Jump to current position in zipfile and read next block into buffer
		if ( fseek( file, posInZipfile, SEEK_SET ) != 0 || fread( buffer, totalRead, 1, file ) != 1 )
			throw FileReadWriteError(FileReadWriteError::READ);
		else {
			// Calculate crc32
			crc32Value = crc32(crc32Value, static_cast<const Bytef *>(buffer), totalRead);

			// Recalculate remaining raw bytes and position in zipfile
			posInZipfile += totalRead;
			remainingFinalSize -= totalRead;
			atEOF = (remainingFinalSize <= 0);
			stream.total_out += totalRead; // need to update this as well, as it's used for Tell()
		}
	}
	// Deflated
	else {
		const Bytef *crcBufStart;
		uLong toRead;
		int err;

		// Init the stream output
		stream.next_out = static_cast<Bytef *>(buffer);
		stream.avail_out = Math::Min(static_cast<uLong>(len), remainingFinalSize);

		// While there's data in output
		while ( stream.avail_out > 0 ) {
			// Refill the input buffer if empty (raw input).
			if ( stream.avail_in == 0 && remainingArchivedSize > 0 ) {
				// Get the number of bytes we want to fill the buffer with.
				toRead = Math::Min(UNZ_BUFSIZE, remainingArchivedSize);

				// If no bytes left to read, return end of file
				if ( toRead == 0 )
					throw FileReadWriteError(FileReadWriteError::END_OF_FILE);

				// Jump to current position in zipfile and read next block into buffer
				if ( fseek( file, posInZipfile, SEEK_SET ) != 0 || fread( readBuffer, toRead, 1, file ) != 1 )
					throw FileReadWriteError(FileReadWriteError::READ);

				// Set the new position in zipfile and recalculate the restbytes.
				posInZipfile += toRead;
				remainingArchivedSize -= toRead;

				// Tell the stream where to find the input buffer, and how much it is.
				stream.next_in = reinterpret_cast<Bytef *>(readBuffer);
				stream.avail_in = toRead;
			}

			// Get a pointer to the beginning of the new uncompressed data.
			crcBufStart = stream.next_out;

			// Decompress the data
			err = inflate(&stream, Z_SYNC_FLUSH);

			// Calculate the bytes read
			toRead = stream.next_out - crcBufStart;

			// Recalculate crc32
			crc32Value = crc32(crc32Value, crcBufStart, toRead);

			// Decrease the number of uncompressed bytes left.
			remainingFinalSize -= toRead;
			atEOF = (remainingFinalSize <= 0);

			// Increase total number of bytes read.
			totalRead += toRead;

			// End of stream
			if ( err == Z_STREAM_END ) {
				if ( totalRead != len )
					throw FileReadWriteError(FileReadWriteError::END_OF_FILE);
				return;
			}

			// Error while decompressing.
			if ( err != Z_OK )
				throw FileReadWriteError(FileReadWriteError::DECOMPRESS);
		}
	}
}

}
