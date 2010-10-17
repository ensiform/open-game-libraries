/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2002-2006 Camilla Berglund, (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Win32 Monitor Code
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

#include "../Platform.h"

#if OG_WIN32

#include <math.h>
#include <algorithm>

namespace og {

int CompareVideoModes( const VideoMode &a, const VideoMode &b ) {
	int resA = (a.width * a.height);
	int resB = (b.width * b.height);
	int diff = resA - resB;
	if ( diff != 0 ) return diff;
	if ( (a.width - b.width) != 0 ) return diff;
	if ( (diff = a.bpp - b.bpp) != 0 ) return diff;

	return a.refreshRate - b.refreshRate;
}

MonitorEx::MonitorEx() {
	hMonitor = NULL;
	modified = iconified = false;
	currentMode = NULL;
	left = top = width = height = 0;

	fsWindow = NULL;
	memset( &monInfo, 0, sizeof(MONITORINFOEX) );
	monInfo.cbSize = sizeof(MONITORINFOEX);
}

bool MonitorEx::Init( HMONITOR hMon ) {
	hMonitor = hMon;
	if ( !GetMonitorInfo( hMonitor, &monInfo ) )
		return false;

	DEVMODE dm;
	dm.dmSize = sizeof( DEVMODE );
	VideoMode vm;

	// Loop through all video modes and extract all the UNIQUE modes
	for( int mode=0; EnumDisplaySettings( monInfo.szDevice, mode, &dm ); mode++ ) {
		// Is it a valid mode? (only list depths >= 15 bpp)
		if( dm.dmBitsPerPel >= 15 ) {
			vm.modeId		= mode;
			vm.width		= dm.dmPelsWidth;
			vm.height		= dm.dmPelsHeight;
			vm.bpp			= dm.dmBitsPerPel;
			vm.refreshRate	= dm.dmDisplayFrequency;
			videoModeList.Append( vm );
		}
	}
	// sort and remove duplicates
	videoModeList.Sort( CompareVideoModes, true );

	if ( videoModeList.IsEmpty() )
		return false;

	// Get desktop display mode
	DEVMODE dmUser;
	dmUser.dmSize = sizeof( DEVMODE );
	EnumDisplaySettings( monInfo.szDevice, ENUM_REGISTRY_SETTINGS, &dmUser );
	userMode.modeId			= ENUM_REGISTRY_SETTINGS;
	userMode.width			= dmUser.dmPelsWidth;
	userMode.height			= dmUser.dmPelsHeight;
	userMode.bpp			= dmUser.dmBitsPerPel;
	userMode.refreshRate	= dmUser.dmDisplayFrequency;

	return true;
}

//!@todo Since gamma is always changed for the whole screen, we should maybe limit it to fullscreen?
bool MonitorEx::SetGamma( float value ) {
	if ( !Mgr.hasGammaRamp )
		return false;
	desiredGamma = value;

	HDC hDC = GetDC(NULL);
	WORD ramp[3][256];
	if ( value == 1.0f ) {
		for ( int i=0; i<256; i++ )
			ramp[0][i] = ramp[1][i] = ramp[2][i] = (i << 8) | i;
	} else {
		value = 1.0f / Clamp( value, 0.5f, 3.0f );
		for ( int i=0; i<256; i++ )
			ramp[0][i] = ramp[1][i] = ramp[2][i] = Clamp( static_cast<int>(pow( i/255.0f, value )*65535 + 0.5), 0, 65535);
	}
	bool ret = SetDeviceGammaRamp( hDC, ramp) == TRUE;
	ReleaseDC( NULL, hDC );
	return ret;
}

void MonitorEx::RestoreGamma( void ) {
	if ( !Mgr.hasGammaRamp )
		return;
	HDC hDC = GetDC(NULL);
	SetDeviceGammaRamp( hDC, Mgr.storedGammaRamp );
	ReleaseDC( NULL, hDC );
}

void MonitorEx::SetDefaultMode( WindowEx *window ) {
	if ( window )
		window->PrepareFullScreen();
	else
		RestoreGamma();

	ChangeDisplaySettingsEx( monInfo.szDevice, NULL, NULL, CDS_FULLSCREEN, NULL );
	modified = false;

	width = userMode.width;
	height = userMode.height;
	fsWindow = window;
}

bool MonitorEx::SetMode( const VideoMode *newMode, Window *window ) {
	// do not change, if we are in fullscreen mode, and the new window is not the current fullscreen window
	if ( fsWindow != NULL && window != NULL && window != fsWindow )
		return false;

	//! @todo	update window
	if ( newMode == NULL || newMode->modeId == ENUM_REGISTRY_SETTINGS ) {
		currentMode = NULL;
		SetDefaultMode( static_cast<WindowEx *>(window) );
		return true;
	}
	if ( !window )
		return false;

	static_cast<WindowEx *>(window)->PrepareFullScreen();

	//! @todo	what if it's iconified?

	DEVMODE dm;
	dm.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings( monInfo.szDevice, newMode->modeId, &dm );

	// Set which fields we want to specify
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

#if 0 // prolly not needed, since GetClosestVideoMode already selects the best available mode for the frequency.
	// Do we have a prefered refresh rate?
	if( refreshRate > 0 ) {
		dm.dmFields |= DM_DISPLAYFREQUENCY;
		dm.dmDisplayFrequency = refreshRate;
	}
#endif

	// If the mode change was not possible, query the current display
	// settings (we'll use the desktop resolution for fullscreen mode)
	modified = ChangeDisplaySettingsEx( monInfo.szDevice, &dm, NULL, CDS_FULLSCREEN, NULL ) == DISP_CHANGE_SUCCESSFUL;
	if( modified )
		currentMode = newMode;
	else {
		currentMode = NULL;
		EnumDisplaySettings( monInfo.szDevice, ENUM_REGISTRY_SETTINGS, &dm );
	}

	// And resize it so that it fits the entire screen
	GetMonitorInfo( hMonitor, &monInfo );

	// Update info
	left = monInfo.rcMonitor.left;
	top = monInfo.rcMonitor.top;
	width = dm.dmPelsWidth;
	height = dm.dmPelsHeight;
//	Mgr.ConvertBPP2RGB( dm.dmBitsPerPel, &redBits, &greenBits, &blueBits ); 

	fsWindow = static_cast<WindowEx *>( window );
	return modified;
}

BOOL CALLBACK MonitorEnumProc( HMONITOR hMonitor, HDC /*hdcMonitor*/, LPRECT /*lprcMonitor*/, LPARAM /*dwData*/ ) {
	MonitorEx *mon = new MonitorEx;
	if ( mon->Init( hMonitor ) )
		Mgr.monitorList.Append( mon );
	else
		delete mon;
	return TRUE; // continue
}

bool Platform::InitMonitors( void ) {
	return EnumDisplayMonitors( NULL, NULL, MonitorEnumProc, 0 ) == TRUE;
}

}

#endif
