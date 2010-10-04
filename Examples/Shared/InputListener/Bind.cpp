/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Bind
-----------------------------------------

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
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

#include "Bind.h"

/*
==============================================================================

  ogBind

==============================================================================
*/
/*
================
ogBind::ogBind
================
*/
ogBind::ogBind( const char *name ) {
	strName = name;
	Reset();
}

/*
================
ogBind::ButtonChange
================
*/
void ogBind::ButtonChange( bool down ) {
	currentState.down += down ? 1 : -1;
}

/*
================
ogBind::AxisChange
================
*/
void ogBind::AxisChange( float axis ) {
	currentState.scale = axis;
}

/*
================
ogBind::OnFrameEnd
================
*/
void ogBind::OnFrameEnd( void ) {
	// only called when this is actually bound to a button/axis
	// might be called multiple times ( once for each button/axis bound to this bind )
	lastState.scale = currentState.scale;
	lastState.down = currentState.down;
}

/*
================
ogBind::Reset
================
*/
void ogBind::Reset( void ) {
	currentState.down = lastState.down = 0;
	currentState.scale = lastState.scale = 0.0f;
}

/*
================
ogBind::IsDown
================
*/
bool ogBind::IsDown( void ) const {
	return currentState.down > 0;
}

/*
================
ogBind::WasDown
================
*/
bool ogBind::WasDown( void ) const {
	return lastState.down > 0;
}

/*
================
ogBind::CurScale
================
*/
float ogBind::CurScale( void ) const {
	if ( currentState.down > 0 )
		return 1.0f;
	else if ( currentState.down < 0 )
		return -1.0f;
	return currentState.scale;
}

/*
================
ogBind::OldScale
================
*/
float ogBind::OldScale( void ) const {
	if ( lastState.down > 0 )
		return 1.0f;
	else if ( lastState.down < 0 )
		return -1.0f;
	return lastState.scale;
}
