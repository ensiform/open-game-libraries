// ==============================================================================
//! @file
//! @brief	Audio Interface Public Interface (shared with game module)
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

#ifndef __OG_AUDIOSYSTEM_H__
#define __OG_AUDIOSYSTEM_H__

//! Open Game Libraries
namespace og {
//! @defgroup Audio Audio (Library)
//! You need to include <og/Audio.h>
//! @see	UserCallbacks
//! @todo	See if thrown errors work without problems.
//!	@todo	Cache a list of sounds + clear all cached sounds
//! @todo	Get progress on caching ( percentage )
//! @todo	Add EAX effects ? Reverb ?
//! @{

	// ==============================================================================
	//! Defines how to play back sounds
	//!
	//! This is what a material definition is for graphics
	// ==============================================================================
	class Sound {
	public:
		float		minDistance;	//!< Minimum hearing distance ( 100% volume )
		float		maxDistance;	//!< Maximum hearing distance ( 0% volume )
		float		volume;			//!< Volume scale, 0.0f - 1.0f
		bool		loop;			//!< Flag to make this sound loop until manually stopped
		StringList	filenames;		//!< Sound filenames to randomly chose from
	};

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
	};

	// ==============================================================================
	//! The audio manager
	//!
	//! This the interface to play stuff.
	//! @note	og::ThreadSafetyClass = og::TSC_MULTIPLE ( except for Init/Shutdown )
	// ==============================================================================
	class AudioSystem {
	public:
	// Static Interface
	//! @addtogroup Audio
	//! @{
		// ==============================================================================
		//! Get a list of available devices
		//!
		//! @param	deviceList		Stringlist to store the results in
		//!
		//! @return	true if we could get the list, otherwise false
		// ==============================================================================
		static bool		GetDeviceList( StringList &deviceList );


		// ==============================================================================
		//! Initialize the Audio Library, loading a default soundfile.
		//!
		//! @param	fileSystem			The FileSystem object
		//! @param	defaultFilename		Default sound to play when a wanted sound is not found.
		//! @param	deviceName			Device name, NULL for default, or use one of the values from GetDeviceList.
		//!
		//! @return	true if OpenAL initialization succeeded and the default sound was loaded, otherwise false.
		// ==============================================================================
		static bool		Init( FileSystemCore *fileSystem, const char *defaultFilename, const char *deviceName );

		// ==============================================================================
		//! Shut down all audio playback and free all resources.
		//!
		//! Clears all audio buffers and emitters.
		// ==============================================================================
		static void		Shutdown( void );

		// ==============================================================================
		//! Set window focus, to mute audio when window is not focused
		//!
		//! @param	hasFocus	Set to true if your window has focus.
		// ==============================================================================
		static void		SetWindowFocus( bool hasFocus );
	//! @}

	// Object Interface
		// These can be used freely.

		// ==============================================================================
		//! Set global volume
		//!
		//! @param	value	Volume level ranging from 0.0 - 1.0
		// ==============================================================================
		virtual void				SetVolume( float value ) = 0;

		// ==============================================================================
		//! Set how many different audio files a Sound may contain (to play sounds at random).
		//!
		//! @param	num		Maximum number of variations
		// ==============================================================================
		virtual void				SetMaxVariations( int num ) = 0;

		// ==============================================================================
		//! Update the audio listener (your heads listening position and orientation)
		//!
		//! @param	origin		Position of your head.
		//! @param	forward		Where you are looking at.
		//! @param	up			Beats me how to describe this short. Should be obvious
		//! @param	velocity	Your moving velocity.
		// ==============================================================================
		virtual void				SetListener( const Vec3 &origin, const Vec3 &forward, const Vec3 &up, const Vec3 &velocity ) = 0;

		// ==============================================================================
		//! Set the doppler factor (mustn't be negative)
		//!
		//! @param	factor		The doppler factor (mustn't be negative)
		//!
		//! Possible values:
		//! @li		0 disables the doppler effect, may help performance, but sounds less realistic.
		//! @li		1.0 will not change the effect at all.
		//! @li		Anything between 0.0 and 1.0 will minimize the Doppler effect.
		//! @li		Anything greater than 1.0 will maximize the effect.
		// ==============================================================================
		virtual void				SetDopplerFactor( float factor ) = 0;

		// ==============================================================================
		//! Set the speed of sound in the current medium (air, water, ..)
		//!
		//! @param	speed	The speed of sound (must be above 0, default: 343.3f)
		//!
		//! Examples:
		//! @li		Air: 343.3 m/s or  1126 ft/s
		//! @li		Water: 1484 m/s or 4868 ft/s
		//!
		//! @note	Recalculate the value to your unit size before passing them.
		// ==============================================================================
		virtual void				SetSpeedOfSound( float speed ) = 0;

		// ==============================================================================
		//! Create an AudioEmitter
		//!
		//! @param	channels	Number of channels you need on this object
		//!
		//! @return	Pointer to a new AudioEmitter object
		// ==============================================================================
		virtual AudioEmitter *		CreateAudioEmitter( int channels=0 ) = 0;

		// ==============================================================================
		//! Free an AudioEmitter previously created by AudioSystem::CreateAudioEmitter
		//!
		//! @param	emitter		Pointer to the object to be freed
		// ==============================================================================
		virtual void				FreeAudioEmitter( AudioEmitter *emitter ) = 0;
	};

	// ==============================================================================
	//! AudioSystem interface pointer (can be passed to the game module)
	//!
	//! @note	This pointer is automatically available in the executable linking to this library,\n
	//!			but needs to be created (and assigned the value of the original) for other modules that need access to it.
	// ==============================================================================
	extern AudioSystem *AS;

//! @}
}

#endif
