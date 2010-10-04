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

#ifndef __OG_MENU_H__
#define __OG_MENU_H__

#include "Game.h"

class ogMenuEntrySub;

/*
==============================================================================

  ogMenuEntry

==============================================================================
*/
class ogMenuEntry {
public:
	ogMenuEntry( const char *displayText, int align, ogMenuEntrySub *parent );
	virtual ~ogMenuEntry() {}

	virtual void OnSelect( void ) = 0;
	virtual void Draw( float x, float y );

protected:
	ogFontText	text;
	ogMenuEntrySub *parent;
};

/*
==============================================================================

  ogMenuEntrySub

==============================================================================
*/
class ogMenuEntrySub : public ogMenuEntry {
public:
	ogMenuEntrySub( const char *displayText, ogMenuEntrySub *parent ) : ogMenuEntry( displayText, 2, parent ), index(0) {}
	~ogMenuEntrySub() {
		int num = children.Num();
		for( int i=0; i<num; i++ )
			delete children[i];
	}

	virtual void OnSelect( void );
	virtual void OnEscape( void );
	virtual void OnUp( void ) { if ( index == 0 ) index = children.Num()-1; else index--; }
	virtual void OnDown( void ) { if ( index == children.Num()-1 ) index = 0; else index++; }

	void	AddChild( ogMenuEntry *child ) {
		children.Append( child );
	}

private:
	friend class ogMenu;
	og::List<ogMenuEntry *> children;
	int		index;
};

/*
==============================================================================

  ogMenuEntryAction

==============================================================================
*/
class ogMenuEntryAction : public ogMenuEntry {
public:
	ogMenuEntryAction( const char *displayText, ogMenuEntrySub *parent, int _action ) : ogMenuEntry( displayText, 2, parent ), action(_action) {}
	void OnSelect( void );

private:
	int		action;
};

/*
==============================================================================

  ogMenu

==============================================================================
*/
class ogMenu {
public:
	ogMenu();
	~ogMenu();

	void	Init( void );
	void	Draw( void );
	void	SetMenuEntry( ogMenuEntrySub *menuEntry );
	void	OnKeyEvent( int key, bool down );
	bool	IsVisible( void ) { return currentMenu != NULL; }

private:
	ogFontText	titleText;
	ogFontText	helpText;
	ogMenuEntrySub *currentMenu;
	ogMenuEntrySub *mainMenu;
	og::Image *cursorLeft;
	og::Image *cursorRight;

	bool escPressed;
	bool upPressed;
	bool downPressed;
	bool enterPressed;
};

#endif
