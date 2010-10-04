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

#include "DeviceDI.h"
#include "XInputChecker.h"

#ifdef OG_WIN32

namespace og {
namespace Fable {

const DWORD AXIS_Y_OFFSET = FIELD_OFFSET(DIJOYSTATE2, lY);
const DWORD AXIS_RY_OFFSET = FIELD_OFFSET(DIJOYSTATE2, lRy);

extern List<Device *> deviceList;
extern HWND hWndInput;
IDirectInput8 *directInput = NULL;

/*
================
EnumAxes
================
*/
BOOL CALLBACK EnumAxes( LPCDIDEVICEOBJECTINSTANCE devObjInst, LPVOID pvRef ) {
	return ((DeviceDI*)pvRef)->AddAxis( devObjInst );
}

/*
================
EnumDevices
================
*/
BOOL CALLBACK EnumDevices( LPCDIDEVICEINSTANCE devInst, LPVOID pvRef ) {
	XInputChecker *checker = (XInputChecker *)pvRef;
	if ( checker->IsXInput( devInst->guidProduct ) )
		return DIENUM_CONTINUE;

	if( GET_DIDEVICE_TYPE(devInst->dwDevType) == DI8DEVTYPE_JOYSTICK ||
		GET_DIDEVICE_TYPE(devInst->dwDevType) == DI8DEVTYPE_GAMEPAD ||
		GET_DIDEVICE_TYPE(devInst->dwDevType) == DI8DEVTYPE_1STPERSON ||
		GET_DIDEVICE_TYPE(devInst->dwDevType) == DI8DEVTYPE_DRIVING ||
		GET_DIDEVICE_TYPE(devInst->dwDevType) == DI8DEVTYPE_FLIGHT)
	{
		IDirectInputDevice8 *device;
		HRESULT hr = directInput->CreateDevice( devInst->guidInstance, &device, NULL );
		if( FAILED(hr) )
			return DIENUM_CONTINUE;
		if ( FAILED( hr = device->SetDataFormat( &c_dfDIJoystick2 ) ) )
			return DIENUM_CONTINUE;
		if ( FAILED(hr = device->SetCooperativeLevel( hWndInput, DISCL_EXCLUSIVE | DISCL_BACKGROUND )) )
			return DIENUM_CONTINUE;

		deviceList.Append( new DeviceDI( devInst->tszInstanceName, device ) );
	}

	return DIENUM_CONTINUE;
}

/*
==============================================================================

  DeviceDI

==============================================================================
*/
/*
================
DeviceDI::DeviceDI
================
*/
DeviceDI::DeviceDI( const WCHAR *name, IDirectInputDevice8 *dev ) {
	deviceName.FromWide( name );
	device = dev;
	for( int i=0; i<DINPUT_MAX_AXES; i++ )
		axisOffset[i] = -1;
	
	DIDEVCAPS caps;
	caps.dwSize = sizeof(DIDEVCAPS);
	device->GetCapabilities(&caps);

	numPOVs = caps.dwPOVs;
	numButtons = caps.dwButtons;
	numAxes = 0;
	device->EnumObjects( EnumAxes, this, DIDFT_AXIS );
}

/*
================
DeviceDI::~DeviceDI
================
*/
DeviceDI::~DeviceDI() {
	if ( device ) {
		device->Unacquire();
		device->Release();
	}
}

/*
================
DeviceDI::AddAxis
================
*/
BOOL DeviceDI::AddAxis( LPCDIDEVICEOBJECTINSTANCE devObjInst ) {
	if ( devObjInst->guidType != GUID_XAxis && devObjInst->guidType != GUID_YAxis
		&& devObjInst->guidType != GUID_ZAxis && devObjInst->guidType != GUID_RxAxis
		&& devObjInst->guidType != GUID_RyAxis && devObjInst->guidType != GUID_RzAxis
		&& devObjInst->guidType != GUID_Slider ) {
		return DIENUM_CONTINUE;
	}

	DIPROPRANGE range;
	range.diph.dwSize = sizeof(DIPROPRANGE);
	range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	range.diph.dwHow = DIPH_BYID;
	range.diph.dwObj = devObjInst->dwType;
	range.lMin = -DINPUT_AXIS_LENGTH;
	range.lMax = DINPUT_AXIS_LENGTH;

	if ( FAILED(device->SetProperty(DIPROP_RANGE, &range.diph)) )
		return DIENUM_STOP;

	axisOffset[numAxes++] = devObjInst->dwOfs;
	/*
	if( ( devObjInst->dwFlags & DIDOI_FFACTUATOR ) != 0 )
		numForceAxes++;
	*/
	return DIENUM_CONTINUE;
}

OG_INLINE LONG GetAxisByOffset( const DIJOYSTATE2 *state, int offset ) {
	return *(LONG *)(((byte *)state)+offset);
}

/*
================
DeviceDI::Refresh
================
*/
void DeviceDI::Refresh( DeviceListener *listener ) {
	HRESULT hr = device->Poll(); 
	if ( FAILED(hr) ) {
		hr = device->Acquire();
		OG_ASSERT( SUCCEEDED(hr) ); // fixme: disconnect
	}

	hr = device->GetDeviceState( sizeof(DIJOYSTATE2), &state );
	OG_ASSERT( SUCCEEDED(hr) ); // fixme: disconnect
	
	if ( listener ) {
		for( uInt i=0; i<numAxes; i++ ) {
			if ( GetAxisByOffset( &state, axisOffset[i] ) != GetAxisByOffset( &stateOld, axisOffset[i] ) )
				listener->OnAxis( i, GetAxis( i ) );
		}
		for( uInt i=0; i<numPOVs; i++ ) {
			if ( state.rgdwPOV[i] != stateOld.rgdwPOV[i] )
				listener->OnPOV( i, GetPOV( i ) );
		}
		for( uInt i=0; i<numButtons; i++ ) {
			if ( state.rgbButtons[i] != stateOld.rgbButtons[i] )
				listener->OnButton( i, GetButton( i ) );
		}
	}
	memcpy( &stateOld, &state, sizeof(state) );
}

/*
================
DeviceDI::GetAxis
================
*/
float DeviceDI::GetAxis( uInt index ) const {
	if ( index >= numAxes )
		return 0.0f;
	
	if ( axisOffset[index] == AXIS_Y_OFFSET || axisOffset[index] == AXIS_RY_OFFSET )
		return -static_cast<float>( static_cast<double>( GetAxisByOffset( &state, axisOffset[index] ) ) / static_cast<double>( DINPUT_AXIS_LENGTH ) );

	return static_cast<float>( static_cast<double>( GetAxisByOffset( &state, axisOffset[index] ) ) / static_cast<double>( DINPUT_AXIS_LENGTH ) );
}

/*
================
DeviceDI::GetPOV
================
*/
int DeviceDI::GetPOV( uInt index ) const {
	if ( LOWORD(state.rgdwPOV[index]) == 0xFFFF )
		return -1;
	return state.rgdwPOV[index] / 4500;
}

/*
================
InitDirectInput
================
*/
bool InitDirectInput( void ) {
	if ( !directInput ) {
		HRESULT hr = DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&directInput, NULL );
		if ( FAILED(hr) )
			return false;

		XInputChecker checker;
		directInput->EnumDevices( DI8DEVCLASS_GAMECTRL, EnumDevices, &checker, DIEDFL_ATTACHEDONLY );
	}
	return true;
}

/*
================
ShutdownDirectInput
================
*/
void ShutdownDirectInput( void ) {
	if( directInput ) {
		directInput->Release();
		directInput = NULL;
	}
}

}
}

#endif
