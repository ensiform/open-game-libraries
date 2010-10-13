/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Win32 Console Window
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

#include "Win32Console.h"

#ifdef OG_WIN32
#include <windows.h>
#include <RichEdit.h>

namespace og {

static ConsoleWindow *conWindow = NULL;
static WinConsoleData_t wcd;

enum {
	WM_CON_PRINT = WM_USER,
	WM_CON_COMMAND,
	
	WM_WAKE_UP
};
enum {
	IDT_ERROR = 1,
	IDT_FADE
};
enum {
	// Input
	CCMD_EXECUTE,
	CCMD_USAGE,
	CCMD_COMPLETION,
	CCMD_HISTORY,
	CCMD_USERMENU,

	// Output
	CCMD_SETINPUT,
};

enum {
	CONID_LOG = 100,
	CONID_COMMAND,
	CONID_OPTIONS,
	CONID_ERROR,

	CON_ID_MENU_QUIT = 120,
	CON_ID_MENU_COPY,
	CON_ID_MENU_SAVE,
	CON_ID_MENU_CLEAR,
	CON_ID_MENU_CUSTOM,
};

const int CONSOLE_BUFFER_SIZE = 16384;

/*
==============================================================================

  Win32 helper functions

==============================================================================
*/
const byte	MASKBITS	= 0x3F;
const byte	MASK1BIT	= 0x80;
const byte	MASK2BIT	= 0xC0;
const byte	MASK3BIT	= 0xE0;
const byte	MASK4BIT	= 0xF0;
const byte	MASK5BIT	= 0xF8;
const byte	MASK6BIT	= 0xFC;
const byte	MASK7BIT	= 0xFE;

/*
================
Utf8ToWChar
================
*/
static wchar_t Utf8ToWChar( const char *data, int *numBytes ) {
	if( data[0] < MASK1BIT && data[0] >= 0 ) {
		*numBytes = 1;
		return data[0];
	} else if((data[0] & MASK3BIT) == MASK2BIT) {
		*numBytes = 2;
		return ((data[0] & 0x1F) << 6) | (data[1] & MASKBITS);
	} else if( (data[0] & MASK4BIT) == MASK3BIT ) {
		*numBytes = 3;
		return ((data[0] & 0x0F) << 12) | ( (data[1] & MASKBITS) << 6) | (data[2] & MASKBITS);
	}
#if 0 //! @todo	if sizof(wchar_t) == 4
	else if( (data[0] & MASK5BIT) == MASK4BIT ) {
		*numBytes = 4;
		return ((data[0] & 0x07) << 18) | ((data[1] & MASKBITS) << 12) |
				((data[2] & MASKBITS) << 6) | (data[3] & MASKBITS);
	} else if( (data[0] & MASK6BIT) == MASK5BIT ) {
		*numBytes = 5;
		return ((data[0] & 0x03) << 24) | ((data[1] & MASKBITS) << 18) | ((data[2] & MASKBITS) << 12) |
				((data[3] & MASKBITS) << 6) | (data[4] & MASKBITS);
	} else if( (data[0] & MASK7BIT) == MASK6BIT ) {
		*numBytes = 6;
		return ((data[0] & 0x01) << 30) | ((data[1] & MASKBITS) << 24) | ((data[2] & MASKBITS) << 18) |
				((data[3] & MASKBITS) << 12) | ((data[4] & MASKBITS) << 6) | (data[5] & MASKBITS);
	}
#endif
	*numBytes = 1;
	return L'?';
}

/*
================
ShiftPressed
================
*/
OG_INLINE bool ShiftPressed( void ) {
	return GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT);
}

/*
================
ControlPressed
================
*/
OG_INLINE bool ControlPressed( void ) {
	return GetAsyncKeyState(VK_LCONTROL) || GetAsyncKeyState(VK_RCONTROL);
}

/*
==============================================================================

  ConsoleWindow

==============================================================================
*/
ConsoleWindow::ConsoleWindow( const ConsoleParams_t *_params ) {
	params = *_params;
	appTitle = params.appTitle;
	logBgImage = params.logBgImage ? params.logBgImage : "";

	hWndMain = NULL;
	hWndLog = NULL;
	hWndCommand = NULL;
	hWndErrorMessage = NULL;
	hWndButtonOptions = NULL;
	completionCursor = 0;
	
	dwStyle = WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
	dwExStyle = WS_EX_WINDOWEDGE|WS_EX_TOOLWINDOW|WS_EX_APPWINDOW;
}

ConsoleWindow::~ConsoleWindow() {
	if ( hWndMain ) {
		if ( hBackBmp ) {
			DeleteObject( hBackBmp );
			hBackBmp = NULL;
		}
		if ( hbrLogBg ) {
			DeleteObject( hbrLogBg );
			hbrLogBg = NULL;
		}
		if ( hbrErrorBg ) {
			DeleteObject( hbrErrorBg );
			hbrErrorBg = NULL;
		}
		if ( hbrErrorBgFlash ) {
			DeleteObject( hbrErrorBgFlash );
			hbrErrorBgFlash = NULL;
		}

		ShowWindow( hWndMain, SW_HIDE );
		CloseWindow( hWndMain );
		DestroyWindow( hWndMain );
		hWndMain = 0;
	}
}

bool ConsoleWindow::Init( void ) {
	// Load the background image
	if ( logBgImage.IsEmpty() )
		hBackBmp = NULL;
	else {
		DynBuffer<wchar_t> strFilename;
		String::ToWide( logBgImage.c_str(), strFilename );

		// Use LoadImage() to get the image loaded into a DIBSection
		hBackBmp = (HBITMAP)LoadImage( NULL, strFilename.data, IMAGE_BITMAP, 0, 0,
			LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
		if( hBackBmp != NULL ) {
			// Get width & height
			BITMAP bm;
			GetObject(hBackBmp, sizeof(BITMAP), &bm );
			bgImgWidth = bm.bmWidth;
			bgImgHeight = bm.bmHeight;
		}
	}

	DynBuffer<wchar_t> windowClass, windowTitle;
	String::ToWide( Format("$* Console" ) << appTitle, windowClass );
	String::ToWide( Format("$* WinConsole" ) << appTitle, windowTitle );

	hInstance = GetModuleHandle(NULL);

	WNDCLASS wc;
	memset( &wc, 0, sizeof( wc ) );
	wc.style			= 0;
	wc.lpfnWndProc		= static_cast<WNDPROC>(WndProc);
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hCursor			= LoadCursor (NULL,IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= windowClass.data;

	// Load user-provided icon if available
	wc.hIcon = LoadIcon( hInstance, L"GLOOT_ICON" );
	if( !wc.hIcon )
		wc.hIcon = LoadIcon( NULL, IDI_WINLOGO );

	if ( !RegisterClass (&wc) )
		return false;

	// Setup colors
#define RGB_FROM_PARM(n) RGB(params.n[0], params.n[1], params.n[2])
	colorLogBg			= RGB_FROM_PARM(colorLogBg);
	colorLogText		= RGB_FROM_PARM(colorLogText);
	colorErrorBg		= RGB_FROM_PARM(colorErrorBg);
	colorErrorBgFlash	= RGB_FROM_PARM(colorErrorBgFlash);
	colorErrorText		= RGB_FROM_PARM(colorErrorText);
	colorErrorTextFlash	= RGB_FROM_PARM(colorErrorTextFlash);
#undef RGB_FROM_PARM
	hbrLogBg		= CreateSolidBrush( colorLogBg );
	hbrErrorBg		= CreateSolidBrush( colorErrorBg );
	hbrErrorBgFlash	= CreateSolidBrush( colorErrorBgFlash );

	// Set up fading
	fadeTime = params.fadeTime;
	if ( fadeTime )
		dwExStyle |= WS_EX_LAYERED;

	// Calculate width and height of full window
	RECT rect = { 0, 0, params.width-1, params.height-1 };
	AdjustWindowRectEx( &rect, dwStyle, FALSE, dwExStyle );
	width = rect.right-rect.left+1;
	height = rect.bottom-rect.top+1;

	// Get center position
	HDC hDC = GetDC( GetDesktopWindow() );
	int x = (GetDeviceCaps( hDC, HORZRES ) - width) / 2;
	int y = (GetDeviceCaps( hDC, VERTRES ) - height) / 2;
	ReleaseDC( GetDesktopWindow(), hDC );

	// Store minimum size
	minWidth = params.minWidth;
	minHeight= params.minHeight;

	// Create window
	hWndMain = CreateWindowEx(
		dwExStyle,			// Extended style
		windowClass.data,	// Class name
		windowTitle.data,	// Window title
		dwStyle,			// Defined window style
		x, y,				// Window position
		width,				// Decorated window width
		height,				// Decorated window height
		NULL,				// No parent window
		NULL,				// No menu
		hInstance,			// hInstance
		0 );				// pass this to WM_CREATE

	if ( hWndMain == NULL )
		return false;

	// Create fonts
	hDC = GetDC( hWndMain );
	int nHeight = -MulDiv( 8, GetDeviceCaps( hDC, LOGPIXELSY ), 72);

	hfGlobalFont = CreateFont( nHeight, 0, 0, 0, FW_LIGHT,
		0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		FF_MODERN | FIXED_PITCH, L"Courier New" );

	ReleaseDC( hWndMain, hDC );

	// Create controls:
	hWndLog = CreateWindowEx(WS_EX_TRANSPARENT, RICHEDIT_CLASS, L"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_NOHIDESEL,
		0, 0, 0, 0, hWndMain, (HMENU)CONID_LOG, hInstance, 0 );
	SendMessage( hWndLog, WM_SETFONT, ( WPARAM ) hfGlobalFont, 0 );

	hWndCommand = CreateWindow( L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOVSCROLL,
		0, 0, 0, 0, hWndMain, (HMENU)CONID_COMMAND, hInstance, 0 );
	SendMessage( hWndCommand, WM_SETFONT, ( WPARAM ) hfGlobalFont, 0 );

	// Create the buttons
	hWndButtonOptions = CreateWindow( L"BUTTON", NULL,
		BS_FLAT | WS_VISIBLE | WS_CHILD,
		0, 0, 0, 0, hWndMain, (HMENU)CONID_OPTIONS, hInstance, NULL );
	SendMessage( hWndButtonOptions, WM_SETTEXT, 0, ( LPARAM ) L"Menu" );

	SysEditWndProc = ( WNDPROC ) SetWindowLong( hWndCommand, GWL_WNDPROC, ( long ) WndProc );
	SysLogWndProc = ( WNDPROC ) SetWindowLong( hWndLog, GWL_WNDPROC, ( long ) WndProc );


	// set character formatting for the log window
	//! @todo	why does outline not work ?
	CHARFORMAT2 cf;
	cf.cbSize=sizeof(CHARFORMAT2);
	cf.dwMask=CFM_OUTLINE|CFM_COLOR|CFM_SIZE|CFM_FACE;
	cf.dwEffects=CFE_OUTLINE;
	cf.crTextColor=RGB(params.colorLogText[0],params.colorLogText[1],params.colorLogText[2]);
	wcscpy(cf.szFaceName,L"Courier New");
	cf.yHeight = 160;
	SendMessage( hWndLog, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf );

	DoResize();

	ShowWindow( hWndMain, SW_SHOWDEFAULT );
	UpdateWindow( hWndMain );
	SetForegroundWindow( hWndMain );
	SetFocus( hWndCommand );

	visLevel = 1;
	fadeTimeStart = SysInfo::GetTime();

	if ( fadeTime )
		SetTimer( hWndMain, IDT_FADE, 10, NULL );

	return true;
}

void ConsoleWindow::WakeUp( void ) {
	PostThreadMessage( id, WM_WAKE_UP, 0, 0 );
}

void ConsoleWindow::Run( void ) {
	MSG msg;
	while( !selfDestruct ) {
		while( GetMessage( &msg, NULL, 0, 0 ) ) {
			if ( msg.message == WM_WAKE_UP )
				break;
			DispatchMessage( &msg );
			TranslateMessage( &msg );
		}
	}
}

void ConsoleWindow::DoResize( void ) {
	int logY = 5;
	int logH = height-32;
	if ( hWndErrorMessage ) {
		SetWindowPos( hWndErrorMessage, HWND_TOP, 4, logY, width-6, 30, SWP_NOZORDER|SWP_NOACTIVATE );
		logY += 32;
		logH -= 32;
	}
	SetWindowPos( hWndLog, HWND_TOP, 4, logY, width-8, logH, SWP_NOZORDER|SWP_NOACTIVATE );
	SetWindowPos( hWndCommand, HWND_TOP, 4, height-23, width-82, 20, SWP_NOZORDER|SWP_NOACTIVATE );
	SetWindowPos( hWndButtonOptions, HWND_TOP, width-74, height-23, 70, 20, SWP_NOZORDER|SWP_NOACTIVATE );
}

/*
================
ConsoleWindow::ShowConsole
================
*/
void ConsoleWindow::Show( Console::VisLevel visLevel, bool quitOnClose ) {
	if ( !hWndMain )
		return;

	this->quitOnClose = quitOnClose;
	if ( visLevel == visLevel )
		return;

	visLevel = visLevel;
	switch( visLevel ) {
		case Console::VIS_HIDDEN:
			ShowWindow( hWndMain, SW_HIDE );
			break;
		case Console::VIS_NORMAL:
			ShowWindow( hWndMain, SW_SHOWNORMAL );
			PostMessage( hWndLog, EM_LINESCROLL, 0, 0xffff );
			break;
		case Console::VIS_ICONIFIED:
			ShowWindow( hWndMain, SW_MINIMIZE );
			break;
		default:
			User::Warning( "Invalid visLevel sent to ShowConsole." );
			break;
	}
}

/*
================
ConsoleWindow::Print
================
*/
void ConsoleWindow::Print( ConPrint *evt ) {
	if ( evt->message.data[0] == '\0' )
		return;

	// Store old selection
	CHARRANGE crOld;
	SendMessage( hWndLog, EM_EXGETSEL, 0, (LPARAM)&crOld );

	CHARRANGE cr;
	cr.cpMin = -1;cr.cpMax = -1;

	// If we removed a newline on the last call to Print, re-add it now.
	if ( evt->lastWasEndl ) {
		evt->lastWasEndl = false;

		// Append newline
		SendMessage( hWndLog, EM_EXSETSEL, 0, (LPARAM)&cr );
		SendMessage( hWndLog, EM_REPLACESEL, 0, (LPARAM)L"\r\n" );
	}
	// Append buffer
	SendMessage( hWndLog, EM_EXSETSEL, 0, (LPARAM)&cr );
	SendMessage( hWndLog, EM_REPLACESEL, 0, (LPARAM)evt->message.data );

	// restore selection
	SendMessage( hWndLog, EM_EXSETSEL, 0, (LPARAM)&crOld );

	// Scroll to end
	SendMessage( hWndLog, WM_VSCROLL, SB_BOTTOM, (LPARAM)NULL );
}

/*
================
ConsoleWindow::FatalError
================
*/
void ConsoleWindow::FatalError( ConPrint *evt ) {
	if ( !hWndErrorMessage ) {
		SetWindowPos(hWndLog, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		hWndErrorMessage = CreateWindowEx( WS_EX_CLIENTEDGE, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
										0, 0, 0, 0,	hWndMain, (HMENU)CONID_ERROR, hInstance, NULL );

		SendMessage( hWndErrorMessage, WM_SETFONT, ( WPARAM ) hfGlobalFont, 0 );
		DoResize();
		SetTimer( hWndMain, IDT_ERROR, 1000, NULL );
	}
	SetWindowText( hWndErrorMessage, evt->message.data );
	Show( Console::VIS_NORMAL, true );
}

/*
================
ConsoleWindow::SetInputText
================
*/
void ConsoleWindow::SetInputText( const wchar_t *msg ) {
	SendMessage( hWndCommand, EM_SETSEL, 0, -1 );
	SendMessage( hWndCommand, EM_REPLACESEL, FALSE, ( LPARAM ) msg );
}

/*
================
ConsoleWindow::GetInputText
================
*/
const wchar_t *ConsoleWindow::GetInputText( void ) {
	static DynBuffer<wchar_t> returnString;
	int size = GetWindowTextLength( hWndCommand ) + 1;
	returnString.CheckSize( size );
	GetWindowText( hWndCommand, returnString.data, size );
	return returnString.data;
}

LRESULT CALLBACK ConsoleWindow::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	if ( conWindow == NULL )
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	if ( uMsg == WM_CREATE ) // only happens for main window here..
		conWindow->hWndMain = hWnd;
	else if ( uMsg == WM_CON_PRINT ) {
		ConPrint *evt = (ConPrint *)wParam;
		conWindow->Print( evt );
		if ( evt->isError )
			conWindow->FatalError( evt );
		delete evt;
		return 0;
	} else if ( uMsg == WM_CON_COMMAND ) {
		ConCommand *cmd = (ConCommand *)wParam;
		switch( cmd->type ) {
			case CCMD_SETINPUT: {
				DynBuffer<wchar_t> wideBuf;
				String::ToWide( cmd->strValue.c_str(), wideBuf );
				conWindow->SetInputText( wideBuf.data );
				}
				break;
		}
		delete cmd;
		return 0;
	}

	if ( hWnd == conWindow->hWndMain )
		return conWindow->MainWndProc( uMsg, wParam, lParam );
	else if ( hWnd == conWindow->hWndLog )
		return conWindow->LogWndProc( uMsg, wParam, lParam );
	else if ( hWnd == conWindow->hWndCommand )
		return conWindow->EditWndProc( uMsg, wParam, lParam );
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

/*
================
MenuOpenCallback
================
*/
void MenuOpenCallback( const char *name, const char *command ) {
	OG_ASSERT( name != NULL && command != NULL );

	DynBuffer<wchar_t> strName;
	String::ToWide( name, strName );

	wcd.menuEntries.Append( strName.data );
	wcd.menuCommands.Append( command );
}

/*
================
WndProc
================
*/
LRESULT ConsoleWindow::MainWndProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	static bool flashErrorColor;
	switch ( uMsg ){
		case WM_ACTIVATE:
			if ( LOWORD( wParam ) != WA_INACTIVE )
				SetFocus( hWndCommand );
			//! @todo	notify user
			break;

		case WM_CLOSE:
			if ( quitOnClose )
				User::OnForceExit();
			else
				Console::ShowConsole( Console::VIS_HIDDEN, false ); //! @todo	notify user
			return 0;

		case WM_CTLCOLORSTATIC:
			if ( ( HWND ) lParam == hWndErrorMessage ) {
				if ( flashErrorColor ) {
					SetTextColor( ( HDC ) wParam, colorErrorText );
					SetBkColor( ( HDC ) wParam, colorErrorBg );
					return ( long ) hbrErrorBg;
				} else {
					SetTextColor( ( HDC ) wParam, colorErrorTextFlash );
					SetBkColor( ( HDC ) wParam, colorErrorBgFlash );
					return ( long ) hbrErrorBgFlash;
				}
			}
		case WM_GETMINMAXINFO: {
				MINMAXINFO *mmi = (MINMAXINFO *)lParam;
				mmi->ptMinTrackSize.x = minWidth;
				mmi->ptMinTrackSize.y = minHeight;
			}
			break;

		case WM_SIZE:
			width = LOWORD(lParam);
			height = HIWORD(lParam);
			if ( hWndLog )
				DoResize();
			break;

		case WM_COMMAND:
			if ( wParam == CONID_OPTIONS ) {
				// Create basic menu
				HMENU hmenu = CreatePopupMenu();
				AppendMenu( hmenu, MF_STRING, CON_ID_MENU_COPY, L"Copy log to clipboard");
				AppendMenu( hmenu, MF_STRING, CON_ID_MENU_SAVE, L"Save log to file");
				AppendMenu( hmenu, MF_STRING, CON_ID_MENU_CLEAR, L"Clear log");

				ConCommand *cmd = new ConCommand(CCMD_USERMENU);
				wcd.cmdQueue.Produce( cmd );
				wcd.menuCondition.Wait(INFINITE);

				if ( !wcd.menuEntries.IsEmpty() ) {
					AppendMenu( hmenu, MF_SEPARATOR, 0, 0);
					int max= wcd.menuEntries.Num();
					for( int i=0; i<max; i++ )
						AppendMenu( hmenu, MF_STRING, CON_ID_MENU_CUSTOM + i, wcd.menuEntries[i].c_str() );
				}

				// Add the exit part
				AppendMenu( hmenu, MF_SEPARATOR, 0, 0);
				AppendMenu( hmenu, MF_STRING, CON_ID_MENU_QUIT, L"Exit");

				// Pop it up
				TPMPARAMS tpmParms;
				tpmParms.cbSize = sizeof(TPMPARAMS);
				GetWindowRect( hWndButtonOptions, &tpmParms.rcExclude);
				int ret = TrackPopupMenuEx( hmenu, TPM_BOTTOMALIGN|TPM_RIGHTALIGN|TPM_RETURNCMD, tpmParms.rcExclude.right, tpmParms.rcExclude.top-1, hWndMain, &tpmParms);

				// Execute the action
				switch( ret ) {
					case CON_ID_MENU_COPY: {
						CHARRANGE cr;
						cr.cpMin = 0;cr.cpMax = -1;
						SendMessage( hWndLog, EM_EXSETSEL, 0, (LPARAM)&cr );
						SendMessage( hWndLog, WM_COPY, 0, 0 );
						}
						break;
					case CON_ID_MENU_SAVE:
						break;
					case CON_ID_MENU_CLEAR:
						if ( hWndErrorMessage == NULL ) {
							CHARRANGE cr;
							cr.cpMin = 0;cr.cpMax = -1;
							SendMessage( hWndLog, EM_EXSETSEL, 0, (LPARAM)&cr );
							SendMessage( hWndLog, EM_REPLACESEL, 0, (LPARAM)L"" );
							UpdateWindow( hWndLog );
							wcd.lastWasEndl = false;
						}
						break;
					case CON_ID_MENU_QUIT:
						User::OnForceExit();
						break;
					default:
						if ( ret >= CON_ID_MENU_CUSTOM ) {
							int index = ret - CON_ID_MENU_CUSTOM;
							if ( index < wcd.menuCommands.Num() ) {
								ConCommand *cmd = new ConCommand(CCMD_EXECUTE);
								cmd->strValue = wcd.menuCommands[index].c_str();
								SetInputText(L"");
								wcd.cmdQueue.Produce( cmd );
							}
						}
						break;
				}
				DestroyMenu( hmenu );
			}
			break;
		case WM_TIMER:
			if ( wParam == IDT_ERROR && hWndErrorMessage ) {
				flashErrorColor = !flashErrorColor;
				InvalidateRect( hWndErrorMessage, NULL, FALSE );
			}
			else if ( wParam == IDT_FADE ) {
				// Do alpha fading
				uInt diff = SysInfo::GetTime() - fadeTimeStart;
				if ( diff >= fadeTime ) {
					dwExStyle &= ~WS_EX_LAYERED;
					SetWindowLong( hWndMain, GWL_EXSTYLE, dwExStyle );
					fadeTime = 0;
					KillTimer( hWndMain, IDT_FADE );
				} else {
					byte alpha = static_cast<byte>(255*static_cast<float>(diff)/static_cast<float>(fadeTime));
					SetLayeredWindowAttributes(hWndMain, 0, alpha, LWA_ALPHA);
				}
			}
			break;
	}

	return DefWindowProc( hWndMain, uMsg, wParam, lParam );
}

/*
================
CompletionCallback
================
*/
void CompletionCallback( const char *str ) {
	DynBuffer<wchar_t> wstr;
	String::ToWide( str, wstr );
	wcd.completionList.Append( wstr.data );
}

/*
================
LogWndProc
================
*/
LRESULT ConsoleWindow::LogWndProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	if ( uMsg == WM_ERASEBKGND ) {
		// Setup Drawing
		HDC hdc = (HDC)wParam;
		HDC hcompdc = CreateCompatibleDC(hdc);

		// Get area to draw
		RECT destRect, wndRect;
		GetClientRect(hWndLog, &wndRect);
		GetClientRect(hWndLog, &destRect);

		// find the center rect
		destRect.left += (destRect.right - destRect.left)/2 - bgImgWidth/2;
		destRect.top += (destRect.bottom - destRect.top)/2 - bgImgHeight/2;
		RECT srcRect = { 0, 0, bgImgWidth, bgImgHeight };

		// Adjust source and destination rect, if out of bounds
		if ( destRect.top < 0 ) {
			srcRect.top = -destRect.top;
			srcRect.bottom -= -2*destRect.top;
			destRect.top = 0;
		}
		if ( destRect.left < 0 ) {
			srcRect.left = -destRect.left;
			srcRect.right -= -2*destRect.left;
			destRect.left = 0;
		} else {
			RECT tRect = { 0, 0, destRect.left, wndRect.bottom };
			FillRect( hdc, &tRect, hbrLogBg );
		}
		destRect.right =  destRect.left + srcRect.right;
		destRect.bottom =  destRect.top + srcRect.bottom;
		if ( destRect.right < wndRect.right ) {
			RECT tRect = { destRect.right, 0, wndRect.right, wndRect.bottom };
			FillRect( hdc, &tRect, hbrLogBg );
		}
		if ( destRect.left > 0 ) {
			RECT tRect = { destRect.left, 0, destRect.right, destRect.top };
			FillRect( hdc, &tRect, hbrLogBg );
		}
		if ( destRect.bottom < wndRect.bottom ) {
			RECT tRect = { destRect.left, destRect.bottom, destRect.right, wndRect.bottom };
			FillRect( hdc, &tRect, hbrLogBg );
		}

		// Draw the image
		SelectObject(hcompdc, hBackBmp);
		BitBlt(hdc, destRect.left, destRect.top, srcRect.right, srcRect.bottom, hcompdc, srcRect.left, srcRect.top, SRCCOPY);

		DeleteDC(hcompdc);
		return 1;
	}
	return CallWindowProc( SysLogWndProc, hWndLog, uMsg, wParam, lParam );
}

/*
================
EditWndProc
================
*/
LRESULT ConsoleWindow::EditWndProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	switch ( uMsg ) {
		case WM_KILLFOCUS:
			if ( ( HWND ) wParam == hWndMain || ( HWND ) wParam == hWndErrorMessage ) {
				SetFocus( hWndCommand );
				return 0;
			}
			break;

		case WM_CHAR:
			// Handled in WM_KEYDOWN now
			if ( wParam == VK_TAB || wParam == VK_RETURN )
				return 0;
			// Reset completion routine on any other char
			completionCursor = 0;
			break;

		case WM_KEYDOWN:
			switch ( wParam ) {
				case VK_UP: {
					ConCommand *cmd = new ConCommand(CCMD_HISTORY);
					cmd->iValue = 1;
					cmd->strValue.FromWide( GetInputText() );
					wcd.cmdQueue.Produce( cmd );
					}
					return 0;
				case VK_DOWN: {
					ConCommand *cmd = new ConCommand(CCMD_HISTORY);
					cmd->iValue = -1;
					cmd->strValue.FromWide( GetInputText() );
					wcd.cmdQueue.Produce( cmd );
					}
					return 0;
				case VK_BACK:
				case VK_DELETE:
					completionCursor = 0; //! @todo	is this needed ? should be handled in WM_CHAR
					break;
				case VK_TAB:
					if ( ControlPressed() ) {
						ConCommand *cmd = new ConCommand(CCMD_USAGE);
						cmd->strValue.FromWide( GetInputText() );
						wcd.cmdQueue.Produce( cmd );
					} else {
						if ( completionCursor <= 0 ) {
							ConCommand *cmd = new ConCommand(CCMD_COMPLETION);
							cmd->strValue.FromWide( GetInputText() );
							wcd.cmdQueue.Produce( cmd );
							wcd.completionCondition.Wait(INFINITE);

							DynBuffer<wchar_t> wideBuf;
							String::ToWide( wcd.completionText.c_str(), wideBuf );
							conWindow->SetInputText( wideBuf.data );
							conWindow->completionCursor = wcd.completionText.Length();
							conWindow->completionPos = 0;
						} else if ( wcd.completionList.Num() ) {
							SetInputText( wcd.completionList[completionPos].c_str() );
							completionPos++;
							if ( completionPos >= wcd.completionList.Num() )
								completionPos = 0;
							SendMessage( hWndCommand, EM_SETSEL, completionCursor, -1 );
						}
					}
					return 0;
				case VK_RETURN: {
					ConCommand *cmd = new ConCommand(CCMD_EXECUTE);
					cmd->strValue.FromWide( GetInputText() );
					SetInputText(L"");
					// Trim both leading and trailing spaces
					cmd->strValue.StripTrailingWhitespaces();
					cmd->strValue.StripLeadingWhitespaces();
					if ( !cmd->strValue.IsEmpty() ) {
						wcd.cmdQueue.Produce( cmd );
						return 0;
					}
					delete cmd;
					return 0;
				}
			}
			break;
	}

	return CallWindowProc( SysEditWndProc, hWndCommand, uMsg, wParam, lParam );
}
/*
==============================================================================

  Console

==============================================================================
*/

/*
================
Console::Init
================
*/
bool Console::Init( const ConsoleParams_t *params ) {
	if ( !::LoadLibrary(L"Riched20.dll") ) {
		User::Error( ERR_SYSTEM_REQUIREMENTS, "Can't load Riched20.dll" );
		return false;
	}
	wcd.lastWasEndl = false;
	wcd.inputHistoryPos = 0;

	conWindow = new ConsoleWindow( params );
	if ( !conWindow->Start("ConsoleWindow", true) ) {
		delete conWindow;
		conWindow = NULL;
	}

	CVar::LinkCVars();
	cmdSystemEx->InitConsoleCmds( params->minWaitTime );
	return true;
}

/*
================
Console::Shutdown
================
*/
void Console::Shutdown( void ) {
	if ( conWindow ) {
		cmdSystemEx->ShutdownConsoleCmds();
		CVar::UnlinkCVars();

		conWindow->Stop();
		conWindow = NULL;
	}
}

/*
================
ProcessConCommand
================
*/
void ProcessConCommand( ConCommand *cmd ) {
	switch( cmd->type ) {
		case CCMD_EXECUTE:
			if ( !cmd->strValue.IsEmpty() ) {
				wcd.inputHistory.Append( cmd->strValue.c_str() );
				wcd.inputHistoryPos = 0;
				Console::Print("]");
				Console::Print( cmd->strValue.c_str() );
				Console::Print("\n");
				cmdSystem->ExecuteCmd( cmd->strValue.c_str() );
			}
			conWindow->completionCursor = 0;
			break;
		case CCMD_USAGE:
			cmdSystem->PrintUsage( cmd->strValue.c_str() );
			break;
		case CCMD_COMPLETION: {
			wcd.completionList.Clear();
			wcd.completionText = cmdSystem->CompleteCmd( cmd->strValue.c_str(), CompletionCallback );
			wcd.completionCondition.Signal();
			}
			break;
		case CCMD_HISTORY: {

			ConCommand *cmdOut = new ConCommand(CCMD_SETINPUT);
			cmdOut->iValue = 0;

			if ( cmd->iValue > 0 ) {
				if ( wcd.inputHistoryPos == 0 )
					wcd.inputSavedText = cmd->strValue.c_str();
				int len = wcd.inputHistory.Num();
				if ( wcd.inputHistoryPos != len ) {
					wcd.inputHistoryPos++;
					conWindow->completionCursor = 0;
					cmdOut->strValue = wcd.inputHistory[len-wcd.inputHistoryPos].c_str();
					PostMessage( conWindow->hWndMain, WM_CON_COMMAND, (WPARAM)cmdOut, 0 );
					cmdOut = NULL;
				}
			} else {
				if ( wcd.inputHistoryPos == 1 ) {
					wcd.inputHistoryPos--;
					conWindow->completionCursor = 0;
					cmdOut->strValue = wcd.inputSavedText.c_str();
					wcd.inputSavedText.Clear();
					PostMessage( conWindow->hWndMain, WM_CON_COMMAND, (WPARAM)cmdOut, 0 );
					cmdOut = NULL;
				} else if ( wcd.inputHistoryPos > 1 ) {
					int len = wcd.inputHistory.Num();
					wcd.inputHistoryPos--;
					conWindow->completionCursor = 0;
					cmdOut->strValue = wcd.inputHistory[len-wcd.inputHistoryPos].c_str();
					PostMessage( conWindow->hWndMain, WM_CON_COMMAND, (WPARAM)cmdOut, 0 );
					cmdOut = NULL;
				}
			}
			if ( cmdOut )
				delete cmdOut;
			}
			break;
		case CCMD_USERMENU:
			// Add custom menu entries
			wcd.menuEntries.Clear();
			wcd.menuCommands.Clear();
			User::OnConsoleMenuOpen( MenuOpenCallback );
			wcd.menuCondition.Signal();
			break;
	}
}

/*
================
Console::Synchronize
================
*/
void Console::Synchronize( void ) {
	if ( conWindow ) {
		ConCommand *cmd;
		while( (cmd = wcd.cmdQueue.Consume()) != NULL ) {
			ProcessConCommand( cmd );
			delete cmd;
		}
		cmdSystemEx->ExecuteCmdQueue();
	}
}

/*
================
Console::ShowConsole
================
*/
void Console::ShowConsole( VisLevel visLevel, bool quitOnClose ) {
	if ( conWindow )
		conWindow->Show( visLevel, quitOnClose );
}

/*
================
Console::Print
================
*/
void Console::Print( const char *pMsg ) {
	if ( conWindow ) {
		ConPrint *evt = new ConPrint( false, wcd.lastWasEndl );
		// first we need to replace \n by \r\n, since thats what windows needs.
		String repl(pMsg);
		wcd.lastWasEndl = repl.StripTrailing("\n") > 0; // also remove last newline character
		repl.Replace( "\n", "\r\n" );

		String::ToWide( repl.c_str(), evt->message ); // Convert to utf16
		PostMessage( conWindow->hWndMain, WM_CON_PRINT, (WPARAM)evt, 0);
	}
}

/*
================
Console::FatalError
================
*/
void Console::FatalError( const char *pMsg ) {
	if ( conWindow ) {
		ConPrint *evt = new ConPrint( true, wcd.lastWasEndl );
		evt->lastWasEndl = wcd.lastWasEndl;
		String::ToWide( pMsg, evt->message ); // Convert to utf16
		PostMessage( conWindow->hWndMain, WM_CON_PRINT, (WPARAM)evt, 0);
	}
}

}
#endif
