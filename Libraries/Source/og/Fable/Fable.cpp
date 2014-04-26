// ==============================================================================
//! @file
//! @brief	Game Controller Input
//! @author	Santo Pfingsten (TTK-Bandit)
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

#include <og/Common.h>
#include <og/Fable.h>

namespace og {

namespace Fable {
List<Device *> deviceList;

bool PlatformInit( void );
void PlatformShutdown( void );

bool Init( void ) {
	return PlatformInit();
}

void Shutdown( void ) {
	int max = deviceList.Num();
	for( int i=0; i<max; i++ )
		delete deviceList[i];
	deviceList.Clear();

	PlatformShutdown();
}

int GetNumDevices( void ) { return deviceList.Num(); }
Device *GetDevice( uInt index ) { return (index >= deviceList.Num()) ? OG_NULL : deviceList[index]; }

}
}
