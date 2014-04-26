/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2002-2006 Camilla Berglund, (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Common Window Header
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

#ifndef __GLOOT_WINDOW_H__
#define __GLOOT_WINDOW_H__

#if OG_WIN32
	#define GLOOT_WNDCLASSNAME L"GLOOT"
#endif

namespace og {
	class MonitorEx;

	class WindowEx : public Window {
	public:
		// ---------------------- Public Window Interface -------------------
		bool		IsOpen( void ) const { return isOpen; }
		bool		HasFocus( void ) const { return hasFocus; }

		bool		ExtensionSupported( const char *extension ) const;
		void *		GetProcAddress( const char *procname ) const;

		bool		MakeCurrent( void );
		void		SetSwapinterval( bool value );
		bool		GetSwapinterval( void ) const { return vsyncEnabled; }
		void		SwapBuffers( void );

		Monitor *	GetMonitor( void ) const;
		void		SetFullScreen( const VideoMode *mode, Monitor *monitor );
		bool		GetFullScreen( void ) const { return fsMonitor != OG_NULL; }

		void		SetTitle( const char *title );
		void		SetSize( int w, int h );
		void		GetSize( int *w, int *h ) const { if( w ) { *w = windowConfig.width; } if ( h ) { *h = windowConfig.height; } }
		void		SetPos( int x, int y );
		void		GetPos( int *x, int *y ) const { if( x ) { *x = windowConfig.x; } if ( y ) { *y = windowConfig.y; } }
		void		SetResizable( bool value );
		bool		IsResizable( void ) const { return (windowConfig.flags & WF_NORESIZE) == 0; }
		void		Iconify( void );
		void		Restore( void );
		bool		IsIconified( void ) const { return isIconified; }
		void		LockMouse( bool locked );

		const WindowConfig *GetWindowConfig( void ) const { return &windowConfig; }

		// ---------------------- Internal WindowEx Members -------------------

		friend class Platform;

	public:
		WindowEx();

		// Window Creation
		void		SetWindowConfig( const WindowConfig *windowCfg ) { windowConfig = *windowCfg; }
		bool		Open( void );
		void		Close( void );
		void		PrepareFullScreen( void );

		bool		CreateContext( void );
		static WindowEx *NewWindow( const WindowConfig *windowCfg );

	protected:
		bool		Create( void );
		void		Destroy( void );
		void		GetFullSize( int *w, int *h );
		void		RefreshWindowParams( void );

		void		GetGLVersion( int *major, int *minor, int *rev );

		void		UpdateCursorClip( void );

	private:
		WindowConfig	windowConfig;
		bool		isOpen, isClosing, hasFocus, isIconified, vsyncEnabled;

		MonitorEx *	fsMonitor; // fullscreen monitor (NULL in windowed mode)

		bool		mouseUseRaw;
		bool		mouseLock;
		int			oldMouseLock;	// Old mouse-lock flag (used for remembering mouse-lock state when iconifying)

		// Rest is platform specific
#if OG_WIN32
		HDC			hDC;			// Private GDI device context
		HGLRC		hRC;			// Permanent rendering context
		HWND		hWnd;			// Window handle
		DWORD		dwStyle;		// Window styles used for window creation
		DWORD		dwExStyle;		// --"--
		int			oldSize[2];
		int			oldPos[2];
		bool		ignoreSizeMove;

		int			ChoosePixelFormat( PIXELFORMATDESCRIPTOR *pfd );
		int			ChoosePixelFormatARB( void );
		static LRESULT CALLBACK WndCallback( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		LRESULT		WndCallback( UINT uMsg, WPARAM wParam, LPARAM lParam );
		void		UpdateStyles( bool fullScreen );

		friend class WindowEvent;
	public:
		HWND		GetHWND( void ) { return hWnd; }
#elif OG_LINUX
	#warning "Need Linux here FIXME"
#elif OG_MACOS_X
	#warning "Need MacOS here FIXME"
#endif
	};
}

#endif
