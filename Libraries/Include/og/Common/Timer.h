// ==============================================================================
//! @file
//! @brief	Clock Tick Profiling
//! @author	Ensiform
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

#ifndef __OG_TIMER_H__
#define __OG_TIMER_H__

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	// ==============================================================================
	//! Timer object
	//!
	//! Used to measure time
	// ==============================================================================
	class Timer {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	elapsed	The initial value in microseconds
		// ==============================================================================
		Timer( uLongLong elapsed=0 );

		// ==============================================================================
		//! Start the timer
		// ==============================================================================
		void			Start( void );

		// ==============================================================================
		//! Stop the timer
		// ==============================================================================
		void			Stop( void );

		// ==============================================================================
		//! Clear the timer
		// ==============================================================================
		void			Clear( void );

		// ==============================================================================
		//! Get the time elapsed in microseconds
		// ==============================================================================
		uLongLong		MicroSeconds( void ) const;

		// ==============================================================================
		//! Get the time elapsed in milliseconds
		// ==============================================================================
		uLong			Milliseconds( void ) const;

		// ==============================================================================
		//! Get the time elapsed in seconds
		// ==============================================================================
		uLong			Seconds( void ) const;
		
		// ==============================================================================
		//! Find out if the timer is currently running
		// ==============================================================================
		bool			IsActive( void ) const;

	private:
		bool			isActive;	//!< Timer is running
		uLongLong		start;		//!< Start time in microseconds
		uLongLong		elapsed;	//!< Elapsed time in microseconds
	};

//! @}
}

#endif
