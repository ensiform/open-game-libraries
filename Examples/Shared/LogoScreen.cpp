/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Intro
-----------------------------------------

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
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

#include "LogoScreen.h"
#include <og/Gloot/glmin.h>
#include <og/Gloot/Gloot.h>

/*
================
DrawDice
================
*/
void DrawDice( const og::Vec3 &origin, og::Angles rotation ) {
	glPushMatrix();
	glTranslatef( origin.x, origin.y, origin.z );
	glRotatef( rotation.pitch, 1, 0, 0 );
	glRotatef( rotation.yaw, 0, 1, 0 );
	glRotatef( rotation.roll, 0, 0, 1 );

	const float HALF_DICE_SIZE = 1.0f;

	float third = 1.0f / 3.0f;
	glBegin(GL_QUADS);

		glNormal3f( 0.0f, 0.0f, 1.0f );
		glTexCoord2f(0,2*third);
		glVertex3f(-HALF_DICE_SIZE, -HALF_DICE_SIZE,  HALF_DICE_SIZE);
		glTexCoord2f(0,1);
		glVertex3f( HALF_DICE_SIZE, -HALF_DICE_SIZE,  HALF_DICE_SIZE);
		glTexCoord2f(0.5f,1);
		glVertex3f( HALF_DICE_SIZE,  HALF_DICE_SIZE,  HALF_DICE_SIZE);
		glTexCoord2f(0.5f,2*third);
		glVertex3f(-HALF_DICE_SIZE,  HALF_DICE_SIZE,  HALF_DICE_SIZE);

		glNormal3f( 0.0f, 0.0f, -1.0f );
		glTexCoord2f(1,0);
		glVertex3f(-HALF_DICE_SIZE, -HALF_DICE_SIZE, -HALF_DICE_SIZE);
		glTexCoord2f(0.5f,0);
		glVertex3f(-HALF_DICE_SIZE,  HALF_DICE_SIZE, -HALF_DICE_SIZE);
		glTexCoord2f(0.5f,third);
		glVertex3f( HALF_DICE_SIZE,  HALF_DICE_SIZE, -HALF_DICE_SIZE);
		glTexCoord2f(1,third);
		glVertex3f( HALF_DICE_SIZE, -HALF_DICE_SIZE, -HALF_DICE_SIZE);

		// todo
		glNormal3f( 0.0f, 1.0f, 0.0f );
		glTexCoord2f(0,2*third);
		glVertex3f(-HALF_DICE_SIZE,  HALF_DICE_SIZE, -HALF_DICE_SIZE);
		glTexCoord2f(0.5f,2*third);
		glVertex3f(-HALF_DICE_SIZE,  HALF_DICE_SIZE,  HALF_DICE_SIZE);
		glTexCoord2f(0.5f,third);
		glVertex3f( HALF_DICE_SIZE,  HALF_DICE_SIZE,  HALF_DICE_SIZE);
		glTexCoord2f(0,third);
		glVertex3f( HALF_DICE_SIZE,  HALF_DICE_SIZE, -HALF_DICE_SIZE);

		// todo
		glNormal3f( 0.0f, -1.0f, 0.0f );
		glTexCoord2f(1,2*third);
		glVertex3f(-HALF_DICE_SIZE, -HALF_DICE_SIZE, -HALF_DICE_SIZE);
		glTexCoord2f(1,third);
		glVertex3f( HALF_DICE_SIZE, -HALF_DICE_SIZE, -HALF_DICE_SIZE);
		glTexCoord2f(0.5f,third);
		glVertex3f( HALF_DICE_SIZE, -HALF_DICE_SIZE,  HALF_DICE_SIZE);
		glTexCoord2f(0.5f,2*third);
		glVertex3f(-HALF_DICE_SIZE, -HALF_DICE_SIZE,  HALF_DICE_SIZE);

		glNormal3f( 1.0f, 0.0f, 0.0f );
		glTexCoord2f(0.5f,2*third);
		glVertex3f( HALF_DICE_SIZE, -HALF_DICE_SIZE, -HALF_DICE_SIZE);
		glTexCoord2f(1,2*third);
		glVertex3f( HALF_DICE_SIZE,  HALF_DICE_SIZE, -HALF_DICE_SIZE);
		glTexCoord2f(1,1);
		glVertex3f( HALF_DICE_SIZE,  HALF_DICE_SIZE,  HALF_DICE_SIZE);
		glTexCoord2f(0.5f,1);
		glVertex3f( HALF_DICE_SIZE, -HALF_DICE_SIZE,  HALF_DICE_SIZE);

		glNormal3f( -1.0f, 0.0f, 0.0f );
		glTexCoord2f(0,third);
		glVertex3f(-HALF_DICE_SIZE, -HALF_DICE_SIZE, -HALF_DICE_SIZE);
		glTexCoord2f(0.5f,third);
		glVertex3f(-HALF_DICE_SIZE, -HALF_DICE_SIZE,  HALF_DICE_SIZE);
		glTexCoord2f(0.5f,0);
		glVertex3f(-HALF_DICE_SIZE,  HALF_DICE_SIZE,  HALF_DICE_SIZE);
		glTexCoord2f(0,0);
		glVertex3f(-HALF_DICE_SIZE,  HALF_DICE_SIZE, -HALF_DICE_SIZE);
	glEnd();
	glPopMatrix();
}

/*
==============================================================================

  ogLogoScreen

==============================================================================
*/
/*
================
ogLogoScreen::ogLogoScreen
================
*/
ogLogoScreen::ogLogoScreen() {
	image = NULL;
	emitter = NULL;
}

/*
================
ogLogoScreen::Init
================
*/
void ogLogoScreen::Init( const og::Vec3 &logoCenter, int time, const og::Sound *sound ) {
	if ( timer.IsActive() )
		return;
	center = logoCenter;
	playTime = time;

	image = og::Image::Find("gfx/logodice.png");
	if ( sound ) {
		if ( !emitter )
			emitter = og::AS->CreateAudioEmitter( 1 );
		emitter->Play( 0, sound );
	}

	timer.Start();
}

/*
================
ogLogoScreen::Draw
================
*/
void ogLogoScreen::Draw( int frameTime ) {
	// Setup lighting
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	float lightValue = 1.0f;
	const int FADE_TIME = 1200;
	int rest = playTime - timer.Milliseconds() - 200;
	if ( rest < FADE_TIME )
		lightValue = og::Math::Clamp( static_cast<float>(rest) / static_cast<float>(FADE_TIME), 0.0f, 1.0f );
	
	GLfloat light_diffuse[] = { lightValue, lightValue, lightValue, 1.0};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

	// Draw logo
	static float scaleAngle = 0.0f;
	const int numChars = 6;
	const float dist = 4.0f;
	struct CharInfos {
		og::Angles	rotation;
		og::Angles	rotAdd;
		og::Vec3	posAdd;
	};
	static CharInfos charsInfos[numChars] = {
		{ og::Angles( 90, 100, -90 ),	og::Angles( -14, 12, 0 ),	og::Vec3( -0.1f, -0.8f, 0.1f ) },
		{ og::Angles( 170, 0, 100 ),	og::Angles( 15, 12, 0 ),	og::Vec3( 0.07f, 0.8f, -0.1f ) },
		{ og::Angles( -80, 75, 180 ),	og::Angles( -14, 13, 0 ),	og::Vec3( 0.03f, -0.6f, 0.3f ) },
		{ og::Angles( -80, -80, 5 ),	og::Angles( 14, -14, 0 ),	og::Vec3( -0.09f, 0.7f, 0.2f ) },
		{ og::Angles( 15, 5, -90 ),		og::Angles( -14, -15, 0 ),	og::Vec3( -0.1f, -0.5f, 0.5f ) },
		{ og::Angles( 80, 0, 100 ),		og::Angles( 13, -14, 0 ),	og::Vec3( -0.12f, 0.7f, 0.6f ) }
	};
	image->BindTexture();

	scaleAngle += 1.0f;
	if ( scaleAngle > 360.0f )
		scaleAngle -= 360.0f;
	float scale = og::Math::Sin( og::Math::Deg2Rad( scaleAngle ) );

	og::Vec3 position( center.x  - (numChars-1) * 0.5f * dist, center.y, center.z );
	for( int i=0; i<numChars; i++ ) {
		DrawDice( position + charsInfos[i].posAdd*scale, charsInfos[i].rotation + charsInfos[i].rotAdd*scale );
		position.x += dist;
	}
}


ogCreditsImage::ogCreditsImage( float x, float y, const char *filename ) : ogCreditsEntry( x, y ) {
	image = og::Image::Find( filename );
	width = static_cast<float>( image->GetWidth() );
	height = static_cast<float>( image->GetHeight() );
	xOffset -= width * 0.5f;
}

void ogCreditsImage::Draw( float y ) {
	image->BindTexture();

	float newY = y + yOffset;
	glBegin(GL_QUADS);
		glTexCoord2f( 0, 0 ); glVertex2f( xOffset, newY );
		glTexCoord2f( 0, 1 ); glVertex2f( xOffset, newY+height );
		glTexCoord2f( 1, 1 ); glVertex2f( xOffset+width, newY+height );
		glTexCoord2f( 1, 0 ); glVertex2f( xOffset+width, newY );
	glEnd();
}

ogCreditsText::ogCreditsText( float width, float y, const char *value, const og::Dict &fontSettings ) : ogCreditsEntry( 0, y ) {
	text.Init( fontSettings );
	text.value = value;
	text.SetWidth( width );
}

void ogCreditsText::Draw( float y ) {
	text.Draw( xOffset, y + yOffset );
}

ogCreditsScreen::ogCreditsScreen() {
	isActive = false;
	isDone = false;
}

ogCreditsScreen::~ogCreditsScreen() {
	int num = entries.Num();
	for( int i=0; i<num; i++ )
		delete entries[i];
}

void ogCreditsScreen::AddEntry( ogCreditsEntry *entry ) {
	entries.Append( entry );
}

void ogCreditsScreen::Init( float height, float speed, const og::Sound *sound ) {
	scrollSpeed = speed;
	scrollHeight = height;
	isActive = true;
	isDone = false;
	logo.Init( og::Vec3( 0, 8, -4 ), 1000 * static_cast<int>(height/speed) + 3000, sound );
}

void ogCreditsScreen::Draw2D( int frameTime ) {
	static float yOffset = 0.0f;
	int num = entries.Num();
	for( int i=0; i<num; i++ )
		entries[i]->Draw( yOffset );

	yOffset -= scrollSpeed * static_cast<float>(frameTime)*0.001f;
	if ( -yOffset > scrollHeight )
		isDone = true;
}

void ogCreditsScreen::Draw3D( int frameTime ) {
	glRotatef( 20, 1, 0, 0 );
	logo.Draw( frameTime );
}
