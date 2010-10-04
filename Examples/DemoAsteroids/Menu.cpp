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
#include "Menu.h"
const float MENU_WIDTH	= 400.0f;
const float MENU_TOP	= 220.0f;
const float MENU_TITLE_TOP = 120.0f;
const float MENU_HELP_TOP = 24.0f;

enum {
	MENU_CONTINUE,
	MENU_RESET,
	MENU_FULLSCREEN,
	MENU_QUIT,
};

ogMenuEntry::ogMenuEntry( const char *displayText, int align, ogMenuEntrySub *_parent ) : text(36,align) {
	parent = _parent;
	if ( parent )
		parent->AddChild( this );

	text.SetWidth( MENU_WIDTH );
	text.value = displayText;
}
void ogMenuEntry::Draw( float x, float y ) {
	text.Draw( x, y );
}

void ogMenuEntrySub::OnEscape( void ) {
	demoWindow.menu.SetMenuEntry( parent );
}

void ogMenuEntrySub::OnSelect( void ) {
	demoWindow.menu.SetMenuEntry( this );
}

void ogMenuEntryAction::OnSelect( void ) {
	switch( action ) {
		case MENU_FULLSCREEN:
			demoWindow.ToggleFullscreen();
			break;

		case MENU_RESET:
			demoWindow.game.Reset();
		case MENU_CONTINUE:
			demoWindow.menu.SetMenuEntry( NULL );
			break;
		case MENU_QUIT:
			demoWindow.ShowCredits( true );
			break;
	}
}

ogMenu::ogMenu() {
	currentMenu = mainMenu = NULL;
	
	escPressed = false;
	upPressed = false;
	downPressed = false;
	enterPressed = false;
}

ogMenu::~ogMenu() {
	if ( mainMenu != NULL )
		delete mainMenu;
}

void ogMenu::Init( void ) {
	// create full menu
	mainMenu = new ogMenuEntrySub( "Main", NULL );
	new ogMenuEntryAction( "Toggle Fullscreen", mainMenu, MENU_FULLSCREEN );
	new ogMenuEntryAction( "Continue", mainMenu, MENU_CONTINUE );
	new ogMenuEntryAction( "Restart", mainMenu, MENU_RESET );
//	ogMenuEntrySub *input = new ogMenuEntrySub( "Input", mainMenu );
	new ogMenuEntryAction( "Quit", mainMenu, MENU_QUIT );

//	new ogMenuEntrySub( "Dummy1", input );
//	new ogMenuEntrySub( "Dummy2", input );
//	new ogMenuEntrySub( "Dummy3", input );

	currentMenu = mainMenu;
	cursorLeft = og::Image::Find( "gfx/asteroids/menucursor_left.tga" );
	cursorRight = og::Image::Find( "gfx/asteroids/menucursor_right.tga" );

	og::Dict dict;
	dict.SetInt("fontAlign", 2);
	dict.SetInt("fontSize", 36);
	dict.SetString("font", "lucida");
	titleText.Init( dict );
	titleText.SetWidth( static_cast<float>(WINDOW_WIDTH) );
	titleText.value = "Lusito's Asteroids Clone!";

	dict.SetInt("fontSize", 24);
	dict.SetString( "fontColor", "1 1 1 0.7" );
	helpText.Init( dict );
	helpText.SetWidth( static_cast<float>(WINDOW_WIDTH) );
	helpText.value = "( Use Arrow keys to navigate, Space to attack )";
}

void ogMenu::SetMenuEntry( ogMenuEntrySub *menuEntry ) {
	currentMenu = menuEntry;
}

void ogMenu::Draw( void ) {
	if ( currentMenu ) {
		helpText.Draw( 0, MENU_HELP_TOP );
		titleText.Draw( 0, MENU_TITLE_TOP );
		float x = (WINDOW_WIDTH - MENU_WIDTH) * 0.5f;
		float y = MENU_TOP;
		int num = currentMenu->children.Num();
		for( int i=0; i<num; i++ ) {
			currentMenu->children[i]->Draw( x, y );
			if ( i == currentMenu->index ) {
				DrawImage( cursorLeft, x-30, y+10, 20, 20 );
				DrawImage( cursorRight, x+MENU_WIDTH+10, y+10, 20, 20 );
			}
			y += 40.0f;
		}
	}
}

void ogMenu::OnKeyEvent( int key, bool down ) {
	if ( !down ) {
		switch( key ) {
			case og::Key::ESC:		escPressed		= false; break;
			case og::Key::UP:		upPressed		= false; break;
			case og::Key::DOWN:		downPressed		= false; break;
			case og::Key::ENTER:	enterPressed	= false; break;
		}
		return;
	}

	switch( key ) {
		case og::Key::ESC:
			if ( !escPressed ) {
				if ( currentMenu != NULL ) {
					demoWindow.game.PlaySound( "menu_close", windowCenter );
					currentMenu->OnEscape();
				} else {
					demoWindow.game.PlaySound( "menu_open", windowCenter );
					currentMenu = mainMenu;
				}
				escPressed = true;
			}
			break;
		case og::Key::UP:
			if ( !upPressed ) {
				if ( currentMenu != NULL ) {
					demoWindow.game.PlaySound( "menu_move", windowCenter );
					currentMenu->OnUp();
				}
				upPressed = true;
			}
			break;
		case og::Key::DOWN:
			if ( !downPressed ) {
				if ( currentMenu != NULL ) {
					demoWindow.game.PlaySound( "menu_move", windowCenter );
					currentMenu->OnDown();
				}
				downPressed = true;
			}
			break;
		case og::Key::ENTER:
			if ( !enterPressed ) {
				if ( currentMenu != NULL ) {
					demoWindow.game.PlaySound( "menu_select", windowCenter );
					currentMenu->children[currentMenu->index]->OnSelect();
				}
				enterPressed = true;
			}
			break;
	}
}
