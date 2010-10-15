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

#include "main.h"
#include "Game.h"

og::Random globalRand;

const og::Vec2 windowCenter( WINDOW_WIDTH*0.5f, WINDOW_HEIGHT*0.5f );

const int maxLevels = 5;
int levelAsteroidCounts[maxLevels][3] = {
	{2,3,4},
	{3,4,6},
	{3,6,10},
	{4,10,15},
	{5,14,20},
};

/*
================
DrawImage
================
*/
void DrawImage( og::Image *image, float x, float y, float width, float height ) {
	image->BindTexture();
	glBegin(GL_QUADS);
		glTexCoord2f( 0, 0 ); glVertex2f( x, y );
		glTexCoord2f( 0, 1 ); glVertex2f( x, y+height );
		glTexCoord2f( 1, 1 ); glVertex2f( x+width, y+height );
		glTexCoord2f( 1, 0 ); glVertex2f( x+width, y );
	glEnd();
}

/*
================
ogGame::ogGame
================
*/
ogGame::ogGame() : entityDecls("entityDecl") {
	entityCount = 0;
	firstFreeEntityNum = 0;
	lastUsedEntityNum = 0;
	memset( entities, NULL, sizeof(ogEntity *) * MAX_GENTITIES );
	
	localPlayer = NULL;
	backImage = NULL;
	backImage2 = NULL;
	lifeImage = NULL;
}

/*
================
ogGame::Init
================
*/
bool ogGame::Init( void ) {
	backImage = og::Image::Find("gfx/asteroids/background.tga");
	backImage2 = og::Image::Find("gfx/asteroids/outro_overlay.tga");
	lifeImage = og::Image::Find("gfx/asteroids/item_extralife.tga");

	og::Dict test;
	test.Set("fontAlign", "1");
	test.Set("fontSize", "24");
	levelText.Init( test );
	levelText.SetWidth( WINDOW_WIDTH-5 );
	levelText.value = "Level: 0";

	test.Set("fontAlign", "2");
	test.Set("fontSize", "48");

	float width = static_cast<float>(WINDOW_WIDTH);
	stateText[GS_GAMEOVER].Init( test );
	stateText[GS_GAMEOVER].SetWidth( width );
	stateText[GS_GAMEOVER].value = "You lost all your lifes, try again.";

	stateText[GS_WIN].Init( test );
	stateText[GS_WIN].SetWidth( width );
	stateText[GS_WIN].value = "Man, you are awesome!";

	stateText[GS_GETREADY].Init( test );
	stateText[GS_GETREADY].SetWidth( width );
	stateText[GS_GETREADY].value = "^1G^0et ^2R^0eady ^3T^0o ^4R^0umble!"; // ^ + a digit is a predefined colorcode

	state = GS_GETREADY;
	stateTime = 2000;

	ogEntityType::RegisterEntityTypes();

	og::FileList *files = og::FS->GetFileList( "decls/entities", ".decl" );
	if ( files ) {
		og::DeclParser parser;
		parser.AddDeclType( &entityDecls );
		do {
			parser.LoadFile( files->GetName() );
		} while ( files->GetNext() );

		parser.SolveInheritance();
		og::FS->FreeFileList( files );
	}

	og::Vec3 forward(0, -1, 0);
	og::Vec3 up(0, 0, -1);
	og::Vec3 origin(0, 0, 1);
	og::AS->SetListener( origin, forward, up );

	globalRand.SeedTime();
	return true;
}

/*
================
ogGame::StartAmbience
================
*/
void ogGame::StartAmbience( void ) {
	soundManager.StartAmbience("ambience");
	SpawnAsteroidsLevel( windowCenter, 25, 2 );
}

/*
================
ogGame::SpawnAsteroidsLevel
================
*/
void ogGame::SpawnAsteroidsLevel( og::Vec2 origin, float radius, int spawnLevel ) {
	SpawnAsteroidsRadial( "asteroid_big", origin, levelAsteroidCounts[spawnLevel][0], radius + 100, radius + 400, 50.0f, 100.0f, 30.0f, 0.7f );
	SpawnAsteroidsRandom( "asteroid_medium", origin, levelAsteroidCounts[spawnLevel][1], radius + 100, radius + 300, 50.0f, 100.0f, 50.0f, 0.7f );
	SpawnAsteroidsRandom( "asteroid_small", origin, levelAsteroidCounts[spawnLevel][2], radius + 100, radius + 200, 50.0f, 100.0f, 60.0f, 0.7f );
}

/*
================
ogGame::Think
================
*/
void ogGame::Think( int frameTime ) {
	// start game
	if ( state == GS_GETREADY && stateTime > 0 && !demoWindow.menu.IsVisible() ) {
		stateTime -= frameTime;
		if ( stateTime <= 0 ) {
			Reset();
			state = GS_PLAYING;
		}
	}

	if ( state == GS_WIN || state == GS_GAMEOVER ) {
		if ( stateTime > 0 ) {
			stateTime -= frameTime;
			if ( stateTime <= 0 )
				demoWindow.WaitForInput( true );
		} else if ( demoWindow.HasInputResult() ) {
			demoWindow.StopWaitForInput();
			Reset();
			return;
		}
	}
	// check for next level
	if ( entityCount <= 1 && state == GS_PLAYING ) {
		if ( !localPlayer ) {
			og::Dict settings;
			settings.Set("origin", "400 300");
			localPlayer = static_cast<ogPlayer *>( ogEntityType::SpawnEntity( "player", &settings ) );
			localPlayer->Reset();
			level = 0;
		}

		SpawnAsteroidsLevel( localPlayer->origin, localPlayer->radius, level );
		level++;
		levelText.value = og::Format("Level: $*") << level;
		if ( level >= maxLevels ) {
			PlaySound( "win", windowCenter );
			state = GS_WIN;
			stateTime = 3000;
			ogEntityType::DestroyEntity( localPlayer );
			localPlayer = NULL;
		}
	}

	if ( state == GS_PLAYING && localPlayer->lifes == -1 ) {
		PlaySound( "lost", windowCenter );
		state = GS_GAMEOVER;
		stateTime = 3000;
		ogEntityType::DestroyEntity( localPlayer );
		localPlayer = NULL;
	}

	if ( localPlayer && !demoWindow.menu.IsVisible() ) {
		// check for attacks
		if ( state == GS_PLAYING && bindAttack.IsFreshDown() )
			localPlayer->Attack();
		static int nextExtraLife = globalRand.RandomInt( 5000, 15000 );

		nextExtraLife -= frameTime; 
		if ( nextExtraLife <= 0 ) {
			SpawnAsteroidsRandom( "item_extralife", localPlayer->origin, 1, 400, 600, 10.0f, 40.0f, 0.0f, 1.0f );
			nextExtraLife = globalRand.RandomInt( 20000, 30000 );
		}
	}

	if ( !demoWindow.menu.IsVisible() || !localPlayer ) {
		// Make the entities think
		float timeScale = static_cast<float>(frameTime)*0.001f;
		if ( timeScale > 1.0f )
			timeScale = 1.0f;
		for( int i=0; i<=lastUsedEntityNum; i++ ) {
			if ( entities[i] )
				entities[i]->Think( timeScale );
		}
	}
}

/*
================
ogGame::Draw
================
*/
void ogGame::Draw( float fadeIn ) {
	if ( fadeIn > 0.0f )
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f-fadeIn );

	float size = static_cast<float>(WINDOW_WIDTH);
	float diff = (size - WINDOW_HEIGHT) * 0.5f;
	DrawImage( backImage, 0, -diff, size, size );

	if ( fadeIn > 0.0f )
		return;

	for( int i=0; i<=lastUsedEntityNum; i++ ) {
		if ( entities[i] )
			entities[i]->Draw();
	}
	if ( state == GS_PLAYING && localPlayer ) {
		for( int i=0; i<localPlayer->lifes; i++ )
			DrawImage( lifeImage, i*16.0f + 5.0f, 0, 16, 16 );
		levelText.Draw( 0, 0 );
	}
	if ( state > GS_PLAYING && !demoWindow.menu.IsVisible() )
		stateText[state].Draw(0, 275);
}

/*
================
ogGame::Reset
================
*/
void ogGame::Reset( void ) {
	int last = lastUsedEntityNum;
	for( int i=0; i<=last; i++ ) {
		if ( entities[i] )
			ogEntityType::DestroyEntity( entities[i] );
	}
	localPlayer = NULL;
	level = 0;
	levelText.value = "Level: 0";
	state = GS_GETREADY;
	stateTime = 2000;
}

/*
================
ogGame::RegisterEntity
================
*/
void ogGame::RegisterEntity( ogEntity *ent ) {
	while( 1 ) {
		if ( firstFreeEntityNum >= MAX_GENTITIES )
			throw ogError("No Free Entities Left");
		if ( entities[firstFreeEntityNum] == NULL )
			break;
		firstFreeEntityNum++;
	}
	ent->entityNumber = firstFreeEntityNum++;
	ent->spawnId = spawnCount++;
	entities[ent->entityNumber] = ent;
	if ( ent->entityNumber > lastUsedEntityNum )
		lastUsedEntityNum = ent->entityNumber;
	entityCount++;
}

/*
================
ogGame::UnregisterEntity
================
*/
void ogGame::UnregisterEntity( ogEntity *ent ){
	OG_ASSERT( ent && ent->entityNumber != -1 && ent->entityNumber <= MAX_GENTITIES && ent == entities[ent->entityNumber] );

	entityCount--;

	entities[ent->entityNumber] = NULL;
	if ( firstFreeEntityNum > ent->entityNumber )
		firstFreeEntityNum = ent->entityNumber;
	if ( ent->entityNumber == lastUsedEntityNum ) {
		do {
			lastUsedEntityNum--;
		} while( lastUsedEntityNum >= 0 && entities[lastUsedEntityNum] == NULL );
	}
}

/*
================
ogGame::Shutdown
================
*/
void ogGame::Shutdown( void ) {
	int last = lastUsedEntityNum;
	for( int i=0; i<=last; i++ ) {
		if ( entities[i] )
			ogEntityType::DestroyEntity( entities[i] );
	}

	spawnCount = 0;

	soundManager.StopAll();
	soundManager.StopAmbience();
}

