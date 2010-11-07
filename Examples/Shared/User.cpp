/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Some shared user functions
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

#include <og/Shared.h>

#define DIALOG_TITLE "Open Game Libraries Example"

namespace og {
	namespace User {
		void Error( og::ErrorId id, const char *msg, const char *param ) {
			// Todo: Throw an exception on the id's you think are important.
			og::String error;
			CreateErrorString( id, msg, param, error );
			og::ErrorDialog( error.c_str(), DIALOG_TITLE );
		}
		void Warning( const char *msg ) {
			og::MessageDialog( og::Format("Warning: $*") << msg, DIALOG_TITLE );
		}
		void AssertFailed( const char *code, const char *function ) {
			og::ErrorDialog( og::Format("Assert($*) failed in $*!") << code << function, DIALOG_TITLE );
		}
	}
}
