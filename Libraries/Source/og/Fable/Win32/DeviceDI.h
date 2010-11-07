// ==============================================================================
//! @file
//! @brief	DirectInput Device
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

#ifndef __OG_DEVICE_DI_H__
#define __OG_DEVICE_DI_H__

#if OG_WIN32

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace og {
namespace Fable {
const int DINPUT_MAX_AXES = 6 + 8; // 6 axes + 8 sliders
const int DINPUT_AXIS_LENGTH = 32767;


/*
==============================================================================

  DeviceDI

==============================================================================
*/
class DeviceDI : public Device {
public:
	// ---------------------- Public Device Interface -------------------

	const char *GetName( void ) { return deviceName.c_str(); }
	void		Refresh( DeviceListener *listener );
	uInt		GetNumButtons( void ) const { return numButtons; }
	uInt		GetNumAxes( void ) const { return numAxes; }
	uInt		GetNumPOVs( void ) const { return numPOVs; }
	bool		GetButton( uInt index ) const { return state.rgbButtons[index] != 0; }
	float		GetAxis( uInt index ) const;
	int			GetPOV( uInt index ) const;

	// ---------------------- Internal DeviceDI Members -------------------

	DeviceDI( const WCHAR *name, IDirectInputDevice8 *dev );
	~DeviceDI();

	BOOL		AddAxis( LPCDIDEVICEOBJECTINSTANCE devObjInst );

private:
	String		deviceName;
	IDirectInputDevice8 *device;

	uInt		numSliders;
	uInt		numAxes;
	uInt		numPOVs;
	uInt		numButtons;
	DIJOYSTATE2 state;
	DIJOYSTATE2 stateOld;
	int			axisOffset[DINPUT_MAX_AXES];
};

}
}

#endif
#endif