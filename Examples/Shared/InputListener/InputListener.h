/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Input Listener
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

#ifndef __DEMO_INPUT_MANAGER_H__
#define __DEMO_INPUT_MANAGER_H__

#include "Bind.h"
#include <og/Gloot/Gloot.h>
#include <og/Fable/Fable.h>
#include <map>

/*
==============================================================================

  ogInputDescriptor

==============================================================================
*/
class ogInputDescriptor {
public:
	enum {
		IN_KEY,
		IN_DEVICE_AXIS,
		IN_DEVICE_BUTTON,
		IN_DEVICE_POV,
		IN_MOUSE_AXIS,
		IN_MOUSE_BUTTON,
		IN_MOUSE_WHEEL
	};
	int GetType( void ) { return type; }
	int GetDeviceId( void ) { return devId; }		// Fable devices only
	int GetId( void ) { return id; }				// keyboard -> which key
													// mouse -> which axis or button
													// Fable device -> which axis, button or pov
	int GetDirection( void ) { return direction; }	// mousewheel -> up(1) or down(-1)
													// mouse or Fable device axis: normal(1) or flipped(-1)
													// Fable device pov: 0-7 ( 0:north, 1 northeast, etc. (clockwise rotation) )

protected:
	int	type;
	int	devId;
	int	id;
	int	direction;
};


class ogKeyListener;
class ogAxisListener;
class ogWheelListener;
class ogPOVListener;
typedef std::map<int, ogKeyListener *> KeyListenerMap;
typedef std::pair<int, ogKeyListener *> KeyListenerPair;

enum {
	WAIT_FOR_ALL	= BIT(0),
	WAIT_NO_AXIS	= BIT(1),
	WAIT_HAS_RESULT	= BIT(2),
};

/*
==============================================================================

  ogDeviceListener

==============================================================================
*/
class ogDeviceListener : public og::Fable::DeviceListener {
protected:

	// ---------------------- Public DeviceListener Interface -------------------

	void	OnAxis( uInt axis, float value );
	void	OnPOV( uInt pov, int value );
	void	OnButton( uInt button, bool down );

	// ---------------------- Internal ogDeviceListener Members -------------------

public:
	ogInputListener *listener;

	og::Fable::Device *device;
	og::List<ogKeyListener *> buttons;
	og::List<ogAxisListener *> axes;
	og::List<ogPOVListener *> povs;
};

/*
==============================================================================

  ogInputListener

==============================================================================
*/
class ogInputListener : public og::WindowListener {
protected:
	// ---------------------- Public WindowListener Interface -------------------

	virtual void	OnKeyEvent( int key, bool down );
	virtual void	OnMouseButton( int button, bool down );
	virtual void	OnMouseWheel( int delta );
	virtual void	OnMouseMove( int x, int y );
	virtual void	OnFocusChange( bool hasFocus );

	// ---------------------- Internal ogInputListener Members -------------------

public:
	ogInputListener();
	virtual ~ogInputListener();

	void		OnFrameStart( void );		// checks for mouse and Fable device movements
	void		OnFrameEnd( void );			// to be called by window listener to update the lastState of each bind.

	void		RegisterBind( ogBind *bind );
	void		UnregisterBind( ogBind *bind );
	void		UnregisterAllBinds( void );
	ogBind *	GetBindForName( const char *name );

	void		LoadConfig( const char *filename );
	void		UnbindAll( ogBind *bind=NULL );
//	const char *InputStringForBind( ogBind *bind, int max, const char *seperator );

	void		InitInput( void );
	void		ShutdownInput( void );
	void		ResetInput( void );

public:
	bool		IsWaitingForInput( void ) { return waitForInput != 0; }
	bool		HasInputResult( void ) { return (waitForInput & WAIT_HAS_RESULT) != 0; }
	void		WaitForInput( bool noAxis = true ) { waitForInput = noAxis ? WAIT_NO_AXIS : WAIT_FOR_ALL; }
	void		StopWaitForInput( void ) { waitForInput = 0; }

private:
	// Keyboard
	void		BindKey( int key, ogBind *bind, bool inversed=false );
	void		UnbindKey( int key );

	// Mouse Axis ( 0 = horizontal, 1 = vertical )
	void		BindMouseAxis( int axis, ogBind *bind, bool inversed=false );
	void		UnbindMouseAxis( int axis );
	// Mouse Button, -1 = wheelUp, -2 = wheelDown
	void		BindMouseButton( int button, ogBind *bind, bool inversed=false );
	void		UnbindMouseButton( int button );

	// Fable Device Axis
	void		BindDeviceAxis( int deviceId, int axis, ogBind *bind, bool inversed=false );
	void		UnbindDeviceAxis( int deviceId, int axis );

	// Fable Device Button
	void		BindDeviceButton( int deviceId, int button, ogBind *bind, bool inversed=false );
	void		UnbindDeviceButton( int deviceId, int button );

	// Fable Device POV (how to handly this ? .. currently 2 axes)
	void		BindDevicePOV( int deviceId, int pov, ogBind *bindX, ogBind *bindY, bool inversedX=false, bool inversedY=false );
	void		UnbindDevicePOV( int deviceId, int pov );

private:
	friend class ogDeviceListener;
	bool		isActive;
	int			mouseX, mouseY;

	og::DictEx<ogBind *> bindList;

	ogKeyListener *		mouseButtonActions[og::Mouse::MaxButtons];
	ogWheelListener *	mouseWheelActions[2]; // 0 = down 1 = up
	ogAxisListener *	mouseAxisActions[2];
	KeyListenerMap		keyActions;

	og::ListEx<ogDeviceListener> fableDevices;

	int	waitForInput;
	ogInputDescriptor *searchResult;
};

#endif
