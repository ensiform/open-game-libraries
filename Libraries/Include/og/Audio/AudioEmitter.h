// ==============================================================================
//! @file
//! @brief	Audio Emitter
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

#ifndef __OG_AUDIO_EMITTER_H__
#define __OG_AUDIO_EMITTER_H__

//! Open Game Libraries
namespace og {
//! @defgroup Audio Audio (Library)
//! @{
	class Sound;

	// ==============================================================================
	//! This positions and plays sounds in the world
	//!
	//! @todo	One channel that can play (non-looping) sounds without stopping anything.
	//!			This channel would be created on play, and freed when done (or StopAll was called).
	//!
	//! This is like a speaker in your world.
	// ==============================================================================
	class AudioEmitter {
	public:
		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~AudioEmitter() {}

		// ==============================================================================
		//! Initializes audio emitter with the number of channels
		//!
		//! @param	channels	Number of channels you need on this object
		// ==============================================================================
		virtual void	Init( int channels ) = 0;

		// ==============================================================================
		//! Number of channels allocated
		//!
		//! @return	Number of channels allocated
		// ==============================================================================
		virtual int		NumChannels( void ) const = 0;

		// ==============================================================================
		//! Play a sound on the specified channel
		//!
		//! @param	channel		Which channel to play the sound on
		//! @param	sound		Sound to play
		//! @param	allowLoop	Allow looping playback
		//!
		//! @note	Don't delete the sound object after passing it,
		//!			since the pointer is passed internally to a different thread
		// ==============================================================================
		virtual void	Play( int channel, const Sound *sound, bool allowLoop=true ) = 0;

		// ==============================================================================
		//! Pause a sound on the specified channel
		//!
		//! @param	channel		Which channel to pause
		// ==============================================================================
		virtual void	Pause( int channel ) = 0;

		// ==============================================================================
		//! Stop playback on the specified channel
		//!
		//! @param	channel		Which channel to stop playback
		// ==============================================================================
		virtual void	Stop( int channel ) = 0;

		// ==============================================================================
		//! Stop playback on all channels
		// ==============================================================================
		virtual void	StopAll( void ) = 0;

		// ==============================================================================
		//! Checks if a channel is currently in use
		//!
		//! @param	channel		Which channel to check
		//!
		//! @return	true if in use, otherwise false
		// ==============================================================================
		virtual bool	IsOccupied( int channel ) = 0;

		// ==============================================================================
		//! Use relative coordinates instead of world coordinates for this emitter
		//!
		//! @param	relative	Relative or not
		// ==============================================================================
		virtual void	SetRelative( bool relative ) = 0;

		// ==============================================================================
		//! Set the position of this emitter
		//!
		//! @param	pos		Position in world coordinates, or if relative mode is set, local coordinates.
		// ==============================================================================
		virtual void	SetPosition( const Vec3 &pos ) = 0;

		// ==============================================================================
		//! Set the velocity (speed & direction) of this emitter
		//!
		//! @param	vel		Velocity, a directional vector with it's length set to the speed it travels at.
		// ==============================================================================
		virtual void	SetVelocity( const Vec3 &vel ) = 0;

		// ==============================================================================
		//! Set directional mode (a speaker for example is louder when you are in front of it than when you're behind it)
		//!
		//! @param	dir				A vector pointing in the direction the speaker is looking at.
		//! @param	innerAngle		Creates a cone in which the full volume is used.
		//! @param	outerAngle		Creates a cone in which the volume fades the more you get away from it.
		//! @param	outerVolume		Volume level outside the cones.
		// ==============================================================================
		virtual void	SetDirectional( const Vec3 &dir, float innerAngle, float outerAngle, float outerVolume ) = 0;

		// ==============================================================================
		//! Use an AudioEffect on this emitter
		//!
		//! @param	effect	The effect to use, NULL to remove the active effect from this emitter
		// ==============================================================================
		virtual void	SetEffect( AudioEffect *effect ) = 0;
	};

//! @}
}

#endif
