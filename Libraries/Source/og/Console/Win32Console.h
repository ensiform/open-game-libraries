/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: CVar System Interface (engine part)
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

#ifndef __OG_WIN32_CONSOLE_H__
#define __OG_WIN32_CONSOLE_H__

#include <string>

#include <og/Console.h>
#include <og/Common/Thread/LockFreeQueue.h>
#include "CmdSystemEx.h"
#include <windows.h>

namespace og {
typedef List<std::wstring> WStringList;

class ConPrint {
public:
	ConPrint( bool error, bool addEndl ) : isError(error), lastWasEndl(addEndl) {}

	bool		isError;
	bool		lastWasEndl;
	DynBuffer<wchar_t> message;
};

class ConCommand {
public:
	ConCommand( int _type ) : type(_type) {}

	int			type;
	int			iValue;
	String		strValue;
};

/*
==============================================================================

  ConsoleWindow

==============================================================================
*/
class ConsoleWindow : public Thread {
public:
	ConsoleWindow( const ConsoleParams_t *_params );
	~ConsoleWindow();

	bool		Init( void );
	
	void		WakeUp( void );
	void		Run( void );
	void		Show( Console::VisLevel visLevel, bool quitOnClose );

	ConsoleParams_t params;
	String		appTitle;
	String		logBgImage;

	HWND		hWndMain;

private:
	void		DoResize( void );
	static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	LRESULT		MainWndProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT		LogWndProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT		EditWndProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	void		Print( ConPrint *evt );
	void		FatalError( ConPrint *evt );
	void		SetInputText( const wchar_t *msg );
	const wchar_t *GetInputText( void );


	HINSTANCE	hInstance;
	WNDPROC		SysEditWndProc;
	WNDPROC		SysLogWndProc;

	HWND		hWndLog;
	HWND		hWndCommand;
	HWND		hWndErrorMessage;
	HWND		hWndButtonOptions;

	COLORREF	colorLogBg;
	COLORREF	colorLogText;
	COLORREF	colorErrorBg;
	COLORREF	colorErrorBgFlash;
	COLORREF	colorErrorText;
	COLORREF	colorErrorTextFlash;

	HFONT		hfGlobalFont;
	HBRUSH		hbrLogBg;
	HBRUSH		hbrErrorBg;
	HBRUSH		hbrErrorBgFlash;
	HBITMAP		hBackBmp;

	int			visLevel;
	bool		quitOnClose;
	int			width, height;
	int			minWidth, minHeight;
	int			bgImgWidth, bgImgHeight;
	uInt		fadeTime;
	uInt		fadeTimeStart;
	
	int			dwStyle;
	int			dwExStyle;

public:
	int			completionCursor;
	int			completionPos;
};

/*
==============================================================================

  WinConsoleData_t

==============================================================================
*/
struct WinConsoleData_t {
	Condition	menuCondition;
	WStringList	menuEntries;
	StringList	menuCommands;

	StringList	inputHistory;
	int			inputHistoryPos;
	String		inputSavedText;

	bool		lastWasEndl;
	Condition	completionCondition;
	WStringList	completionList;
	String		completionText;

	LockFreeQueue<ConCommand> cmdQueue;
};

}
#endif
