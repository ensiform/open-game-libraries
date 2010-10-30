// ==============================================================================
//! @file
//! @brief	DirectInput Device
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2007-2010 Lusito Software
// ==============================================================================
//
// The Open Game Libraries.
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// ==============================================================================

#include "DeviceDI.h"
#include <Dbt.h>
#include <og/Shared/Thread/Thread.h>

#if OG_WIN32

namespace og {
namespace Fable {

bool InitDirectInput( void );
void ShutdownDirectInput( void );

bool InitXInput( void );
void ShutdownXInput( void );

HINSTANCE hInstance = NULL;
ATOM classAtom = 0;
HWND hWndInput = NULL;
const UINT WM_WAKE_UP = WM_USER;

#define FABLE_WNDCLASSNAME L"FableHidden"
const GUID GuidDevInterfaceHID = { 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } };

LRESULT CALLBACK FableWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	if ( uMsg == WM_DEVICECHANGE ) {
		if ( wParam == DBT_DEVICEARRIVAL ) {
			//! @todo	notify user
		} else if ( wParam == DBT_DEVICEREMOVECOMPLETE ) {
			//! @todo	notify user
		}
	}
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

/*
==============================================================================

  HiddenWindow

==============================================================================
*/
class HiddenWindow : public Thread {
public:
	HiddenWindow() {}
	~HiddenWindow() { Shutdown(); }

	bool	Init( void ) {
		// Retrieve GLOOT instance handle
		hInstance = GetModuleHandle( NULL );

		// Create window class
		WNDCLASS wc;
		wc.style			= 0;								// Style
		wc.lpfnWndProc		= (WNDPROC)FableWndProc;			// Message handler
		wc.cbClsExtra		= 0;								// No extra class data
		wc.cbWndExtra		= 0;								// No extra window data
		wc.hInstance		= hInstance;						// Set instance
		wc.hCursor			= LoadCursor( NULL, IDC_ARROW );	// Load arrow pointer
		wc.hbrBackground	= NULL;								// No background
		wc.lpszMenuName		= NULL;								// No menu
		wc.lpszClassName	= FABLE_WNDCLASSNAME;				// Set class name
		wc.hIcon			= LoadIcon( NULL, IDI_WINLOGO );	// Icon

		// Register the window class
		classAtom = RegisterClass( &wc );
		if( !classAtom ) {
			Shutdown();
			return false;
		}

		// Create invisible window
		hWndInput = CreateWindowEx( WS_EX_TOOLWINDOW, FABLE_WNDCLASSNAME, L"Fable Hidden", WS_POPUP,
										0, 0, 10, 10, NULL, NULL, hInstance, NULL );

		if ( !hWndInput ) {
			Shutdown();
			return false;
		}

		// Request notification on HID device plug in/out
		DEV_BROADCAST_DEVICEINTERFACE notificationFilter;
		ZeroMemory(&notificationFilter, sizeof(notificationFilter));

		notificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		notificationFilter.dbcc_classguid = GuidDevInterfaceHID;
		notificationFilter.dbcc_size = sizeof(notificationFilter);

		HDEVNOTIFY hDevNotify = RegisterDeviceNotification( hWndInput, &notificationFilter,
			DEVICE_NOTIFY_WINDOW_HANDLE );

		if( hDevNotify == NULL ) {
			Shutdown();
			return false;
		}
		return true;
	}

protected:
	void	WakeUp( void ) { PostThreadMessage( nativeId, WM_WAKE_UP, 0, 0 ); }
	void	Run( void ) {
		MSG msg;
		while( keepRunning ) {
			while( GetMessage( &msg, NULL, 0, 0 ) ) {
				if ( msg.message == WM_WAKE_UP )
					break;
				DispatchMessage( &msg );
				TranslateMessage( &msg );
			}
		}
	}

	void Shutdown( void ) {
		ShutdownXInput();
		ShutdownDirectInput();

		if ( hWndInput != NULL ) {
			DestroyWindow( hWndInput );
			hWndInput = NULL;
		}
		if( classAtom ) {
			// Unregister window class
			UnregisterClass( FABLE_WNDCLASSNAME, hInstance );
			classAtom = 0;
		}
	}
};

HiddenWindow *hiddenWnd = NULL;

/*
================
PlatformInit
================
*/
bool PlatformInit( void ) {
	if ( !hiddenWnd ) {
		hiddenWnd = new HiddenWindow;
		if ( !hiddenWnd->Start("HiddenWindow", true) ) {
			delete hiddenWnd;
			hiddenWnd = NULL;
			return false;
		}
#ifdef __MINGW32__
#warning "XInputChecker is not working with MinGW, fix it"
		if ( !InitDirectInput() ) {
#else
		if ( !InitXInput() || !InitDirectInput() ) {
#endif
			hiddenWnd->Stop();
			hiddenWnd = NULL;
			return false;
		}
	}
	return true;
}

/*
================
PlatformShutdown
================
*/
void PlatformShutdown( void ) {
	if ( hiddenWnd ) {
		hiddenWnd->Stop();
		hiddenWnd = NULL;
	}
}

}
}

#endif
