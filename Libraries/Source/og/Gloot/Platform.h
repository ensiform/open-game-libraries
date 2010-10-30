/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2002-2006 Camilla Berglund, (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Common Manager Header
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

#ifndef __GLOOT_MANAGER_H__
#define __GLOOT_MANAGER_H__

#include <og/Shared/Shared.h>

#if OG_WIN32
	#include <windows.h>
#elif OG_LINUX
	#include <unistd.h>
	#include <signal.h>
	#include <X11/Xlib.h>
	#include <X11/Xlib-xcb.h> // for XGetXCBConnection, link with libX11-xcb
	#include <X11/xcb.h>
	#include <X11/keysym.h>
	#include <X11/Xatom.h>
	#include <GL/glx.h>
#elif OG_MACOS_X
	#warning "Need MacOS here FIXME"
#endif

#include <og/Common/Common.h>
#include <og/Shared/Thread/LockFreeQueue.h>

#include <og/Gloot/glmin.h>
#include <og/Gloot/gloot.h>
#include <map>
#include <string>
#include "WindowEx.h"
#include "MonitorEx.h"

namespace og {
// Some platform specific OpenGL extensions we need
#if OG_WIN32
	// wglSwapIntervalEXT typedef (Win32 buffer-swap interval control)
	typedef int (APIENTRY * WGLSWAPINTERVALEXT_T) (int);
	// wglChoosePixelFormatARB typedef
	typedef BOOL (WINAPI * WGLCHOOSEPIXELFORMATARB_T) (HDC, const int *, const FLOAT *, UINT, int *, UINT *);
	// wglGetPixelFormatAttribivARB typedef
	typedef BOOL (WINAPI * WGLGETPIXELFORMATATTRIBIVARB_T) (HDC, int, int, UINT, const int *, int *);
	// wglGetExtensionStringEXT typedef
	typedef const char *(APIENTRY * WGLGETEXTENSIONSSTRINGEXT_T)( void );
	// wglGetExtensionStringARB typedef
	typedef const char *(APIENTRY * WGLGETEXTENSIONSSTRINGARB_T)( HDC );
	typedef HGLRC (APIENTRY * WGLCREATECONTEXTATTRIBSARB_T)( HDC, HGLRC, const int * );

#elif OG_LINUX
	// glXSwapIntervalSGI typedef (X11 buffer-swap interval control)
	typedef int (* GLXSWAPINTERVALSGI_T) ( int interval );
	// glxGetExtensionStringARB typedef
	typedef GLXContext (* GLXCREATECONTEXTATTRIBSARB_T)( Display *dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int *attrib_list );
	typedef std::map<xcb_window_t, WindowEx *> WinEvtMap;
	typedef std::pair<xcb_window_t, WindowEx *> WinEvtPair;
#elif OG_MACOS_X
	#warning "Need MacOS here FIXME"
#endif

	typedef std::map<WPARAM, int> KeyMap;
	typedef std::pair<WPARAM, int> KeyPair;

	bool StringInExtensionString( const char *string, const char *extensions );

	class WindowEvent {
	public:
		WindowEvent( WindowEx *window, uInt msg, uInt wParam, uInt lParam ) {
			OG_ASSERT( window != NULL );
			this->window = window;
			this->msg = msg;
			this->wParam = wParam;
			this->lParam = lParam;
		}
		void Execute( void );
	private:
		WindowEx *	window;
		uInt		msg, wParam, lParam;
	};

#if OG_WIN32
	class Win32MessageThread : public Thread {
	public:
		Win32MessageThread() {}

	protected:
		void	WakeUp( void );
		void	Run( void );
	};
#endif

	class Platform {
	public:
		Platform();

		bool		Init( void );
		void		Shutdown( void );

		// These are only to be called by the user and the manager itself
	protected:
		bool		InitMonitors( void );
		void		ShutdownMonitors( void );
		void		InitInput( void );
		void		ShutdownInput( void );

		Window *	NewWindow( const WindowConfig *windowConfig );
		void		DeleteWindow( Window *wnd );

		// Some internal public variables and functions
	public:
		bool		initialized;
		int			GLVersion[3];
		bool		supportsMultiSampling;
		List<byte>	multiSampleValues;

		List<WindowEx *> windowList;
		List<MonitorEx *> monitorList;

		WindowEx	*activeWindow;

 		KeyMap		keypadMap;
		KeyMap		keyMap;
		DictEx<int>	keyStringMap;

		// windows only supports one gammaramp for all monitors( or is it just one for the primary ? )
		bool		hasGammaRamp;
		short		storedGammaRamp[3][256];

		// Rest is platform specific
#if OG_WIN32
		bool		DoSilentVideoTest( void );
		bool		wglExtensionSupported( HDC hDC, const char *extension );
		bool		wglInitExtensions( HDC hDC );
		void		KeyEvent( WindowEx *window, WPARAM wParam, LPARAM lParam, bool down );
		bool		WndInputCallback( WindowEx *window, UINT uMsg, WPARAM wParam, LPARAM lParam );

		HINSTANCE	hInstance;
		ATOM		classAtom;
		HHOOK		keyboardHook;
		bool		disableSysKeys;

		// Thread data
		Thread *	messageThread;
		Condition	newWindowEndEvent;
		LockFreeQueue<WindowEvent> eventQueue;

		// Platform specific extensions (context specific)
		WGLSWAPINTERVALEXT_T			wglSwapIntervalEXT;
		WGLCHOOSEPIXELFORMATARB_T		wglChoosePixelFormatARB;
		WGLGETPIXELFORMATATTRIBIVARB_T	wglGetPixelFormatAttribivARB;
		WGLGETEXTENSIONSSTRINGEXT_T		wglGetExtensionsStringEXT;
		WGLGETEXTENSIONSSTRINGARB_T		wglGetExtensionsStringARB;
		WGLCREATECONTEXTATTRIBSARB_T	wglCreateContextAttribsARB;
#elif OG_LINUX
 		WinEvtMap		windowEventMap;
		int		eventBase;
		int		errorBase;

		Display *			display;
		xcb_connection_t *	connection;
		int					defaultScreen;	//! @todo	do we need this here ?
		xcb_screen_t *		screen;

		// Platform specific extensions (context specific)
		GLXSWAPINTERVALSGI_T			glXSwapIntervalSGI;
		GLXCREATECONTEXTATTRIBSARB_T	glXCreateContextAttribsARB;
#elif OG_MACOS_X
	#warning "Need MacOS here FIXME"
#endif
	};
	extern Platform Mgr;
}

#endif
