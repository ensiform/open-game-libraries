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

#include <og/Gloot.h>
#include <og/Common.h>
#include "../Shared/SoundManager.h"
#include "../Shared/FontText.h"

#define APP_TITLE "Hello OpenAL world"

/*
==============================================================================

  TextDrawer ( helps drawing lines )

==============================================================================
*/
class TextDrawer {
public:
	TextDrawer() {}
	~TextDrawer() {
		for( int i=0; i<lines.Num(); i++ )
			delete lines[i];
	}
	void	AddLine( const char *text )  {
		ogFontText *item = new ogFontText(18, og::Font::LEFT);
		item->value = text;
		lines.Append(item);
	}
	void	SetLine( int index, const char *text ) {
		if ( index < lines.Num() && index >= 0 )
			lines[index]->value = text;
	}
	void	Draw( int x, int y, bool upwards, int maxLines ) {
		int num = lines.Num();
		if ( num == 0 )
			return;
		if ( maxLines > 0 && num > maxLines )
			num = maxLines;

		float step = 20.0f;
		float px = static_cast<float>(x);
		float py = static_cast<float>(y);
		if ( upwards )
			py -= (num-1) * step;

		for( int i=0; i<num; i++ ) {
			lines[i]->Draw( px, py );
			py += step;
		}
	}

private:
	og::List<ogFontText	*> lines;
};

/*
==============================================================================

  ogDemoWindow

==============================================================================
*/
class ogDemoWindow : public og::WindowListener {
public:
	ogDemoWindow();
	virtual bool	Init( void );
	bool	SetupWindow( int x, int y, byte multiSamples, const char *title, int *contextAttribs );

	bool	IsOpen( void ) { return window && window->IsOpen(); }
	void	Draw( void );

protected:
	void	OnCharEvent( int ch );
	void	OnMouseButton( int button, bool down );
	bool	OnClose( bool forced );

	void	UpdateSound( void );

	og::Window *window;
	og::Vec3	soundPos;
	og::Vec3	soundPosOld;
	bool		mouseDown;
	bool		manualPosition;
	bool		showHelp;
	og::Angles	soundAngle;
	ogSoundManager soundManager;
	og::AudioEmitter *audioEmitter;
	og::Timer	frameTimer;
	TextDrawer	helpLines;
	TextDrawer	infoLines;
};
