/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2002-2006 Camilla Berglund, (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Common Window Code
Note:    Some of the code is based on GLFW 2.6 (See Readme.txt)
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

#include "Platform.h"

namespace og {

void WindowEx::Close( void ) {
	if ( !isOpen || isClosing )
		return;

	isClosing = true;

	// Call the user-supplied callback, if it exists
	if( windowConfig.listener )
		windowConfig.listener->OnClose( true );

	// Revert fullscreen changes and destroy window
	if ( fsMonitor ) {
		fsMonitor->Revert();
		fsMonitor = OG_NULL;
	}
	Destroy();

	isOpen = false;
}

bool WindowEx::ExtensionSupported( const char *extension ) const {
	// Is GLOOT initialized?
	if( !Mgr.initialized || !isOpen )
		return false;

	// Extension names should not have spaces
	const char *result = strchr( extension, ' ' );
	if( result || *extension == '\0' )
		return false;

	// Check if extension is in the standard OpenGL extensions string
	const char *extensions = (const char *) glGetString( GL_EXTENSIONS );
	if( extensions != OG_NULL && StringInExtensionString( extension, extensions ) )
		return true;

	// Additional platform specific extension checking (e.g. WGL)
#if OG_WIN32
	return Mgr.wglExtensionSupported( hDC, extension );
#elif OG_LINUX
	return Mgr.glxExtensionSupported( extension );
#elif OG_MACOS_X
	#warning "Need MacOS here FIXME"
#endif
}

}
