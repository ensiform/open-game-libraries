/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Secure Hash Generator Tool
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

#include <og/Common/Common.h>
#include <stdio.h>

int main( int argc, char* argv[] ) {
	if( argc == 1 ) {
		printf("Usage: %s 'filename1' [filename2,...]\n", argv[0] );
		return 1;
	}

	og::SecureHash sha;
	for( int i=1; i<argc; i++ ) {
		if( !sha.ComputeFile(argv[i]) ) {
			printf("Error: Can't generate hash for file '%s'\n", argv[i] );
			return 0;
		}
		printf("%s\n", sha.GetHexResult() );
	}

	return 1;
}
