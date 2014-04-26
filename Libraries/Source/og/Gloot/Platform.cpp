/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2002-2006 Camilla Berglund, (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Common Manager Code
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
#include <algorithm>
#include <og/Math.h>

namespace og {

void Gloot::GetGLVersion( int *major, int *minor, int *rev ) {
	// Is GLOOT initialized?
	if( !Mgr.initialized )
		return;

	// Return parsed values
	if( major )	*major	= Mgr.GLVersion[0];
	if( minor )	*minor	= Mgr.GLVersion[1];
	if( rev )	*rev	= Mgr.GLVersion[2];
}

Window *Gloot::NewWindow( const WindowConfig *windowCfg ) {
	// Need a valid size
	if( !windowCfg || windowCfg->size != sizeof(WindowConfig) || windowCfg->width <= 0 || windowCfg->height <= 0 )
		return OG_NULL;

	// Ask for a new window
	WindowEx *wnd = WindowEx::NewWindow( windowCfg );

	// Create GL Context
	if ( !wnd->IsOpen() || !wnd->CreateContext() ) {
		delete wnd;
		return OG_NULL;
	}
	// Get OpenGL version
	if ( Mgr.windowList.IsEmpty() && Mgr.GLVersion[0] == -1 ) {
		// Get OpenGL version string
		const GLubyte *version = glGetString( GL_VERSION );
		if( version ) {
			// Parse string
			GLubyte *ptr = (GLubyte*) version;
			for( Mgr.GLVersion[0] = 0; *ptr >= '0' && *ptr <= '9'; ptr ++ )
				Mgr.GLVersion[0] = 10*Mgr.GLVersion[0] + (*ptr - '0');

			if( *ptr == '.' ) {
				ptr ++;
				for( Mgr.GLVersion[1] = 0; *ptr >= '0' && *ptr <= '9'; ptr ++ )
					Mgr.GLVersion[1] = 10*Mgr.GLVersion[1] + (*ptr - '0');

				if( *ptr == '.' ) {
					ptr ++;
					for( Mgr.GLVersion[2] = 0; *ptr >= '0' && *ptr <= '9'; ptr ++ )
						Mgr.GLVersion[2] = 10*Mgr.GLVersion[2] + (*ptr - '0');
				}
			}
		}
	}
	Mgr.windowList.Append( wnd );
	return wnd;
}

void Gloot::DeleteWindow( Window *wnd ) {
	if( !wnd )
		return;
	WindowEx *theWnd = static_cast<WindowEx*>(wnd);
	int index = Mgr.windowList.Find( theWnd );
	if ( index != -1 )
		Mgr.windowList.Remove( index );
	theWnd->Close();
	delete theWnd;
}

int Gloot::NumMonitors( void ) { return Mgr.monitorList.Num(); }
const Monitor *Gloot::GetMonitor( int index ) {
	return (index < 0 || index >= Mgr.monitorList.Num()) ? OG_NULL : Mgr.monitorList[index];
}

const float PI = 3.14159265358979323846f;
const float DEG_TO_RAD = PI/180.0f;
const float RAD_TO_DEG = 180.0f/PI;

OG_INLINE float Deg2Rad( float f ) {
	return f * DEG_TO_RAD;
}
OG_INLINE float Rad2Deg( float f ) {
	return f * RAD_TO_DEG;
}

void Gloot::CreatePerspectiveMatrix( float fovX, float aspect, float zNear, float zFar, float matrix[16] ) {
	static const float limit = Math::Deg2Rad( 179.0f );
	float fovRX = Math::Deg2Rad(fovX);
	float x = aspect / tanf( fovRX * 0.5f );
	float fovRY = 2.0f * atan2f( 1.0f, x );

	fovRX += 0.5f * (fovRX-fovRY);
	if ( fovRX > limit )
			fovRX = limit;

	x = aspect / tanf( fovRX * 0.5f );
	fovRY = 2.0f * atan2f( 1.0f, x );
	if ( fovRY > limit )
			fovRY = limit;

	float xFac = tanf( fovRX * 0.5f );
	float yFac = tanf( fovRY * 0.5f );

	float depth = zFar - zNear;
	matrix[0]  = 1.0f/xFac; matrix[1]  = 0.0f; matrix[2]  = 0.0f; matrix[3]  = 0.0f;
	matrix[4]  = 0.0f; matrix[5]  = 1.0f/yFac; matrix[6]  = 0.0f; matrix[7]  = 0.0f;
	matrix[8]  = 0.0f; matrix[9]  = 0.0f; matrix[10] = -(zFar+zNear)/depth; matrix[11] = -1.0f;
	matrix[12] = 0.0f; matrix[13] = 0.0f; matrix[14] = -2.0f*zFar*zNear/depth; matrix[15] = 0.0f;
}

void Gloot::CreateCameraMatrix( const float origin[3], const float forward[3], const float right[3], const float up[3], float matrix[16] ) {
	matrix[0]  = right[0]; matrix[1]  = up[0]; matrix[2]  = -forward[0]; matrix[3]  = 0.0f;
	matrix[4]  = right[1]; matrix[5]  = up[1]; matrix[6]  = -forward[1]; matrix[7]  = 0.0f;
	matrix[8]  = right[2]; matrix[9]  = up[2]; matrix[10] = -forward[2]; matrix[11]  = 0.0f;

	matrix[12]  = -(right[0]*origin[0] + right[1]*origin[1] + right[2]*origin[2]);
	matrix[13]  = -(up[0]*origin[0] + up[1]*origin[1] + up[2]*origin[2]);
	matrix[14] = forward[0]*origin[0] + forward[1]*origin[1] + forward[2]*origin[2];
	matrix[15]  = 1.0f;
}

bool StringInExtensionString( const char *string, const char *extensions ) {
	// It takes a bit of care to be fool-proof about parsing the
	// OpenGL extensions string. Don't be fooled by sub-strings, etc.
	const char *start = extensions;
	const char *result, *terminator;
	while( 1 ) {
		result = strstr( start, string );
		if( !result )
			return false;

		terminator = result + strlen( string );
		if( result == start || *(result - 1) == ' ' ) {
			if( *terminator == ' ' || *terminator == '\0' )
				break;
		}
		start = terminator;
	}
	return true;
}

}

// this will be called on win32 too (see win32manager.cpp)
int main( int argc, char *argv[] ) {
	// Platform specific initialization
	if( !og::Mgr.Init() ) {
		og::Mgr.Shutdown();
		return -1;
	}

	int result = og::User::Main( argc, argv );

	// Close All OpenGL windows
	int max = og::Mgr.windowList.Num();
	for( int i=0; i<max; i++ ) {
		og::Mgr.windowList[i]->Close();
		delete og::Mgr.windowList[i];
	}
	og::Mgr.windowList.Clear();

	// Platform specific termination
	og::Mgr.Shutdown();
	return result;
}