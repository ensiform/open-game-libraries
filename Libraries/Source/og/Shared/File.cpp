/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Reading Data from files
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

#include <og/Shared/FileSystemCore.h>
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

  File

==============================================================================
*/
/*
================
File::ReadInt
================
*/
int File::ReadInt( void ) {
	Read( endianBuf, 4 );
	return LittleLong(endianBuf);
}

/*
================
File::ReadIntArray
================
*/
void File::ReadIntArray( int *values, int num ) {
	for( int i=0; i<num; i++ )
		values[i] = ReadInt();
}

/*
================
File::ReadUint
================
*/
uInt File::ReadUint( void ) {
	Read( endianBuf, 4 );
	return static_cast<uInt>(LittleLong(endianBuf));
}

/*
================
File::ReadShort
================
*/
short File::ReadShort( void ) {
	Read( endianBuf, 2 );
	return LittleShort(endianBuf);
}

/*
================
File::ReadShortArray
================
*/
void File::ReadShortArray( short *values, int num ) {
	for( int i=0; i<num; i++ )
		values[i] = ReadShort();
}

/*
================
File::ReadUshort
================
*/
uShort File::ReadUshort( void ) {
	Read( endianBuf, 2 );
	return static_cast<uShort>(LittleShort(endianBuf));
}

/*
================
File::ReadChar
================
*/
char File::ReadChar( void ) {
	Read( endianBuf, 1 );
	return static_cast<char>(endianBuf[0]);
}

/*
================
File::ReadByte
================
*/
byte File::ReadByte( void ) {
	Read( endianBuf, 1 );
	return endianBuf[0];
}

/*
================
File::ReadFloat
================
*/
float File::ReadFloat( void ) {
	Read( endianBuf, 4 );
	return LittleFloat(endianBuf);
}

/*
================
File::ReadFloatArray
================
*/
void File::ReadFloatArray( float *values, int num ) {
	for( int i=0; i<num; i++ )
		values[i] = ReadFloat();
}

/*
================
File::ReadBool
================
*/
bool File::ReadBool( void ) {
	Read( endianBuf, 1 );
	return (endianBuf[0] == 1);
}

/*
================
File::WriteInt
================
*/
void File::WriteInt( int value ) {
	*reinterpret_cast<int *>(endianBuf) = LittleLong(value);
	Write( endianBuf, 4 );
}

/*
================
File::WriteIntArray
================
*/
void File::WriteIntArray( const int *values, int num ) {
	for( int i=0; i<num; i++ )
		WriteInt( values[i] );
}

/*
================
File::WriteUint
================
*/
void File::WriteUint( uInt value ) {
	*reinterpret_cast<int *>(endianBuf) = LittleLong(value);
	Write( endianBuf, 4 );
}

/*
================
File::WriteShort
================
*/
void File::WriteShort( short value ) {
	*reinterpret_cast<short *>(endianBuf) = LittleShort(value);
	Write( endianBuf, 2 );
}

/*
================
File::WriteUshort
================
*/
void File::WriteUshort( uShort value ) {
	*reinterpret_cast<short *>(endianBuf) = LittleShort(value);
	Write( endianBuf, 2 );
}

/*
================
File::WriteChar
================
*/
void File::WriteChar( char value ) {
	Write( &value, 1 );
}

/*
================
File::WriteByte
================
*/
void File::WriteByte( byte value ) {
	Write( &value, 1 );
}

/*
================
File::WriteFloat
================
*/
void File::WriteFloat( float value ) {
	*reinterpret_cast<float *>(endianBuf) = LittleFloat(value);
	Write( endianBuf, 4 );
}

/*
================
File::WriteFloatArray
================
*/
void File::WriteFloatArray( const float *values, int num ) {
	for( int i=0; i<num; i++ )
		WriteFloat( values[i] );
}

/*
================
File::WriteBool
================
*/
void File::WriteBool( bool value ) {
	byte b = value ? 1 : 0;
	Write( &b, 1 );
}

/*
==============================================================================

  FileReadWriteError

==============================================================================
*/
/*
================
FileReadWriteError::FileReadWriteError
================
*/
FileReadWriteError::FileReadWriteError( const char *msg ) {
	customMsg = msg;
	type = CUSTOM;
}
FileReadWriteError::FileReadWriteError( ErrorType type ) {
	this->type = type;
}

/*
================
FileReadWriteError::ToString
================
*/
const char *FileReadWriteError::ToString( void ) {
	switch(type) {
			case CUSTOM:		return customMsg.c_str();
			case DECOMPRESS:	return "Decompression failed";
			case READ:			return "Read failed";
			case WRITE:			return "Write failed";
			case REWIND:		return "Rewind failed";
			case SEEK:			return "Seek failed";
			case FLUSH:			return "Flush failed";
			case END_OF_FILE:	return "Unexpected end of file";
	}
	return "Unknown read/write error";
}

}
