/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Audio Demonstration
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

#include <stdio.h>
#include <math.h>
#include <string.h>

#include <og/Gloot/glmin.h>
#include <og/Image.h>
#include <og/FileSystem.h>

#include "main.h"

const int WINDOW_SIZE = 400;
const og::Vec3 windowCenter(WINDOW_SIZE/2, WINDOW_SIZE/2, 0.0f);

/*
==============================================================================

  ogDemoWindow

==============================================================================
*/
/*
================
ogDemoWindow::ogDemoWindow
================
*/
ogDemoWindow::ogDemoWindow() {
	helpText[0] = NULL;
	helpText[1] = NULL;
	window = NULL;
	mouseDown = false;
	manualPosition = false;
	soundPos = windowCenter;
	soundPosOld = soundPos;
}

/*
================
ogDemoWindow::SetupWindow
================
*/
bool ogDemoWindow::SetupWindow( int x, int y, byte multiSamples, const char *title, int *contextAttribs ) {
	og::WindowConfig windowCfg = {
		sizeof(og::WindowConfig),				// size of the struct
		title,									// window title
		og::WF_DOUBLEBUFFER|og::WF_NORESIZE,	// flags
		x, y, WINDOW_SIZE, WINDOW_SIZE,			// rect
		multiSamples,							// multisamples

		// Pixel Format:
		{8, 8, 8, 0},							// rgba bits
		8, 8,									// depth & stencil bits
		{0, 0, 0, 0},							// rgba accum bits
		0,										// aux buffers

		this,									// Window Listener
		contextAttribs,							// OpenGL attributes list
		NULL									// Shared Context Window
	};

	// Open OpenGL window
	window = og::Gloot::NewWindow( &windowCfg );
	if( !window )
		return false;
	
	// Enable vertical sync (on cards that support it)
	window->MakeCurrent();
	window->SetSwapinterval( true );
	return true;
}

/*
================
ogDemoWindow::Init
================
*/
bool ogDemoWindow::Init( void ) {
	int contextAttribs[] = {
		og::glContext::MAJOR_VERSION, 2,
		og::glContext::MINOR_VERSION, 1,
		0
	};
	if ( !SetupWindow( 0, 0, 0, APP_TITLE, contextAttribs ) )
		return false;

	og::StringList deviceList;
	og::AudioSystem::GetDeviceList( deviceList );

	if ( !og::AudioSystem::Init( og::FS, "sounds/default.wav", deviceList.IsEmpty() ? NULL : deviceList[0].c_str() ) )
		return false;
	if ( !soundManager.Init( "sounds/default.wav" ) )
		return false;
	if ( !og::Image::Init( og::FS, "gfx/default.tga", window->GetProcAddress("glCompressedTexImage2DARB") ) )
		return false;
	if ( !og::Font::Init( og::FS, "Arial" ) )
		return false;

	og::Vec3 forward(0, -1, 0);
	og::Vec3 up(0, 0, -1);
	og::AS->SetListener( windowCenter, forward, up, og::c_vec3::origin );

	audioEmitter = og::AS->CreateEmitter( 1 );
	audioEmitter->Play( 0, soundManager.Find( "helicopter" ) );
	UpdateSound();

	helpText[0] = new ogFontText(18, og::Font::LEFT);
	helpText[0]->value = "Right Mouse: Toggle mode ( auto )";
	helpText[1] = new ogFontText(18, og::Font::LEFT);
	helpText[1]->value = "Left Mouse: Move sound position";
	return true;
}

/*
================
ogDemoWindow::UpdateSound
================
*/
void ogDemoWindow::UpdateSound( void ) {
	int frameTime;
	if ( !frameTimer.IsActive() ) {
		frameTime = 0;
		frameTimer.Start();
	} else {
		frameTimer.Stop();
		frameTime = frameTimer.MilliSeconds();
		frameTimer.Start();
	}
	float timeScale = static_cast<float>(frameTime)*0.001f;
	if ( !manualPosition ) {
		soundAngle.yaw += 80.0f * timeScale;
		soundPos = windowCenter + soundAngle.ToForward() * 80.0f;
	} else if ( mouseDown ) {
		int x, y, wx, wy;
		og::Gloot::GetMousePos( &x, &y );
		window->GetPos(&wx, &wy);
		soundPos.Set( x-wx, y-wy, 0.0f );
	}
	audioEmitter->SetPosition( soundPos );
	if ( frameTime > 0 )
		audioEmitter->SetVelocity( (soundPos-soundPosOld)/frameTime );

	soundPosOld = soundPos;
}

/*
================
ogDemoWindow::Draw
================
*/
void ogDemoWindow::Draw( void ) {
	if ( !IsOpen() )
		return;

	window->MakeCurrent();

	// Clear color buffer
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport
	glViewport( 0, 0, WINDOW_SIZE, WINDOW_SIZE );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, WINDOW_SIZE, WINDOW_SIZE, 0, -100, 100 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	UpdateSound();

	glPointSize(10.0f);
	glEnable(GL_POINT_SMOOTH);
	glBegin( GL_POINTS );
		glVertex2f( windowCenter.x, windowCenter.y );
		glColor3f( 0.5f, 0.5f, 0.0f );
		glVertex2f( soundPos.x, soundPos.y );
	glEnd();

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_TEXTURE_2D );
	helpText[0]->Draw( 5.0f, 5.0f );
	helpText[1]->Draw( 5.0f, 25.0f );
	glDisable( GL_TEXTURE_2D );

	// Swap buffers
	window->SwapBuffers();
}

/*
================
ogDemoWindow::OnMouseButton
================
*/
void ogDemoWindow::OnMouseButton( int button, bool down ) {
	if ( button == og::Mouse::ButtonLeft )
		mouseDown = down;
	else if ( button == og::Mouse::ButtonRight && down ) {
		manualPosition = !manualPosition;
		helpText[0]->value = og::Format( "Right Mouse: Toggle mode ( $* )" ) << (manualPosition ? "manual" : "auto");
		if ( frameTimer.IsActive() )
			frameTimer.Stop();
	}
}

/*
================
ogDemoWindow::OnClose
================
*/
bool ogDemoWindow::OnClose( bool forced ) {
	if ( !forced )
		return true;

	og::SafeDelete( helpText[0] );
	og::SafeDelete( helpText[1] );

	og::Font::Shutdown();
	og::Image::Shutdown();
	og::AudioSystem::Shutdown();

	audioEmitter = NULL;
	window = NULL;
	return true;
}

/*
================
og::User::Main
================
*/
int og::User::Main( int argc, char *argv[] ) {
	if ( !og::FileSystem::Init( ".gpk", ".", ".", "base" ) )
		return 0;

	if ( !og::FS->FileExists("sounds/default.wav") ) {
		og::FileSystem::Shutdown();
		printf( "You might wanna check your working directory ?" );
		return 0;
	}
	ogDemoWindow myWindow;
	myWindow.Init();

	while( myWindow.IsOpen() ) {
		og::Gloot::Synchronize();
		myWindow.Draw();
		og::Sleep(16);
	}
	og::FileSystem::Shutdown();
	return 0;
}

/*
================
og::User::IsPakFileAllowed
================
*/
bool og::User::IsPakFileAllowed( og::PakFile *pakFile ) {
	return true;
}
