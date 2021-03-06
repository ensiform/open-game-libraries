/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Somewhat of a demo game
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

#include "Game.h"

#include <stdio.h>

class EngineExportGame : public GameImport {
public:
	int GetApiVersion( void ) const { return GAME_API_VERSION; }

	void SayWorld( const char *name ) {
		printf( "Engine: World %s\n", name );
	}
};

EngineExportGame gameImport;

int main( int argc, char* argv[] ) {

	printf("Trying to load plugin\n");
	GameExport *game = (GameExport *)og::Plugin::Load("TestPluginGame.dll", &gameImport);
	if( game == NULL ) {
		printf("Failed to load plugin\n");
		return -1;
	}

	game->SayHello("Engine");
	og::Plugin::Unload(game);
	game = NULL;
	return 1;
}
