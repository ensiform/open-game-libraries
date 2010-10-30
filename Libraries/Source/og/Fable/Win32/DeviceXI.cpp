// ==============================================================================
//! @file
//! @brief	XInput Device
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

#include <og/Shared/Shared.h>
#include <og/Math/Math.h>

#if defined(__MINGW32__)
#warning "XInputChecker is not working with MinGW, fix it"
#endif
#if OG_WIN32 && !defined(__MINGW32__)

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "DeviceXI.h"

#ifndef XUSER_MAX_COUNT
	#define XUSER_MAX_COUNT 4
#endif

namespace og {
namespace Fable {

extern List<Device *> deviceList;
const WORD XINPUT_GAMEPAD_DPAD_ALL = XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_DOWN | XINPUT_GAMEPAD_DPAD_LEFT | XINPUT_GAMEPAD_DPAD_RIGHT;

const WORD XINPUT_GAMEPAD_BUTTONS[] = {
	XINPUT_GAMEPAD_A,
	XINPUT_GAMEPAD_B,
	XINPUT_GAMEPAD_X,
	XINPUT_GAMEPAD_Y,
	XINPUT_GAMEPAD_LEFT_THUMB,
	XINPUT_GAMEPAD_RIGHT_THUMB,
	XINPUT_GAMEPAD_LEFT_SHOULDER,
	XINPUT_GAMEPAD_RIGHT_SHOULDER,
	XINPUT_GAMEPAD_START,
	XINPUT_GAMEPAD_BACK
};

/*
==============================================================================

  DeviceXI

==============================================================================
*/
/*
================
DeviceXI::Refresh
================
*/
void DeviceXI::Refresh( DeviceListener *listener ) {
	ZeroMemory( &state, sizeof(XINPUT_STATE) );
	XInputGetState( index, &state );

	if ( listener ) {
		if ( state.Gamepad.sThumbLX != stateOld.Gamepad.sThumbLX )
			listener->OnAxis( 0, GetAxis( 0 ) );
		if ( state.Gamepad.sThumbLY != stateOld.Gamepad.sThumbLY )
			listener->OnAxis( 1, GetAxis( 1 ) );
		if ( state.Gamepad.sThumbRX != stateOld.Gamepad.sThumbRX )
			listener->OnAxis( 2, GetAxis( 2 ) );
		if ( state.Gamepad.sThumbRY != stateOld.Gamepad.sThumbRY )
			listener->OnAxis( 3, GetAxis( 3 ) );
		if ( state.Gamepad.bLeftTrigger != stateOld.Gamepad.bLeftTrigger )
			listener->OnAxis( 4, GetAxis( 4 ) );
		if ( state.Gamepad.bRightTrigger != stateOld.Gamepad.bRightTrigger )
			listener->OnAxis( 5, GetAxis( 5 ) );

		if ( (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_ALL) != (stateOld.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_ALL) )
			listener->OnPOV( 0, GetPOV( 0 ) );

		for( uInt i=0; i<XINPUT_MAX_BUTTONS; i++ ) {
			if ( (state.Gamepad.wButtons & XINPUT_GAMEPAD_BUTTONS[i]) != (stateOld.Gamepad.wButtons & XINPUT_GAMEPAD_BUTTONS[i]) )
				listener->OnButton( i, GetButton(i) );
		}
	}
	memcpy( &stateOld, &state, sizeof(state) );
}

/*
================
DeviceXI::GetButton
================
*/
bool DeviceXI::GetButton( uInt index ) const {
	if ( index >= XINPUT_MAX_BUTTONS )
		return false;
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_BUTTONS[index]) != 0;
}

/*
================
DeviceXI::GetAxis
================
*/
float DeviceXI::GetAxis( uInt index ) const {
	switch( index ) {
		case 0:
			return static_cast<float>( static_cast<double>(state.Gamepad.sThumbLX) / XINPUT_AXIS_LENGTH );
		case 1:
			return static_cast<float>( static_cast<double>(state.Gamepad.sThumbLY) / XINPUT_AXIS_LENGTH );
		case 2:
			return static_cast<float>( static_cast<double>(state.Gamepad.sThumbRX) / XINPUT_AXIS_LENGTH );
		case 3:
			return static_cast<float>( static_cast<double>(state.Gamepad.sThumbRY) / XINPUT_AXIS_LENGTH );
		case 4:
			return static_cast<float>(state.Gamepad.bLeftTrigger) / XINPUT_TRIGGER_LENGTH;
		case 5:
			return static_cast<float>(state.Gamepad.bRightTrigger) / XINPUT_TRIGGER_LENGTH;
	}
	return 0.0f;
}

/*
================
DeviceXI::GetPOV
================
*/
int DeviceXI::GetPOV( uInt index ) const {
	if ( index >= XINPUT_MAX_POVS )
		return -1;

	Vec2 vec;
	if ( (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0 )
		vec.y = 1;
	else if ( (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0 )
		vec.y = -1;

	if ( (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0 )
		vec.x = -1;
	else if ( (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0 )
		vec.x = 1;

	if ( vec.IsZero() )
		return -1;

	int ret = static_cast<int>( vec.ToAngle() ) / 45;
	return ret == 8 ? 0 : ret;
}

/*
================
InitXInput
================
*/
bool InitXInput( void ) {
	XINPUT_STATE state;
	for( uInt i=0; i<XUSER_MAX_COUNT; i++ ) {
		ZeroMemory( &state, sizeof(XINPUT_STATE) );
		if( XInputGetState( i, &state ) == ERROR_SUCCESS )
			deviceList.Append( new DeviceXI( i ) );
	}
	return true;
}

/*
================
ShutdownXInput
================
*/
void ShutdownXInput( void ) {
}

}
}

#endif
