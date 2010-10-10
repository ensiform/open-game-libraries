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


#ifndef __OG_MAIN_H__
#define __OG_MAIN_H__

#include <og/Gloot/glmin.h>
#include <og/Gloot/Gloot.h>
#include <og/Common/Common.h>
#include <og/FileSystem/FileSystem.h>
#include <og/Audio/AudioSystem.h>
#include <og/Image/Image.h>
#include <og/Font/Font.h>
#include "Game.h"
#include "Menu.h"

#include "../Shared/InputListener/InputListener.h"

#define APP_TITLE "Asteroids Demo"

// The virtual screen size
const int	WINDOW_WIDTH	= 800;
const int	WINDOW_HEIGHT	= 600;

/*
==============================================================================

  ogDemoWindow

==============================================================================
*/
class ogDemoWindow : public ogInputListener {
public:
	ogDemoWindow();
	bool	Init( void );

	bool	IsOpen( void ) { return window && window->IsOpen(); }
	void	Draw( void );
	void	DrawIntro( int frameTime );
	void	DrawOutro( int frameTime );
	void	DrawGame( int frameTime );

	void	SetupOrtho( void );
	void	SetupPerspective( void );

	void	Quit( void );
	void	ShowCredits( bool quitAfter );
	void	ToggleFullscreen( void );

protected:
	bool	OnClose( bool forced );
	void	OnSize( int w, int h );
	void	OnKeyEvent( int key, bool down );
	void	OnFocusChange( bool hasFocus );

public:
	og::Window *window;
	int			width, height;
	og::Timer	frameTimer;

	ogLogoScreen	intro;
	ogCreditsScreen	outro;
	float		fadeIn;
	bool		skipFrame;
	ogGame		game;
	ogMenu		menu;
};

extern ogDemoWindow demoWindow;

extern ogBind bindAttack;
extern ogBind bindAccelerate;
extern ogBind bindTurn;

#endif
