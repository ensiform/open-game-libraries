/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2002-2006 Camilla Berglund, (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Common Monitor Header
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

#ifndef __GLOOT_MONITOR_H__
#define __GLOOT_MONITOR_H__

namespace og {
	void ConvertBPP2RGB( int bpp, int *r, int *g, int *b );

	class MonitorEx : public Monitor {
		// ---------------------- Public Monitor Interface -------------------

		bool		SetGamma( float value );
		void		RestoreGamma( void );

		void		Lock( void ) { modeMutex.Lock(); }
		void		Unlock( void ) { modeMutex.Unlock(); }
		bool		IsReserved( void ) { return fsWindow != NULL; }	// if a window has reserved this monitor for fullscreen mode (also true when the window is iconified)

		int			NumVideoModes( void ) const { return videoModeList.Num(); }
		const VideoMode *GetVideoMode( int i ) const;

		const VideoMode *GetClosestVideoMode( int w, int h, int bpp, int rate );
		bool		SetMode( const VideoMode *mode, Window *window );

		// ---------------------- Internal MonitorEx Members -------------------

	public:
		MonitorEx();
		~MonitorEx() { Shutdown(); }

		friend class WindowEx;

		// These are only to be called by the user and the monitor itself
	protected:
		void		Shutdown( void );
		void		SetDefaultMode( WindowEx *window );

		void		OnIconify( void );
		void		OnUnIconify( void );
		void		Revert( void );

	private:
		Mutex		modeMutex, gammaMutex;
		bool		modified, iconified;
		List<VideoMode> videoModeList;
		VideoMode	userMode;
		const VideoMode *currentMode;
		WindowEx *	fsWindow;
		float		desiredGamma;

		// Rest is platform specific
#if OG_WIN32
		MONITORINFOEX	monInfo;
		HMONITOR		hMonitor;
		const char *	szDevice;

		// current mode info:
		int			left, top, width, height;

	public:
		bool		Init( HMONITOR hMon );
		bool		IsHMonitor( HMONITOR other ) { return hMonitor == other; }
#elif OG_LINUX
		bool		hasGammaRamp;
		XF86VidModeGamma storedGamma;
		
		bool	Init( int screenId );
#elif OG_MACOS_X
    #warning "Need MacOS here FIXME"
#endif
	};
}

#endif
