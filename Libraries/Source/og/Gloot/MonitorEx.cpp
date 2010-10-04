/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2002-2006 Camilla Berglund, (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Common Monitor Code
Note:    Some of the code is based on GLFW 2.6 (See Readme.txt)
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

#include "Platform.h"

namespace og {

void MonitorEx::Shutdown( void ) {
	Revert();
	videoModeList.Clear();
}

void MonitorEx::OnIconify( void ) {
	if ( modified ) {
		iconified = true;
		SetDefaultMode( fsWindow );
		RestoreGamma();
	}
}

void MonitorEx::OnUnIconify( void ) {
	if ( iconified ) {
		modified = SetMode( currentMode, fsWindow );
		iconified = false;
		SetGamma( desiredGamma );
	}
}

void MonitorEx::Revert( void ) {
	if ( modified && !iconified )
		SetDefaultMode( NULL );

	modified = false;
	iconified = false;
	fsWindow = NULL;
	currentMode = NULL;
}

const VideoMode *MonitorEx::GetClosestVideoMode( int w, int h, int bpp, int rate ) {
	// If colorbits < 15 (e.g. 0) or >= 24, default to 32 bpp
	if( bpp < 15 || bpp >= 24 )
		bpp = 32;

	int match, rr;
	DEVMODE dm;
	dm.dmSize = sizeof( DEVMODE );

	// Find best match
	int bestmatch = 0x7fffffff;
	int bestrr = 0x7fffffff;
	const VideoMode *bestmode = NULL;

	int max = videoModeList.Num();
	for( int i=0; i<max; i++ ) {
		VideoMode &vm = videoModeList[i];
		match = labs(vm.bpp - bpp);
		match = ( match << 25 ) | ( (vm.width - w) * (vm.width - w) + (vm.height - h) * (vm.height - h) );
		if( match < bestmatch ) {
			bestmatch = match;
			bestmode = &vm;
			bestrr = (vm.refreshRate - rate) * (vm.refreshRate - rate);
		}
		else if( match == bestmatch && rate > 0 ) {
			rr = (vm.refreshRate - rate) * (vm.refreshRate - rate);
			if( rr < bestrr ) {
				bestmatch = match;
				bestmode = &vm;
				bestrr = rr;
			}
		}
	}

	return bestmode;
}

const VideoMode *MonitorEx::GetVideoMode( int i ) const {
	if ( i == VIDEOMODE_USER )
		return &userMode;
	return &videoModeList[i];
}

void Platform::ShutdownMonitors( void ) {
	int max = monitorList.Num();
	for( int i=0; i<max; i++ )
		delete monitorList[i];
	monitorList.Clear();
}

void ConvertBPP2RGB( int bpp, int *r, int *g, int *b ) {
	// Special case: BPP = 32
	if( bpp == 32 ) bpp = 24;

	// Convert "bits per pixel" to red, green & blue sizes
	*r = *g = *b = bpp / 3;
	int delta = bpp - (*r * 3);

	if( delta >= 1 )
		*g += 1;
	if( delta == 2 )
		*r += 1;
}

}
