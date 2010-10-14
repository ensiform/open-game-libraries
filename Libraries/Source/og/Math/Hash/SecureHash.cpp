/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Secure Hash Algorithm (SHA1)
Note:    Original code by Adrien Pinet, speed improved by over 70%
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

#include <og/Math/Math.h>
#include <stdio.h>

namespace og {
const uInt HASH_BUFFER_SIZE = 4096;

/*
================
SHA1CircularShift
================
*/
OG_INLINE uInt SHA1CircularShift( byte bits, uInt word ) {
	return (word << bits) | (word >> (32-bits));
}

/*
================
BigLong
================
*/
#if OG_LITTLE_ENDIAN
	OG_INLINE uInt BigLong( const byte *buf )	{ return static_cast<int>(buf[0]<<24) + static_cast<int>(buf[1]<<16) + static_cast<int>(buf[2]<<8) + buf[3]; }
#else
	OG_INLINE int BigLong( const byte *buf )	{ return *reinterpret_cast<int *>(buf); }
#endif

/*
==============================================================================

  SecureHash

==============================================================================
*/

/*
================
SecureHash::Reset
================
*/
void SecureHash::Reset( void ) {
	lengthLow = 0;
	lengthHigh = 0;
	messageBlockSize = 0;

	intermediateHash[0] = 0x67452301;
	intermediateHash[1] = 0xEFCDAB89;
	intermediateHash[2] = 0x98BADCFE;
	intermediateHash[3] = 0x10325476;
	intermediateHash[4] = 0xC3D2E1F0;

	memset( byteResult, 0, 20 );
	memset( hexResult, 0, 41 );
}

/*
================
SecureHash::Finish
================
*/
void SecureHash::Finish( void ) {
	ProcessRemaining();

	memset( messageBlock, 0, 64 );
	messageBlockSize = 0;

	lengthLow = 0;
	lengthHigh = 0;

	hexResult[0] = 0;
	char *p = hexResult;
	for( int i=0; i < 20; i++, p+=2 ) {
		byteResult[i] = static_cast<byte>( intermediateHash[i >> 2] >> 8 * (3 - (i & 0x03)) );
		sprintf( p, "%02x", byteResult[i] );
	}
}

/*
================
SecureHash::ComputeFile
================
*/
bool SecureHash::ComputeFile( const char *fileName ) {
	FILE *file = fopen( fileName, "rb" );	
	if( !file )
		return false;

	Reset();

	byte buffer[HASH_BUFFER_SIZE];
	int read;
	do {
		if ( !( read = fread( buffer, 1, HASH_BUFFER_SIZE, file ) ) )
			break;
		if( !AddBuffer( buffer, read ) ) {
			fclose(file);
			return false;
		}
	} while( read == HASH_BUFFER_SIZE );

	Finish();

	fclose(file);
	return true;
}

/*
================
SecureHash::AddBuffer
================
*/
bool SecureHash::AddBuffer( const byte* buffer, uInt bufferSize ) {
	uInt llSize;
	do {
		messageBlockSize = Min( bufferSize, (uInt)64 );
		bufferSize -= messageBlockSize;

		llSize = 8 * messageBlockSize;
		lengthLow += llSize;

		// check for overflow
		if ( lengthLow < llSize && ++lengthHigh == 0 ) {
			memcpy( messageBlock, buffer, messageBlockSize );
			return false;
		}

		if( messageBlockSize < 64 ) 
			memcpy( messageBlock, buffer, messageBlockSize );
		else{
			ProcessMessageBlock(buffer);
			buffer += messageBlockSize;
			messageBlockSize = 0;
		}

	} while( bufferSize );

	return true;
}

/*
================
SecureHash::ProcessMessageBlock
================
*/
const uInt RoundConst[] = { 0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6 };
void SecureHash::ProcessMessageBlock( const byte* buffer ) {
	uInt temp;
	uInt W[80];

	for( int i=0; i < 16; i++ )
		W[i] = BigLong(buffer+i*4);

	for( int i=16; i < 80; i++ )
		W[i] = SHA1CircularShift(1,W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16]);

	uInt A = intermediateHash[0];
	uInt B = intermediateHash[1];
	uInt C = intermediateHash[2];
	uInt D = intermediateHash[3];
	uInt E = intermediateHash[4];

#define SHA1_ROUND(round,f)\
	for( int i=20*round; i < 20*round+20; i++ ) {\
		temp =  SHA1CircularShift(5,A) + (f) + E + W[i] + RoundConst[round];\
		E = D;\
		D = C;\
		C = SHA1CircularShift(30,B);\
		B = A;\
		A = temp;\
	}

	SHA1_ROUND( 0, (B & C) | ((~B) & D))
	SHA1_ROUND( 1, (B ^ C ^ D))
	SHA1_ROUND( 2, (B & C) | (B & D) | (C & D))
	SHA1_ROUND( 3, (B ^ C ^ D))

	intermediateHash[0] += A;
	intermediateHash[1] += B;
	intermediateHash[2] += C;
	intermediateHash[3] += D;
	intermediateHash[4] += E;
}

/*
================
SecureHash::ProcessRemaining
================
*/
void SecureHash::ProcessRemaining( void ) {
	messageBlock[messageBlockSize++] = 0x80;
	if ( messageBlockSize < 56 )
		memset( &messageBlock[messageBlockSize], 0, 56-messageBlockSize );
	else if ( messageBlockSize > 56 ) {
		if ( messageBlockSize < 64 )
			memset( &messageBlock[messageBlockSize], 0, 64-messageBlockSize );
		ProcessMessageBlock(messageBlock);
		memset( messageBlock, 0, 56 );
	}
	messageBlock[56] = static_cast<byte>( lengthHigh >> 24 );
	messageBlock[57] = static_cast<byte>( lengthHigh >> 16 );
	messageBlock[58] = static_cast<byte>( lengthHigh >> 8 );
	messageBlock[59] = static_cast<byte>( lengthHigh );
	messageBlock[60] = static_cast<byte>( lengthLow >> 24 );
	messageBlock[61] = static_cast<byte>( lengthLow >> 16 );
	messageBlock[62] = static_cast<byte>( lengthLow >> 8 );
	messageBlock[63] = static_cast<byte>( lengthLow );

	ProcessMessageBlock(messageBlock);
}

}
