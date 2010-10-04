// ==============================================================================
//! @file
//! @brief	Public Gloot Interface(Monitor Only)
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

#ifndef __OG_GLOOT_MONITOR_H__
#define __OG_GLOOT_MONITOR_H__

#include <og/Setup.h>

//! Open Game Libraries
namespace og {
	//! @defgroup Gloot Gloot (Library)
	//! @{

	const int VIDEOMODE_USER = -2;	//!< Constant index for the users original video mode

	// ==============================================================================
	//! The video mode structure used by GetVideoModes()
	// ==============================================================================
	struct VideoMode {
		int		width;			//!< Width in pixels
		int		height;			//!< Height in pixels
		int		bpp;			//!< Bits Per Pixel
		int		refreshRate;	//!< Refresh rate

	private:
		friend class MonitorEx;
		int		modeId;			//!< For internal use, not for use with GetVideoMode()
	};

	// ==============================================================================
	//! Interface to a monitor
	//!
	//! This offers you the ability to find out videomodes,
	//! set gamma, and lock the monitor for fullscreen use.
	// ==============================================================================
	class Monitor {
	public:
		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~Monitor() {}

		// ==============================================================================
		//! Set the gamma value for the monitor
		//!
		//! @param	value	The gamma value, ranging from x to x ?
		//!
		//! @return	true if it succeeds, false if it fails
		//!
		//! @note	Win32 only supports one gamma ramp.
		//!			I'm not sure if it is used for all monitors or just the main one.
		// ==============================================================================
		virtual	bool	SetGamma( float value ) = 0;

		// ==============================================================================
		//! Restores original gamma
		// ==============================================================================
		virtual	void	RestoreGamma( void ) = 0;

		// ==============================================================================
		//! Locks this monitor
		//!
		//! If your application has multiple windows, you should lock the monitor
		//! before you try to check for and change to fullscreen mode.
		//!
		//! @see Unlock
		// ==============================================================================
		virtual void	Lock( void ) = 0;

		// ==============================================================================
		//! Unlocks this monitor
		//!
		//! @see Lock
		// ==============================================================================
		virtual void	Unlock( void ) = 0;

		// ==============================================================================
		//! Check if a monitor is already in use for fullscreen (by this application)
		//!
		//! @return	true if a window has reserved this monitor for fullscreen mode, false if not.
		//!
		//! @note	This is even true if said window is iconified.
		// ==============================================================================
		virtual bool	IsReserved( void ) = 0;

		// ==============================================================================
		//! Number of video modes available on this monitor
		//!
		//! @return	The number of video modes available
		// ==============================================================================
		virtual	int		NumVideoModes( void ) const = 0;

		// ==============================================================================
		//! Get video mode information
		//!
		//! @param	i	The index of the video mode
		//!
		//! @return	NULL if out of range, else the video mode
		//!
		//! @see NumVideoModes
		// ==============================================================================
		virtual	const VideoMode *GetVideoMode( int i ) const = 0;

		// ==============================================================================
		//! Get the closest video mode to the specified parameters
		//!
		//! @param	w		The wanted width
		//! @param	h		The wanted height
		//! @param	bpp		The wanted bpp
		//! @param	rate	The wanted refreshrate rate
		//!
		//! @return	NULL if it fails, else the closest video mode
		// ==============================================================================
		virtual const VideoMode *GetClosestVideoMode( int w, int h, int bpp, int rate ) = 0;
	};

//! @}
}

#endif
