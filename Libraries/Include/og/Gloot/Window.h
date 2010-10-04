// ==============================================================================
//! @file
//! @brief	Public Gloot Interface(Window Only)
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2002-2006 Camilla Berglund
//! @note	Copyright (C) 2007-2010 Lusito Software
//! @note	Some of the code is based on GLFW 2.6 (See Readme.txt)
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

#ifndef __OG_GLOOT_WINDOW_H__
#define __OG_GLOOT_WINDOW_H__

#include <og/Setup.h>

//! Open Game Libraries
namespace og {
	//! @defgroup Gloot Gloot (Library)
	//! @{

	class Window;
	class Monitor;
	struct VideoMode;

	// ==============================================================================
	//! OpenGL 3/4 context creation values
	//!
	//! @see	WindowConfig
	//! @note	Luckily, these are the same for each platform, so we can directly pass them to OpenGL
	// ==============================================================================
	namespace glContext {
		const int MAJOR_VERSION				= 0x2091;	//!< The major gl version you want
		const int MINOR_VERSION				= 0x2092;	//!< The minor gl version you want
		const int LAYER_PLANE				= 0x2093;	//!< wglCreateLayerContext
		const int FLAGS						= 0x2094;	//!< @see	glContextFlags
		const int PROFILE_MASK				= 0x9126;	//!< @see	glContextProfile
		const int RESET_NOTIFICATION_STRATEGY = 0x8256;	//!< @see	glContextRNS ( GL version >= 3.0 )
	}

	// ==============================================================================
	//! Accepted bits in the attribute value for glContext::FLAGS
	//!
	//! @see	glContext
	// ==============================================================================
	namespace glContextFlags {
		const int DEBUG						= 0x0001;		//!< Debug features, see GL 3 spec
		const int FORWARD_COMPATIBLE		= 0x0002;		//!< Ignore deprecated features of the specified GL version ( >= 3.0 )
		const int ROBUST_ACCESS				= 0x00000004;	//!< Robust buffer access
	}

	// ==============================================================================
	//! Accepted bits in the attribute value for glContext::PROFILE_MASK
	//!
	//! @see	glContext
	// ==============================================================================
	namespace glContextProfile {
		const int CORE						= 0x00000001;	//!< Default profile
		const int COMPATIBILITY				= 0x00000002;	//!< Compatible profile ( GL version >= 3.2 )
		const int ES2						= 0x00000004;	//!< Embedded Systems ( GL version == 2.0 )
	}

	// ==============================================================================
	//! Accepted bits in the attribute value for glContext::RESET_NOTIFICATION_STRATEGY
	//!
	//! @see	glContext
	// ==============================================================================
	namespace glContextRNS {
		const int NO_RESET_NOTIFICATION		= 0x8261;	//!< No reset notification
		const int LOSE_CONTEXT_ON_RESET		= 0x8252;	//!< Lose context
	}

	// ==============================================================================
	//! Window listener
	//!
	//! Callback class for windows. When a window gets certain messages, they will be forwarded.
	// ==============================================================================
	class WindowListener {
	protected:
		friend class WindowEx;
		friend class WindowEvent;

		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~WindowListener() {}

		// ==============================================================================
		//! When a user or the OS wants to close the window
		//!
		//! @param	forced	true if there is no avoiding it
		//!
		//! @return	true if you want to allow this close command 
		// ==============================================================================
		virtual bool		OnClose( bool forced ) { return true; }

		// ==============================================================================
		//! Whenever the window loses or regains focus.
		//!
		//! @param	hasFocus	The new status 
		// ==============================================================================
		virtual void		OnFocusChange( bool hasFocus ) {}

		// ==============================================================================
		//! When a window gets iconified
		//!
		//! @param	iconify	The new status
		// ==============================================================================
		virtual void		OnIconify( bool iconify ) {}

		// ==============================================================================
		//! When the window needs to be redrawn
		// ==============================================================================
		virtual void		OnDamage( void ) {}

		// ==============================================================================
		//! When the window got resized
		//!
		//! @param	w	The new width
		//! @param	h	The new height
		// ==============================================================================
		virtual void		OnSize( int w, int h ) {}

		// ==============================================================================
		//! When the window got moved
		//!
		//! @param	x	The new x coordinate
		//! @param	y	The new y coordinate
		// ==============================================================================
		virtual void		OnMove( int x, int y ) {}
		
	// Keyboard Events
		// ==============================================================================
		//! A key has been pressed or released
		//!
		//! @param	key		The key of interest
		//! @param	down	true if it is pressed, false if released
		// ==============================================================================
		virtual void		OnKeyEvent( int key, bool down ) {}

		// ==============================================================================
		//! A character has been input
		//!
		//! @param	ch	The character that was added
		// ==============================================================================
		virtual void		OnCharEvent( int ch ) {}

	// Mouse Events
		// ==============================================================================
		//! A mouse button has been pressed
		//!
		//! @param	button	The button of interest
		//! @param	down	true if it is pressed, false if released
		// ==============================================================================
		virtual void		OnMouseButton( int button, bool down ) {}

		// ==============================================================================
		//! The mouse wheel has been turned
		//!
		//! @param	delta	How many clicks the mouse wheel did
		// ==============================================================================
		virtual void		OnMouseWheel( int delta ) {}

		// ==============================================================================
		//! The mouse has been moved ( only called when mouse is locked )
		//!
		//! @param	x	Horizontal distance from the window center ( positive is up )
		//! @param	y	Vertical distance from the window center ( positive is right )
		// ==============================================================================
		virtual void		OnMouseMove( int x, int y ) {}
	};

	// ==============================================================================
	//! Window Flags (bitflags)
	// ==============================================================================
	enum WindowFlags {
		WF_FULLSCREEN	= BIT(0),	//!< Start in fullscreen
		WF_NORESIZE		= BIT(1),	//!< Do not allow manual resizing
		WF_DOUBLEBUFFER	= BIT(2),	//!< Use double buffering
		WF_STEREO		= BIT(3),	//!< Use stereo drawing
	};

	// ==============================================================================
	//! Window Spawn parameters
	//!
	//! The equivalent of a PixelFormatDescriptor.\n
	//! They define a wish list of what attributes you'd like to have in your window.
	// ==============================================================================
	struct WindowConfig {
		int				size;				//!< size of this structure: set to sizeof(WindowConfig)

		const char *	title;				//!< Window title (can be changed afterwards)
		unsigned int	flags;				//!< @see WindowFlags
		int				x;					//!< Distance from left in pixels
		int				y;					//!< Distance from top in pixels
		int				width;				//!< Width in pixels
		int				height;				//!< Height in pixels
		byte			multiSamples;		//!< How much FSAA do you want ?

		// Pixel Format:
		byte			rgbaBits[4];		//!< RGBA bits
		byte			depthBits;			//!< Depth bits
		byte			stencilBits;		//!< Stencil bits
		byte			rgbaAccumBits[4];	//!< Accumulation bits
		byte			auxBuffers;			//!< No. of aux buffers

		WindowListener *listener;			//!< Pointer to a window listener, can be NULL

		const int *		contextAttribs;		//!< OpenGL 3 attributes list

		//! If you want to share an OpenGL context among multiple windows,
		//! point sharedContext to the main window (not for the main window itself)
		Window *		sharedContext;
	};

	// ==============================================================================
	//! Window object
	//!
	//! Gives you access to modify a window or get information.
	// ==============================================================================
	class Window {
	public:
		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~Window() {}

		// ==============================================================================
		//! Check if the window is still open
		//!
		//! @return	true if still open, false if not
		// ==============================================================================
		virtual bool	IsOpen( void ) const = 0;

		// ==============================================================================
		//! Check if the window has focus
		//!
		//! @return	true if it has focus, false if not
		// ==============================================================================
		virtual bool	HasFocus( void ) const = 0;

		// ==============================================================================
		//! Extension supported
		//!
		//! @param	extension	The extension
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		virtual bool	ExtensionSupported( const char *extension ) const = 0;

		// ==============================================================================
		//! Get an OpenGL proc address
		//!
		//! @param	procname	The procname
		//!
		//! @return	NULL if it fails, else the proc address
		// ==============================================================================
		virtual void *	GetProcAddress( const char *procname ) const = 0;

		// ==============================================================================
		//! Activate the OpenGL rendering context of this window
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		virtual	bool	MakeCurrent( void ) = 0;

		// ==============================================================================
		//! Enable or disable Vertical Sync
		//!
		//! @param	value	true to enable, false to disable
		// ==============================================================================
		virtual	void	SetSwapinterval( bool value ) = 0;

		// ==============================================================================
		//! Check if Vertical Sync is enabled
		//!
		//! @return	true if enabled, false if not
		// ==============================================================================
		virtual	bool	GetSwapinterval( void ) const = 0;

		// ==============================================================================
		//! Swap GL buffers
		// ==============================================================================
		virtual	void	SwapBuffers( void ) = 0;

		// ==============================================================================
		//! Find the monitor the window is currently on
		//!
		//! @return	NULL if it fails, else the monitor
		// ==============================================================================
		virtual Monitor *GetMonitor( void ) const = 0;

		// ==============================================================================
		//! Set full screen mode
		//!
		//! @param	mode		The video mode to use, NULL if monitor is NULL
		//! @param	monitor		The monitor to go full screen on, NULL to set windowed mode
		// ==============================================================================
		virtual void	SetFullScreen( const VideoMode *mode, Monitor *monitor ) = 0;

		// ==============================================================================
		//! Find out if monitor is in full screen
		//!
		//! @return	true if in full screen mode
		// ==============================================================================
		virtual bool	GetFullScreen( void ) const = 0;

		// ==============================================================================
		//! Set the window title
		//!
		//! @param	title	The new window title ( UTF-8 )
		// ==============================================================================
		virtual	void	SetTitle( const char *title ) = 0;

		// ==============================================================================
		//! Change the window size
		//!
		//! @param	w	The new width
		//! @param	h	The new height
		// ==============================================================================
		virtual	void	SetSize( int w, int h ) = 0;

		// ==============================================================================
		//! Get the size
		//!
		//! @param	w	If non-NULL, receives the window width
		//! @param	h	If non-NULL, receives the window height
		// ==============================================================================
		virtual void	GetSize( int *w, int *h ) const = 0;

		// ==============================================================================
		//! Moves the window to the specified position
		//!
		//! @param	x	The x coordinate
		//! @param	y	The y coordinate
		// ==============================================================================
		virtual	void	SetPos( int x, int y ) = 0;

		// ==============================================================================
		//! Get the current window position
		//!
		//! @param	x	If non-NULL, receives the window x coordinate
		//! @param	y	If non-NULL, receives the window y coordinate
		// ==============================================================================
		virtual void	GetPos( int *x, int *y ) const = 0;

		// ==============================================================================
		//! Enable or disable user resizing of the window
		//!
		//! @param	value	true to enable
		// ==============================================================================
		virtual	void	SetResizable( bool value ) = 0;

		// ==============================================================================
		//! Find out if the window is currently resizable
		//!
		//! @return	true if is resizable, false if not
		// ==============================================================================
		virtual	bool	IsResizable( void ) const = 0;

		// ==============================================================================
		//! Iconifies (minimizes) this window
		//!
		//! @see Restore
		// ==============================================================================
		virtual	void	Iconify( void ) = 0;

		// ==============================================================================
		//! Restores this window if it was iconified
		//!
		//! @see Iconify
		// ==============================================================================
		virtual	void	Restore( void ) = 0;

		// ==============================================================================
		//! Find out if the window is currently iconified
		//!
		//! @return	true if iconified, false if not
		// ==============================================================================
		virtual	bool	IsIconified( void ) const = 0;

		// ==============================================================================
		//! Lock the mouse pointer to this window
		//!
		//! @param	locked	true to lock, false to unlock
		// ==============================================================================
		virtual	void	LockMouse( bool locked ) = 0;

		// ==============================================================================
		//! Get the window configuration
		//!
		//! @return	The current settings of this window
		//!
		//! @note	Not every information is available.. need to check the details myself
		// ==============================================================================
		virtual const WindowConfig *GetWindowConfig( void ) const = 0;
	};

//! @}
}

#endif
