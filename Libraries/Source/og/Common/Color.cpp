/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Color class
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

#include <og/Common/Common.h>

namespace og {

/*
==============================================================================

  Color

==============================================================================
*/

namespace c_color {
	Color transparent( 0.0f, 0.0f, 0.0f, 0.0f );
	Color white( 1.0f, 1.0f, 1.0f, 1.0f );
	Color black( 0.0f, 0.0f, 0.0f, 1.0f );

	Color gray( 0.5f, 0.5f, 0.5f, 1.0f );
	Color gray_dark( 0.25f, 0.25f, 0.25f, 1.0f );
	Color gray_light( 0.75f, 0.75f, 0.75f, 1.0f );

	Color red( 1.0f, 0.0f, 0.0f, 1.0f );
	Color green( 0.0f, 1.0f, 0.0f, 1.0f );
	Color blue( 0.0f, 0.0f, 1.0f, 1.0f );

	Color cyan( 0.0f, 1.0f, 1.0f, 1.0f );
	Color magenta( 1.0f, 0.0f, 1.0f, 1.0f );
	Color yellow( 1.0f, 1.0f, 0.0f, 1.0f );

	Color orange( 1.0f, 0.5f, 0.0f, 1.0f );
	Color purple( 0.75f, 0.0f, 1.0f, 1.0f );
	Color pink( 1.0f, 0.0f, 1.0f, 1.0f );
	Color brown( 0.5f, 0.25f, 0.0f, 1.0f );
}

}
