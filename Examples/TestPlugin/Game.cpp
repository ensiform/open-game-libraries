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

GameImport *engine;

class Game : public GameExport {
public:
	int GetApiVersion( void ) const { return GAME_API_VERSION; }

	void OnLoad( og::PluginImportBase *pluginImport ) {
		engine = (GameImport *)pluginImport;
		engine->SayWorld("Game::OnLoad");
	}
	void OnUnload( void ) {
		engine->SayWorld("Game::OnUnload");
		engine = NULL;
	}

	void SayHello( const char *name ) {
		printf( "Game: Hello %s\n", name );
	}
};

Game gameObject;

og::PluginExportBase *getPluginExport( void ) {
	printf("getPluginExport called\n");
	return &gameObject;
}
