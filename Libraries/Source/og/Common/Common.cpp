/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Basic Library stuff
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

FileSystemCore *commonFS = NULL;

Format &operator << ( Format &fmt, const String &value ) {
	fmt.TryPrint( "%s", value.c_str() );
	return fmt.Finish();
}

Format &operator << ( Format &fmt, const Color &value ) {
	byte r, g, b, a;
	value.ToBytes( r, g, b, a );
	fmt.TryPrint( "#%X%X%X%X", r, g, b, a );
	return fmt.Finish();
}

}
