/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2002-2006 Camilla Berglund, (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Win32 Manager Code
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

#ifdef OG_WIN32
#include <shlobj.h>
#include <algorithm>
#include <process.h>

#define WGL_DRAW_TO_WINDOW_ARB		0x2001
#define WGL_SUPPORT_OPENGL_ARB		0x2010
#define WGL_ACCELERATION_ARB		0x2003
#define WGL_FULL_ACCELERATION_ARB	0x2027
#define WGL_SAMPLES_ARB				0x2042

namespace og {

Platform Mgr;

/*
==============================================================================

  Win32 helper functions

==============================================================================
*/
Platform::Platform() {
	initialized = false;
	GLVersion[0] = GLVersion[1] = GLVersion[2] = -1;

	wglSwapIntervalEXT				= NULL;
	wglChoosePixelFormatARB			= NULL;
	wglGetPixelFormatAttribivARB	= NULL;
	wglGetExtensionsStringEXT		= NULL;
	wglGetExtensionsStringARB		= NULL;
	wglCreateContextAttribsARB		= NULL;
	supportsMultiSampling			= false;

	activeWindow					= NULL;

	messageThread					= NULL;
	keyboardHook					= NULL;
	disableSysKeys					= false;

	hasGammaRamp					= false;
}

bool Platform::Init( void ) {
	// Get desktop gammaramp, so we can restore it later
	if ( !hasGammaRamp ) {
		HDC hDC = GetDC( NULL );
		hasGammaRamp = GetDeviceGammaRamp( hDC, storedGammaRamp ) == TRUE;
		ReleaseDC( NULL, hDC );
	}
	// Init all monitors
	if ( !InitMonitors() )
		return false;

	InitInput();

	if ( !messageThread ) {
		messageThread = new Win32MessageThread;
		messageThread->Start("Win32MessageThread");
	}

	// With the Borland C++ compiler, we want to disable FPU exceptions
	// (this is recommended for OpenGL applications under Windows)
#ifdef __BORLANDC__
	_control87( MCW_EM, MCW_EM );
#endif

	// Retrieve GLOOT instance handle
	hInstance = GetModuleHandle( NULL );

	// Create window class
	WNDCLASS wc;
	wc.style			= CS_HREDRAW|CS_VREDRAW|CS_OWNDC;	// Redraw on...
	wc.lpfnWndProc		= (WNDPROC)WindowEx::WndCallback;	// Message handler
	wc.cbClsExtra		= 0;								// No extra class data
	wc.cbWndExtra		= 0;								// No extra window data
	wc.hInstance		= hInstance;						// Set instance
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );	// Load arrow pointer
	wc.hbrBackground	= NULL;								// No background
	wc.lpszMenuName		= NULL;								// No menu
	wc.lpszClassName	= GLOOT_WNDCLASSNAME;				// Set class name

	// Load user-provided icon if available
	wc.hIcon = LoadIcon( hInstance, L"GLOOT_ICON" );
	if( !wc.hIcon )
		wc.hIcon = LoadIcon( NULL, IDI_WINLOGO );

	// Register the window class
	classAtom = RegisterClass( &wc );
	if( !classAtom )
		return false;

	if ( !DoSilentVideoTest() ) {
		// Unregister window class
		UnregisterClass( GLOOT_WNDCLASSNAME, hInstance );
		classAtom = 0;
		return false;
	}

	timeBeginPeriod( 1 );
	initialized = true;
	return true;
}

void Platform::Shutdown( void ) {
	if( keyboardHook != NULL ) {
		UnhookWindowsHookEx( keyboardHook );
		keyboardHook = NULL;
	}
	ShutdownInput();
	ShutdownMonitors();

	// Terminate message thread
	if ( messageThread ) {
		messageThread->Stop();
		messageThread = NULL;
	}

	WindowEvent *evt;
	while( (evt=Mgr.eventQueue.Consume()) != NULL )
		delete evt;

	if( classAtom ) {
		// Unregister window class
		UnregisterClass( GLOOT_WNDCLASSNAME, hInstance );
		classAtom = 0;
	}

	timeEndPeriod( 1 );
	initialized = false;
}

bool Platform::wglExtensionSupported( HDC hDC, const char *extension ) {
	// Try wglGetExtensionsStringEXT
	if( wglGetExtensionsStringEXT != NULL ) {
		const char *extensions = (const char *) wglGetExtensionsStringEXT();
		if( extensions && StringInExtensionString( extension, extensions ) )
			return true;
	}

	// Try wglGetExtensionsStringARB
	if( wglGetExtensionsStringARB != NULL ) {
		const char *extensions = (const char *) wglGetExtensionsStringARB( hDC );
		if( extensions != NULL && StringInExtensionString( extension, extensions ) )
			return true;
	}
	return false;
}

bool Platform::wglInitExtensions( HDC hDC ) {
	// Initialize WGL-specific OpenGL extensions
	wglGetExtensionsStringEXT = (WGLGETEXTENSIONSSTRINGEXT_T) wglGetProcAddress( "wglGetExtensionsStringEXT" );
	if( !wglGetExtensionsStringEXT ) {
		// Try wglGetExtensionsStringARB
		wglGetExtensionsStringARB = (WGLGETEXTENSIONSSTRINGARB_T) wglGetProcAddress( "wglGetExtensionsStringARB" );
		if( !wglGetExtensionsStringARB )
			return false;
	}

	// Try multiSampling support.
	supportsMultiSampling = wglExtensionSupported( hDC, "WGL_ARB_multisample" );
	if( wglExtensionSupported( hDC, "WGL_EXT_swap_control" ) )
		wglSwapIntervalEXT = (WGLSWAPINTERVALEXT_T)wglGetProcAddress( "wglSwapIntervalEXT" );

	if( wglExtensionSupported( hDC, "WGL_ARB_create_context" ) )
		wglCreateContextAttribsARB = (WGLCREATECONTEXTATTRIBSARB_T)wglGetProcAddress( "wglCreateContextAttribsARB" );

	if( wglExtensionSupported( hDC, "WGL_ARB_pixel_format" ) ) {
		wglChoosePixelFormatARB = (WGLCHOOSEPIXELFORMATARB_T) wglGetProcAddress( "wglChoosePixelFormatARB" );
		wglGetPixelFormatAttribivARB = (WGLGETPIXELFORMATATTRIBIVARB_T) wglGetProcAddress( "wglGetPixelFormatAttribivARB" );
	}
	return true;
}

bool Platform::DoSilentVideoTest( void ) {
	// Create invisible window
	HWND hWnd = CreateWindowEx( WS_EX_TOOLWINDOW, GLOOT_WNDCLASSNAME, L"GLOOT Test", WS_POPUP,
								0, 0, 10, 10, NULL, NULL, hInstance, NULL );
	if( !hWnd )
		return false;

	// Get a device context
	HDC hDC = GetDC( hWnd );
	if( !hDC )
		return false;

	// Set simple pixel format
	PIXELFORMATDESCRIPTOR pfd;
	memset( &pfd, 0, sizeof(PIXELFORMATDESCRIPTOR) );
	pfd.nSize			= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion		= 1;
	pfd.dwFlags			= PFD_DRAW_TO_WINDOW |PFD_SUPPORT_OPENGL |PFD_DEPTH_DONTCARE;

	pfd.iPixelType		= PFD_TYPE_RGBA;		// Request an RGBA format
	pfd.cColorBits		= 24;					// Color bits (ex. alpha)
	pfd.cRedBits		= 8;					// Red bits
	pfd.cGreenBits		= 8;					// Green bits
	pfd.cBlueBits		= 8;					// Blue bits
	pfd.cAlphaBits		= 0;					// Alpha bits
	pfd.cDepthBits		= 8;					// Depth buffer bits
	pfd.cStencilBits	= 8;					// Stencil buffer bits
	pfd.cAuxBuffers		= 8;					// No. of aux buffers
	pfd.iLayerType		= PFD_MAIN_PLANE;		// Drawing layer: main
	pfd.bReserved		= 0;					// (reserved)
	pfd.dwLayerMask		= 0;					// Ignored
	pfd.dwVisibleMask	= 0;
	pfd.dwDamageMask	= 0;


	// Find a matching pixel format
	int PixelFormat = ChoosePixelFormat( hDC, &pfd );
	if( !PixelFormat )
		return false;

	// Get actual pixel format description
	if( !DescribePixelFormat( hDC, PixelFormat, sizeof(pfd), &pfd ) )
		return false;

	// Set the pixel-format
	if ( !SetPixelFormat( hDC, PixelFormat, &pfd ) )
		return false;

	// Get a rendering context
	HGLRC hRC = wglCreateContext( hDC );
	if( !hRC )
		return false;

	// Activate the OpenGL rendering context
	if( !wglMakeCurrent( hDC, hRC ) )
		return false;

	// Init extensions
	if ( !wglInitExtensions( hDC ) )
		return false;

	// Find out the max number of multiSampels
	if( wglGetPixelFormatAttribivARB ){
		int attribs[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			0, 0 };

		const int MAX_PIXEL_FORMATS = 1024;
		int PixelFormats[MAX_PIXEL_FORMATS];
		UINT numFormats;
		if( !wglChoosePixelFormatARB( hDC, attribs, NULL, MAX_PIXEL_FORMATS, PixelFormats, &numFormats ) || numFormats <= 0 )
			return false;

		if ( supportsMultiSampling ) {
			const int attribs[] = { WGL_SAMPLES_ARB };
			int values[1];
			for( int i=0; i<numFormats; i++ ) {
				wglGetPixelFormatAttribivARB( hDC, PixelFormats[i], 0, 1, attribs, values);
				if ( multiSampleValues.Find( values[0] ) == -1 )
					multiSampleValues.Append( values[0] );
			}
			supportsMultiSampling = !multiSampleValues.IsEmpty();
		}
	}

	// Cleanup
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( hRC );
	ReleaseDC( hWnd, hDC );
	DestroyWindow( hWnd );
	return true;
}



void Gloot::Synchronize( void ) {
	WindowEvent *evt;
	while( (evt=Mgr.eventQueue.Consume()) != NULL ) {
		evt->Execute();
		delete evt;
	}
}

void Gloot::GetMousePos( int *x, int *y ) {
	POINT  pos;
	GetCursorPos( &pos );
	if ( x )
		*x = pos.x;
	if ( y )
		*y = pos.y;
}

void Gloot::SetMousePos( int x, int y ) {
	SetCursorPos( x, y );
}

void Gloot::DisableSystemKeys( bool disable ) {
	Mgr.disableSysKeys = disable;
}

}

int ParseCommandLine( char *lpCmdLine, char *argv[] )  {
	int argc = 0;
	bool inQuote;

	// We can use the lpCmdLine buffer, since it's long enough and writable
	char *write = argv ? lpCmdLine : NULL;
	for( int i=0; lpCmdLine[i]; i++ ) {
		// Skip whitespaces
		for( ; isspace(lpCmdLine[i]); i++ ) {
			if ( !lpCmdLine[i] )
				return argc;
		}
		if ( write )
			argv[argc] = write;
		argc++;

		// Parse until arg end
		inQuote = false;
		for( ; lpCmdLine[i]; i++ ) {
			// Quoted strings will be added entirely, but without the quote chars
			if ( lpCmdLine[i] == '\"' ) {
				inQuote = !inQuote;
				continue;
			}
			if ( inQuote || !isspace( lpCmdLine[i] ) ) {
				if ( write )
					*(write++) = lpCmdLine[i];
				continue;
			}
			if ( write )
				*(write++) = '\0';
			break;
		}
	}
	// Make sure the last arg is terminated
	if ( write )
		*write = '\0';
	return argc;
}

int main( int argc, char *argv[] );
int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int ) {
	og::String strCommandLine;
	strCommandLine.FromWide( GetCommandLineW() );

	// One call to count argc, one to fill argv
	int argc = ParseCommandLine( strCommandLine.raw_ptr(), NULL );
	char **argv = new char *[argc * sizeof(char *)];
	ParseCommandLine( strCommandLine.raw_ptr(), argv );

	int result = main( argc, argv );
    delete[] argv;
	return result;
}

#endif
