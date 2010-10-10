// ==============================================================================
//! @file
//! @brief	Public Gloot Interface(Full)
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2002-2006 Camilla Berglund
//! @note	Copyright (C) 2007-2010 Lusito Software
//! @note	Some of the code is based on GLFW 2.6 (See Readme.txt)
//! @note	Thread safety class: Unknown
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

#ifndef __OG_GLOOT_H__
#define __OG_GLOOT_H__

#include <og/Shared/Shared.h>
#include <og/Gloot/Window.h>
#include <og/Gloot/Monitor.h>
#include <og/Gloot/Input.h>

//! Open Game Libraries
namespace og {
	//! @defgroup Gloot Gloot (Library)
	//! You need to include <og/Gloot/Gloot.h>
	//! @see	UserCallbacks
	//! @todo	Make windowList threadsafe
	//! @todo	InputManager for the demo.
	//! @todo	Check where Error and Warning Messages would be appropriate
	//! @todo	Add checks for out of memory stuff
	//! @todo	Linux window code
	//! @todo	Mac code\n
	//!			It will use a shared context for fullscreen toggle.\n
	//!			Before using the windows context, check if the windows context is already a shared context.
	//! @{

	//! Static Gloot Interface
	namespace Gloot {
	//! @addtogroup Gloot
	//! @{
		// ==============================================================================
		//! Sleeps for a specified time
		//!
		//! @param	msec	Number of milliseconds to sleep.
		// ==============================================================================
		void		Sleep( int msec );

		// ==============================================================================
		//! Get the OpenGL version. 
		//!
		//! @param	major	If non-NULL, receives the major version number
		//! @param	minor	If non-NULL, receives the minor version number
		//! @param	rev		If non-NULL, receives the revision number
		//!
		//! @note	A window needs to be created before calling this (except on win32)
		// ==============================================================================
		void		GetGLVersion( int *major, int *minor, int *rev );

		// ==============================================================================
		//! Synchronize window thread ( so window events get forwarded )
		//!
		//! @todo	Move this to Window ?
		// ==============================================================================
		void		Synchronize( void );

		// ==============================================================================
		//! Number of monitors
		//!
		//! @return	The number of available monitors
		// ==============================================================================
		int			NumMonitors( void );

		// ==============================================================================
		//! Get the specified monitor
		//!
		//! @param	index	The index of the monitor
		//!
		//! @return	NULL if out of range, else the monitor
		// ==============================================================================
		const Monitor *GetMonitor( int index );

		// ==============================================================================
		//! Create a new window
		//!
		//! @param	windowConfig	The window configuration
		//!
		//! @return	NULL if it fails, otherwise a new window object
		// ==============================================================================
		Window *	NewWindow( const WindowConfig *windowConfig );

		// ==============================================================================
		//! Delete a window previously created by NewWindow
		//!
		//! @param	wnd	The window to delete
		// ==============================================================================
		void		DeleteWindow( Window *wnd );

		// ==============================================================================
		//! Create the perspective matrix
		//!
		//! @param	fovX	The horizontal field of view
		//! @param	aspect	The aspect ratio (width/height)
		//! @param	zNear	The near distance
		//! @param	zFar	The far distance
		//! @param	matrix	The resulting matrix
		// ==============================================================================
		void		CreatePerspectiveMatrix( float fovX, float aspect, float zNear, float zFar, float matrix[16] );

		// ==============================================================================
		//! Create the camera matrix for the specified position and angle
		//!
		//! @param	origin	The origin
		//! @param	forward	The forward direction
		//! @param	right	The right direction
		//! @param	up		The up direction
		//! @param	matrix	The resulting matrix
		// ==============================================================================
		void		CreateCameraMatrix( const float origin[3], const float forward[3], const float right[3], const float up[3], float matrix[16] );

	// Input:
		// ==============================================================================
		//! Get the mouse cursor position
		//!
		//! @param	x	If non-NULL, receives the x coordinate
		//! @param	y	If non-NULL, receives the y coordinate
		// ==============================================================================
		void		GetMousePos( int *x, int *y );

		// ==============================================================================
		//! Move the mouse cursor to the specified position
		//!
		//!
		//! @param	x	The new x coordinate
		//! @param	y	The new y coordinate
		// ==============================================================================
		void		SetMousePos( int x, int y );

		// ==============================================================================
		//! Disables system keys (like the windows key)
		//!
		//! @param	disable	true to disable, false to enable. 
		// ==============================================================================
		void		DisableSystemKeys( bool disable );

		// ==============================================================================
		//! Get the key id for the specified string
		//!
		//! @param	name	The name of the key
		//!
		//! @return	-1 if it fails, otherwise the key id
		// ==============================================================================
		int 		GetKeyId( const char *name );

		// ==============================================================================
		//! Get the string for the specified key id
		//!
		//! @param	key		The key id
		//!
		//! @return	NULL if it fails, otherwise the name of the key
		// ==============================================================================
		const char *GetKeyString( int key );
	//! @}
	}

	//! Functions the user must supply
	namespace User {
	//! @addtogroup UserCallbacks
	//! @{
		//! @defgroup UserGloot Gloot (User)
		//!	Functions the user must supply, if using the Gloot library.
		//! @{

			// ==============================================================================
			//! Main entry-point for this application
			//!
			//! @param	argc	Number of command-line arguments
			//! @param	argv	Array of command-line argument strings
			//!
			//! @return	Exit-code for the process - 0 for success, else an error code
			// ==============================================================================
			int			Main( int argc, char *argv[] );
		//! @}
	//! @}
	}

//! @}
}

#endif
