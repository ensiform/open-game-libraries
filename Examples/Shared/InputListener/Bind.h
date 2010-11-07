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

#ifndef __DEMO_BIND_H__
#define __DEMO_BIND_H__

#include <og/Common.h>

/*
==============================================================================

  ogBind

==============================================================================
*/
class ogBind {
public:
	ogBind( const char *name );

	void			ButtonChange( bool down );
	void			AxisChange( float axis );
	void			OnFrameEnd( void );
	void			Reset( void );

	bool			IsFreshDown( void ) const { return IsDown() && !WasDown(); }
	bool			IsDown( void ) const;
	bool			WasDown( void ) const;

	float			CurScale( void ) const;
	float			OldScale( void ) const;

	void			SetSensitivity( float value );
	float			GetSensitivity( void ) const;

private:
	friend class ogInputListener;
	og::String		strName;
	bool			useScale;

	struct State {
		int		down;		// How many buttons are down
		float	scale;		// This will act different depending on inputType:
							// Button(needs 2):
							//	-1.0f (negative button is down)
							//	 1.0f (positive button is down)
							//	 0.0f (no/both buttons)
							// Mouse: Mouse move distance set to a percentage value ( MOUSE_AXIS_DISTANCE is 100% )
							// Fable Device: At which angle the stick is
							// No matter what, the values always stay within -1.0f and +1.0f
	};
	State currentState;
	State lastState;
};

#endif
