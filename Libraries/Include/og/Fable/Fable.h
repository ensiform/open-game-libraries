// ==============================================================================
//! @file
//! @brief	Public Game Controller Interface
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2007-2010 Lusito Software
//!
//! @todo	Force Feedback
//! @todo	Linux & Mac
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

#ifndef __OG_FABLE_H__
#define __OG_FABLE_H__

#include <og/Shared/Shared.h>

//! Open Game Libraries
namespace og {
	//! @defgroup Fable Fable (Library)
	//! You need to include <og/Fable/Fable.h>
	//! @{

	//! Static Fable Interface
	namespace Fable {
	//! @addtogroup Fable
	//! @{

#if 0
		const int MAX_EFFECT_AXES = 4; // fixme

		class Effect {
		public:
			enum Type {
				// Constant Force
				CONSTANT_FORCE,
				
				// Ramp Force
				RAMP_FORCE,

				// Periodic
				SQUARE,
				SINE,
				TRIANGLE,
				SAW_TOOTH_UP,
				SAW_TOOTH_DOWN,

				// Conditional
				SPRING,
				DAMPER,
				INERTIA,
				FRICTION,

				// Custom Force
				CUSTOM_FORCE
			};
			Effect( Type t ) {
				OG_ASSERT( numAxes <= MAX_EFFECT_AXES );
				type = t;
			}

			uInt	numAxes;
			uInt	axes[MAX_EFFECT_AXES];
			float	direction[MAX_EFFECT_AXES]; // 0.0f -> 360.0f
			uInt	lifeTime;
			float	gain;
			uInt	triggerButton;
			uInt	triggerCoolDown;
			uInt	startDelay;

			// Envelope
			float	fadeInLevel;
			uInt	fadeInTime;
			float	fadeOutLevel;
			uInt	fadeOutTime;

		private:
			Type	type;
		};

		class EffectConstant : public Effect {
		public:
			EffectConstant() : Effect( CONSTANT_FORCE ) {}

			float	magnitude;	// -1.0f -> 1.0f
		};

		class EffectRamp : public Effect {
		public:
			EffectRamp() : Effect( RAMP_FORCE ) {}

			float	startLevel;	// -1.0f -> 1.0f
			float	endLevel;	// -1.0f -> 1.0f
		};

		class EffectPeriodic : public Effect {
		public:
			EffectPeriodic( Effect::Type t ) : Effect( t ) {
				OG_ASSERT( t == SQUARE || t == SINE || t == TRIANGLE || t == SAW_TOOTH_UP || t == SAW_TOOTH_DOWN );
			}

			float	magnitude;	// 0.0f -> 1.0f
			float	offset;		// -1.0f -> 1.0f
			uInt	phase;
			uInt	period;
		};

		class EffectConditional : public Effect {
		public:
			EffectConditional( Effect::Type t ) : Effect( t ) {
				OG_ASSERT( t == SPRING || t == DAMPER || t == INERTIA || t == FRICTION );
			}

			float	offset;					// -1.0f -> 1.0f
			float	positiveCoefficient;	// -1.0f -> 1.0f
			float	negativeCoefficient;	// -1.0f -> 1.0f
			float	positiveSaturation;		// 0.0f -> 1.0f
			float	negativeSaturation;		// 0.0f -> 1.0f
			float	deadBand;				// 0.0f -> 1.0f
		};
#endif

		// ==============================================================================
		//! Device listener
		//!
		//! Callback class for devices. When the device state changes this will be notified.
		// ==============================================================================
		class DeviceListener {
		protected:
			friend class DeviceDI;
			friend class DeviceXI;

			// ==============================================================================
			//! Virtual destructor
			// ==============================================================================
			virtual ~DeviceListener() {}

			// ==============================================================================
			//! A axis has been moved
			//!
			//! @param	axis	The axis of interest
			//! @param	value	The new value
			// ==============================================================================
			virtual void	OnAxis( uInt axis, float value ) = 0;

			// ==============================================================================
			//! A POV has been changed
			//!
			//! @param	pov		The pov of interest
			//! @param	value	The new value
			// ==============================================================================
			virtual void	OnPOV( uInt pov, int value ) = 0;

			// ==============================================================================
			//! A button has been pressed
			//!
			//! @param	button	The button of interest
			//! @param	down	true if it is pressed, false if released
			// ==============================================================================
			virtual void	OnButton( uInt button, bool down ) = 0;
		};

		// ==============================================================================
		//! Device object
		//!
		//! Retrieves device information
		// ==============================================================================
		class Device {
		public:
			// ==============================================================================
			//! Virtual destructor
			// ==============================================================================
			virtual ~Device() {}

			// ==============================================================================
			//! Get the name of the device
			//!
			//! @return	The name of the device ( UTF-8 )
			// ==============================================================================
			virtual const char *GetName( void ) = 0;

			// ==============================================================================
			//! Refresh the device axes and buttons
			//!
			//! @param	listener	The listener to update ( can be NULL )
			// ==============================================================================
			virtual void	Refresh( DeviceListener *listener ) = 0;

			// ==============================================================================
			//! Get the number of buttons on this device
			//!
			//! @return	The number of buttons
			// ==============================================================================
			virtual uInt	GetNumButtons( void ) const = 0;

			// ==============================================================================
			//! Get the number of axes on this device
			//!
			//! @return	The number of axes
			// ==============================================================================
			virtual uInt	GetNumAxes( void ) const = 0;

			// ==============================================================================
			//! Get the number of POVs on this device
			//!
			//! @return	The number of POVs
			// ==============================================================================
			virtual uInt	GetNumPOVs( void ) const = 0;

			// ==============================================================================
			//! Get the button with the specified index on this device
			//!
			//! @param	index	The index of the button
			//!
			//! @return	The button
			// ==============================================================================
			virtual bool	GetButton( uInt index ) const = 0;

			// ==============================================================================
			//! Get the axis with the specified index on this device
			//!
			//! @param	index	The index of the axis
			//!
			//! @return	The axis
			// ==============================================================================
			virtual float	GetAxis( uInt index ) const = 0;

			// ==============================================================================
			//! Get the pov with the specified index on this device
			//!
			//! @param	index	The index of the POV
			//!
			//! @return	The pov direction.. -1=centered, 0=north, 1=northeast, 2=east, ...
			// ==============================================================================
			virtual int		GetPOV( uInt index ) const = 0;

			// todo:
#if 0
			virtual void	SetGain( float level ) = 0;
			virtual uInt	GetNumForceAxes( void ) = 0;
//			virtual ForceEffectsList *GetForceEffectsList( void ) const = 0;
//			virtual void	FreeForceEffectsList( ForceEffectsList *list ) const = 0;

			virtual void	Play( const Effect *effect ) = 0;
			virtual void	Update( const Effect *effect ) = 0;
			virtual void	Stop( const Effect *effectt ) = 0;
#endif
		};

		// ==============================================================================
		//! Initialize Fable
		// ==============================================================================
		bool		Init( void );

		// ==============================================================================
		//! Shut down Fable
		// ==============================================================================
		void		Shutdown( void );

		// ==============================================================================
		//! Get the number of game controllers
		//!
		//! @return	The number of game controllers available
		// ==============================================================================
		int			GetNumDevices( void );

		// ==============================================================================
		//! Get the specified game controller object
		//!
		//! @param	index	The index of the game controller
		//!
		//! @return	NULL if out of range, otherwise the game controller object
		// ==============================================================================
		Device *	GetDevice( uInt index );
	//! @}
	}
//! @}
}

#endif
