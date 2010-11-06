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

#include "AudioSystemEx.h"

namespace og {

/*
==============================================================================

  EE_Base

==============================================================================
*/
class EE_Base : public QueuedEvent {
public:
	EE_Base( AudioEmitterEx *emt ) : emitter(emt) {}

protected:
	AudioEmitterEx *emitter;
};

/*
==============================================================================

  EE_Play

==============================================================================
*/
class EE_Play : public EE_Base {
public:
	EE_Play( AudioEmitterEx *emt, int ch, const Sound *snd, bool loopAllowed )
		: EE_Base(emt), channel(ch), sound(snd), allowLoop(loopAllowed) {}

	void	Execute( void )  {
		if ( emitter->IsValidChannel( channel ) ) {
			AudioSource *source = emitter->sndChannels[channel];
			if ( source == NULL )
				source = audioSystemObject.audioThread->FindFreeAudioSource();

			if ( source ) {
				if ( source->Play( emitter, channel, sound, allowLoop ) ) {
					emitter->sndChannels[channel] = source;
					emitter->sndChannels[channel]->OnUpdate( &emitter->details );
				}
				return;
			}
			User::Warning("No more free sound Sources!");
		}
	}

private:
	int channel;
	const Sound *sound;
	bool allowLoop;
};

/*
==============================================================================

  EE_Pause

==============================================================================
*/
class EE_Pause : public EE_Base {
public:
	EE_Pause( AudioEmitterEx *emt, int ch ) : EE_Base(emt), channel(ch) {}

	void	Execute( void )  {
		if ( emitter->IsValidChannel( channel ) && emitter->sndChannels[channel] != NULL )
			emitter->sndChannels[channel]->Pause();
	}

private:
	int channel;
};

/*
==============================================================================

  EE_Stop

==============================================================================
*/
class EE_Stop : public EE_Base {
public:
	EE_Stop( AudioEmitterEx *emt, int ch ) : EE_Base(emt), channel(ch) {}

	void	Execute( void )  {
		if ( channel == -1 ) {
			for( int i=0; i<emitter->numChannels; i++ ) {
				if ( emitter->sndChannels[i] != NULL )
					emitter->sndChannels[i]->Stop();
			}
		}
		else if ( emitter->IsValidChannel( channel ) && emitter->sndChannels[channel] != NULL )
			emitter->sndChannels[channel]->Stop();
	}

private:
	int channel;
};

/*
==============================================================================

  EE_Update

==============================================================================
*/
class EE_Update : public EE_Base {
public:
	EE_Update( AudioEmitterEx *emt ) : EE_Base(emt) {}

	void	Update( void )  {
		if ( emitter->IsValidChannel( 0 ) ) {
			for( int i=0; i<emitter->numChannels; i++ ) {
				if ( emitter->sndChannels[i] != NULL )
					emitter->sndChannels[i]->OnUpdate( &emitter->details );
			}
		}
	}
};

/*
==============================================================================

  EE_SetRelative

==============================================================================
*/
class EE_SetRelative : public EE_Update {
public:
	EE_SetRelative( AudioEmitterEx *emt, bool val ) : EE_Update(emt), value(val) {}

	void	Execute( void )  { emitter->details.relative = value; Update(); }

private:
	bool	value;
};

/*
==============================================================================

  EE_SetPosition

==============================================================================
*/
class EE_SetPosition : public EE_Update {
public:
	EE_SetPosition( AudioEmitterEx *emt, const Vec3 &val ) : EE_Update(emt), value(val) {}

	void	Execute( void )  { emitter->details.origin = value; Update(); }

private:
	Vec3	value;
};

/*
==============================================================================

  EE_SetVelocity

==============================================================================
*/
class EE_SetVelocity : public EE_Update {
public:
	EE_SetVelocity( AudioEmitterEx *emt, const Vec3 &val ) : EE_Update(emt), value(val) {}

	void	Execute( void )  { emitter->details.velocity = value; Update(); }

private:
	Vec3	value;
};

/*
==============================================================================

  EE_SetDirectional

==============================================================================
*/
class EE_SetDirectional : public EE_Update {
public:
	EE_SetDirectional( AudioEmitterEx *emt, const Vec3 &dir, float innerAng, float outerAng, float outerVol )
		: EE_Update(emt), direction(dir), innerAngle(innerAng), outerAngle(outerAng), outerVolume(outerVol) {}

	void	Execute( void )  {
		emitter->details.direction = direction;
		emitter->details.innerAngle = innerAngle;
		emitter->details.outerAngle = outerAngle;
		emitter->details.outerVolume = outerVolume;
		Update();
	}

private:
	Vec3	direction;
	float	innerAngle;
	float	outerAngle;
	float	outerVolume;
};

/*
==============================================================================

  AudioEmitterEx

==============================================================================
*/

/*
================
AudioEmitterEx::AudioEmitterEx
================
*/
AudioEmitterEx::AudioEmitterEx() {
	sndChannels = NULL;
	numChannels = 0;
	details.relative	= false;
	details.innerAngle	= 360.0f;
	details.outerAngle	= 360.0f;
	details.outerVolume	= 0.0f;
}

/*
================
AudioEmitterEx::~AudioEmitterEx
================
*/
AudioEmitterEx::~AudioEmitterEx() {
	Clear();
}

/*
================
AudioEmitterEx::Init
================
*/
void AudioEmitterEx::Init( int channels ) {
	Clear();
	numChannels = channels;
	sndChannels = new AudioSource *[numChannels];
	if ( !sndChannels ) {
		User::Error( ERR_OUT_OF_MEMORY, "new AudioSource *[numChannels]", Format() << (sizeof(AudioSource *) * numChannels) );
		return;
	}
	for( int i=0; i<numChannels; i++ )
		sndChannels[i] = NULL;
}

/*
================
AudioEmitterEx::Clear
================
*/
void AudioEmitterEx::Clear( void ) {
	if ( sndChannels ) {
		StopAll();
		delete[] sndChannels;
		sndChannels = NULL;
		numChannels = 0;
	}
}

/*
================
AudioEmitterEx::OnSourceStop
================
*/
void AudioEmitterEx::OnSourceStop( int channel ) {
	if ( IsValidChannel( channel ) )
		sndChannels[channel] = NULL;
}

/*
================
AudioEmitterEx::IsValidChannel
================
*/
bool AudioEmitterEx::IsValidChannel( int channel ) const {
	if ( channel < 0 || channel >= numChannels ) {
		User::Warning("AudioEmitterEx: channel is out of range."); //! @todo	this should probably be an error
		return false;
	}
	return true;
}

/*
================
AudioEmitterEx::Play
================
*/
void AudioEmitterEx::Play( int channel, const Sound *sound, bool allowLoop ) {
	if ( audioSystemObject.audioThread )
		audioSystemObject.audioThread->AddEvent( new EE_Play( this, channel, sound, allowLoop ) );
}

/*
================
AudioEmitterEx::Stop
================
*/
void AudioEmitterEx::Stop( int channel ) {
	if ( audioSystemObject.audioThread )
		audioSystemObject.audioThread->AddEvent( new EE_Stop( this, channel ) );
}

/*
================
AudioEmitterEx::StopAll
================
*/
void AudioEmitterEx::StopAll( void ) {
	if ( audioSystemObject.audioThread )
		audioSystemObject.audioThread->AddEvent( new EE_Stop( this, -1 ) );
}

/*
================
AudioEmitterEx::Pause
================
*/
void AudioEmitterEx::Pause( int channel ) {
	if ( audioSystemObject.audioThread )
		audioSystemObject.audioThread->AddEvent( new EE_Pause( this, channel ) );
}

/*
================
AudioEmitterEx::IsOccupied
================
*/
bool AudioEmitterEx::IsOccupied( int channel ) {
	return IsValidChannel( channel ) && sndChannels[channel] != NULL;
}

/*
================
AudioEmitterEx::SetRelative
================
*/
void AudioEmitterEx::SetRelative( bool value ) {
	if ( audioSystemObject.audioThread )
		audioSystemObject.audioThread->AddEvent( new EE_SetRelative( this, value ) );
}

/*
================
AudioEmitterEx::SetPosition
================
*/
void AudioEmitterEx::SetPosition( const Vec3 &pos ) {
	if ( audioSystemObject.audioThread )
		audioSystemObject.audioThread->AddEvent( new EE_SetPosition( this, pos ) );
}

/*
================
AudioEmitterEx::SetVelocity
================
*/
void AudioEmitterEx::SetVelocity( const Vec3 &vel ) {
	if ( audioSystemObject.audioThread )
		audioSystemObject.audioThread->AddEvent( new EE_SetVelocity( this, vel ) );
}

/*
================
AudioEmitterEx::SetDirectional
================
*/
void AudioEmitterEx::SetDirectional( const Vec3 &dir, float innerAngle, float outerAngle, float outerVolume ) {
	if ( audioSystemObject.audioThread )
		audioSystemObject.audioThread->AddEvent( new EE_SetDirectional( this, dir, innerAngle, outerAngle, outerVolume ) );
}

}
