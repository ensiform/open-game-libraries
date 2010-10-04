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

#include <og/Gloot/Gloot.h>
#include <og/Common/Common.h>

#define DEMO_OPENGL3

/*
==============================================================================

  SimpleWindowClass (OpenGL 1/2)

==============================================================================
*/
class SimpleWindowClass : public og::WindowListener {
public:
	SimpleWindowClass();
	virtual bool	Init( bool isSecond );
	bool	SetupWindow( int x, int y, byte multiSamples, const char *title, int *contextAttribs );

	bool	IsOpen( void ) { return window && window->IsOpen(); }
	void	Draw( void );

protected:
	void	OnKeyEvent( int key, bool down );
	virtual void	DrawView( float modelview[16], float perspective[16] );

	og::Window *window;
	og::Angles camAngles;
};

/*
==============================================================================

  AdvancedWindowClass (OpenGL 3/4)

==============================================================================
*/
#ifdef DEMO_OPENGL3
class AdvancedWindowClass : public SimpleWindowClass {
public:
	AdvancedWindowClass();

	virtual bool	Init( bool isSecond );

protected:
	virtual void	DrawView( float modelview[16], float perspective[16] );

	bool	CompileShader( const char *filename, GLuint ID );
	void	SetData( void );
	bool	OnClose( bool forced );

private:
	GLuint	m_programID;
	GLuint	m_vertShID;
	GLuint	m_fragShID;
	
	unsigned int m_vboID[2];
};
#endif
