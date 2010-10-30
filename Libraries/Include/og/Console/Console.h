// ==============================================================================
//! @file
//! @brief	Native Console Interface
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2007-2010 Lusito Software
//! @note	Thread safety class: Unknown
//!
//! @todo	Test unicode more.
//! @todo	Add access functions to log lines
//! @todo	CVar/Cmd: decide how to handle custom flags, and which flags should be available from the start
//! @todo	Win32: Add coloring for the RichEdit log.
//! @todo	Win32: Finish conversion to RichEdit.
//! @todo	Win32: Decide whether the background image for the log window
//!			should be build in to the exe or loaded on startup.
//! @todo	Linux: Code rewrite
//! @todo	Mac: .. Create a log window like on windows
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

#ifndef __OG_CONSOLE_H__
#define __OG_CONSOLE_H__

#include <og/Common/Common.h>
#include <og/Math/Math.h>
#include <og/FileSystem/FileSystem.h>
#include <og/Console/CmdSystem.h>
#include <og/Console/CVarSystem.h>

//! Open Game Libraries
namespace og {
//! @defgroup Console Console (Library)
//! You need to include <og/Console/Console.h>
//! @see	UserCallbacks
//! @{

	// ==============================================================================
	//! Console parameters
	//!
	//! Different options to set for the console window.
	// ==============================================================================
	struct ConsoleParams_t {
		const char *appTitle;			//!< Your application name
		int		minWaitTime;			//!< Wait at least this long(ms) when a wait command was called.
#if OG_WIN32
		int		width, height;			//!< Width and height of the console window
		int		minWidth, minHeight;	//!< Minimum width/height, resizing below this value is not possible.
		int		fadeTime;				//!< Time(ms) to fade window alpha from 0 to 255

		const char *logBgImage;			//!< Filename of the log window background image, can be NULL.
		byte	colorLogBg[3];			//!< Background color of the log window
		byte	colorLogText[3];		//!< Color of the log window text
		byte	colorErrorBg[3];		//!< Background color of the error window text
		byte	colorErrorBgFlash[3];	//!< Background flash color of the error window text
		byte	colorErrorText[3];		//!< Color of the error window text
		byte	colorErrorTextFlash[3];	//!< Flash color of the error window text
#elif OG_LINUX
	#warning "Need Linux here FIXME"
#elif OG_MACOS_X
	#warning "Need MacOS here FIXME"
#endif
	};

	//! Static Console Interface
	namespace Console {
	//! @addtogroup Console
	//! @{
		// ==============================================================================
		//! Console Visible Levels
		//!
		//! These won't work on linux, since linux uses a TTY console instead of a window
		// ==============================================================================
		enum VisLevel {
			VIS_HIDDEN,		//!< Invisible
			VIS_NORMAL,		//!< Visible
			VIS_ICONIFIED	//!< Minimized to taskbar
		};

		// ==============================================================================
		//! Initializes this object. 
		//!
		//! @param	params	Options for controlling the operation. 
		//!
		//! @return	true if it succeeds, false if it fails. 
		// ==============================================================================
		bool		Init( const ConsoleParams_t *params );

		// ==============================================================================
		//! Shutdowns console
		//!
		//! Unlinks all cvars & commands, and closes the window.
		// ==============================================================================
		void		Shutdown( void );

		// ==============================================================================
		//! Synchronize window thread ( so window events get forwarded )
		// ==============================================================================
		void		Synchronize( void );

		// ==============================================================================
		//! Shows the console
		//!
		//! @param	visLevel	The vis level
		//! @param	quitOnClose	If set to true, a quit event will be send when the user clicks
		//!						on the Close Button of the window, otherwise it will just be hidden.
		//! @see ConVisLevel
		// ==============================================================================
		void		ShowConsole( VisLevel visLevel, bool quitOnClose );

		// ==============================================================================
		//! Adds the text to the log window
		//!
		//! @param	message	The text to be printed
		// ==============================================================================
		void		Print( const char *message );

		// ==============================================================================
		//! Print a fatal error message and go into error state
		//!
		//! @param	message	The error message
		// ==============================================================================
		void		FatalError( const char *message );
	//! @}
	}

	//! Functions the user must supply
	namespace User {
	//! @addtogroup UserCallbacks
	//! @{
		//! @defgroup UserConsole Console (User)
		//!	Functions the user must supply, if using the Console library.
		//! @{

			// ==============================================================================
			//! When the console menu is opened, this is used to populate it.
			//! @param callback				Callback function to add entries to the menu.
			//!
			//! \b Callback \b parameters:
			//! @li \a name		- The displayed text.
			//! @li \a command	- The command to execute if this entry was selected.
			// ==============================================================================
			void			OnConsoleMenuOpen( void( *callback )( const char *name, const char *command ) );

			// ==============================================================================
			//! Called when the user explicitly wants to quit the application
			//!
			//! For example when the user selected the entry "Quit" in the menu or entered "quit" as a command.
			// ==============================================================================
			void			OnForceExit( void );
		//! @}
	//! @}
	}

//! @}
}
#include <og/Console/CmdSystem.inl>
#include <og/Console/CvarSystem.inl>

#endif
