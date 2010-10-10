/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2002-2006 Camilla Berglund, (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Win32 Input Code
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

namespace og {

#ifndef WM_MOUSEWHEEL
 #define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
 #define WHEEL_DELTA 120
#endif

#ifndef WM_XBUTTONDOWN
 #define WM_XBUTTONDOWN 0x020B
#endif
#ifndef WM_XBUTTONUP
 #define WM_XBUTTONUP 0x020C
#endif
#ifndef XBUTTON1
 #define XBUTTON1 1
#endif
#ifndef XBUTTON2
 #define XBUTTON2 2
#endif

const int VK_EXTENDED_FLAG = 0x01000000;

LRESULT CALLBACK glootKeyboardHook( int nCode, WPARAM wParam, LPARAM lParam ) {
	// If not a keyboard event or we don't have an active window, allow it
	if( !Mgr.disableSysKeys || nCode != HC_ACTION || Mgr.activeWindow == NULL )
		return CallNextHookEx( Mgr.keyboardHook, nCode, wParam, lParam );

	// Check if it's a system key
	bool sysKey = false;
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT) lParam;
	switch( wParam ) {
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			switch( p->vkCode ) {
				// Alt + Esc || Ctrl + Esc
		case VK_ESCAPE:
			sysKey = (p->flags & LLKHF_ALTDOWN) != 0 || (GetKeyState(VK_CONTROL) & 0x8000) != 0;
			break;
			// Alt + F4
		case VK_F4:
			sysKey = (p->flags & LLKHF_ALTDOWN) != 0;
			break;
			// Left Win || Right Win || Menu
		case VK_LWIN:
		case VK_RWIN:
		case VK_APPS:
			sysKey = true;
			break;
			}
			break;

		default:
			break;
	}

	// No sysKey, allow it
	if( !sysKey )
		return CallNextHookEx( Mgr.keyboardHook, nCode, wParam, lParam );

	// Pass to active window message loop
	//! @todo I wonder if we can use this directly instead of going through PostMessage
	PostMessage( Mgr.activeWindow->GetHWND(), wParam, p->vkCode, 0 );
	return 1;
}

void Platform::KeyEvent( WindowEx *window, WPARAM wParam, LPARAM lParam, bool down ) {
	// Check which key was pressed or released
	switch( wParam ) {
		// The SHIFT keys require special handling
		case VK_SHIFT:
			if ( !down ) {
				// Special trick: release both shift keys on SHIFT up event
				Mgr.eventQueue.Produce( new WindowEvent( window, WM_KEYDOWN, Key::LSHIFT, 0 ) );
				Mgr.eventQueue.Produce( new WindowEvent( window, WM_KEYDOWN, Key::RSHIFT, 0 ) );
			} else {
				// Compare scan code for this key with that of VK_RSHIFT in
				// order to determine which shift key was pressed (left or right)
				if( MapVirtualKey( VK_RSHIFT, MAPVK_VK_TO_VSC ) == ((lParam & 0x01ff0000) >> 16) )
					Mgr.eventQueue.Produce( new WindowEvent( window, WM_KEYDOWN, Key::RSHIFT, 1 ) );
				else
					Mgr.eventQueue.Produce( new WindowEvent( window, WM_KEYDOWN, Key::LSHIFT, 1 ) );
			}
			return;

		// The CTRL keys require special handling
		case VK_CONTROL:
			// Is this an extended key (i.e. right key)?
			if( lParam & VK_EXTENDED_FLAG )
				Mgr.eventQueue.Produce( new WindowEvent( window, WM_KEYDOWN, Key::RCTRL, down ) );
			else {
				// Here is a trick: "Alt Gr" sends LCTRL, then RALT. We only
				// want the RALT message, so we try to see if the next message
				// is a RALT message. In that case, this is a false LCTRL!
				DWORD msg_time = GetMessageTime();
				MSG next_msg;
				if( PeekMessage( &next_msg, window->hWnd, 0, 0, PM_NOREMOVE ) ) {
					if( next_msg.message == WM_KEYDOWN || next_msg.message == WM_SYSKEYDOWN ) {
						// Next message is a RALT down message, which means that this is NOT a proper LCTRL message!
						if( next_msg.wParam == VK_MENU && (next_msg.lParam & VK_EXTENDED_FLAG) && next_msg.time == msg_time )
							return;
					}
				}
				Mgr.eventQueue.Produce( new WindowEvent( window, WM_KEYDOWN, Key::LCTRL, down ) );
			}
			return;

		// The ALT keys require special handling
		case VK_MENU:
			// Is this an extended key (i.e. right key)?
			Mgr.eventQueue.Produce( new WindowEvent( window, WM_KEYDOWN, (lParam & VK_EXTENDED_FLAG) ? Key::RALT : Key::LALT, down ) );
			return;

		// The ENTER keys require special handling
		case VK_RETURN:
			// Is this an extended key (i.e. keypad)?
			Mgr.eventQueue.Produce( new WindowEvent( window, WM_KEYDOWN, (lParam & VK_EXTENDED_FLAG) ? Key::KP_ENTER : Key::ENTER, down ) );
			return;
	}

	// Check for numeric keypad keys
	KeyMap::iterator it;
	if ( !(lParam & VK_EXTENDED_FLAG) ) {
		it = keypadMap.find( MapVirtualKey( HIWORD(lParam) & 0xFF, MAPVK_VSC_TO_VK) );
		if ( it != keypadMap.end() ) {
			Mgr.eventQueue.Produce( new WindowEvent( window, WM_KEYDOWN, it->second, down ) );
			return;
		}
	}

	// Special keys (non character keys)
	it = keyMap.find( wParam );
	if ( it != keyMap.end() ) {
		Mgr.eventQueue.Produce( new WindowEvent( window, WM_KEYDOWN, it->second, down ) );
		return;
	}

	// The rest (should be printable keys)
	// Convert to printable character (ISO-8859-1 or Unicode)
	wParam = MapVirtualKey( wParam, MAPVK_VK_TO_CHAR ) & 0x0000FFFF;
	if ( wParam != 0 ) {
		// Make sure that the character is uppercase
		wParam = (WPARAM) CharUpper( (LPWSTR) wParam );

		// Valid ISO-8859-1 character?
		if( (wParam >= 32 && wParam <= 126) || (wParam >= 160 && wParam <= 255) )
			Mgr.eventQueue.Produce( new WindowEvent( window, WM_KEYDOWN, wParam, down ) );
#if 0
		// unicode
		//! @todo	my tests with a greek keyboard layout still had values betwen 0 and 255
		// is this needed at all ?
		else if( (wParam >= 256) )
			Mgr.eventQueue.Produce( new WindowEvent( window, WM_KEYDOWN, wParam, down ) );
#endif
	}
}

bool Platform::WndInputCallback( WindowEx *window, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	if ( window->isClosing )
		return true;

	switch( uMsg ) {
		// Is a key being pressed?
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			// Translate and report key press
			KeyEvent( window, wParam, lParam, true );
			return true;

		// Is a key being released?
		case WM_KEYUP:
		case WM_SYSKEYUP:
			// Translate and report key release
			KeyEvent( window, wParam, lParam, false );
			return true;

		case WM_CHAR:
			Mgr.eventQueue.Produce( new WindowEvent( window, uMsg, wParam, 0 ) );
			return true;

		// Were any of the mouse-buttons pressed?
		case WM_LBUTTONDOWN:
			SetCapture( window->hWnd );
			Mgr.eventQueue.Produce( new WindowEvent( window, WM_LBUTTONDOWN, Mouse::Button1, 1 ) );
			return true;
		case WM_RBUTTONDOWN:
			SetCapture( window->hWnd );
			Mgr.eventQueue.Produce( new WindowEvent( window, WM_LBUTTONDOWN, Mouse::Button2, 1 ) );
			return true;
		case WM_MBUTTONDOWN:
			SetCapture( window->hWnd );
			Mgr.eventQueue.Produce( new WindowEvent( window, WM_LBUTTONDOWN, Mouse::Button3, 1 ) );
			return true;
		case WM_XBUTTONDOWN:
			if( HIWORD(wParam) == XBUTTON1 ) {
				SetCapture( window->hWnd );
				Mgr.eventQueue.Produce( new WindowEvent( window, WM_LBUTTONDOWN, Mouse::Button4, 1 ) );
			} else if( HIWORD(wParam) == XBUTTON2 ) {
				SetCapture( window->hWnd );
				Mgr.eventQueue.Produce( new WindowEvent( window, WM_LBUTTONDOWN, Mouse::Button5, 1 ) );
			}
			return true;

		// Were any of the mouse-buttons released?
		case WM_LBUTTONUP:
			ReleaseCapture();
			Mgr.eventQueue.Produce( new WindowEvent( window, WM_LBUTTONDOWN, Mouse::Button1, 0 ) );
			return true;
		case WM_RBUTTONUP:
			ReleaseCapture();
			Mgr.eventQueue.Produce( new WindowEvent( window, WM_LBUTTONDOWN, Mouse::Button2, 0 ) );
			return true;
		case WM_MBUTTONUP:
			ReleaseCapture();
			Mgr.eventQueue.Produce( new WindowEvent( window, WM_LBUTTONDOWN, Mouse::Button3, 0 ) );
			return true;
		case WM_XBUTTONUP:
			if( HIWORD(wParam) == XBUTTON1 ) {
				ReleaseCapture();
				Mgr.eventQueue.Produce( new WindowEvent( window, WM_LBUTTONDOWN, Mouse::Button4, 0 ) );
			} else if( HIWORD(wParam) == XBUTTON2 ) {
				ReleaseCapture();
				Mgr.eventQueue.Produce( new WindowEvent( window, WM_LBUTTONDOWN, Mouse::Button5, 0 ) );
			}
			return true;

		// Mouse wheel action?
		case WM_MOUSEWHEEL:
			Mgr.eventQueue.Produce( new WindowEvent( window, WM_MOUSEWHEEL, (((int)wParam) >> 16) / WHEEL_DELTA, 0 ) );
			return true;
	}
	return false;
}

void Platform::InitInput( void ) {
#define REGISTER_KEY(map,in,out) keyStringMap[#out] = Key::out; map[in] = Key::out;

	// Numeric keypad keys
	REGISTER_KEY( keypadMap,	VK_INSERT,			KP_0 )
	REGISTER_KEY( keypadMap,	VK_END,				KP_1 )
	REGISTER_KEY( keypadMap,	VK_DOWN,			KP_2 )
	REGISTER_KEY( keypadMap,	VK_NEXT,			KP_3 )
	REGISTER_KEY( keypadMap,	VK_LEFT,			KP_4 )
	REGISTER_KEY( keypadMap,	VK_CLEAR,			KP_5 )
	REGISTER_KEY( keypadMap,	VK_RIGHT,			KP_6 )
	REGISTER_KEY( keypadMap,	VK_HOME,			KP_7 )
	REGISTER_KEY( keypadMap,	VK_UP,				KP_8 )
	REGISTER_KEY( keypadMap,	VK_PRIOR,			KP_9 )
	REGISTER_KEY( keypadMap,	VK_DIVIDE,			KP_DIVIDE )
	REGISTER_KEY( keypadMap,	VK_MULTIPLY,		KP_MULTIPLY )
	REGISTER_KEY( keypadMap,	VK_SUBTRACT,		KP_SUBTRACT )
	REGISTER_KEY( keypadMap,	VK_ADD,				KP_ADD )
	REGISTER_KEY( keypadMap,	VK_DELETE,			KP_DECIMAL )

	// Special keys (non character keys)

	// Function Keys
	REGISTER_KEY( keyMap,		VK_F1,				F1 )
	REGISTER_KEY( keyMap,		VK_F2,				F2 )
	REGISTER_KEY( keyMap,		VK_F3,				F3 )
	REGISTER_KEY( keyMap,		VK_F4,				F4 )
	REGISTER_KEY( keyMap,		VK_F5,				F5 )
	REGISTER_KEY( keyMap,		VK_F6,				F6 )
	REGISTER_KEY( keyMap,		VK_F7,				F7 )
	REGISTER_KEY( keyMap,		VK_F8,				F8 )
	REGISTER_KEY( keyMap,		VK_F9,				F9 )
	REGISTER_KEY( keyMap,		VK_F10,				F10 )
	REGISTER_KEY( keyMap,		VK_F11,				F11 )
	REGISTER_KEY( keyMap,		VK_F12,				F12 )
	REGISTER_KEY( keyMap,		VK_F13,				F13 )
	REGISTER_KEY( keyMap,		VK_F14,				F14 )
	REGISTER_KEY( keyMap,		VK_F15,				F15 )
	REGISTER_KEY( keyMap,		VK_F16,				F16 )
	REGISTER_KEY( keyMap,		VK_F17,				F17 )
	REGISTER_KEY( keyMap,		VK_F18,				F18 )
	REGISTER_KEY( keyMap,		VK_F19,				F19 )
	REGISTER_KEY( keyMap,		VK_F20,				F20 )
	REGISTER_KEY( keyMap,		VK_F21,				F21 )
	REGISTER_KEY( keyMap,		VK_F22,				F22 )
	REGISTER_KEY( keyMap,		VK_F23,				F23 )
	REGISTER_KEY( keyMap,		VK_F24,				F24 )

	// 3 Block (non-mac)
	REGISTER_KEY( keyMap,		VK_SNAPSHOT,		PRINT )
	REGISTER_KEY( keyMap,		VK_SCROLL,			SCROLL )
	REGISTER_KEY( keyMap,		VK_PAUSE,			PAUSE )

	// 6 Block
	REGISTER_KEY( keyMap,		VK_INSERT,			INSERT )
	REGISTER_KEY( keyMap,		VK_DELETE,			DEL )
	REGISTER_KEY( keyMap,		VK_HOME,			HOME )
	REGISTER_KEY( keyMap,		VK_END,				END )
	REGISTER_KEY( keyMap,		VK_PRIOR,			PAGEUP )
	REGISTER_KEY( keyMap,		VK_NEXT,			PAGEDOWN )

	// Arrow Keys
	REGISTER_KEY( keyMap,		VK_UP,				UP )
	REGISTER_KEY( keyMap,		VK_DOWN,			DOWN )
	REGISTER_KEY( keyMap,		VK_LEFT,			LEFT )
	REGISTER_KEY( keyMap,		VK_RIGHT,			RIGHT )

	// Keypad
	REGISTER_KEY( keyMap,		VK_NUMPAD0,			KP_0 )
	REGISTER_KEY( keyMap,		VK_NUMPAD1,			KP_1 )
	REGISTER_KEY( keyMap,		VK_NUMPAD2,			KP_2 )
	REGISTER_KEY( keyMap,		VK_NUMPAD3,			KP_3 )
	REGISTER_KEY( keyMap,		VK_NUMPAD4,			KP_4 )
	REGISTER_KEY( keyMap,		VK_NUMPAD5,			KP_5 )
	REGISTER_KEY( keyMap,		VK_NUMPAD6,			KP_6 )
	REGISTER_KEY( keyMap,		VK_NUMPAD7,			KP_7 )
	REGISTER_KEY( keyMap,		VK_NUMPAD8,			KP_8 )
	REGISTER_KEY( keyMap,		VK_NUMPAD9,			KP_9 )
	REGISTER_KEY( keyMap,		VK_NUMLOCK,			KP_NUMLOCK )
	REGISTER_KEY( keyMap,		VK_OEM_NEC_EQUAL,	KP_EQUAL )
	REGISTER_KEY( keyMap,		VK_DIVIDE,			KP_DIVIDE )
	REGISTER_KEY( keyMap,		VK_MULTIPLY,		KP_MULTIPLY )
	REGISTER_KEY( keyMap,		VK_SUBTRACT,		KP_SUBTRACT )
	REGISTER_KEY( keyMap,		VK_ADD,				KP_ADD )
	REGISTER_KEY( keyMap,		VK_DECIMAL,			KP_DECIMAL )

	// Modifiers & Menu Keys
	REGISTER_KEY( keyMap,		VK_LWIN,			LSUPER )
	REGISTER_KEY( keyMap,		VK_RWIN,			RSUPER )
	REGISTER_KEY( keyMap,		VK_APPS,			MENU )

	// Misc Keys
	REGISTER_KEY( keyMap,		VK_ESCAPE,			ESC )
	REGISTER_KEY( keyMap,		VK_TAB,				TAB )
	REGISTER_KEY( keyMap,		VK_CAPITAL,			CAPSLOCK )
	REGISTER_KEY( keyMap,		VK_BACK,			BACKSPACE )
	REGISTER_KEY( keyMap,		VK_SPACE,			SPACE )

#undef REGISTER_KEY
}

void Platform::ShutdownInput( void ) {
	keypadMap.clear();
	keyMap.clear();
	keyStringMap.Clear();
}

int Gloot::GetKeyId( const char *name ) {
	// single ascii character..
	if ( name[1] == '\0' )
		return name[0];
	// some unknown key written in hex, probably unicode
//	else if ( String::CmpPrefix( name, "0x") == 0 ) {
//		return HexToInt(name);

	// Special key
	int index = Mgr.keyStringMap.Find( name );
	if ( index != -1 )
		return Mgr.keyStringMap[index];
	return Key::UNKNOWN;
}

const char *Gloot::GetKeyString( int key ) {
	static char ascii[2] = { '\0', '\0' };
	if ( isalnum( key ) ) {
		ascii[0] = key;
		return ascii;
	}

	int index = Mgr.keyStringMap.FindByValue( key );
	return index == -1 ? NULL : Mgr.keyStringMap.GetKey( index ).c_str();
}

}

#endif
