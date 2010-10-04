/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: FileSystem Demonstration
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

#include <vector>
#include <string>
#include <og/Common/Common.h>
#include <og/FileSystem/FileSystem.h>
#include "../Shared/Shared.h"

og::StringList allowedPakFiles;

namespace og {
namespace User {
	bool	IsPakFileAllowed( PakFile *pakFile ) {
		static SecureHash sha;
		if ( !sha.ComputeFile( pakFile->GetFilename() ) )
			return false;

		const char *checksum = sha.GetHexResult();
		if ( allowedPakFiles.Find( checksum ) != -1 ) {
			printf( "Adding file '%s'\n", pakFile->GetFilename() );
			return true;
		}
		printf( "Skipping file '%s'\n", pakFile->GetFilename() );
		return false;
	}

	void	Error( ErrorId id, const char *msg, const char *param ) {
		// Todo: Throw an exception on the id's you think are important.
		og::String result;
		getErrorString( id, msg, param, result );
		printf( "%s\n", result.c_str() );
	}
	void	Warning( const char *msg ) {
		printf( msg );
	}
	void	AssertFailed( const char *code, const char *function ) {
		printf( "Assert(%s) failed in %s!", code, function );
	}
}
}

bool ReadPureList( const char *filename ) {
	FILE *f = fopen(filename, "r");
	if ( !f ) {
		printf("Error: Can't open '%s'.\n", filename);
		return false;
	}

	const int MAX_BUFFER = 4096;
	char buffer[MAX_BUFFER];
	int len;
	while( fgets( buffer, MAX_BUFFER, f ) != NULL ) {
		len = strlen(buffer);
		if ( len == 41 && buffer[40] == '\n' ) {
			len--;
			buffer[40] = '\0';
		}
		if ( len == 40 )
			allowedPakFiles.Append( buffer );
	}
	fclose(f);
	if ( allowedPakFiles.IsEmpty() ) {
		printf("No hash values found in '%s'\n", filename);
		return false;
	}
	return !allowedPakFiles.IsEmpty();
}
int main( int argc, char* argv[] ) {
	og::Common::Init();
	if ( !ReadPureList("purelist.txt") )
		return 0;

	if ( og::FileSystem::Init( ".gpk", ".", ".", "base" ) ) {
		byte *buffer = NULL;
		og::FS->LoadFile( "print.txt", &buffer );
		if ( buffer ) {
			printf( "%s\n", buffer );
			og::FS->FreeFile( buffer );
		}
		og::FileSystem::Shutdown();
	}
	og::Common::Shutdown();
	return 0;
}
