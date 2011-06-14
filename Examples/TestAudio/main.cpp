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
	window = NULL;
	mouseDown = false;
	manualPosition = false;
	showHelp = false;
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

int currentReverb = 0;
og::AudioEffect *effect = NULL;
og::Angles viewAngles(0, -90, 0);

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
	if ( !soundManager.Init( "sounds/default.wav" ) || !soundManager.InitReverbs() )
		return false;
	if ( !og::Image::Init( og::FS, "gfx/default.tga", window->GetProcAddress("glCompressedTexImage2DARB") ) )
		return false;
	if ( !og::Font::Init( og::FS, "Arial" ) )
		return false;

	effect = og::AS->CreateEffect();
	effect->Load( soundManager.GetReverb(currentReverb) );

	viewAngles = og::Vec3(0, -1, 0).ToAngles();
	og::Vec3 forward = viewAngles.ToForward();
	og::Vec3 up(0, 0, -1);
	og::AS->SetListener( windowCenter, forward, up, og::c_vec3::origin );
	og::AS->SetUnitLength(0.125f); // just some wild number: 8 units per meter

	audioEmitter = og::AS->CreateEmitter( 1 );
	audioEmitter->SetEffect( effect );
	audioEmitter->Play( 0, soundManager.Find( "helicopter" ) );
	UpdateSound();

	helpLines.AddLine("^3H^0: Show Help");
	helpLines.AddLine("^3Right Mouse^0: Toggle position mode");
	helpLines.AddLine("^3Left Mouse^0: Move sound (manual mode)");
	helpLines.AddLine("^3Middle Mouse^0: Look at");
	helpLines.AddLine("^3E^0: Toggle reverb");
	helpLines.AddLine("^3+^0: Next reverb");
	helpLines.AddLine("^3-^0: Previous reverb");

	infoLines.AddLine("Position mode: ^2Auto");
	infoLines.AddLine( og::Format("Reverb (^3$*^0/^3$*^0): ^2$*") << (currentReverb+1) << soundManager.GetNumReverbs() << soundManager.GetReverbName( currentReverb ) );
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
		soundAngle.yaw += 30.0f * timeScale;
		soundPos = windowCenter + soundAngle.ToForward() * 80.0f;
	} else if ( mouseDown ) {
		int x, y, wx, wy;
		og::Gloot::GetMousePos( &x, &y );
		window->GetPos(&wx, &wy);
		soundPos.Set( static_cast<float>(x-wx), static_cast<float>(y-wy), 0.0f );
	}
	audioEmitter->SetPosition( soundPos );
	if ( frameTime > 0 )
		audioEmitter->SetVelocity( (soundPos-soundPosOld)/static_cast<float>(frameTime) );

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

	// the view direction
	viewAngles.yaw = viewAngles.yaw - 45.0f;
	og::Vec3 end1 = windowCenter + viewAngles.ToForward() * 50.0f;
	viewAngles.yaw = viewAngles.yaw + 90.0f;
	og::Vec3 end2 = windowCenter + viewAngles.ToForward() * 50.0f;
	viewAngles.yaw = viewAngles.yaw - 45.0f;
	glLineWidth( 3.0f );
	glColor3f( 0.0f, 0.0f, 1.0f );
	glEnable(GL_LINE_SMOOTH);
	glBegin( GL_LINES );
		glVertex2f( windowCenter.x, windowCenter.y );
		glVertex2f( end1.x, end1.y );
		glVertex2f( windowCenter.x, windowCenter.y );
		glVertex2f( end2.x, end2.y );
	glEnd();

	// the center and sound position
	glPointSize(10.0f);
	glEnable(GL_POINT_SMOOTH);
	glBegin( GL_POINTS );
		glColor3f( 1.0f, 1.0f, 1.0f );
		glVertex2f( windowCenter.x, windowCenter.y );
		glColor3f( 0.5f, 0.5f, 0.0f );
		glVertex2f( soundPos.x, soundPos.y );
	glEnd();

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_TEXTURE_2D );

	helpLines.Draw( 5, 5, false, showHelp ? -1 : 1 );
	infoLines.Draw( 5, WINDOW_SIZE-25, true, -1 );

	glDisable( GL_TEXTURE_2D );

	// Swap buffers
	window->SwapBuffers();
}

/*
================
ogDemoWindow::OnCharEvent
================
*/
#include <ctype.h>
void ogDemoWindow::OnCharEvent( int ch ) {
	static bool reverbEnabled = true;
	switch( tolower(ch) ) {
		case 'e':
			reverbEnabled = !reverbEnabled;
			audioEmitter->SetEffect( reverbEnabled ? effect : NULL  );
			if ( reverbEnabled )
				infoLines.SetLine( 1, og::Format("Reverb (^3$*^0/^3$*^0): ^2$*") << (currentReverb+1) << soundManager.GetNumReverbs() << soundManager.GetReverbName( currentReverb ) );
			else
				infoLines.SetLine( 1, "Reverb: ^2Disabled" );
			break;
		case '+':
			currentReverb++;
			if ( currentReverb >= soundManager.GetNumReverbs() )
				currentReverb = 0;
			effect->Load( soundManager.GetReverb( currentReverb ) );
			infoLines.SetLine( 1, og::Format("Reverb (^3$*^0/^3$*^0): ^2$*") << (currentReverb+1) << soundManager.GetNumReverbs() << soundManager.GetReverbName( currentReverb ) );
			break;
		case '-':
			currentReverb--;
			if ( currentReverb < 0 )
				currentReverb = soundManager.GetNumReverbs()-1;
			effect->Load( soundManager.GetReverb( currentReverb ) );
			infoLines.SetLine( 1, og::Format("Reverb (^3$*^0/^3$*^0): ^2$*") << (currentReverb+1) << soundManager.GetNumReverbs() << soundManager.GetReverbName( currentReverb ) );
			break;
		case 'h':
			showHelp = !showHelp;
			helpLines.SetLine( 0, showHelp ? "^3H^0: Hide Help" : "^3H^0: Show Help" );
			break;
	}
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
		infoLines.SetLine( 0, og::Format( "Position mode: ^2$*" ) << (manualPosition ? "Manual" : "Auto") );

		if ( frameTimer.IsActive() )
			frameTimer.Stop();
	}
	else if ( button == og::Mouse::ButtonMiddle ) {
		int x, y, wx, wy;
		og::Gloot::GetMousePos( &x, &y );
		window->GetPos(&wx, &wy);
		og::Vec3 dir = og::Vec3( static_cast<float>(x-wx), static_cast<float>(y-wy), 0 ) - windowCenter;
		viewAngles = dir.ToAngles();
		og::Vec3 forward = viewAngles.ToForward();
		og::Vec3 up(0, 0, -1);
		og::AS->SetListener( windowCenter, forward, up, og::c_vec3::origin );
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
	if ( !og::FileSystem::SimpleInit( ".gpk", "base", ".", "." ) )
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
