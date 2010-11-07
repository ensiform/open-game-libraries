// ==============================================================================
//! @file
//! @brief	XInput Check
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2007-2010 Lusito Software
//! @note	Modified version from the DirectInput sample from Microsoft
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

#if defined(__MINGW32__)
#warning "XInputChecker is not working with MinGW, fix it"
#endif
#if OG_WIN32 && !defined(__MINGW32__)

#include "XInputChecker.h"
#include <wbemidl.h>

namespace og {
namespace Fable {

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

/*
==============================================================================

  XInputChecker

==============================================================================
*/
/*
================
XInputChecker::XInputChecker
================
*/
XInputChecker::XInputChecker() {
	// CoInit if needed
	HRESULT hr = CoInitialize( NULL );
	bool bCleanupCOM = SUCCEEDED( hr );

	// Create WMI
	IWbemLocator* pIWbemLocator = NULL;
	hr = CoCreateInstance( __uuidof( WbemLocator ), NULL, CLSCTX_INPROC_SERVER, __uuidof( IWbemLocator ), ( LPVOID* )&pIWbemLocator );
	if( FAILED( hr ) || pIWbemLocator == NULL ) {
		if( bCleanupCOM )
			CoUninitialize();
		return;
	}

	IEnumWbemClassObject* pEnumDevices = NULL;
	IWbemServices* pIWbemServices = NULL;
	IWbemClassObject* pDevices[20] = {0};

	BSTR bstrDeviceID = NULL;
	BSTR bstrClassName = NULL;
	BSTR bstrNamespace = NULL;

	// Do just once
	do {
		bstrNamespace = SysAllocString( L"\\\\.\\root\\cimv2" );	if( bstrNamespace == NULL )	break;
		bstrDeviceID = SysAllocString( L"DeviceID" );				if( bstrDeviceID == NULL )	break;
		bstrClassName = SysAllocString( L"Win32_PNPEntity" );		if( bstrClassName == NULL )	break;

		// Connect to WMI
		hr = pIWbemLocator->ConnectServer( bstrNamespace, NULL, NULL, 0L, 0L, NULL, NULL, &pIWbemServices );
		if( FAILED( hr ) || pIWbemServices == NULL )
			break;

		// Switch security level to IMPERSONATE
		CoSetProxyBlanket( pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
						   RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0 );

		// Get list of Win32_PNPEntity devices
		hr = pIWbemServices->CreateInstanceEnum( bstrClassName, 0, NULL, &pEnumDevices );
		if( FAILED( hr ) || pEnumDevices == NULL )
			break;

		// Loop over all devices
		DWORD uReturned = 0;
		VARIANT var;
		for( ; ; ) {
			// Get 20 at a time
			hr = pEnumDevices->Next( 10000, 20, pDevices, &uReturned );
			if( FAILED( hr ) || uReturned == 0 )
				break;

			for( int i = 0; i < uReturned; i++ ) {
				// For each device, get its device ID
				hr = pDevices[i]->Get( bstrDeviceID, 0L, &var, NULL, NULL );
				if( SUCCEEDED( hr ) && var.vt == VT_BSTR && var.bstrVal != NULL ) {
					// Check if the device ID contains "IG_".  If it does, then it’s an XInput device
					// Unfortunately this information can not be found by just using DirectInput
					if( wcsstr( var.bstrVal, L"IG_" ) ) {
						// If it does, then get the VID/PID from var.bstrVal
						DWORD dwPid = 0, dwVid = 0;
						WCHAR* strVid = wcsstr( var.bstrVal, L"VID_" );
						if( strVid && swscanf( strVid, L"VID_%4X", &dwVid ) != 1 )
							dwVid = 0;
						WCHAR* strPid = wcsstr( var.bstrVal, L"PID_" );
						if( strPid && swscanf( strPid, L"PID_%4X", &dwPid ) != 1 )
							dwPid = 0;

						listIds.Alloc() = MAKELONG( dwVid, dwPid );
					}
				}
				SAFE_RELEASE( pDevices[i] );
			}
		}
	}
	while( 0 ); // Do just once

	if( bstrNamespace )
		SysFreeString( bstrNamespace );
	if( bstrDeviceID )
		SysFreeString( bstrDeviceID );
	if( bstrClassName )
		SysFreeString( bstrClassName );

	for( int i=0; i<20; i++ )
		SAFE_RELEASE( pDevices[i] );
	SAFE_RELEASE( pEnumDevices );
	SAFE_RELEASE( pIWbemLocator );
	SAFE_RELEASE( pIWbemServices );

	if( bCleanupCOM )
		CoUninitialize();
}

/*
================
XInputChecker::IsXInput
================
*/
bool XInputChecker::IsXInput( const GUID &guid ) {
	LinkedList<DWORD>::nodeType *node = listIds.GetFirstNode();
	while( node != NULL ) {
		if( node->value == guid.Data1 )
			return true;
		node = node->GetNext();
	}
	return false;
}

}
}

#endif
