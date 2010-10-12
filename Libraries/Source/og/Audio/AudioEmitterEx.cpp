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
	if ( !audioSystemObject.audioThread )
		return;
	EmitterEvent *evt = new EmitterEvent( this );
	evt->type = EmitterEvent::PLAY;
	evt->channel = channel;
	evt->sound = sound;
	evt->allowLoop = allowLoop;
	audioSystemObject.audioThread->AddEvent( evt );
}

/*
================
AudioEmitterEx::Stop
================
*/
void AudioEmitterEx::Stop( int channel ) {
	if ( !audioSystemObject.audioThread )
		return;
	EmitterEvent *evt = new EmitterEvent( this );
	evt->type = EmitterEvent::STOP;
	evt->channel = channel;
	audioSystemObject.audioThread->AddEvent( evt );
}

/*
================
AudioEmitterEx::StopAll
================
*/
void AudioEmitterEx::StopAll( void ) {
	if ( !audioSystemObject.audioThread )
		return;
	EmitterEvent *evt = new EmitterEvent( this );
	evt->type = EmitterEvent::STOPALL;
	audioSystemObject.audioThread->AddEvent( evt );
}

/*
================
AudioEmitterEx::Pause
================
*/
void AudioEmitterEx::Pause( int channel ) {
	if ( !audioSystemObject.audioThread )
		return;
	EmitterEvent *evt = new EmitterEvent( this );
	evt->type = EmitterEvent::PAUSE;
	evt->channel = channel;
	audioSystemObject.audioThread->AddEvent( evt );
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
	if ( !audioSystemObject.audioThread )
		return;
	EmitterEvent *evt = new EmitterEvent( this );
	evt->type = EmitterEvent::UPDATE;
	details.relative = value;
	audioSystemObject.audioThread->AddEvent( evt );
}

/*
================
AudioEmitterEx::SetPosition
================
*/
void AudioEmitterEx::SetPosition( const Vec3 &pos ) {
	if ( !audioSystemObject.audioThread )
		return;
	EmitterEvent *evt = new EmitterEvent( this );
	evt->type = EmitterEvent::UPDATE;
	details.origin = pos;
	audioSystemObject.audioThread->AddEvent( evt );
}

/*
================
AudioEmitterEx::SetVelocity
================
*/
void AudioEmitterEx::SetVelocity( const Vec3 &vel ) {
	if ( !audioSystemObject.audioThread )
		return;
	EmitterEvent *evt = new EmitterEvent( this );
	evt->type = EmitterEvent::UPDATE;
	details.velocity = vel;
	audioSystemObject.audioThread->AddEvent( evt );
}

/*
================
AudioEmitterEx::SetDirectional
================
*/
void AudioEmitterEx::SetDirectional( const Vec3 &dir, float innerAngle, float outerAngle, float outerVolume ) {
	if ( !audioSystemObject.audioThread )
		return;
	EmitterEvent *evt = new EmitterEvent( this );
	evt->type = EmitterEvent::UPDATE;
	details.direction = dir;
	details.innerAngle = innerAngle;
	details.outerAngle = outerAngle;
	details.outerVolume = outerVolume;
	audioSystemObject.audioThread->AddEvent( evt );
}


/*
================
AudioEmitterEx::OnEvent
================
*/
void AudioEmitterEx::OnEvent( EmitterEvent *evt ) {
	switch( evt->type ) {
		case EmitterEvent::PLAY:
			if ( IsValidChannel( evt->channel ) ) {
				AudioSource *source = NULL;
				if ( sndChannels[evt->channel] != NULL )
					source = sndChannels[evt->channel];
				else
					source = audioSystemObject.audioThread->FindFreeAudioSource();

				if ( source ) {
					if ( source->Play( this, evt->channel, evt->sound, evt->allowLoop ) ) {
						sndChannels[evt->channel] = source;
						sndChannels[evt->channel]->OnUpdate( &details );
					}
					return;
				}
				User::Warning("No more free sound Sources!");
			}
			break;

		case EmitterEvent::PAUSE:
			if ( IsValidChannel( evt->channel ) && sndChannels[evt->channel] != NULL )
					sndChannels[evt->channel]->Pause();
			break;

		case EmitterEvent::STOP:
			if ( IsValidChannel( evt->channel ) && sndChannels[evt->channel] != NULL )
				sndChannels[evt->channel]->Stop();
			break;

		case EmitterEvent::STOPALL:
			for( int i=0; i<numChannels; i++ ) {
				if ( sndChannels[i] != NULL )
					sndChannels[i]->Stop();
			}
			break;

		case EmitterEvent::UPDATE:
			if ( IsValidChannel( 0 ) ) {
				for( int i=0; i<numChannels; i++ ) {
					if ( sndChannels[i] != NULL )
						sndChannels[i]->OnUpdate( &details );
				}
			}
			break;
	}
}

}
