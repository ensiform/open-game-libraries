/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Model loading demo
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

#include <og/Console/Console.h>
#include<gl/glew.h>
#include <stdio.h>
#include <og/Image/Image.h>
#include "main.h"
#include "../Shared/Shared.h"

#pragma comment(lib, "../../Thirdparty/gl/glew32.lib")

#define APP_TITLE "Model Loader"
ogDemoWindow demoWindow;
bool doQuit = false;

bool og::CVar::linkedToEngine = false;
og::CVar *og::CVar::firstLink = NULL;

og::CVar g_fov( "g_fov", "65", og::CVAR_GAME|og::CVAR_ARCHIVE|og::CVAR_FLOAT, "Diagonal Field Of View", 20.0f, 160.0f );
const char *validMinFilters[] = { "GL_NEAREST", "GL_LINEAR", "GL_NEAREST_MIPMAP_NEAREST", "GL_LINEAR_MIPMAP_NEAREST", "GL_NEAREST_MIPMAP_LINEAR", "GL_LINEAR_MIPMAP_LINEAR", NULL };
const char *validMagFilters[] = { "GL_NEAREST", "GL_LINEAR", NULL };
og::CVar img_minFilter( "img_minFilter", "GL_LINEAR_MIPMAP_NEAREST", og::CVAR_GAME|og::CVAR_ARCHIVE|og::CVAR_ENUM, "Min Texture Filter", validMinFilters );
og::CVar img_magFilter( "img_magFilter", "GL_LINEAR", og::CVAR_GAME|og::CVAR_ARCHIVE|og::CVAR_ENUM, "Mag Texture Filter", validMagFilters );

void FilterChangedCB( void ) {
	og::Image::SetFilters( img_minFilter.GetInt(), img_magFilter.GetInt() );
}

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
	lastTime = 0;
	camHeight = 2.0f;
	camRange = 5.0f;
	drag = 0;
	dragPoint[0] = 0;
	dragPoint[1] = 0;

	shiftDown = false;
	ctrlDown = false;
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

	og::WindowConfig windowCfg = {
		sizeof(og::WindowConfig),				// size of the struct
		APP_TITLE,								// window title
		og::WF_DOUBLEBUFFER,					// flags
		0, 0, 800, 600,							// rect
		1,										// multisamples

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

	GLenum err = glewInit();

	img_minFilter.SetModifiedCallback( FilterChangedCB );
	img_magFilter.SetModifiedCallback( FilterChangedCB );

	if ( !og::Image::Init( og::FS, "gfx/default.tga", window->GetProcAddress("glCompressedTexImage2DARB") ) )
		return false;
	og::Image::SetFilters( img_minFilter.GetInt(), img_magFilter.GetInt() );

	if ( !model.Load("models/TheVampSuzanne/TheVampSuzanne.md3") )
		return false;

	glEnable( GL_MULTISAMPLE );
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable(GL_POINT_SMOOTH);
	
	if ( glPointParameterfvEXT ) {
		float linear[] = { 0.0f, 0.12f, 0.0f };
		glPointParameterfvEXT(GL_DISTANCE_ATTENUATION_EXT, linear);
		glPointParameterfEXT(GL_POINT_FADE_THRESHOLD_SIZE_EXT, 2.0f);
	}

	// Setup some lighting
	glShadeModel( GL_SMOOTH );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	GLfloat LightAmbient[]= { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv( GL_LIGHT0, GL_AMBIENT, LightAmbient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse );

	// VBO:
	glEnable(GL_VERTEX_ARRAY);
	glEnable(GL_NORMAL_ARRAY);
	glEnable(GL_TEXTURE_COORD_ARRAY);
	return true;
}

/*
================
ogDemoWindow::DrawLineMap
================
*/
void ogDemoWindow::DrawLineMap( void ) {
	glColor3f( 0.0f, 0.0f, 0.0f );
	glBegin( GL_LINES );
	for( int i = -10 ; i <= 10 ; ++i ) {
		glVertex3i( 10, i, 0 );
		glVertex3i( -10, i, 0 );
		glVertex3i( i, 10, 0 );
		glVertex3i( i, -10, 0 );
	}
	glEnd();

	glBegin( GL_POINTS );
		glVertex3i( 0, 0, 0 );
	glEnd();
}

/*
================
ogDemoWindow::ModelToolDrag
================
*/
void ogDemoWindow::ModelToolDrag( float length, int action, int axis ) {
	if ( action == 3 ) {
		length /= 2;
		// Rotation
		og::Angles angles = model.GetAngles();
		angles[axis] += length;
		model.SetAngles( angles );
	} else if ( action == 2 ) {
		length /= -60;
		// Size scaling
		// axis doesn't matter, always sizing all axes.
		og::Vec3 scale = model.GetScale();
		scale.x += length;
		if ( scale.x < 0.01f )
			scale.x = 0.01f;
		scale.y = scale.z = scale.x;
		
		model.SetScale( scale );
	} else {
		length /= 32;
		// Translation
		og::Vec3 offset = model.GetOffset();
		offset[axis] += length;
		model.SetOffset( offset );
	}
}

/*
================
ogDemoWindow::DoDrag
================
*/
void ogDemoWindow::DoDrag( void ) {
	bool leftDown = mouseButtons[og::Mouse::ButtonLeft];
	bool rightDown = mouseButtons[og::Mouse::ButtonRight];
	bool midDown = mouseButtons[og::Mouse::ButtonMiddle];

	if ( !midDown && !leftDown && !rightDown ) {
		drag = 0;
		return;
	}

	int pt[2];
	og::Gloot::GetMousePos( &pt[0], &pt[1] );
	int diff[2] = { pt[0] -dragPoint[0], pt[1] -dragPoint[1]};

	if ( ctrlDown || shiftDown )
		ModelToolDrag( (float)diff[1], (ctrlDown + 2*shiftDown), (midDown ? 3 : leftDown + 2*rightDown)-1 );
	else if ( midDown || (leftDown && rightDown) )
		camHeight += diff[1] * 0.02f;
	else if ( leftDown ) {
		camAngles.pitch += diff[1];
		camAngles.yaw -= diff[0];
		if ( camAngles.pitch > 89.0f )
			camAngles.pitch = 89.0f;
		else if ( camAngles.pitch < -89.0f )
			camAngles.pitch = -89.0f;
	} else if ( rightDown ) {
		camRange -= diff[1] * 0.02f;
		if ( camRange < 0.0f )
			camRange = 0.0f;
	} else {
		drag = 0;
		return;
	}
	dragPoint[0] = pt[0];
	dragPoint[1] = pt[1];
}

/*
================
ogDemoWindow::Draw
================
*/
void ogDemoWindow::Draw( void ) {
	if ( !IsOpen() )
		return;
	if ( drag )
		DoDrag();

	int frameTime;
	if ( lastTime == 0 )
		frameTime = 0;
	else
		frameTime = og::SysInfo::GetTime() - lastTime;
	lastTime = og::SysInfo::GetTime();

	window->MakeCurrent();

	// Get window size (may be different than the requested size)
	int width, height;
	window->GetSize( &width, &height );
	height = height > 0 ? height : 1;

	// Clear color buffer
	glClearColor( 0.6f, 0.6f, 0.6f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	// Select and setup the projection matrix
	float perspective[16];
	og::Gloot::CreatePerspectiveMatrix( g_fov.GetFloat(), (float)width/(float)height, 0.1f, 2048.0f, perspective );
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( perspective );

	glViewport(0, 0, width, height );
	glScissor(0, 0, width, height );

	glEnable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE );
	glClearDepth( 1.0f );

	// Reset GL display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// find camera position and directional vectors
	og::Vec3 camPos( 0.0f, 0.0f, camHeight );
	camPos -= camRange * camAngles.ToForward();
	og::Vec3 forward, right, up;
	camAngles.ToVectors( &forward, &right, &up );

	// Update light position
	GLfloat LightPosition[4] = { camPos.x, camPos.y, camPos.z, 1.0f };
	glLightfv( GL_LIGHT0, GL_POSITION, LightPosition );

	// Select and setup the modelview matrix
	float modelview[16];
	og::Gloot::CreateCameraMatrix( &camPos.x, &forward.x, &right.x, &up.x, modelview );
	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( modelview );

	glPushMatrix();

	// Draw the model
	glEnable(GL_TEXTURE_2D);
	glEnable( GL_LIGHTING );
	model.Draw();
	glDisable( GL_LIGHTING );
	glDisable( GL_TEXTURE_2D );

	DrawLineMap();

	// Draw camera focus point
	glDisable( GL_DEPTH_TEST );
	glPointSize(10.0f);
	glBegin( GL_POINTS );
		glColor4f( 1.0f, 1.0f, 0.0f, 0.5f );
		glVertex3f( 0.0f, 0.0f, camHeight );
	glEnd();
	glEnable( GL_DEPTH_TEST );

	glPopMatrix();

	// Swap buffers
	window->SwapBuffers();
}

/*
================
ogDemoWindow::OnClose
================
*/
bool ogDemoWindow::OnClose( bool forced ) {
	if ( !forced )
		return true;

	og::Image::Shutdown();

	return true;
}

/*
================
ogDemoWindow::OnFocusChange
================
*/
void ogDemoWindow::OnFocusChange( bool hasFocus ) {
	shiftDown = false;
	ctrlDown = false;
}

/*
================
ogDemoWindow::OnKeyEvent
================
*/
void ogDemoWindow::OnKeyEvent( int key, bool down ) {
	switch( key ) {
		case og::Key::LSHIFT:
		case og::Key::RSHIFT:
			shiftDown = down;
			break;
		case og::Key::LCTRL:
		case og::Key::RCTRL:
			ctrlDown = down;
			break;
	}

	switch( key ) {
		case og::Key::ESC:
			doQuit = true;
			break;
	}
}

/*
================
ogDemoWindow::OnMouseButton
================
*/
void ogDemoWindow::OnMouseButton( int button, bool down ) {
	if ( button <= og::Mouse::Button3 && mouseButtons[button] != down ) {
		if ( down ) {
			if ( demoWindow.drag == 0 )
				og::Gloot::GetMousePos( &dragPoint[0], &dragPoint[1] );
			drag++;
		} else {
			if ( drag > 0 )
				drag--;
		}
		mouseButtons[button] = down;
	}
}

/*
================
InitConsole
================
*/
bool InitConsole( void ) {
	og::ConsoleParams_t conParms;
	conParms.appTitle = "Open Game";

#ifdef OG_WIN32
	conParms.width = 600;
	conParms.height = 380;
	conParms.minWidth = 400;
	conParms.minHeight = 250;
	conParms.fadeTime = 500;

	conParms.logBgImage = "Console.bmp";
#define SETCOLORPARM(n,r,g,b) conParms.n[0]=r;conParms.n[1]=g;conParms.n[2]=b;
	SETCOLORPARM( colorLogBg, 0x50, 0x50, 0x50 );
	SETCOLORPARM( colorLogText, 0xff, 0xff, 0xff );
	SETCOLORPARM( colorErrorBg, 0x80, 0x80, 0x80 );
	SETCOLORPARM( colorErrorBgFlash, 0xff, 0x00, 0x00 );
	SETCOLORPARM( colorErrorText, 0xff, 0xff, 0xff );
	SETCOLORPARM( colorErrorTextFlash, 0xff, 0xff, 0xff );
#undef SETCOLORPARM
#endif

	return og::Console::Init( &conParms );
}

/*
================
og::User::Main
================
*/
int og::User::Main( int argc, char *argv[] ) {
	if ( !og::Shared::Init() )
		return 0;

	if ( InitConsole() ) {
		if ( og::FileSystem::Init( ".gpk", ".", ".", "base" ) ) {
			if ( !og::FS->FileExists("gfx/default.tga") ) {
				og::Console::FatalError( "You might wanna check your working directory ?" );
				//! @todo	wait for console close
				og::FileSystem::Shutdown();
				return 0;
			}

			Model::SetFileSystem( og::FS );

			Gloot::DisableSystemKeys( true );
			if( !demoWindow.Init() ) {
				Model::SetFileSystem( NULL );
				og::FileSystem::Shutdown();
				return 0;
			}

			og::Timer fpsTimer;
			fpsTimer.Start();
			uInt count = 0;
			while( demoWindow.IsOpen() && !doQuit ) {
				og::Gloot::Synchronize();
				og::Console::Synchronize();
				demoWindow.Draw();
				
				if ( fpsTimer.MicroSeconds() < 200000 )
					count++;
				else {
					fpsTimer.Stop();
					float fps = static_cast<float>(count * 1000000.0/static_cast<double>(fpsTimer.MicroSeconds()));
					demoWindow.window->SetTitle( og::Format(APP_TITLE" | FPS: $*" ) << SetPrecision(2) << fps );
					count = 0;
					fpsTimer.Start();
				}
				og::Shared::Sleep( 10 );
			}

			Gloot::DisableSystemKeys( false );
			Model::SetFileSystem( NULL );
			og::FileSystem::Shutdown();
		}
		og::Console::Shutdown();
	}
    return 0;
}

/*
================
og::User::IsPakFileAllowed
================
*/
bool og::User::IsPakFileAllowed( og::PakFile *pakFile ) {
	og::Console::Print( og::Format("Adding file '$*'\n" ) << pakFile->GetFilename() );
	return true;
}

/*
================
og::User::Error
================
*/
void og::User::Error( og::ErrorId id, const char *msg, const char *param ) {
	// Todo: Throw an exception on the id's you think are important.

	og::String result;
	getErrorString( id, msg, param, result );
	og::Console::FatalError( og::Format( "Error: $*\n" ) << result );
}

/*
================
og::User::Warning
================
*/
void og::User::Warning( const char *msg ) {
	og::Console::Print( og::Format( "$*\n" ) << msg );
}

/*
================
og::User::AssertFailed
================
*/
void og::User::AssertFailed( const char *code, const char *function ) {
	og::Console::FatalError( og::Format( "Assert($*) failed in $*!\n" ) << code << function );
}

/*
================
og::User::OnConsoleMenuOpen
================
*/
void og::User::OnConsoleMenuOpen( void( *callback )( const char *name, const char *command ) ) {
	callback( "List All Commands", "listcmds" );
	callback( "List All CVars", "listcvars" );
}

/*
================
og::User::OnForceExit
================
*/
void og::User::OnForceExit( void ) {
	doQuit = true;
}

// Keep in sync with cmdFlags_t
const char *cmdFlagNames[] = {
	"INIT",
	"CHEAT",
	"DEVELOPER",
	"ENGINE",
	"RENDERER",
	"SOUND",
	"INPUT",
	"NETWORK",
	"GAME",

	NULL
};

// Keep in sync with cvarFlags_t
const char *cvarFlagNames[] = {
	"BOOL",
	"INTEGER",
	"FLOAT",
	"ENUM",
	"UNLINKED",
	"MODIFIED",
	"ARCHIVE",
	"ROM",
	"INIT",
	"CHEAT",
	"DEVELOPER",
	"MAPRESTART",
	"VIDRESTART",
	"SNDRESTART",
	"ENGINE",
	"RENDERER",
	"SOUND",
	"INPUT",
	"NETWORK",
	"GAME",
	"NETSYNC",
	"BROWSERFEED",
	"SERVERINFO",
	"CLIENT",

	NULL
};
