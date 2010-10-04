/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Model loading demo
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

#include <og/Gloot/glmin.h>
#include <og/Gloot/Gloot.h>
#include <og/Common/Common.h>
#include "ModelGL.h"

/*
==============================================================================

  ogDemoWindow

==============================================================================
*/
class ogDemoWindow : public og::WindowListener {
public:
	ogDemoWindow();
	bool	Init( void );

	bool	IsOpen( void ) { return window && window->IsOpen(); }

	void	ModelToolDrag( float length, int action, int axis );
	void	DoDrag( void );
	void	DrawLineMap( void );
	void	Draw( void );
	void	ResetGame( void );

protected:
	bool	OnClose( bool forced );
	void	OnFocusChange( bool hasFocus );
	void	OnKeyEvent( int key, bool down );
	void	OnMouseButton( int button, bool down );

public:
	og::Window *window;
	int			lastTime;

	og::Angles	camAngles;
	float		camHeight, camRange;
	int			drag;
	int			dragPoint[2];
	bool		mouseButtons[3];
	bool		shiftDown;
	bool		ctrlDown;

	ogModelGL	model;
};
