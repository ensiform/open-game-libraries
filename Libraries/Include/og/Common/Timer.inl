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

#ifndef __OG_TIMER_INL__
#define __OG_TIMER_INL__

namespace og {

/*
==============================================================================

  Timer

==============================================================================
*/

/*
=================
Timer::Timer
=================
*/
OG_INLINE Timer::Timer( uLongLong _elapsed ) {
	isActive = false;
	elapsed = _elapsed;
}

/*
=================
Timer::Clear
=================
*/
OG_INLINE void Timer::Clear( void ) {
	isActive = false;
	elapsed = 0;
}

/*
===============
Timer::IsActive
===============
*/
OG_INLINE bool Timer::IsActive( void ) const {
	return isActive;
}

/*
=================
Timer::Start
=================
*/
OG_INLINE void Timer::Start( void ) {
	OG_ASSERT( !isActive );
	isActive = true;
	start = SysInfo::GetHiResTime();
}

/*
=================
Timer::Stop
=================
*/
OG_INLINE void Timer::Stop( void ) {
	OG_ASSERT( isActive );
	elapsed = SysInfo::GetHiResTime() - start;
	isActive = false;
}

/*
=================
Timer::MicroSeconds
=================
*/
OG_INLINE uLongLong Timer::MicroSeconds( void ) const {
	return !isActive ? elapsed : SysInfo::GetHiResTime() - start;
}

/*
=================
Timer::Milliseconds
=================
*/
OG_INLINE uLong Timer::Milliseconds( void ) const {
	return static_cast<uLong>(MicroSeconds() * 0.001);
}

/*
=================
Timer::Seconds
=================
*/
OG_INLINE uLong Timer::Seconds( void ) const {
	return static_cast<uLong>(MicroSeconds() * 0.000001);
}

}

#endif
