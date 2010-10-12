/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Somewhat of a demo application
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

#ifndef __OG_GAME_H__
#define __OG_GAME_H__

#include "EntityType.h"
#include "Entities.h"
#include "../Shared/LogoScreen.h"
#include "../Shared/SoundManager.h"
const int	PORTAL_BORDER	= 50;
const int	MAX_GENTITIES	= 1024;
const int	MAX_SOUNDS		= 20;

extern og::Random globalRand;
extern const og::Vec2 windowCenter;

void DrawImage( og::Image *image, float x, float y, float width, float height );

/*
================
ogError

class to throw errors
================
*/
class ogError {
public:
	ogError( const char *err, bool fatal=false ) { isFatal = fatal; error = err; }
	bool		isFatal;
	og::String	error;
};

enum {
	GS_PLAYING = -1,
	GS_GAMEOVER,
	GS_WIN,
	GS_GETREADY
};

/*
==============================================================================

  ogGame

==============================================================================
*/
class ogGame {
public:
	ogGame();
	bool	Init( void );
	void	Shutdown( void );
	void	Think( int frametime );
	void	Draw( float fadeIn );
	void	SpawnAsteroidsLevel( og::Vec2 origin, float radius, int spawnLevel );
	void	Reset( void );
	void	StartAmbience( void );
	void	ShowAsteroids( void );
	OG_INLINE void	PlaySound( const char *name, const og::Vec2 &origin ) { soundManager.Play( name, origin-windowCenter ); }

public:
	ogPlayer *	localPlayer;
	og::Image *	backImage;
	og::Image *	backImage2; // overlay for outro
	og::Image *	lifeImage;
	ogFontText	levelText;
	ogFontText	stateText[3];
	int			state;
	int			stateTime;

	ogSoundManager2D soundManager;
	og::DeclType	entityDecls;
	void		RegisterEntity( ogEntity *ent );
	void		UnregisterEntity( ogEntity *ent );

	ogEntity *	entities[MAX_GENTITIES];
	int			firstFreeEntityNum;
	int			lastUsedEntityNum;
	int			spawnCount;
	int			entityCount;
	int			level;
};

#endif
