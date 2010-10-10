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

#include <og/Common/Common.h>
#include <og/Fable/Fable.h>

#ifdef OG_WIN32

#include <XInput.h>

namespace og {
namespace Fable {

const int XINPUT_MAX_BUTTONS	= 10;
const int XINPUT_MAX_AXES		= 6;
const int XINPUT_MAX_POVS		= 1;
const int XINPUT_AXIS_LENGTH	= 32767;
const int XINPUT_TRIGGER_LENGTH	= 255;

/*
==============================================================================

  DeviceXI

==============================================================================
*/
class DeviceXI : public Device {
public:
	// ---------------------- Public Device Interface -------------------

	const char *GetName( void ) { return deviceName.c_str(); }
	void		Refresh( DeviceListener *listener );
	uInt		GetNumButtons( void ) const { return XINPUT_MAX_BUTTONS; }
	uInt		GetNumAxes( void ) const { return XINPUT_MAX_AXES; }
	uInt		GetNumPOVs( void ) const { return XINPUT_MAX_POVS; }
	bool		GetButton( uInt index ) const;
	float		GetAxis( uInt index ) const;
	int			GetPOV( uInt index ) const;

	// ---------------------- Internal DeviceXI Members -------------------

	DeviceXI( uInt idx ) : index(idx) {
		deviceName = Format("XInput$*") << index;
	}

private:
	String			deviceName;
	XINPUT_STATE	state;
	XINPUT_STATE	stateOld;
	uInt			index;
};

}
}

#endif
