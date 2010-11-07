/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Logo Screen
-----------------------------------------

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
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

#ifndef __DEMO_LOGO_SCREEN_H__
#define __DEMO_LOGO_SCREEN_H__

#include <og/Common.h>
#include <og/Image.h>
#include <og/Audio.h>
#include "FontText.h"

/*
==============================================================================

  ogLogoScreen

==============================================================================
*/
class ogLogoScreen {
public:
	ogLogoScreen();
	void	Init( const og::Vec3 &logoCenter, int time, const og::Sound *sound );
	void	Draw( int frameTime );
	bool	IsDone( void ) { return timer.Milliseconds() > playTime; }

private:
	void	DrawLogo( void );

	og::Image	*image;
	og::AudioEmitter *emitter;
	og::Timer	timer;

	// Settings
	og::Vec3	center;
	int			playTime;
};

class ogCreditsEntry {
public:
	ogCreditsEntry( float x, float y ) : xOffset(x), yOffset(y) {}
	virtual ~ogCreditsEntry() {}

	virtual void	Draw( float y ) = 0;

protected:
	float	xOffset, yOffset;
	float	width, height;
};

class ogCreditsImage : public ogCreditsEntry {
public:
	ogCreditsImage( float x, float y, const char *filename );

	void	Draw( float y );

protected:
	og::Image *image;
};

class ogCreditsText : public ogCreditsEntry {
public:
	ogCreditsText( float width, float y, const char *value, const og::Dict &fontSettings );

	void	Draw( float y );

protected:
	ogFontText text;
};

class ogCreditsScreen {
public:
	ogCreditsScreen();
	~ogCreditsScreen();

	void	AddEntry( ogCreditsEntry *entry );
	void	Init( float height, float speed, const og::Sound *sound );
	void	Draw2D( int frameTime );
	void	Draw3D( int frameTime );
	bool	IsActive( void ) const { return isActive; }
	bool	IsDone( void ) const { return isDone; }

private:
	bool			isActive;
	bool			isDone;
	ogLogoScreen	logo;
	og::List<ogCreditsEntry *> entries;
	float			scrollSpeed;
	float			scrollHeight;
};

#endif
