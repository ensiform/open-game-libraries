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

#include <stdio.h>
#include "main.h"

#if 0 // set to 1 to see image preloading bar
const int INTRO_TIME = 100;
#else
const int INTRO_TIME = 4000;
#endif

ogDemoWindow demoWindow;

ogBind bindAttack("attack");
ogBind bindAccelerate("accelerate");
ogBind bindTurn("turn");

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
	fadeIn = 1.1f;
	skipFrame = false;
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
		og::WF_DOUBLEBUFFER|og::WF_NORESIZE,	// flags
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,		// rect
		4,										// multisamples

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

	og::StringList deviceList;
	og::AudioSystem::GetDeviceList( deviceList );

	if ( !og::AudioSystem::Init( og::FS, "sounds/default.wav", deviceList.IsEmpty() ? NULL : deviceList[0].c_str() ) )
		return false;
	if ( !game.soundManager.Init( "sounds/default.wav" ) || !game.soundManager.SetupEmitters( MAX_SOUNDS ) )
		return false;
	if ( !og::Image::Init( og::FS, "gfx/default.tga", window->GetProcAddress("glCompressedTexImage2DARB") ) )
		return false;
	if ( !og::Font::Init( og::FS, "Arial" ) )
		return false;

	InitInput();

	bgLoader.Start();
	bgLoader.SetNumWorkers(4);
	const char *extensions[] = { ".tga", ".png", ".jpg", ".dds", NULL };
	for( int i=0; extensions[i] != NULL; i++ ) {
		og::FileList *files = og::FS->GetFileList("", extensions[i]);
		if ( files ) {
			for( int j=0; j<files->Num(); j++ )
				bgLoader.AddTask( og::Image::PreloadImage( files->GetName(j) ) );
			og::FS->FreeFileList( files );
		}
	}

	// Register bindings and load config
	RegisterBind( &bindAttack );
	RegisterBind( &bindAccelerate );
	RegisterBind( &bindTurn );
	LoadConfig( "input.xdecl" );

	//window->SetGamma(0.75f);
	return true;
}

/*
================
ogDemoWindow::Draw
================
*/
void ogDemoWindow::Draw( void ) {
	if ( !IsOpen() )
		return;

	int frameTime;
	if ( !frameTimer.IsActive() ) {
		frameTime = 0;
		frameTimer.Start();
	} else {
		frameTimer.Stop();
		frameTime = frameTimer.MilliSeconds();
		frameTimer.Start();
	}

	OnFrameStart();

	window->MakeCurrent();

	// Clear color buffer
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get window size (may be different than the requested size)
	window->GetSize( &width, &height );
	height = height > 0 ? height : 1;

	// finish preloading if not done yet.
	if ( !bgLoader.IsDone() ) {
		bgLoader.Synchronize();
		if ( bgLoader.IsDone() ) {
			bgLoader.Stop();
			game.Init();
		}
	}

	if ( !intro.IsDone() )
		DrawIntro( frameTime );
	// This shows a loading bar in case not all images have been loaded within the intro screen
	else if ( !bgLoader.IsDone() )
		DrawLoading( bgLoader.GetProgress() );
	else {
		if ( fadeIn == 1.0f ) {
			game.StartAmbience();
			skipFrame = true;
		} else if ( outro.IsDone() ) {
			Quit();
			return;
		}
		if ( outro.IsActive() )
			DrawOutro( frameTime );
		else
			DrawGame( frameTime );
	}

	// Swap buffers
	window->SwapBuffers();
	OnFrameEnd();
}

/*
================
ogDemoWindow::DrawLoading
================
*/
void ogDemoWindow::DrawLoading( float pct ) {
	SetupOrtho();

	float barHeight = 50.0f;
	float y = height/2.0f - barHeight/2.0f;

	glDisable( GL_TEXTURE_2D );
	glColor3f( 1.0f, 1.0f, 1.0f );
	glBegin(GL_QUADS);
		glTexCoord2f( 0, 0 ); glVertex2f( 0, y );
		glTexCoord2f( 0, 1 ); glVertex2f( 0, y+barHeight );
		glTexCoord2f( 1, 1 ); glVertex2f( width*pct, y+barHeight );
		glTexCoord2f( 1, 0 ); glVertex2f( width*pct, y );
	glEnd();
	glEnable( GL_TEXTURE_2D );
}

/*
================
ogDemoWindow::DrawIntro
================
*/
void ogDemoWindow::DrawIntro( int frameTime ) {
	if ( fadeIn == 1.1f ) {
		intro.Init( og::c_vec3::origin, INTRO_TIME, game.soundManager.Find("intro_track") );
		fadeIn = 1.0f;
	}
	SetupPerspective();
	intro.Draw( frameTime );
}

/*
================
ogDemoWindow::DrawOutro
================
*/
void ogDemoWindow::DrawOutro( int frameTime ) {
	SetupOrtho();

	float size = static_cast<float>(WINDOW_WIDTH);
	float diff = (size - WINDOW_HEIGHT) * 0.5f;
	DrawImage( game.backImage, 0, -diff, size, size );
	outro.Draw2D( frameTime );
	DrawImage( game.backImage2, 0, -diff, size, size );

	SetupPerspective();
	outro.Draw3D( frameTime );
}

/*
================
ogDemoWindow::DrawGame
================
*/
void ogDemoWindow::DrawGame( int frameTime ) {
	// Setup Orthographic view
	SetupOrtho();

	// Think Game
	if ( fadeIn <= 0.0f ) {
		// Skip the first frame, as it has a high frameTime
		if ( skipFrame )
			skipFrame = false;
		else
			game.Think( frameTime );
	}

	// Draw Game
	game.Draw( fadeIn );

	if ( fadeIn > 0.0f ) {
		fadeIn -= 0.05f;
		if ( fadeIn < 0.0f )
			menu.Init();
	}
	else if ( menu.IsVisible() )
		menu.Draw();
}

/*
================
ogDemoWindow::SetupOrtho
================
*/
void ogDemoWindow::SetupOrtho( void ) {
	float wantedAspectRatio = static_cast<float>(WINDOW_WIDTH)/static_cast<float>(WINDOW_HEIGHT);
	float windowAspectRatio = static_cast<float>(width)/static_cast<float>(height);
	int	clip[4] = { 0, 0, width, height };
	float ortho[4] = { 0, WINDOW_HEIGHT, WINDOW_WIDTH, 0 };
	if ( wantedAspectRatio > windowAspectRatio ) {
		float h = width / wantedAspectRatio;
		float f = 1.0f - (h / height);

		h = (WINDOW_HEIGHT * f) * 0.5f ;
		ortho[1] += h;
		ortho[3] -= h;
		h = (height * f);
		clip[1] += og::Math::FtoiFast(h * 0.5f);
		clip[3] -= og::Math::FtoiFast(h);
	} else {
		float w = height * wantedAspectRatio;
		float f = 1.0f - (w / width);

		w = (WINDOW_WIDTH * f) * 0.5f ;
		ortho[0] += w;
		ortho[2] -= w;
		w = (height * f);
		clip[0] += og::Math::FtoiFast(w * 0.5f);
		clip[2] -= og::Math::FtoiFast(w);
	}
	og::Font::SetScreenScale( static_cast<float>(height)/static_cast<float>(WINDOW_HEIGHT) );
	// Set viewport
	glViewport( 0, 0, width, height );

	glDisable( GL_LIGHTING );
	glEnable(GL_SCISSOR_TEST);
	glScissor(clip[0], clip[1], clip[2], clip[3]);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( ortho[0], ortho[2], ortho[1], ortho[3], -100, 100 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	glEnable( GL_TEXTURE_2D );
	glDisable( GL_DEPTH_TEST );
	glDisable(GL_CULL_FACE);

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/*
================
ogDemoWindow::SetupPerspective
================
*/
void ogDemoWindow::SetupPerspective( void ) {
	og::Vec3 origin( 0, 0, 20 );
	og::Vec3 forward( 0, 0, -1 );
	og::Vec3 right( 1, 0, 0 );
	og::Vec3 up( 0, 1, 0 );

	float modelview[16];
	float perspective[16];
	og::Gloot::CreatePerspectiveMatrix( 65.0f, (float)width/(float)height, 0.1f, 2048.0f, perspective );
	og::Gloot::CreateCameraMatrix( &origin.x, &forward.x, &right.x, &up.x, modelview );

	// Select and setup the projection matrix
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( perspective );

	// Select and setup the modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( modelview );

	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glShadeModel( GL_SMOOTH );
}

/*
================
ogDemoWindow::OnSize
================
*/
void ogDemoWindow::OnSize( int w, int h ) {
	window->MakeCurrent();
	glDisable(GL_SCISSOR_TEST);
	// Clear the front buffer to black
	GLfloat	clearcolor[4];
	GLint	drawbuffer;
	glGetIntegerv( GL_DRAW_BUFFER, &drawbuffer );
	glGetFloatv( GL_COLOR_CLEAR_VALUE, clearcolor );
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	if( drawbuffer == GL_BACK )
		window->SwapBuffers();
	glClearColor( clearcolor[0], clearcolor[1], clearcolor[2], clearcolor[3] );
}

/*
================
ogDemoWindow::OnClose
================
*/
bool ogDemoWindow::OnClose( bool forced ) {
	if ( !forced )
		return true;

	bgLoader.Stop();
	game.Shutdown();

	ShutdownInput();
	og::Font::Shutdown();
	og::Image::Shutdown();
	og::AudioSystem::Shutdown();

	window = NULL;
	return true;
}

/*
================
ogDemoWindow::Quit
================
*/
void ogDemoWindow::Quit( void ) {
	og::Gloot::DeleteWindow( window );
}

/*
================
ogDemoWindow::ShowCredits
================
*/
void ogDemoWindow::ShowCredits( bool quitAfter ) {
	game.Shutdown();

	og::Dict titleFont;
	titleFont.Set("fontAlign", "2");
	titleFont.Set("fontSize", "48");

	og::Dict textFont;
	textFont.Set("fontAlign", "2");
	textFont.Set("fontSize", "24");

	struct TextEntries {
		const char *title;
		const char **subtexts;
		const char **images;
	};
	const char *byLusito[] = {		"Made by Team Lusito", NULL };
	const char *programmers[] = {	"Santo Pfingsten", "Ensiform", NULL };
	const char *sndArtists[] = {	"Bart Kelsey", "Bliss", "broumbroum", "inferno",
									"RunnerPack", "Simon_Lacelle", "THE_bizniss", NULL };
	const char *musicArtists[] = {	"Joost Egelie", "Pogotron", "lucasgonze", NULL };
	const char *gfxImages[] = {		"gfx/logos/xiller.png", NULL };
	const char *ogLibs[] = {		"Gloot", "Fable", "FileSystem", "AudioSystem", "Common", "Image", "Font", NULL };
	const char *thirdLibs[] = {		"zLib", "libPNG", "libJPEG", "OGG / Vorbis", "OpenAL Soft", "OpenGL", NULL };
	const char *specialThanks[] = {	"www.FreeSound.org", "www.Jamendo.com", "www.OpenGameArt.org", NULL };

	TextEntries textEntrires[] = {
		{ "Asteroids Clone",		byLusito,		NULL },
		{ "Programmers",			programmers,	NULL },
		{ "2D Artwork",				NULL,			gfxImages },
		{ "Sound Artists",			sndArtists,		NULL },
		{ "Music",					musicArtists,	NULL },
		{ "Inhouse Libraries",		ogLibs,			NULL },
		{ "Thirdparty Libraries",	thirdLibs,		NULL },
		{ "Special Thanks to",		specialThanks,	NULL },

		{ NULL, NULL },
	};

	float xCenter = WINDOW_WIDTH * 0.5f;
	float yOffset = WINDOW_HEIGHT - 100.0f;

	for( int i=0; textEntrires[i].title != NULL; i++ ) {
		outro.AddEntry( new ogCreditsText( WINDOW_WIDTH, yOffset, textEntrires[i].title, titleFont ) );
		yOffset += 70.0f;
		if ( textEntrires[i].subtexts != NULL ) {
			for( int j=0; textEntrires[i].subtexts[j] != NULL; j++ ) {
				outro.AddEntry( new ogCreditsText( WINDOW_WIDTH, yOffset, textEntrires[i].subtexts[j], textFont ) );
				yOffset += 40.0f;
			}
		}
		if ( textEntrires[i].images != NULL ) {
			for( int j=0; textEntrires[i].images[j] != NULL; j++ ) {
				outro.AddEntry( new ogCreditsImage( xCenter, yOffset, textEntrires[i].images[j] ) );
				yOffset += 150.0f;
			}
		}
		yOffset += 50.0f;
	}

	outro.Init( yOffset, 85.0f, game.soundManager.Find("outro_track") );
}

/*
================
ogDemoWindow::ToggleFullscreen
================
*/
void ogDemoWindow::ToggleFullscreen( void ) {
	og::Monitor *monitor = window->GetMonitor();
	if ( window->GetFullScreen() ) {
		window->LockMouse( false );
		window->SetFullScreen( NULL, NULL );
	} else {
		window->LockMouse( true );
		window->SetFullScreen( NULL, monitor );
	}
}

/*
================
ogDemoWindow::OnKeyEvent
================
*/
void ogDemoWindow::OnKeyEvent( int key, bool down ) {
	if ( !intro.IsDone() || fadeIn > 0.0f )
		return;

	if ( outro.IsActive() ) {
		if ( key == og::Key::ESC )
			Quit();
		return;
	}

	menu.OnKeyEvent( key, down );
	if ( !menu.IsVisible() )
		ogInputListener::OnKeyEvent( key, down );
}

/*
================
ogDemoWindow::OnFocusChange
================
*/
void ogDemoWindow::OnFocusChange( bool hasFocus ) {
	ogInputListener::OnFocusChange( hasFocus );
	og::AS->SetWindowFocus( hasFocus );
}


/*
==============================================================================

  og::User

==============================================================================
*/

/*
================
og::User::Main
================
*/
int og::User::Main( int argc, char *argv[] ) {
	if ( !og::FileSystem::SimpleInit( ".gpk", "base", ".", "." ) )
		return 0;

	if ( !og::FS->FileExists("gfx/asteroids/spaceship.tga") ) {
		og::FileSystem::Shutdown();
		printf( "You might wanna check your working directory ?" );
		return 0;
	}

	Gloot::DisableSystemKeys( true );
	if( !demoWindow.Init() ) {
		og::FileSystem::Shutdown();
		return 0;
	}

	og::Timer fpsTimer;
	fpsTimer.Start();
	uInt count = 0;
	while( demoWindow.IsOpen() ) {
		if ( fpsTimer.MicroSeconds() < 200000 )
			count++;
		else {
			fpsTimer.Stop();
			float fps = static_cast<float>(count * 1000000.0/static_cast<double>(fpsTimer.MicroSeconds()));
			demoWindow.window->SetTitle( og::Format(APP_TITLE" | FPS: $*" ) << SetPrecision(2) << fps );
			count = 0;
			fpsTimer.Start();
		}
		og::Sleep( 10 );
		og::Gloot::Synchronize();
		demoWindow.Draw();
	}

	demoWindow.UnregisterAllBinds();
	Gloot::DisableSystemKeys( false );
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
