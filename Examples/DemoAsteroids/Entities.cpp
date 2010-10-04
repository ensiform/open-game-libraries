/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Somewhat of a demo application
Note:    Some of the code is based on GLFW 2.6 (See Readme.txt)
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


/*
================
SpawnAsteroidsRadial
================
*/
void SpawnAsteroidsRadial( const char *classname, og::Vec2 origin, int count, float minDist, float maxDist, float minSpeed, float maxspeed, float maxRot, float spreadPct ) {
	og::Vec2 dir;
	float angleStep = 360.0f / count;
	float spreadAngle = angleStep * spreadPct * 0.5f;
	float angle = globalRand.RandomFloat( 0, 360.0f);
	og::Dict addSettings;
	for( int i=0; i<count; i++, angle+=angleStep ) {
		dir.FromAngle( globalRand.RandomFloat( angle-spreadAngle, angle+spreadAngle) );
		addSettings.SetVec2("origin", origin + dir * globalRand.RandomFloat( minDist, maxDist ) );
		addSettings.SetVec2("velocity", dir *globalRand.RandomFloat( minSpeed, maxspeed ) );
		addSettings.SetFloat("rotation", angle);
		addSettings.SetFloat("rotationSpeed", globalRand.RandomFloat( -maxRot, maxRot));
		ogEntityType::SpawnEntity( classname, &addSettings );
	}
}

/*
================
SpawnAsteroidsRandom
================
*/
void SpawnAsteroidsRandom( const char *classname, og::Vec2 origin, int count, float minDist, float maxDist, float minSpeed, float maxspeed, float maxRot, float spreadPct ) {
	og::Vec2 dir;
	float angleStep = 360.0f / count;
	float spreadAngle = angleStep * spreadPct;
	float angle = globalRand.RandomFloat( 0, 360.0f);
	og::Dict addSettings;
	for( int i=0; i<count; i++, angle+=angleStep ) {
		dir.FromAngle( globalRand.RandomFloat( angle-spreadAngle, angle+spreadAngle) );
		addSettings.SetVec2("origin", origin + dir * globalRand.RandomFloat( minDist, maxDist ) );
		dir.FromAngle( globalRand.RandomFloat( 0, 360.0f) );
		addSettings.SetVec2("velocity", dir *globalRand.RandomFloat( minSpeed, maxspeed ) );
		addSettings.SetFloat("rotationSpeed", globalRand.RandomFloat( -maxRot, maxRot));
		ogEntityType::SpawnEntity( classname, &addSettings );
	}
}


/*
==============================================================================

  ogEntity

==============================================================================
*/
DEFINE_ENTITY_TYPE(ogEntity, NULL)

/*
================
ogEntity::ogEntity
================
*/
ogEntity::ogEntity() {
	doExplode = false;
	image = NULL;
	drawRadius = 0.0f;
	radius = 0.0f;
	rotation = 0;
	rotationSpeed = 0;
	entityNumber = -1;
}

/*
================
ogEntity::Spawn
================
*/
void ogEntity::Spawn( void ) {
	image = og::Image::Find(settings.GetString("image"));
	og::Vec2 halfImageSize = settings.GetVec2("imageSize") * 0.5f;
	imageVerts[0].Set( -halfImageSize.x, -halfImageSize.y );
	imageVerts[1].Set( halfImageSize.x, halfImageSize.y );
	float t = og::Math::Square(halfImageSize.x) + og::Math::Square(halfImageSize.y);
	if ( t != 0.0f )
		drawRadius = og::Math::Sqrt(t);
	origin = settings.GetVec2("origin");
	velocity = settings.GetVec2("velocity");
	radius = settings.GetFloat("radius");
	rotation = settings.GetFloat("rotation");
	rotationSpeed = settings.GetFloat("rotationSpeed");
	popTime = popTimeFull = settings.GetFloat("popTime", "0.5");
}

/*
================
ogEntity::ogEntity
================
*/
void ogEntity::Think( float timeScale ) {
	origin += velocity * timeScale;
	rotation += rotationSpeed * timeScale;

	if ( origin.x < 0.0f )
		origin.x += WINDOW_WIDTH;
	else if ( origin.x >= WINDOW_WIDTH )
		origin.x -= WINDOW_WIDTH;

	if ( origin.y < 0.0f )
		origin.y += WINDOW_HEIGHT;
	else if ( origin.y >= WINDOW_HEIGHT )
		origin.y -= WINDOW_HEIGHT;

	portalOffset.Zero();
	if ( origin.x < drawRadius )
		portalOffset.x = WINDOW_WIDTH;
	else if ( origin.x >= WINDOW_WIDTH-drawRadius )
		portalOffset.x = -WINDOW_WIDTH;
	if ( origin.y < drawRadius )
		portalOffset.y = WINDOW_HEIGHT;
	else if ( origin.y >= WINDOW_HEIGHT-drawRadius )
		portalOffset.y = -WINDOW_HEIGHT;

	if ( popTime > 0.0f ) {
		popTime -= timeScale;
		if ( popTime < 0.0f )
			popTime = 0.0f;
	}

	if ( demoWindow.game.state != GS_PLAYING )
		return;

	CheckCollisions( origin );
	if ( !portalOffset.IsZero() ) {
		if ( portalOffset.x != 0.0f && portalOffset.y != 0.0f ) {
			CheckCollisions( origin + og::Vec2( 0, portalOffset.y ) );
			CheckCollisions( origin + og::Vec2( portalOffset.x, 0 ) );
		}
		CheckCollisions( origin + portalOffset );
	}
	if ( doExplode ) {
		demoWindow.game.PlaySound( settings.GetString("snd_destroy"), origin );
		Explode();
	}
}

/*
================
ogEntity::ogEntity
================
*/
void ogEntity::Draw( void ) {
	DrawSelf( origin.x, origin.y );
	if ( !portalOffset.IsZero() ) {
		if ( portalOffset.x != 0.0f && portalOffset.y != 0.0f ) {
			DrawSelf( origin.x, origin.y + portalOffset.y );
			DrawSelf( origin.x + portalOffset.x, origin.y );
		}
		DrawSelf( origin.x + portalOffset.x, origin.y + portalOffset.y );
	}
}

/*
================
ogEntity::ogEntity
================
*/
void ogEntity::CheckCollisions( og::Vec2 pos ) {
	float dist, maxDist;
	for( int i=entityNumber; i<=demoWindow.game.lastUsedEntityNum; i++ ) {
		if ( demoWindow.game.entities[i] && demoWindow.game.entities[i] != this ) {
			dist = (pos - demoWindow.game.entities[i]->origin).Length();
			maxDist = demoWindow.game.entities[i]->radius + radius;
			if ( dist < maxDist ) {
				Collide( demoWindow.game.entities[i] );
				demoWindow.game.entities[i]->Collide( this );
			}
		}
	}
}

/*
================
ogEntity::ogEntity
================
*/
void ogEntity::DrawImage( og::Image *img, float scale ) {
	img->BindTexture();
	glBegin(GL_QUADS);
		glTexCoord2f( 0, 0 ); glVertex2f( imageVerts[0].x * scale, imageVerts[0].y * scale );
		glTexCoord2f( 0, 1 ); glVertex2f( imageVerts[0].x * scale, imageVerts[1].y * scale );
		glTexCoord2f( 1, 1 ); glVertex2f( imageVerts[1].x * scale, imageVerts[1].y * scale );
		glTexCoord2f( 1, 0 ); glVertex2f( imageVerts[1].x * scale, imageVerts[0].y * scale );
	glEnd();
}

/*
================
ogEntity::ogEntity
================
*/
void ogEntity::DrawSelf( float x, float y ) {
	glPushMatrix();
		glTranslatef( x, y, 0.0f );
		glRotatef( rotation, 0.0f, 0.0f, 1.0f );

		float scale = popTimeFull <= 0.0f ? 1.0f : 1.0f - popTime/popTimeFull;
		DrawImage( image, scale );
	glPopMatrix();
}

/*
================
ogEntity::ogEntity
================
*/
void ogEntity::Explode( void ) {
	ogEntityType::DestroyEntity( this );
}


/*
==============================================================================

  ogPlayer

==============================================================================
*/
DEFINE_ENTITY_TYPE(ogPlayer, ogEntity)

/*
================
ogPlayer::Spawn
================
*/
void ogPlayer::Spawn( void ) {
	imageShield = og::Image::Find(settings.GetString("image_shield"));
	imageThrust = og::Image::Find(settings.GetString("image_thrust"));
	imageReverse = og::Image::Find(settings.GetString("image_reverse"));
	imageLeft = og::Image::Find(settings.GetString("image_left"));
	imageRight = og::Image::Find(settings.GetString("image_right"));
	acceleration = settings.GetFloat("acceleration");
	maxSpeed = settings.GetFloat("maxSpeed");
	lifes = settings.GetInt("lifes");
	spawnProtection = settings.GetFloat("spawnProtection");
	spawnProtectionFade = settings.GetFloat("spawnProtectionFade");
	blastClass = settings.GetString("blast");
}

/*
================
ogPlayer::Collide
================
*/
void ogPlayer::Collide( ogEntity *ent ) {
	if ( ent->IsType( ogAsteroid::GetStaticType() ) ) {
		if ( IsProtected() )
			return;
		doExplode = true;
	} else if ( ent->IsType( ogItem::GetStaticType() ) ) {
		ogItem *item = (ogItem *)ent;
		lifes += item->extraLifes;
	}
}

/*
================
ogPlayer::Think
================
*/
void ogPlayer::Think( float timeScale ) {
	float accelScale = bindAccelerate.CurScale();
	float turnScale = bindTurn.CurScale();
	if ( turnScale != 0.0f ) {
		if ( accelScale < 0.0f )
			turnScale *= 0.6f;
		rotation += (turnScale * 200.0f) * timeScale;
	}

	if ( accelScale != 0.0f ) {
		if ( accelScale < 0.0f )
			accelScale *= 0.3f;
		og::Vec2 dir;
		dir.FromAngle( rotation-90 );
		velocity += dir * acceleration * (accelScale * timeScale);
		velocity.CapLength( maxSpeed );
	}

	if ( spawnProtection > 0.0f ) {
		spawnProtection -= timeScale;
		if ( spawnProtection < 0.0f )
			spawnProtection = 0.0f;
	}
	ogEntity::Think( timeScale );
}

/*
================
ogPlayer::Draw
================
*/
void ogPlayer::Draw( void ) {
	if ( demoWindow.game.state == GS_PLAYING )
		ogEntity::Draw();
}

/*
================
ogPlayer::DrawSelf
================
*/
void ogPlayer::DrawSelf( float x, float y ) {
	glPushMatrix();
		glTranslatef( x, y, 0.0f );
		glRotatef( rotation, 0.0f, 0.0f, 1.0f );

		float scale = popTimeFull <= 0.0f ? 1.0f : 1.0f - popTime/popTimeFull;

		DrawImage( image, scale );
		
		float accelScale = bindAccelerate.CurScale();
		if ( accelScale > 0.0f ) 
			DrawImage( imageThrust, scale );
		else if ( accelScale < 0.0f ) 
			DrawImage( imageReverse, scale );

		float turnScale = bindTurn.CurScale();
		if ( turnScale > 0.0f ) 
			DrawImage( imageRight, scale );
		else if ( turnScale < 0.0f ) 
			DrawImage( imageLeft, scale );

		if ( IsProtected() ) {
			if ( spawnProtection < spawnProtectionFade )
				scale =	static_cast<float>(spawnProtection) / spawnProtectionFade;
			DrawImage( imageShield, scale );
		}
	glPopMatrix();
}

/*
================
ogPlayer::Explode
================
*/
void ogPlayer::Explode( void ) {
	og::String spawnClass = settings.GetString("spawn");
	int spawnCount = settings.GetInt("spawnCount");
	if ( !spawnClass.IsEmpty() && spawnCount > 0 )
		SpawnAsteroidsRadial( spawnClass.c_str(), origin, spawnCount, radius*0.15f, radius*0.35f, 50.0f, 100.0f, 60.0f, 0.35f );

	lifes--;
	Reset();
}

/*
================
ogPlayer::Reset
================
*/
void ogPlayer::Reset( void ) {
	origin = windowCenter;
	velocity.Set( 0, 0 );
	rotation = 0.0f;
	doExplode = false;
	demoWindow.game.PlaySound( settings.GetString("snd_teleport"), origin );
	spawnProtection = settings.GetFloat("spawnProtection");
	popTime = popTimeFull;
}

/*
================
ogPlayer::Attack
================
*/
void ogPlayer::Attack( void ) {
	og::Dict addSettings;
	og::Vec2 dir;
	dir.FromAngle( rotation-90 );
	addSettings.SetVec2("origin", origin + dir * radius*0.75f );
	addSettings.SetVec2("velocity", dir );
	addSettings.SetFloat("rotation", rotation);
	static_cast<ogPlayer *>( ogEntityType::SpawnEntity( blastClass.c_str(), &addSettings ) );
	demoWindow.game.PlaySound( settings.GetString("snd_shot"), origin );
}


/*
==============================================================================

  ogBlast

==============================================================================
*/
DEFINE_ENTITY_TYPE(ogBlast, ogEntity)

/*
================
ogBlast::Spawn
================
*/
void ogBlast::Spawn( void ) {
	velocity *= settings.GetFloat("speed");
	timeLeft = settings.GetFloat("lifeTime");
}

/*
================
ogBlast::Think
================
*/
void ogBlast::Think( float timeScale ) {
	timeLeft -= timeScale;
	if ( timeLeft <= 0 ) {
		Explode();
		return;
	}
	ogEntity::Think( timeScale );
}

/*
================
ogBlast::Collide
================
*/
void ogBlast::Collide( ogEntity *ent ) {
	if ( ent->IsType( ogAsteroid::GetStaticType() ) )
		doExplode = true;
}


/*
==============================================================================

  ogAsteroid

==============================================================================
*/
DEFINE_ENTITY_TYPE(ogAsteroid, ogEntity)

/*
================
ogAsteroid::Collide
================
*/
void ogAsteroid::Collide( ogEntity *ent ) {
	if ( ent->IsType( ogBlast::GetStaticType() ) || ent->IsType( ogPlayer::GetStaticType() ) ) {
		if ( ent->IsType( ogPlayer::GetStaticType() ) ) {
			ogPlayer *player = static_cast<ogPlayer *>(ent);
			if ( player->IsProtected() )
				return;
		}
		doExplode = true;

		og::String spawnClass = settings.GetString("spawn");
		int spawnCount = settings.GetInt("spawnCount");
		if ( !spawnClass.IsEmpty() && spawnCount > 0 )
			SpawnAsteroidsRadial( spawnClass.c_str(), origin, spawnCount, radius*0.15f, radius*0.35f, 50.0f, 100.0f, 60.0f, 0.7f );
	}
}


/*
==============================================================================

  ogSplitter

==============================================================================
*/
DEFINE_ENTITY_TYPE(ogSplitter, ogEntity)

/*
================
ogSplitter::Spawn
================
*/
void ogSplitter::Spawn( void ) {
	timeLeft = lifeTime = settings.GetFloat("lifeTime");
}

/*
================
ogSplitter::Think
================
*/
void ogSplitter::Think( float timeScale ) {
	timeLeft -= timeScale;
	if ( timeLeft <= 0 ) {
		Explode();
		return;
	}
	ogEntity::Think( timeScale );
}

/*
================
ogSplitter::Draw
================
*/
void ogSplitter::Draw( void ) {
	glColor4f( 1.0f, 1.0f, 1.0f, timeLeft/lifeTime );
	ogEntity::Draw();
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
}


/*
==============================================================================

  ogItem

==============================================================================
*/
DEFINE_ENTITY_TYPE(ogItem, ogEntity)

/*
================
ogItem::Spawn
================
*/
void ogItem::Spawn( void ) {
	extraLifes = settings.GetInt("extraLifes");
	timeLeft = settings.GetFloat("lifeTime");
}

/*
================
ogItem::Collide
================
*/
void ogItem::Collide( ogEntity *ent ) {
	if ( ent->IsType( ogPlayer::GetStaticType() ) )
		doExplode = true;
}

/*
================
ogItem::Think
================
*/
void ogItem::Think( float timeScale ) {
	timeLeft -= timeScale;
	if ( timeLeft <= 0 ) {
		Explode();
		return;
	}
	ogEntity::Think( timeScale );
}
