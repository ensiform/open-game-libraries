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

#include <stdio.h>

#define DEMO_OPENGL3
#ifdef DEMO_OPENGL3
 #include <gl/glew.h>
 #pragma comment(lib, "../../Thirdparty/GL/glew32.lib")
#else
 #include <og/Gloot/glmin.h>
#endif

#include "main.h"

#include <math.h>
#include <string.h>

/*
==============================================================================

  SimpleWindowClass (OpenGL 1/2)

==============================================================================
*/
/*
================
SimpleWindowClass::SimpleWindowClass
================
*/
SimpleWindowClass::SimpleWindowClass() {
	window = NULL;
}

/*
================
SimpleWindowClass::SetupWindow
================
*/
bool SimpleWindowClass::SetupWindow( int x, int y, byte multiSamples, const char *title, int *contextAttribs ) {
	og::WindowConfig windowCfg = {
		sizeof(og::WindowConfig),				// size of the struct
		title,									// window title
		og::WF_DOUBLEBUFFER,					// flags
		x, y, 800, 600,							// rect
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

	//window->SetGamma(0.75f);
	return true;
}

/*
================
SimpleWindowClass::Init
================
*/
bool SimpleWindowClass::Init( bool isSecond ) {
#ifdef DEMO_OPENGL3
	const char *title = "Hello OpenGL 1/2 World";
#else
	const char *title = isSecond ? "Hello Second World" : "Hello First World";
#endif

	int contextAttribs[] = {
		og::glContext::MAJOR_VERSION, 2,
		og::glContext::MINOR_VERSION, 1,
		0
	};
	if ( !SetupWindow( isSecond*200, isSecond*200, (isSecond ? 0 : 8), title, contextAttribs ) )
		return false;

	camAngles.yaw = isSecond ? 90.0f : 0.0f;
	return true;
}

/*
================
SimpleWindowClass::Draw
================
*/
void SimpleWindowClass::Draw( void ) {
	if ( !IsOpen() )
		return;

	window->MakeCurrent();

	// Get window size (may be different than the requested size)
	int width, height;
	window->GetSize( &width, &height );
	height = height > 0 ? height : 1;

	// Set viewport
	glViewport( 0, 0, width, height );
	// Clear color buffer
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	og::Vec3 origin;
	og::Vec3 forward;
	og::Vec3 right;
	og::Vec3 up;
	camAngles.ToVectors( &forward, &right, &up );
	origin = forward * -14.0f;
	camAngles.yaw += 1.0f;

	float modelview[16];
	float perspective[16];
	og::Gloot::CreatePerspectiveMatrix( 65.0f, (float)width/(float)height, 0.1f, 2048.0f, perspective );
	og::Gloot::CreateCameraMatrix( &origin.x, &forward.x, &right.x, &up.x, modelview );

	DrawView( modelview, perspective );

	// Swap buffers
	window->SwapBuffers();
}

/*
================
SimpleWindowClass::DrawView
================
*/
void SimpleWindowClass::DrawView( float modelview[16], float perspective[16] ) {
	// Select and setup the projection matrix
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( perspective );

	// Select and setup the modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( modelview );

	// Draw a colorful triangle
	glBegin( GL_TRIANGLES );
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex3f( -5.0f, 0.0f, -4.0f );
		glColor3f( 0.0f, 1.0f, 0.0f );
		glVertex3f( 5.0f, 0.0f, -4.0f );
		glColor3f( 0.0f, 0.0f, 1.0f );
		glVertex3f( 0.0f, 0.0f, 6.0f );
	glEnd();
}

/*
================
SimpleWindowClass::OnKeyEvent
================
*/
void SimpleWindowClass::OnKeyEvent( int key, bool down ) {
	if ( key == og::Key::ENTER && down ) {
		og::Monitor *monitor = window->GetMonitor();
		// We lock the monitor, so no other gloot window can access it while we try to do our fullscreen change.
		monitor->Lock();
		if ( window->GetFullScreen() ) {
			window->SetFullScreen( NULL, NULL );
			window->LockMouse(false);
		}
		else {
			if ( monitor->IsReserved() ) {
				printf( "Monitor is already in use for fullscreen!" );
				return;
			}
			window->LockMouse(true);

			int width, height;
			window->GetSize( &width, &height );
			const og::VideoMode *mode = monitor->GetClosestVideoMode( width, height, 24, 0 );
			window->SetFullScreen( mode, monitor );
			monitor->SetGamma( 0.5f );
		}
		monitor->Unlock();
	}
}



#ifdef DEMO_OPENGL3
/*
==============================================================================

  AdvancedWindowClass (OpenGL 3/4)

==============================================================================
*/
/*
================
AdvancedWindowClass::AdvancedWindowClass
================
*/
AdvancedWindowClass::AdvancedWindowClass() {
	m_vertShID = 0;
	m_fragShID = 0;
	m_programID = 0;
}

/*
================
AdvancedWindowClass::Init
================
*/
bool AdvancedWindowClass::Init( bool isSecond ) {
	int contextAttribs[] = {
		og::glContext::MAJOR_VERSION, 3,
		og::glContext::MINOR_VERSION, 2,
		og::glContext::FLAGS, og::glContextFlags::FORWARD_COMPATIBLE,
		0
	};
	if ( !SetupWindow( 200, 200, 8, "Hello OpenGL 3/4 World", contextAttribs ) )
		return false;

	GLenum err = glewInit();

	glClearColor (1.0, 1.0, 1.0, 0.0);

	m_vertShID = glCreateShader( GL_VERTEX_SHADER );
	if( !CompileShader( "minimal.vert", m_vertShID ) )
		printf( "Vertex shader compiling error!" );
	
	m_fragShID = glCreateShader( GL_FRAGMENT_SHADER );
	if( !CompileShader( "minimal.frag", m_fragShID ) )
		printf( "Fragment shader compiling error!" );

	m_programID = glCreateProgram();
	glAttachShader( m_programID, m_vertShID );
	glAttachShader( m_programID, m_fragShID );

	glBindAttribLocation( m_programID, 0, "in_Position" );
	glBindAttribLocation( m_programID, 1, "in_Color" );

	glLinkProgram( m_programID );

	int param;
	glGetProgramiv( m_programID, GL_LINK_STATUS, &param );
	if( param != GL_TRUE )
		printf( "Program linking error!" );

	glUseProgram( m_programID );

	SetData();

	camAngles.yaw = 90.0f;
	return true;
}

/*
================
AdvancedWindowClass::CompileShader
================
*/
bool AdvancedWindowClass::CompileShader( const char *filename, GLuint ID ) {
	if ( filename[0] ) {
		FILE *fp = fopen( filename, "rt" );
		if( fp != NULL ) {
			fseek( fp, 0, SEEK_END );
			int count = ftell( fp );
			rewind( fp );
			if( count > 0 ) {
				char *text = new char [count+1];
				count = fread( text, 1,count, fp );
				text[count] = '\0';
				fclose( fp );

				const char *code = text;
				glShaderSource( ID, 1, &code, NULL );
				glCompileShader( ID );

				delete[] text;
				int param;
				glGetShaderiv( ID, GL_COMPILE_STATUS, &param );
				return (param == GL_TRUE);

			}
			fclose( fp );
		}
	}
	return false;
}

/*
================
AdvancedWindowClass::SetData
================
*/
void AdvancedWindowClass::SetData( void ) {
	GLfloat vert[9] = {	 -5.0f, 0.0f, -4.0f,
						5.0f, 0.0f, -4.0f,
						 0.0f, 0.0f, 6.0f };

	GLfloat col[9] = {	1.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 1.0f };

	glGenBuffers(2, &m_vboID[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(col), col, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

/*
================
AdvancedWindowClass::OnClose
================
*/
bool AdvancedWindowClass::OnClose( bool forced ) {
	window->MakeCurrent();

	if ( m_vertShID ) {
		if ( m_programID )
			glDetachShader( m_programID, m_vertShID );
		glDeleteShader( m_vertShID );
		m_vertShID = 0;
	}
	if ( m_fragShID ) {
		if ( m_programID )
			glDetachShader( m_programID, m_fragShID );
		glDeleteShader( m_fragShID );
		m_fragShID = 0;
	}
	if ( m_programID ) {
		glDeleteProgram( m_programID );
		m_programID = 0;
	}

	return true;
}

/*
================
AdvancedWindowClass::DrawView
================
*/
void AdvancedWindowClass::DrawView( float modelview[16], float perspective[16] ) {
	GLint projectionLoc = glGetUniformLocation(m_programID, "projectionMatrix");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, perspective);
	GLint modelViewLoc = glGetUniformLocation(m_programID, "modelViewMatrix");
	glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, modelview);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}
#endif



/*
================
og::User::Main
================
*/
int og::User::Main( int argc, char *argv[] ) {
	if ( !og::Shared::Init() )
		return 0;
	Gloot::DisableSystemKeys( true );
	SimpleWindowClass myWindowA;
#ifdef DEMO_OPENGL3
	AdvancedWindowClass myWindowB;
#else
	SimpleWindowClass myWindowB;
#endif

	myWindowA.Init( false );
	myWindowB.Init( true );

	while( myWindowA.IsOpen() || myWindowB.IsOpen() ) {
		og::Gloot::Synchronize();
		myWindowA.Draw();
		myWindowB.Draw();
		og::Gloot::Sleep(16);
    }
	Gloot::DisableSystemKeys( false );
    return 0;
}

/*
================
og::User::Error
================
*/
void og::User::Error( og::ErrorId id, const char *msg, const char *param ) {
	fprintf( stderr, "GLOOT Error(%d): '%s':'%s'\n", id, msg, param?param:"-" );
}

/*
================
og::User::AssertFailed
================
*/
void og::User::AssertFailed( const char *code, const char *function ) {
	fprintf( stderr, "Assert(%s) failed in %s!", code, function );
}
