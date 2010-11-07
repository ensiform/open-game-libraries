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

#include "InputListener.h"
#include <og/Math.h>

const float	FABLE_AXIS_DETECTION_TRIGGER = 0.75f;
const int	MOUSE_AXIS_DETECTION_TRIGGER = 100;

#define SAFE_RELEASE_CONTROL(ctrl) {\
		if ( ctrl != NULL ) {\
			ctrl->Reset();\
			delete ctrl;\
			ctrl = NULL;\
		}\
	}

/*
==============================================================================

  ogControlListener

==============================================================================
*/
class ogControlListener {
public:
	ogControlListener( ogBind *_bind ) : bind(_bind) {}
	virtual ~ogControlListener() {}

	virtual void	Reset( void ) { bind->Reset(); }
	virtual void	OnFrameEnd( void ) { bind->OnFrameEnd(); }

protected:
	friend class ogInputListener;
	ogBind *bind;
};

/*
==============================================================================

  ogKeyListener

==============================================================================
*/
class ogKeyListener : public ogControlListener {
public:
	ogKeyListener( ogBind *bind, bool _inversed ) : ogControlListener(bind), last(false), inversed(_inversed) {}

	OG_INLINE void	SetValue( bool value ) {
		if ( value != last ) {
			bind->ButtonChange( inversed ? !value : value );
			last = value;
		}
	}
	void	Reset( void ) { last = false; ogControlListener::Reset(); }

protected:
	bool	last;
	bool	inversed;
};

/*
==============================================================================

  ogWheelListener

==============================================================================
*/
class ogWheelListener : public ogControlListener {
public:
	ogWheelListener( ogBind *bind ) : ogControlListener(bind), isTriggered(false) {}

	OG_INLINE void	Trigger( void ) {
		if ( !isTriggered ) {
			// false will be set on frame end
			bind->ButtonChange( true );
			isTriggered = true;
		}
	}
	void	OnFrameEnd( void ) {
		if ( isTriggered ) {
			bind->ButtonChange( false );
			isTriggered = false;
		}
		ogControlListener::OnFrameEnd();
	}

protected:
	bool	isTriggered;
};

/*
==============================================================================

  ogAxisListener

==============================================================================
*/
class ogAxisListener : public ogControlListener {
public:
	ogAxisListener( ogBind *bind, bool _inversed ) : ogControlListener(bind), inversed(_inversed) {}

	OG_INLINE void	SetValue( float value ) {
		if ( value != last ) {
			bind->AxisChange( inversed ? -value : value );
			last = value;
		}
	}

protected:
	float	last;
	bool	inversed;
};

/*
==============================================================================

  ogPOVListener

==============================================================================
*/
class ogPOVListener {
public:
	ogPOVListener( ogBind *_bindX, bool _inversedX, ogBind *_bindY, bool _inversedY )
		: bindX(_bindX), bindY(_bindY), last(-1), inversedX(_inversedX), inversedY(_inversedY) {}

	OG_INLINE void	SetValue( int value ) {
		if ( last != value ) {
			og::Vec2 axes;
			if ( value == 0 || value == 1 || value == 7 )
				axes.y = 1.0f;
			else if ( value >= 3 && value <= 5 )
				axes.y = -1.0f;
			if ( value >= 1 && value <= 3 )
				axes.x = 1.0f;
			else if ( value >= 5 && value <= 7 )
				axes.x = -1.0f;
			bindX->AxisChange( inversedX ? -axes.x : axes.x );
			bindY->AxisChange( inversedY ? -axes.y : axes.y );
			last = value;
		}
	}

	void	Reset( void ) { last = -1; bindX->Reset(); bindY->Reset(); }
	void	OnFrameEnd( void ) { bindX->OnFrameEnd(); bindY->OnFrameEnd(); }

protected:
	int		last;
	bool	inversedX;
	bool	inversedY;

protected:
	friend class ogInputListener;
	ogBind *bindX;
	ogBind *bindY;
};

/*
==============================================================================

  ogDeviceListener

==============================================================================
*/
void ogDeviceListener::OnAxis( uInt axis, float value ) {
	if ( listener->waitForInput != 0 ) {
		if ( !(listener->waitForInput & WAIT_NO_AXIS|WAIT_HAS_RESULT) ) {
			if ( og::Math::Fabs( value ) > FABLE_AXIS_DETECTION_TRIGGER ) {
				listener->waitForInput |= WAIT_HAS_RESULT;
				// fixme: create searchResult
			}
		}
		return;
	}
	if ( axis < device->GetNumAxes() && axes[axis] != NULL )
		axes[axis]->SetValue( value );
}
void ogDeviceListener::OnPOV( uInt pov, int value ) {
	if ( listener->waitForInput != 0 ) {
		if ( !(listener->waitForInput & WAIT_HAS_RESULT) ) {
			listener->waitForInput |= WAIT_HAS_RESULT;
			// fixme: create searchResult
		}
		return;
	}
	if ( pov < device->GetNumPOVs() && povs[pov] != NULL )
		povs[pov]->SetValue( value );
}
void ogDeviceListener::OnButton( uInt button, bool down ) {
	if ( listener->waitForInput != 0 ) {
		if ( !(listener->waitForInput & WAIT_HAS_RESULT) ) {
			listener->waitForInput |= WAIT_HAS_RESULT;
			// fixme: create searchResult
		}
		return;
	}
	if ( button < device->GetNumButtons() && buttons[button] != NULL )
		buttons[button]->SetValue( down );
}

/*
==============================================================================

  ogInputListener

==============================================================================
*/
/*
================
ogInputListener::ogInputListener
================
*/
ogInputListener::ogInputListener() {
	memset( mouseButtonActions, 0, sizeof(mouseButtonActions) );
	memset( mouseWheelActions, 0, sizeof(mouseWheelActions) );
	memset( mouseAxisActions, 0, sizeof(mouseAxisActions) );
	mouseX = mouseY = 0;

	waitForInput = 0;
	searchResult = NULL;
}

/*
================
ogInputListener::~ogInputListener
================
*/
ogInputListener::~ogInputListener() {
	ShutdownInput();
}

/*
================
ogInputListener::RegisterBind
================
*/
void ogInputListener::RegisterBind( ogBind *bind ) {
	bindList[bind->strName.c_str()] = bind;
}

/*
================
ogInputListener::UnregisterBind
================
*/
void ogInputListener::UnregisterBind( ogBind *bind ) {
	UnbindAll( bind );
	int index = bindList.Find( bind->strName.c_str() );
	if ( index != -1 )
		bindList.Remove( index );
}

/*
================
ogInputListener::UnregisterAllBinds
================
*/
void ogInputListener::UnregisterAllBinds( void ) {
	UnbindAll();
	bindList.Clear();
}

/*
================
ogInputListener::UnregisterAllBinds
================
*/
ogBind *ogInputListener::GetBindForName( const char *name ) {
	int index = bindList.Find( name );
	if ( index != -1 )
		return bindList[index];
	return NULL;
}

/*
================
ogInputListener::LoadConfig
================
*/
void ogInputListener::LoadConfig( const char *filename ) {
	og::XDeclParser parser;
	if ( !parser.LoadFile(filename) )
		return;

	ogBind *bind = NULL;
	const og::XDeclNode *root = parser.GetFirstChildByName("Keyboard");
	if ( root ) {
		const og::XDeclNode *node = root->GetFirstChildByName("Key");
		og::String id, bindName;
		int key;
		while( node ) {
			const og::Dict &dict = node->GetDict();
			if ( dict.Get("id", "", id ) ) {
				if ( dict.Get("bind", "", bindName ) ) {
					bind = GetBindForName( bindName.c_str() );
					key = og::Gloot::GetKeyId( id.c_str() );
					if ( bind != NULL && key != -1 )
						BindKey( key, bind, dict["inverse"] );
				}
			}
			node = node->GetNextByName();
		}
	}

	root = parser.GetFirstChildByName("Mouse");
	if ( root ) {
		const og::XDeclNode *node = root->GetFirstChildByName("Axis");
		og::String bindName;
		while( node ) {
			const og::Dict &dict = node->GetDict();
			if ( dict.Get("bind", "", bindName ) ) {
				bind = GetBindForName( bindName.c_str() );
				if ( bind != NULL )
					BindMouseAxis( dict["id"], bind, dict["inverse"] );
			}
			node = node->GetNextByName();
		}

		node = root->GetFirstChildByName("Button");
		while( node ) {
			const og::Dict &dict = node->GetDict();
			if ( dict.Get("bind", "", bindName ) ) {
				bind = GetBindForName( bindName.c_str() );
				if ( bind != NULL )
					BindMouseButton( dict["id"], bind, dict["inverse"] );
			}
			node = node->GetNextByName();
		}
	}

	root = parser.GetFirstChildByName("Device");
	while ( root ) {
		og::String vendor;
		root->GetDict().Get( "vendor", "", vendor );

		int max = fableDevices.Num();
		int deviceId;
		for( deviceId=0; deviceId<max; deviceId++ ) {
			if ( vendor.Icmp( fableDevices[deviceId].device->GetName() ) == 0 )
				break;
		}
		if ( deviceId == max ) {
			root = root->GetNextByName();
			continue;
		}

		const og::XDeclNode *node = root->GetFirstChildByName("Axis");
		og::String bindName;
		while( node ) {
			const og::Dict &dict = node->GetDict();
			if ( dict.Get("bind", "", bindName ) ) {
				bind = GetBindForName( bindName.c_str() );
				if ( bind != NULL )
					BindDeviceAxis( deviceId, dict["id"], bind, dict["inverse"] );
			}
			node = node->GetNextByName();
		}

		node = root->GetFirstChildByName("Button");
		while( node ) {
			const og::Dict &dict = node->GetDict();
			if ( dict.Get("bind", "", bindName ) ) {
				bind = GetBindForName( bindName.c_str() );
				if ( bind != NULL )
					BindDeviceButton( deviceId, dict["id"], bind, dict["inverse"] );
			}
			node = node->GetNextByName();
		}

		const og::XDeclNode *pov = root->GetFirstChildByName("POV");
		while( pov ) {
			int povid = pov->GetDict()["id"];
			bool axes = pov->GetDict()["axes"];
			if ( axes ) {
				node = pov->GetFirstChildByName("Axis");
				int numAxes = 0;
				bind = NULL;
				ogBind *bind2 = NULL;
				bool inversed[2];
				while( node ) {
					const og::Dict &dict = node->GetDict();
					if ( dict.Get("bind", "", bindName ) ) {
						if ( !bind ) {
							bind = GetBindForName( bindName.c_str() );
							inversed[0] = dict["inverse"];
						} else {
							bind2 = GetBindForName( bindName.c_str() );
							inversed[1] = dict["inverse"];
							break;
						}
					}
					node = node->GetNextByName();
				}
				if ( bind && bind2 )
					BindDevicePOV( deviceId, povid, bind, bind2, inversed[0], inversed[1] );
			} else {
				// fixme
			}
			pov = pov->GetNextByName();
		}
		root = root->GetNextByName();
	}
}

/*
================
ogInputListener::UnbindAll
================
*/
void ogInputListener::UnbindAll( ogBind *bind ) {
	KeyListenerMap::iterator it = keyActions.begin();
	while( it != keyActions.end() ) {
		if ( bind == NULL || it->second->bind == bind ) {
			it->second->Reset();
			delete it->second;
			it = keyActions.erase( it );
		}
		else
			it++;
	}

	for( int i=0; i<og::Mouse::MaxButtons; i++ ) {
		if ( mouseButtonActions[i] != NULL ) {
			if ( bind == NULL || mouseButtonActions[i]->bind == bind )
				SAFE_RELEASE_CONTROL( mouseButtonActions[i] )
		}
	}

	for( int i=0; i<2; i++ ) {
		if ( mouseWheelActions[i] != NULL ) {
			if ( bind == NULL || mouseWheelActions[i]->bind == bind )
				SAFE_RELEASE_CONTROL( mouseWheelActions[i] )
		}
		if ( mouseAxisActions[i] != NULL ) {
			if ( bind == NULL || mouseAxisActions[i]->bind == bind )
				SAFE_RELEASE_CONTROL( mouseAxisActions[i] )
		}
	}
}

/*
================
ogInputListener::OnKeyEvent
================
*/
void ogInputListener::OnKeyEvent( int key, bool down ) {
	if ( waitForInput != 0 ) {
		if ( !(waitForInput & WAIT_HAS_RESULT) ) {
			waitForInput |= WAIT_HAS_RESULT;
			// fixme: create searchResult
		}
		return;
	}
	KeyListenerMap::iterator it = keyActions.find( key );
	if ( it != keyActions.end() )
		it->second->SetValue( down );
}

/*
================
ogInputListener::OnMouseButton
================
*/
void ogInputListener::OnMouseButton( int button, bool down ) {
	if ( waitForInput != 0 ) {
		if ( !(waitForInput & WAIT_HAS_RESULT) ) {
			waitForInput |= WAIT_HAS_RESULT;
			// fixme: create searchResult
		}
		return;
	}
	if ( mouseButtonActions[button] != NULL )
		mouseButtonActions[button]->SetValue( down );
}

/*
================
ogInputListener::OnMouseWheel
================
*/
void ogInputListener::OnMouseWheel( int delta ) {
	if ( waitForInput != 0 ) {
		if ( !(waitForInput & WAIT_HAS_RESULT) ) {
			waitForInput |= WAIT_HAS_RESULT;
			// fixme: create searchResult
		}
		return;
	}
	int index = delta > 0;
	if ( mouseWheelActions[index] != NULL )
		mouseWheelActions[index]->Trigger();
}

/*
================
ogInputListener::OnMouseMove
================
*/
void ogInputListener::OnMouseMove( int x, int y ) {
	if ( waitForInput != 0 ) {
		if ( !(waitForInput & WAIT_NO_AXIS|WAIT_HAS_RESULT) ) {
			waitForInput |= WAIT_HAS_RESULT;
			// fixme: create searchResult
		}
	}
	mouseX += x;
	mouseY += y;
}

/*
================
ogInputListener::BindKey
================
*/
void ogInputListener::BindKey( int key, ogBind *bind, bool inversed ) {
	UnbindKey( key );
	keyActions[key] = new ogKeyListener( bind, inversed );
}

/*
================
ogInputListener::UnbindKey
================
*/
void ogInputListener::UnbindKey( int key ) {
	KeyListenerMap::iterator it = keyActions.find( key );
	if ( it != keyActions.end() ) {
		it->second->Reset();
		delete it->second;
		keyActions.erase( it );
	}
}

/*
================
ogInputListener::BindMouseAxis
================
*/
void ogInputListener::BindMouseAxis( int axis, ogBind *bind, bool inversed ) {
	UnbindMouseAxis( axis );
	mouseAxisActions[axis] = new ogAxisListener( bind, inversed );
}

/*
================
ogInputListener::UnbindMouseAxis
================
*/
void ogInputListener::UnbindMouseAxis( int axis ) {
	SAFE_RELEASE_CONTROL( mouseAxisActions[axis] )
}

/*
================
ogInputListener::BindMouseButton
================
*/
void ogInputListener::BindMouseButton( int button, ogBind *bind, bool inversed ) {
	UnbindMouseButton( button );
	if ( button == -1 || button == -2 )
		mouseWheelActions[button+2] = new ogWheelListener( bind );
	else if ( button >= 0 )
		mouseButtonActions[button] = new ogKeyListener( bind, inversed );
}

/*
================
ogInputListener::UnbindMouseButton
================
*/
void ogInputListener::UnbindMouseButton( int button ) {
	if ( button == -1 || button == -2 )
		SAFE_RELEASE_CONTROL( mouseWheelActions[button+2] )
	else if ( button >= 0 )
		SAFE_RELEASE_CONTROL( mouseButtonActions[button] )
}

/*
================
ogInputListener::BindDeviceAxis
================
*/
void ogInputListener::BindDeviceAxis( int deviceId, int axis, ogBind *bind, bool inversed ) {
	UnbindDeviceAxis( deviceId, axis );
	if ( deviceId < fableDevices.Num() && axis < fableDevices[deviceId].device->GetNumAxes() )
		fableDevices[deviceId].axes[axis] = new ogAxisListener( bind, inversed );
}

/*
================
ogInputListener::UnbindDeviceAxis
================
*/
void ogInputListener::UnbindDeviceAxis( int deviceId, int axis ) {
	if ( deviceId < fableDevices.Num() && axis < fableDevices[deviceId].device->GetNumAxes() )
		SAFE_RELEASE_CONTROL( fableDevices[deviceId].axes[axis] )
}

/*
================
ogInputListener::BindDeviceButton
================
*/
void ogInputListener::BindDeviceButton( int deviceId, int button, ogBind *bind, bool inversed ) {
	UnbindDeviceButton( deviceId, button );
	if ( deviceId < fableDevices.Num() && button < fableDevices[deviceId].device->GetNumButtons() )
		fableDevices[deviceId].buttons[button] = new ogKeyListener( bind, inversed );
}

/*
================
ogInputListener::UnbindDeviceButton
================
*/
void ogInputListener::UnbindDeviceButton( int deviceId, int button ) {
	if ( deviceId < fableDevices.Num() && button < fableDevices[deviceId].device->GetNumButtons() )
		SAFE_RELEASE_CONTROL( fableDevices[deviceId].buttons[button] )
}

/*
================
ogInputListener::BindDevicePOV
================
*/
void ogInputListener::BindDevicePOV( int deviceId, int pov, ogBind *bindX, ogBind *bindY, bool inversedX, bool inversedY ) {
	UnbindDevicePOV( deviceId, pov );
	if ( deviceId < fableDevices.Num() && pov < fableDevices[deviceId].device->GetNumPOVs() )
		fableDevices[deviceId].povs[pov] = new ogPOVListener( bindX, inversedX, bindY, inversedY );
}

/*
================
ogInputListener::UnbindDevicePOV
================
*/
void ogInputListener::UnbindDevicePOV( int deviceId, int pov ) {
	if ( deviceId < fableDevices.Num() && pov < fableDevices[deviceId].device->GetNumPOVs() )
		SAFE_RELEASE_CONTROL( fableDevices[deviceId].povs[pov] )
}

/*
================
ogInputListener::InitInput
================
*/
void ogInputListener::InitInput( void ) {
	og::Fable::Init();
	int max = og::Fable::GetNumDevices();
	int num;
	for( int i=0; i<max; i++ ) {
		ogDeviceListener &jl = fableDevices.Alloc();
		jl.device = og::Fable::GetDevice(i);
		jl.listener = this;
		num = jl.device->GetNumButtons();
		for( int j=0; j<num; j++ )
			jl.buttons.Append(NULL);
		num = jl.device->GetNumAxes();
		for( int j=0; j<num; j++ )
			jl.axes.Append(NULL);
		num = jl.device->GetNumPOVs();
		for( int j=0; j<num; j++ )
			jl.povs.Append(NULL);
	}
}

/*
================
ogInputListener::ShutdownInput
================
*/
void ogInputListener::ShutdownInput( void ) {
	for( int i=0; i<og::Mouse::MaxButtons; i++ ) {
		if ( mouseButtonActions[i] != NULL ) {
			delete mouseButtonActions[i];
			mouseButtonActions[i] = NULL;
		}
	}

	for( int i=0; i<2; i++ ) {
		if ( mouseWheelActions[i] != NULL ) {
			delete mouseWheelActions[i];
			mouseWheelActions[i] = NULL;
		}
		if ( mouseAxisActions[i] != NULL ) {
			delete mouseAxisActions[i];
			mouseAxisActions[i] = NULL;
		}
	}
	int max = fableDevices.Num();
	int num;
	for( int i=0; i<max; i++ ) {
		ogDeviceListener &jl = fableDevices[i];
		num = jl.device->GetNumButtons();
		for( int j=0; j<num; j++ ) {
			if ( jl.buttons[j] != NULL )
				delete jl.buttons[j];
		}
		num = jl.device->GetNumAxes();
		for( int j=0; j<num; j++ ) {
			if ( jl.axes[j] != NULL )
				delete jl.axes[j];
		}
		num = jl.device->GetNumPOVs();
		for( int j=0; j<num; j++ ) {
			if ( jl.povs[j] != NULL )
				delete jl.povs[j];
		}
	}
	fableDevices.Clear();
	og::Fable::Shutdown();
	UnbindAll();
}

/*
================
ogInputListener::OnFocusChange
================
*/
void ogInputListener::OnFocusChange( bool hasFocus ) {
	if ( isActive == hasFocus )
		return;
	isActive = hasFocus;
	if ( isActive ) {
		og::Gloot::DisableSystemKeys( false );
		ResetInput();
	} else {
		og::Gloot::DisableSystemKeys( true );
	}
}

/*
================
ogInputListener::ResetInput
================
*/
void ogInputListener::ResetInput( void ) {
	KeyListenerMap::iterator it;
	for( it = keyActions.begin(); it != keyActions.end(); it++ )
		it->second->Reset();

	for( int i=0; i<og::Mouse::MaxButtons; i++ ) {
		if ( mouseButtonActions[i] != NULL )
			mouseButtonActions[i]->Reset();
	}

	for( int i=0; i<2; i++ ) {
		if ( mouseWheelActions[i] != NULL )
			mouseWheelActions[i]->Reset();
		if ( mouseAxisActions[i] != NULL )
			mouseAxisActions[i]->Reset();
	}
}

/*
================
CalculateMouseAxis
================
*/
OG_INLINE float CalculateMouseAxis( int pixels ) {
	const float MOUSE_AXIS_DISTANCE = 10.0f;
	if ( pixels == 0 )
		return 0.0f;
	if ( pixels > MOUSE_AXIS_DISTANCE )
		return 1.0f;
	if ( pixels < -MOUSE_AXIS_DISTANCE )
		return -1.0f;
	return pixels / MOUSE_AXIS_DISTANCE;
}

/*
================
ogInputListener::OnFrameStart
================
*/
void ogInputListener::OnFrameStart( void ) {
	int max = fableDevices.Num();
	for( int i=0; i<max; i++ )
		fableDevices[i].device->Refresh( &fableDevices[i] );

	if ( waitForInput != 0 ) {
		if ( (waitForInput & WAIT_HAS_RESULT) != 0 )
			return;
		if ( !(waitForInput & WAIT_NO_AXIS) ) {
			if ( abs(mouseX) > MOUSE_AXIS_DETECTION_TRIGGER ) {
				waitForInput |= WAIT_HAS_RESULT;
				// fixme: create searchResult
				mouseX = 0;
			}
			else if ( abs(mouseY) > MOUSE_AXIS_DETECTION_TRIGGER ) {
				waitForInput |= WAIT_HAS_RESULT;
				// fixme: create searchResult
				mouseY = 0;
			}
		}
	} else {
		if ( mouseX != 0 ) {
			if ( mouseAxisActions[0] != NULL )
				mouseAxisActions[0]->SetValue( CalculateMouseAxis( mouseX ) );
			mouseX = 0;
		}
		if ( mouseY != 0 ) {
			if ( mouseAxisActions[1] != NULL )
				mouseAxisActions[1]->SetValue( CalculateMouseAxis( mouseY ) );
			mouseY = 0;
		}
	}
}

/*
================
ogInputListener::OnFrameEnd
================
*/
void ogInputListener::OnFrameEnd( void ) {
	KeyListenerMap::iterator it;
	for( it = keyActions.begin(); it != keyActions.end(); it++ )
		it->second->OnFrameEnd();

	for( int i=0; i<og::Mouse::MaxButtons; i++ ) {
		if ( mouseButtonActions[i] != NULL )
			mouseButtonActions[i]->OnFrameEnd();
	}

	for( int i=0; i<2; i++ ) {
		if ( mouseWheelActions[i] != NULL )
			mouseWheelActions[i]->OnFrameEnd();
		if ( mouseAxisActions[i] != NULL ) {
			mouseAxisActions[i]->OnFrameEnd();
			mouseAxisActions[i]->SetValue( 0.0f );
		}
	}
	int max = fableDevices.Num();
	int num;
	for( int i=0; i<max; i++ ) {
		ogDeviceListener &jl = fableDevices[i];
		num = jl.device->GetNumButtons();
		for( int j=0; j<num; j++ ) {
			if ( jl.buttons[j] != NULL )
				jl.buttons[j]->OnFrameEnd();
		}
		num = jl.device->GetNumAxes();
		for( int j=0; j<num; j++ ) {
			if ( jl.axes[j] != NULL )
				jl.axes[j]->OnFrameEnd();
		}
		num = jl.device->GetNumPOVs();
		for( int j=0; j<num; j++ ) {
			if ( jl.povs[j] != NULL )
				jl.povs[j]->OnFrameEnd();
		}
	}
}