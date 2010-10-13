/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2002-2006 Camilla Berglund, (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Win32 Window Code
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

//! @todo: handle pos, size, etc calls from the user while in fullscreen
#ifdef OG_WIN32

#include <math.h>

#ifndef HID_USAGE_PAGE_GENERIC
	#define HID_USAGE_PAGE_GENERIC	((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
	#define HID_USAGE_GENERIC_MOUSE	((USHORT) 0x02)
#endif

namespace og {

#define WGL_DRAW_TO_WINDOW_ARB		0x2001
#define WGL_SUPPORT_OPENGL_ARB		0x2010
#define WGL_ACCELERATION_ARB		0x2003
#define WGL_FULL_ACCELERATION_ARB	0x2027
#define WGL_DOUBLE_BUFFER_ARB		0x2011
#define WGL_STEREO_ARB				0x2012
#define WGL_COLOR_BITS_ARB			0x2014
#define WGL_RED_BITS_ARB			0x2015
#define WGL_GREEN_BITS_ARB			0x2017
#define WGL_BLUE_BITS_ARB			0x2019
#define WGL_ALPHA_BITS_ARB			0x201B
#define WGL_ACCUM_BITS_ARB			0x201D 
#define WGL_ACCUM_RED_BITS_ARB		0x201E 
#define WGL_ACCUM_GREEN_BITS_ARB	0x201F 
#define WGL_ACCUM_BLUE_BITS_ARB		0x2020 
#define WGL_ACCUM_ALPHA_BITS_ARB	0x2021 
#define WGL_DEPTH_BITS_ARB			0x2022
#define WGL_STENCIL_BITS_ARB		0x2023
#define WGL_AUX_BUFFERS_ARB			0x2024 
#define WGL_SAMPLE_BUFFERS_ARB		0x2041
#define WGL_SAMPLES_ARB				0x2042

enum {
	WM_MODE_CHANGE = WM_USER,
	WM_RESIZEABLE_CHANGE,
	WM_ICONIFY_CHANGE,
	WM_MOUSE_LOCK,
	WM_SET_TITLE,
	WM_CREATE_NEW_WINDOW,

	WM_WAKE_UP
};

LRESULT CALLBACK glootKeyboardHook( int nCode, WPARAM wParam, LPARAM lParam );

void Win32MessageThread::WakeUp( void ) {
	PostThreadMessage( id, WM_WAKE_UP, 0, 0 );
}

void Win32MessageThread::Run( void ) {
	Mgr.keyboardHook = SetWindowsHookEx( WH_KEYBOARD_LL, glootKeyboardHook, Mgr.hInstance, 0 );
	while( !selfDestruct ) {
		// Check for new window messages
		MSG msg;
		while( GetMessage( &msg, NULL, 0, 0 ) ) {
			if ( msg.message == WM_WAKE_UP )
				break;
			if ( msg.message == WM_CREATE_NEW_WINDOW ) {
				OG_ASSERT( msg.wParam );
				WindowEx *window = (WindowEx*)msg.wParam;
				window->Open();
				window = NULL;
				Mgr.newWindowEndEvent.Signal();
			}
			DispatchMessage( &msg );
			TranslateMessage( &msg );
		}
	}
}

WindowEx::WindowEx() {
	memset( &windowConfig, 0, sizeof(WindowConfig) );
	isOpen			= false;
	isClosing		= false;
	hasFocus		= false;
	isIconified		= false;
	vsyncEnabled	= false;

	hDC				= NULL;
	hRC				= NULL;
	hWnd			= NULL;
	dwStyle			= 0;
	dwExStyle		= 0;

	fsMonitor		= NULL;

	mouseUseRaw		= true;
	mouseLock		= false;
	oldMouseLock	= -1;
	ignoreSizeMove	= false;
}

void *WindowEx::GetProcAddress( const char *procname ) const {
	if( !isOpen )
		return NULL;
	return (void *) wglGetProcAddress( procname ); 
}

WindowEx *WindowEx::NewWindow( const WindowConfig *windowCfg ) {
	WindowEx *wnd = new WindowEx;
	wnd->SetWindowConfig( windowCfg );

	OG_ASSERT( Mgr.messageThread != NULL );
	PostThreadMessage( Mgr.messageThread->GetId(), WM_CREATE_NEW_WINDOW, (WPARAM)wnd, 0 );
	Mgr.newWindowEndEvent.Wait( INFINITE );
	return wnd;
}

// Window Creation
bool WindowEx::Create( void ) {
	isOpen = false;

	hDC = NULL;
	hRC = NULL;

	// Set window size to true requested size (adjust for window borders)
	RECT rect = { 0, 0, windowConfig.width-1, windowConfig.height-1 };

	// Adjust according to window styles
	AdjustWindowRectEx( &rect, dwStyle, FALSE, dwExStyle );

	// Calculate width and height of full window
	int full_width = rect.right-rect.left+1;
	int full_height = rect.bottom-rect.top+1;

	// Adjust window position to working area (e.g. if the task bar is at
	// the top of the display). Fullscreen windows are always opened in
	// the upper left corner regardless of the desktop working area.
	RECT wa;
	SystemParametersInfo( SPI_GETWORKAREA, 0, &wa, 0 );
	int x = wa.left + windowConfig.x;
	int y = wa.top + windowConfig.y;

	DynBuffer<wchar_t> strTitle;
	String::ToWide( windowConfig.title, strTitle );

	// Create window
	hWnd = CreateWindowEx(
				dwExStyle,			// Extended style
				GLOOT_WNDCLASSNAME,	// Class name
				strTitle.data,		// Window title
				dwStyle,			// Defined window style
				x, y,				// Window position
				full_width,			// Decorated window width
				full_height,		// Decorated window height
				NULL,				// No parent window
				NULL,				// No menu
				Mgr.hInstance,		// hInstance
				this );				// pass this to WM_CREATE

	if( !hWnd )
		return false;

	// Get a device context
	hDC = GetDC( hWnd );
	if( !hDC )
		return false;

	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; 
	Rid[0].dwFlags = RIDEV_INPUTSINK;   
	Rid[0].hwndTarget = hWnd;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

	isOpen = true;
	return true;
}

// Window Creation
bool WindowEx::CreateContext( void ) {
	isOpen = false;

	// Choose pixel format
	PIXELFORMATDESCRIPTOR pfd;
	int PixelFormat = Mgr.wglChoosePixelFormatARB ? ChoosePixelFormatARB() : ChoosePixelFormat( &pfd );
	if ( !PixelFormat )
		return false;

	// Get actual pixel format description
	if( !DescribePixelFormat( hDC, PixelFormat, sizeof(pfd), &pfd ) )
		return false;

	// Set the pixel-format
	if( !SetPixelFormat( hDC, PixelFormat, &pfd ) )
		return false;

	// Get a rendering context
	HGLRC hRC_simple = wglCreateContext( hDC );
	if( !hRC_simple )
		return false;

	// Try creating an advanced context.
	if ( !Mgr.wglCreateContextAttribsARB )
		hRC = hRC_simple;
	else {
		if ( !windowConfig.sharedContext )
			hRC = Mgr.wglCreateContextAttribsARB( hDC, 0, windowConfig.contextAttribs );
		else {
			WindowEx *sharedContext = static_cast<WindowEx *>(windowConfig.sharedContext);
			hRC = Mgr.wglCreateContextAttribsARB( hDC, sharedContext->hRC, windowConfig.contextAttribs );
		}
		if( !hRC ) {
#if 0
			// Fall back to an OpenGL 3.0 context.
			attribList[3] = 0;
			hRC = wglCreateContextAttribsARB( hDC, 0, attribList );

			if( !hRC )
#endif
				hRC = hRC_simple;
		}
		else {
			// we got an advanced one, delete the simple one.
			wglMakeCurrent( NULL, NULL );
			wglDeleteContext( hRC_simple );
		}
	}
	if ( hRC == hRC_simple && windowConfig.sharedContext ) {
		WindowEx *sharedContext = static_cast<WindowEx *>(windowConfig.sharedContext);
		wglShareLists( hRC, sharedContext->hRC );
	}

	// Activate the OpenGL rendering context
	isOpen = wglMakeCurrent( hDC, hRC ) == TRUE;

	// Start by clearing the front buffer to black (avoid ugly desktop remains in our OpenGL window)
	glClear( GL_COLOR_BUFFER_BIT );
	::SwapBuffers( hDC );

	// Get window parameters (such as color buffer bits etc)
	RefreshWindowParams();

	return isOpen;
}

void WindowEx::Destroy( void ) {
	// Delete the rendering context
	if( hRC ){
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( hRC );
		hRC = NULL;
	}

	// Release the device context
	if( hDC ) {
		ReleaseDC( hWnd, hDC );
		hDC = NULL;
	}

	// Destroy the window
	if( hWnd ) {
		ShowWindow( hWnd, SW_HIDE );
		SetWindowLongPtr( hWnd, GWL_USERDATA, NULL );
		DestroyWindow( hWnd );
		hWnd = NULL;
	}
}

void WindowEx::UpdateStyles( bool fullScreen ) {
	if ( fullScreen ) {
		dwStyle = WS_POPUP;
		dwExStyle = WS_EX_APPWINDOW;
	} else {
		// Set common window styles
		dwStyle		= WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		dwExStyle	= WS_EX_APPWINDOW;

		if( !(windowConfig.flags & WF_NORESIZE) ) {
			dwStyle		|= WS_MAXIMIZEBOX | WS_SIZEBOX;
			dwExStyle	|= WS_EX_WINDOWEDGE;
		}
	}
	if ( hWnd != NULL ) {
		ignoreSizeMove = true;
		SetWindowLong( hWnd, GWL_STYLE, dwStyle );
		SetWindowLong( hWnd, GWL_EXSTYLE, dwExStyle );
		ignoreSizeMove = false;
	}
}

bool WindowEx::Open( void ) {
	UpdateStyles( false );

	// Get the best multiSamples match
	if( Mgr.wglChoosePixelFormatARB && Mgr.supportsMultiSampling && windowConfig.multiSamples > 0 ) {
		byte value, best = 0;
		int max = Mgr.multiSampleValues.Num();
		for( int i=0; i<max; i++ ) {
			value = Mgr.multiSampleValues[i];
			if ( value == windowConfig.multiSamples ) {
				best = windowConfig.multiSamples;
				break;
			}
			if ( value > best )
				best = value;
		}
		windowConfig.multiSamples = best;
	}
	else
		windowConfig.multiSamples = 0;

	// Create the window
	if( !Create() ) {
		Close();
		return false;
	}

	SetForegroundWindow( hWnd );
	SetFocus( hWnd );

	return true;
}

int WindowEx::ChoosePixelFormat( PIXELFORMATDESCRIPTOR *pfd ) {
	// Set required pixel format
	pfd->nSize			= sizeof(PIXELFORMATDESCRIPTOR);
	pfd->nVersion		= 1;
	pfd->dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL; // Draw to window & Support OpenGL
	pfd->iPixelType		= PFD_TYPE_RGBA;						// Request an RGBA format
	pfd->cColorBits		= (BYTE) (windowConfig.rgbaBits[0] + windowConfig.rgbaBits[1] + windowConfig.rgbaBits[2]); // Color bits (ex. alpha)
	pfd->cRedBits		= (BYTE) windowConfig.rgbaBits[0];		// Red bits
	pfd->cGreenBits		= (BYTE) windowConfig.rgbaBits[1];		// Green bits
	pfd->cBlueBits		= (BYTE) windowConfig.rgbaBits[2];		// Blue bits
	pfd->cAlphaBits		= (BYTE) windowConfig.rgbaBits[3];		// Alpha bits
	pfd->cAccumBits		= (BYTE) (windowConfig.rgbaAccumBits[0] + windowConfig.rgbaAccumBits[1] + windowConfig.rgbaAccumBits[2] + windowConfig.rgbaAccumBits[3]); // Accum. bits
	pfd->cAccumRedBits	= (BYTE) windowConfig.rgbaAccumBits[0];	// Accumulation red bits
	pfd->cAccumGreenBits= (BYTE) windowConfig.rgbaAccumBits[1];	// Accumulation green bits
	pfd->cAccumBlueBits	= (BYTE) windowConfig.rgbaAccumBits[2];	// Accumulation blue bits
	pfd->cAccumAlphaBits= (BYTE) windowConfig.rgbaAccumBits[3];	// Accumulation alpha bits
	pfd->cDepthBits		= (BYTE) windowConfig.depthBits;		// Depth buffer bits
	pfd->cStencilBits	= (BYTE) windowConfig.stencilBits;		// Stencil buffer bits
	pfd->cAuxBuffers	= (BYTE) windowConfig.auxBuffers;		// No. of aux buffers
	pfd->iLayerType		= PFD_MAIN_PLANE;						// Drawing layer: main

	// We do not need a depth buffer
	if( windowConfig.depthBits <= 0 )
		pfd->dwFlags |= PFD_DEPTH_DONTCARE;

	// Request doublebuffer
	if ( windowConfig.flags & WF_DOUBLEBUFFER )
		pfd->dwFlags |= PFD_DOUBLEBUFFER;

	// Request stereo
	if( windowConfig.flags & WF_STEREO )
		pfd->dwFlags |= PFD_STEREO;

	// Find a matching pixel format
	return ::ChoosePixelFormat( hDC, pfd );
}

int WindowEx::ChoosePixelFormatARB( void ) {
	int count = 0;
	int attribs[128];
#define SetWGLAttribute( name, value ) { attribs[ count++ ] = name; attribs[ count++ ] = value; }

	SetWGLAttribute( WGL_DRAW_TO_WINDOW_ARB,	GL_TRUE );
	SetWGLAttribute( WGL_ACCELERATION_ARB,		WGL_FULL_ACCELERATION_ARB );
	SetWGLAttribute( WGL_SUPPORT_OPENGL_ARB,	GL_TRUE );
	SetWGLAttribute( WGL_COLOR_BITS_ARB,		windowConfig.rgbaBits[0] + windowConfig.rgbaBits[1] + windowConfig.rgbaBits[2] );
	SetWGLAttribute( WGL_RED_BITS_ARB,			windowConfig.rgbaBits[0] );
	SetWGLAttribute( WGL_GREEN_BITS_ARB,		windowConfig.rgbaBits[1] );
	SetWGLAttribute( WGL_BLUE_BITS_ARB,			windowConfig.rgbaBits[2] );
	SetWGLAttribute( WGL_ALPHA_BITS_ARB,		windowConfig.rgbaBits[3] );
	SetWGLAttribute( WGL_DEPTH_BITS_ARB,		windowConfig.depthBits );
	SetWGLAttribute( WGL_STENCIL_BITS_ARB,		windowConfig.stencilBits );
	SetWGLAttribute( WGL_AUX_BUFFERS_ARB,		windowConfig.auxBuffers );

	if( windowConfig.rgbaAccumBits[0] || windowConfig.rgbaAccumBits[1] ||windowConfig. rgbaAccumBits[2] || windowConfig.rgbaAccumBits[3] ) {
		SetWGLAttribute( WGL_ACCUM_BITS_ARB, windowConfig.rgbaAccumBits[0] + windowConfig.rgbaAccumBits[1] + windowConfig.rgbaAccumBits[2] + windowConfig.rgbaAccumBits[3] );
		SetWGLAttribute( WGL_ACCUM_RED_BITS_ARB, windowConfig.rgbaAccumBits[0] );
		SetWGLAttribute( WGL_ACCUM_GREEN_BITS_ARB, windowConfig.rgbaAccumBits[1] );
		SetWGLAttribute( WGL_ACCUM_BLUE_BITS_ARB, windowConfig.rgbaAccumBits[2] );
		SetWGLAttribute( WGL_ACCUM_ALPHA_BITS_ARB, windowConfig.rgbaAccumBits[3] );
	}

	if( windowConfig.flags & WF_STEREO )
		SetWGLAttribute( WGL_STEREO_ARB,			GL_TRUE );
	if( windowConfig.flags & WF_DOUBLEBUFFER )
		SetWGLAttribute( WGL_DOUBLE_BUFFER_ARB,		GL_TRUE );

	if( Mgr.supportsMultiSampling && windowConfig.multiSamples > 0 ) {
		SetWGLAttribute( WGL_SAMPLE_BUFFERS_ARB, 1 );
		SetWGLAttribute( WGL_SAMPLES_ARB, windowConfig.multiSamples );
	}

	SetWGLAttribute( 0, 0 );
#undef SetWGLAttribute

	int PixelFormat = 0;
	UINT numFormats;
	if( !Mgr.wglChoosePixelFormatARB( hDC, attribs, NULL, 1, &PixelFormat, &numFormats ) || numFormats <= 0 )
		return 0;
	return PixelFormat;
}

void WindowEx::RefreshWindowParams( void ) {
	// Obtain a detailed description of current pixel format
	int PixelFormat = ::GetPixelFormat( hDC );

	if( !Mgr.wglGetPixelFormatAttribivARB ) {
		PIXELFORMATDESCRIPTOR pfd;
		DescribePixelFormat( hDC, PixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd );

		// "Standard" window parameters
		windowConfig.rgbaBits[0]		= pfd.cRedBits;
		windowConfig.rgbaBits[1]		= pfd.cGreenBits;
		windowConfig.rgbaBits[2]		= pfd.cBlueBits;
		windowConfig.rgbaBits[3]		= pfd.cAlphaBits;
		windowConfig.depthBits			= pfd.cDepthBits;
		windowConfig.stencilBits		= pfd.cStencilBits;
		windowConfig.rgbaAccumBits[0]	= pfd.cAccumRedBits;
		windowConfig.rgbaAccumBits[1]	= pfd.cAccumGreenBits;
		windowConfig.rgbaAccumBits[2]	= pfd.cAccumBlueBits;
		windowConfig.rgbaAccumBits[3]	= pfd.cAccumAlphaBits;
		windowConfig.auxBuffers			= pfd.cAuxBuffers;
		windowConfig.multiSamples		= 0;

		if ( pfd.dwFlags & PFD_STEREO )
			windowConfig.flags |= WF_STEREO;
		else if ( windowConfig.flags & WF_STEREO )
			windowConfig.flags &= ~WF_STEREO;
	} else {
		const int attribs[] = {
			WGL_RED_BITS_ARB,
			WGL_GREEN_BITS_ARB,
			WGL_BLUE_BITS_ARB,
			WGL_ALPHA_BITS_ARB,
			WGL_DEPTH_BITS_ARB,
			WGL_STENCIL_BITS_ARB,
			WGL_ACCUM_RED_BITS_ARB,
			WGL_ACCUM_GREEN_BITS_ARB,
			WGL_ACCUM_BLUE_BITS_ARB,
			WGL_ACCUM_ALPHA_BITS_ARB,
			WGL_AUX_BUFFERS_ARB,
			WGL_SAMPLES_ARB,
			WGL_STEREO_ARB
		};

		const int numValues = sizeof(attribs) / sizeof(attribs[0]);
		int values[numValues];

		Mgr.wglGetPixelFormatAttribivARB( hDC, PixelFormat, 0, numValues, attribs, values);

		// "Standard" window parameters
		windowConfig.rgbaBits[0]		= values[1];
		windowConfig.rgbaBits[1]		= values[2];
		windowConfig.rgbaBits[2]		= values[3];
		windowConfig.rgbaBits[3]		= values[4];
		windowConfig.depthBits			= values[5];
		windowConfig.stencilBits		= values[6];
		windowConfig.rgbaAccumBits[0]	= values[7];
		windowConfig.rgbaAccumBits[1]	= values[8]; 
		windowConfig.rgbaAccumBits[2]	= values[9];
		windowConfig.rgbaAccumBits[3]	= values[10];
		windowConfig.auxBuffers		= values[11];
		windowConfig.multiSamples	= Mgr.supportsMultiSampling ? values[12] : 0;
		
		if ( values[13] != 0 )
			windowConfig.flags |= WF_STEREO;
		else if ( windowConfig.flags & WF_STEREO )
			windowConfig.flags &= ~WF_STEREO;
	}
}

LRESULT CALLBACK WindowEx::WndCallback( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	WindowEx *window;
	if ( uMsg == WM_CREATE ) {
		window = (WindowEx*)((CREATESTRUCT *)lParam)->lpCreateParams;
		if ( window )
			window->hWnd = hWnd;
		SetWindowLongPtr( hWnd, GWL_USERDATA, (LONG_PTR)window );
	} else 
		window = (WindowEx *)(LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA);

	if ( window ) {
		// catch ALT F4, otherwise it will be used as normal input.
		//! @todo	make this optional for the user to decide.
		if ( uMsg == WM_SYSKEYDOWN && wParam == VK_F4 ) {
			if ( window->fsMonitor == NULL )
				return window->WndCallback( uMsg, wParam, lParam );
		}
		// Try for input messages
		if ( Mgr.WndInputCallback( window, uMsg, wParam, lParam ) )
			return 0;
		return window->WndCallback( uMsg, wParam, lParam );
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT WindowEx::WndCallback( UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	// Handle certain window messages
	switch( uMsg ) {
		case WM_SET_TITLE:
			SetWindowText( hWnd, (LPCWSTR)wParam );
			return 0;
		case WM_MOUSE_LOCK:
			if ( mouseLock == (wParam != 0) )
				break;
			if ( wParam != 0 ) {
				RECT ClipWindowRect;

				// Hide cursor
				ShowCursor( FALSE );

				// Clip cursor to the window
				if( GetWindowRect( hWnd, &ClipWindowRect ) )
					ClipCursor( &ClipWindowRect );

				// Capture cursor to user window
				SetCapture( hWnd );
				mouseLock = true;
			} else {
				// Un-capture cursor
				ReleaseCapture();

				// Disable cursor clipping
				ClipCursor( NULL );

				// Show cursor
				ShowCursor( TRUE );
				mouseLock = false;
			}
			return 0;
		case WM_MOUSEMOVE:
			if ( hasFocus && mouseLock && !mouseUseRaw ) {
				int x = (int)((short)LOWORD(lParam));
				int y = (int)((short)HIWORD(lParam));

				int center[2] = { windowConfig.width/2, windowConfig.height/2 };
				if( x != center[0] || y != center[1] ) {
					Mgr.eventQueue.Produce( new WindowEvent( this, uMsg, x - center[0], y - center[1] ) );
				}
			}
			return 0;
		case WM_INPUT: 
			if ( hasFocus && mouseLock && mouseUseRaw ) {
				UINT dwSize = 40;
				static BYTE lpb[40];
				GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

				RAWINPUT* raw = (RAWINPUT*)lpb;
				if ( raw->header.dwType == RIM_TYPEMOUSE ) {
					if ( raw->data.mouse.lLastX != 0 || raw->data.mouse.lLastY != 0 )
						Mgr.eventQueue.Produce( new WindowEvent( this, WM_MOUSEMOVE, raw->data.mouse.lLastX, raw->data.mouse.lLastY ) );
				}
			}
			break;
		case WM_MODE_CHANGE: {
			MonitorEx *monitor = (MonitorEx *)wParam;
			if ( monitor == fsMonitor )
				return 0;
			// Changed to fullscreen
			if ( monitor ) {
				fsMonitor = monitor;
				fsMonitor->SetMode( (const VideoMode *)lParam, this );

				// Change window style (no border, no titlebar, ...)
				UpdateStyles( true );

				// And resize it so that it fits the entire screen
				SetWindowPos( hWnd, HWND_TOP, fsMonitor->left, fsMonitor->top, fsMonitor->width, fsMonitor->height, SWP_FRAMECHANGED );
			} else {
				fsMonitor->Revert();
				fsMonitor = NULL;

				// Restore window styles
				UpdateStyles( false );

				// Restore original size and position
				SetSize( oldSize[0], oldSize[1] );
				SetPos( oldPos[0], oldPos[1] );
			}
			ShowWindow( hWnd, SW_SHOW );
			SetForegroundWindow( hWnd );
			SetFocus( hWnd );

			// window needs to be refreshed
			PostMessage( hWnd, WM_PAINT, 0, 0 );
			}
			return 0;

		case WM_RESIZEABLE_CHANGE:
			// resizeable
			if( wParam )
				windowConfig.flags |= WF_NORESIZE;
			else if ( windowConfig.flags & WF_NORESIZE )
				windowConfig.flags &= ~WF_NORESIZE;

			UpdateStyles( fsMonitor != NULL );

			//! @todo	may need to update size here ?
			return 0;

		case WM_ICONIFY_CHANGE:
			if ( wParam ) {
				// Unlock mouse
				if( mouseLock && oldMouseLock == -1 ) {
					oldMouseLock = mouseLock;
					SendMessage( hWnd, WM_MOUSE_LOCK, 0, 0 );
				}
				// Deactivate window
				CloseWindow( hWnd );
				SetFocus( NULL );
			} else {
				// Re-lock mouse
				if( oldMouseLock == 1 )
					SendMessage( hWnd, WM_MOUSE_LOCK, 1, 0 );
				oldMouseLock = -1;

				// Activate window
				OpenIcon( hWnd );
				ShowWindow( hWnd, SW_SHOW );
				SetFocus( hWnd );
			}
			return 0;

		// Intercept system commands (forbid certain actions/events)
		case WM_SYSCOMMAND:
			switch( wParam ) {
				// Screensaver trying to start or monitor trying to enter powersave?
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
					if( fsMonitor )
						return 0;
					break;
				// User trying to access application menu using ALT?
				case SC_KEYMENU:
					return 0;
			}
			break;

		// Window activate (iconification?)
		case WM_ACTIVATE: {
			bool Active = LOWORD(wParam) != WA_INACTIVE;
			bool Iconified = HIWORD(wParam) != 0;
			if ( isOpen ) {
				// Were we deactivated/iconified?
				if( (!Active || Iconified) && !isIconified ) {
					// If we are in fullscreen mode we need to iconify
					if( fsMonitor ) {
						// Minimize window
						CloseWindow( hWnd );
						fsMonitor->OnIconify();
					}
					if ( mouseLock && oldMouseLock == -1 ) {
						oldMouseLock = mouseLock;
						SendMessage( hWnd, WM_MOUSE_LOCK, 0, 0 );
					}
				}
				else if( Active || !Iconified ) {
					// If we are in fullscreen mode we need to maximize
					if( fsMonitor && isIconified ) {
						// Activate window
						OpenIcon( hWnd );
						ShowWindow( hWnd, SW_SHOW );
						SetFocus( hWnd );

						fsMonitor->OnUnIconify();
					}
					if( oldMouseLock == 1 )
						SendMessage( hWnd, WM_MOUSE_LOCK, 1, 0 );
					oldMouseLock = -1;
				}
			}
			Mgr.activeWindow = Active ? this : NULL;

			if ( hasFocus != Active ) {
				hasFocus = Active;
				Mgr.eventQueue.Produce( new WindowEvent( this, WM_ACTIVATE, 0, 0 ) );
			}
			if ( isIconified != Iconified ) {
				isIconified = Iconified;
				Mgr.eventQueue.Produce( new WindowEvent( this, WM_ICONIFY_CHANGE, 0, 0 ) );
			}

			}
			break;

		// Close Window
		case WM_CLOSE:
			Mgr.eventQueue.Produce( new WindowEvent( this, uMsg, 0, 0 ) );
			break;

		// Resize window
		case WM_SIZE:
			if ( !ignoreSizeMove )
				Mgr.eventQueue.Produce( new WindowEvent( this, uMsg, LOWORD(lParam), HIWORD(lParam) ) );
			break;

		// Move window
		case WM_MOVE:
			if ( !ignoreSizeMove )
				Mgr.eventQueue.Produce( new WindowEvent( this, uMsg, LOWORD(lParam), HIWORD(lParam) ) );
			break;

		// Window needs to be redrawn
		case WM_PAINT:
			Mgr.eventQueue.Produce( new WindowEvent( this, uMsg, 0, 0 ) );
			break;
	}

	// Pass all unhandled messages to DefWindowProc
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

void WindowEvent::Execute( void ) {
	if ( window->isClosing )
		return;

	// Handle certain window messages
	switch( msg ) {
		// Window activate (iconification?)
		case WM_ACTIVATE:
			// Call the user-supplied callback, if it exists
			if( window->windowConfig.listener )
				window->windowConfig.listener->OnFocusChange( window->hasFocus );
			break;

		case WM_ICONIFY_CHANGE:
			if( window->windowConfig.listener )
				window->windowConfig.listener->OnIconify( window->isIconified );
			break;

		// Close Window
		case WM_CLOSE:
			// Check if the program wants us to close the window
			if ( !window->windowConfig.listener || window->windowConfig.listener->OnClose( false ) )
				window->Close();
			break;

		// Resize window
		case WM_SIZE:
			if ( !(wParam == 0 && lParam == 0) ) {
				// Get the new size
				int w = wParam;
				int h = lParam;

				if ( w == window->windowConfig.width && h == window->windowConfig.height )
					break;

				window->windowConfig.width = w;
				window->windowConfig.height = h;

				if( window->mouseLock )
					window->UpdateCursorClip();
				// Call the user-supplied callback, if it exists
				if( window->windowConfig.listener )
					window->windowConfig.listener->OnSize( window->windowConfig.width, window->windowConfig.height );
			}
			break;

		// Move window
		case WM_MOVE:
			if( window->mouseLock )
				window->UpdateCursorClip();

			{
				// Get the new pos
				int x = wParam;
				int y = lParam;

				if ( x == 33536 && y == 33536 )
					break;
				if ( x == window->windowConfig.x && y == window->windowConfig.y )
					break;

				window->windowConfig.x = x;
				window->windowConfig.y = y;

				// Call the user-supplied callback, if it exists
				if( window->windowConfig.listener )
					window->windowConfig.listener->OnMove( x, y );
			}

			break;

		// Window needs to be redrawn
		case WM_PAINT:
			// Call the user-supplied callback, if it exists
			if( window->windowConfig.listener )
				window->windowConfig.listener->OnDamage();
			break;

			
		case WM_KEYDOWN:
			// Call the user-supplied callback, if it exists
			if( window->windowConfig.listener )
				window->windowConfig.listener->OnKeyEvent( wParam, ( lParam != 0 ) );
			break;

		case WM_CHAR:
			// Call the user-supplied callback, if it exists
			if( window->windowConfig.listener )
				window->windowConfig.listener->OnCharEvent( wParam );
			break;

		// Were any of the mouse-buttons pressed?
		case WM_LBUTTONDOWN:
			// Call the user-supplied callback, if it exists
			if( window->windowConfig.listener )
				window->windowConfig.listener->OnMouseButton( wParam, ( lParam != 0 ) );
			break;
		// Mouse wheel action?
		case WM_MOUSEWHEEL:
			// Call the user-supplied callback, if it exists
			if( window->windowConfig.listener )
				window->windowConfig.listener->OnMouseWheel( wParam );
			break;
		case WM_MOUSEMOVE:
			if ( window->hasFocus && window->mouseLock ) {
				// Call the user-supplied callback, if it exists
				if( window->windowConfig.listener )
					window->windowConfig.listener->OnMouseMove( wParam, -1 * lParam );
				
				int center[2] = { window->windowConfig.width/2, window->windowConfig.height/2 };
				int x, y;
				Gloot::GetMousePos( &x, &y );
				if ( x != center[0] || y != center[1] )
					Gloot::SetMousePos( window->windowConfig.width/2, window->windowConfig.height/2 );
			}
			break;
	}
}

bool WindowEx::MakeCurrent( void ) {
	// Activate the OpenGL rendering context
	return wglMakeCurrent( hDC, hRC ) != 0;
}

void WindowEx::PrepareFullScreen( void ) {
	// Store old position
	oldSize[0] = windowConfig.width;
	oldSize[1] = windowConfig.height;
	oldPos[0] = windowConfig.x;
	oldPos[1] = windowConfig.y;
}

void WindowEx::SetFullScreen( const VideoMode *mode, Monitor *monitor ) {
	PostMessage( hWnd, WM_MODE_CHANGE, (WPARAM)monitor, (LPARAM)mode );
}

void WindowEx::SetTitle( const char *title ) {
	static DynBuffer<wchar_t> strTitle;
	String::ToWide( title, strTitle );
	PostMessage( hWnd, WM_SET_TITLE, (WPARAM)strTitle.data, 0 );
}

void WindowEx::SetSize( int w, int h ) {
	// Don't do anything if the window size did not change
	if( !isOpen || fsMonitor )
		return;

	// Set window size to true requested size (adjust for window borders)
	RECT rect = { 0, 0, w-1, h-1 };

	// Adjust according to window styles
	AdjustWindowRectEx( &rect, dwStyle, FALSE, dwExStyle );

	SetWindowPos( hWnd, HWND_TOP, 0, 0, rect.right-rect.left+1, rect.bottom-rect.top+1, SWP_ASYNCWINDOWPOS|SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER );
}

void WindowEx::SetPos( int x, int y ) {
	if ( !fsMonitor ) {
		// Set window size to true requested size (adjust for window borders)
		RECT rect = { x, y, 100, 100 };

		// Adjust according to window styles
		AdjustWindowRectEx( &rect, dwStyle, FALSE, dwExStyle );

		SetWindowPos( hWnd, HWND_TOP, rect.left, rect.top, 0, 0, SWP_ASYNCWINDOWPOS|SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER );
	}
}

void WindowEx::Iconify( void ) {
	// Minimize window
	PostMessage( hWnd, WM_ICONIFY_CHANGE, 1, 0 );
}

void WindowEx::Restore( void ) {
	// Restore window
	PostMessage( hWnd, WM_ICONIFY_CHANGE, 0, 0 );
}

void WindowEx::SetResizable( bool value ) {
	if ( value != IsResizable() )
		PostMessage( hWnd, WM_RESIZEABLE_CHANGE, value, 0 );
}

void WindowEx::SetSwapinterval( bool value ) {
	if( Mgr.wglSwapIntervalEXT ) {
		Mgr.wglSwapIntervalEXT( value );
		vsyncEnabled = value;
	}
}

void WindowEx::SwapBuffers( void ) {
	// Update display-buffer
	::SwapBuffers( hDC );
}

Monitor *WindowEx::GetMonitor( void ) const {
	if ( fsMonitor )
		return fsMonitor;
	HMONITOR hMonitor = ::MonitorFromWindow( hWnd, MONITOR_DEFAULTTONEAREST );

	int max = Mgr.monitorList.Num();
	for( int i=0; i<max; i++ ) {
		if ( Mgr.monitorList[i]->IsHMonitor(hMonitor) )
			return Mgr.monitorList[i];
	}
	return NULL;
}

// mouse stuff
void WindowEx::LockMouse( bool locked ) {
	if( !isOpen )
		return;

	PostMessage( hWnd, WM_MOUSE_LOCK, locked, 0 );
}

void WindowEx::UpdateCursorClip( void ) {
	// If the mouse is locked, update the clipping rect
	RECT ClipWindowRect;
	if( GetWindowRect( hWnd, &ClipWindowRect ) )
		ClipCursor( &ClipWindowRect );
}

}

#endif
