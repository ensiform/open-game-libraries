// ==============================================================================
//! @file
//! @brief	Audio Source
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
#include "AudioStream.h"

#include <al/efx.h>

namespace og {

/*
==============================================================================

  AudioSource

==============================================================================
*/

/*
================
AudioSource::AudioSource
================
*/
AudioSource::AudioSource( AudioSource *previous, uInt sourceNum ) {
	emitter		= OG_NULL;
	emitterChannel = 0;
	safeLevel	= 0;
	alSourceNum	= sourceNum;
	streamData	= OG_NULL;
	next		= OG_NULL;
	prev		= previous;
	if ( previous )
		previous->next = this;

	alGenBuffers( AUDIOSRC_BUFFERS, alBuffers );
}

/*
================
AudioSource::~AudioSource
================
*/
AudioSource::~AudioSource() {
	delete next;
	if ( IsActive() )
		Stop();
	alDeleteSources( 1, &alSourceNum );
	alDeleteBuffers( AUDIOSRC_BUFFERS, alBuffers );
}

/*
================
AudioSource::Play
================
*/
bool AudioSource::Play( AudioEmitterEx *_emitter, int channel, const Sound *sound, bool allowLoop ) {
	OG_ASSERT( _emitter != OG_NULL && sound != OG_NULL );

	Stop();

	emitter = _emitter;
	emitterChannel = channel;

	int num = sound->filenames.Num();
	if ( num == 0 ) {
		emitter = OG_NULL;
		emitterChannel = 0;
		return false;
	}

	if ( audioSystemObject.maxVariations > 0 && num > audioSystemObject.maxVariations )
		num = audioSystemObject.maxVariations;

	const char *filename;
	if ( num == 1 )
		filename = sound->filenames[0].c_str();
	else {
		// Multiple sounds available, randomly choose one.
		Random rnd;
		filename = sound->filenames[ rnd.RandomInt(0, num-1) ].c_str();
	}

	alSourcef( alSourceNum, AL_REFERENCE_DISTANCE, sound->minDistance );
	alSourcef( alSourceNum, AL_MAX_DISTANCE, sound->maxDistance );
	alSourcef( alSourceNum, AL_GAIN, sound->volume );

	SetEffect( emitter->effect );

	bool loop = allowLoop ? sound->loop : false;
	audioSystemObject.audioThread->PlayStream( this, filename, loop );

	return CheckAlErrors();
}

/*
================
AudioSource::Pause
================
*/
void AudioSource::Pause( void ) {
	if ( IsActive() )
		alSourcePause( alSourceNum ); //! @todo	unpause ?
}

/*
================
AudioSource::Stop
================
*/
void AudioSource::Stop( void ) {
	if ( IsActive() ) {
		alSourceStop( alSourceNum );
		alSourcei( alSourceNum, AL_BUFFER, 0 );
		CheckAlErrors();

		delete streamData;
		streamData = OG_NULL;

		SetEffect(OG_NULL);
		emitter->OnSourceStop( emitterChannel );
		emitter = OG_NULL;
		emitterChannel = 0;
	}
}

/*
================
AudioSource::Frame
================
*/
void AudioSource::Frame( void ) {
	if ( IsActive() ) {
		// if not playing anymore
		ALenum state;
		alGetSourcei( alSourceNum, AL_SOURCE_STATE, &state );
		if ( state == AL_STOPPED ) {
			if ( !streamData->loop && streamData->isDone ) {
				Stop();
				return;
			}
			alSourcePlay( alSourceNum );
			CheckAlErrors();
		}
		OG_ASSERT( streamData->stream );
		streamData->stream->UpdateStream( this );
	}
}

/*
================
AudioSource::OnUpdate
================
*/
void AudioSource::OnUpdate( const AudioSourceSetup *setup ) {
	if ( IsActive() ) {
		alSourcei( alSourceNum, AL_SOURCE_RELATIVE, setup->relative );
		alSourcefv( alSourceNum, AL_POSITION, &setup->origin.x );
		alSourcefv( alSourceNum, AL_VELOCITY, &setup->velocity.x );
		alSourcefv( alSourceNum, AL_DIRECTION, &setup->direction.x );
		if ( !setup->direction.IsZero() ) {
			alSourcef( alSourceNum, AL_CONE_INNER_ANGLE, setup->innerAngle );
			alSourcef( alSourceNum, AL_CONE_OUTER_ANGLE, setup->outerAngle );
			alSourcef( alSourceNum, AL_CONE_OUTER_GAIN, setup->outerVolume );
		}
	}
}

/*
================
AudioSource::SetEffect
================
*/
bool AudioSource::SetEffect( AudioEffectEx *effect ) {
	CheckAlErrors();
	if ( effect )
		alSource3i( alSourceNum, AL_AUXILIARY_SEND_FILTER, effect->alEffectSlotNum, 0, AL_FILTER_NULL );
	else
		alSource3i( alSourceNum, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL );
	return CheckAlErrors();
}

}
