// ==============================================================================
//! @file
//! @brief	Audio System
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
#include <al/alext.h>

namespace og {
FileSystemCore *audioFS = NULL;
// The object and global pointer
AudioSystemEx audioSystemObject;
AudioSystem *AS = &audioSystemObject;

const int MAX_AUDIOSOURCES = 64;
const int MAX_SAFE_LEVELS = 8;
const float FOCUS_STEP = 0.05f;

/*
================
alErrorMessage
================
*/
const char *alErrorMessage( ALenum error ) {
	switch ( error ) {
		case AL_NO_ERROR:
			return "No error";
		case AL_INVALID_NAME:
			return "Invalid name";
		case AL_INVALID_ENUM:
			return "Invalid enumerator";
		case AL_INVALID_VALUE:
			return "Invalid value";
		case AL_INVALID_OPERATION:
			return "Invalid operation";
		case AL_OUT_OF_MEMORY:
			return "Out of memory";
		default:
			return "Unknown error";
	}
}

/*
================
CheckAlErrors
================
*/
bool CheckAlErrors( void ) {
	ALenum error = alGetError();
	if( error != AL_NO_ERROR ) {
		User::Warning( Format("OpenAL Error: $*!" ) << alErrorMessage(error) );
		return false;
	}
	return true;
}

/*
================
AudioThread::Run
================
*/
void AudioThread::Run( void ) {
	AudioSource *source = NULL;
	uInt alSourceNum;
	for( int i=0; i<MAX_AUDIOSOURCES; i++ ) {
		alGenSources( 1, &alSourceNum );
		if ( CheckAlErrors() == false )
			break;
		source = new AudioSource( source, alSourceNum );
		if ( firstAudioSource == NULL )
			firstAudioSource = source;
	}/*
	if ( firstAudioSource == NULL ) {
		Shutdown();
		return false;
	}*/
	wakeUpEvent.Lock();
	while( keepRunning ) {
		eventQueue.ProcessAll();

		// update audio sources
		for( AudioSource *source = firstAudioSource; source != NULL; source = source->next ) {
			if ( source->IsActive() )
				source->Frame();
		}

		// do focus fading
		if ( audioSystemObject.windowFocus && audioSystemObject.focusVolume != 1.0f ) {
			audioSystemObject.focusVolume += FOCUS_STEP;
			if ( audioSystemObject.focusVolume > 1.0f )
				audioSystemObject.focusVolume = 1.0f;
			audioSystemObject.SetVolume( audioSystemObject.volume );
		} else if ( !audioSystemObject.windowFocus && audioSystemObject.focusVolume != 0.0f ) {
			audioSystemObject.focusVolume -= FOCUS_STEP;
			if ( audioSystemObject.focusVolume < 0.0f )
				audioSystemObject.focusVolume = 0.0f;
			audioSystemObject.SetVolume( audioSystemObject.volume );
		}

		// Wait for an event or 20ms passed
		wakeUpEvent.Wait(20);
	}
	wakeUpEvent.Unlock();

	// Consume remaining events
	eventQueue.ProcessAll();

	SafeDelete( firstAudioSource );
	SafeDelete( defaultStream );

	int num = audioStreams.Num();
	for( int i=0; i<num; i++ )
		delete audioStreams[i];
	audioStreams.Clear();
}

/*
================
AudioThread::FindFreeAudioSource
================
*/
AudioSource *AudioThread::FindFreeAudioSource( void ) {
	for( int level=-1; level<MAX_SAFE_LEVELS; level++ ) {
		for( AudioSource *source = firstAudioSource; source != NULL; source = source->next ) {
			if ( !source->IsActive() || level >= source->safeLevel ) {
				if ( source->IsActive() )
					source->Stop();
				return source;
			}
		}
	}
	return NULL;
}

void AudioThread::PlayStream( AudioSource *src, const char *filename, bool loop ) {
	AudioStream *stream;
	int index = audioSystemObject.audioThread->audioStreams.Find( filename );
	if ( index != -1 ) 
		stream = audioSystemObject.audioThread->audioStreams[index];
	else {
		stream = AudioStream::Open( filename );
		audioSystemObject.audioThread->audioStreams[filename] = stream;
	}

	if ( !stream->StartStream( src, loop ) ) {
		index = audioSystemObject.audioThread->audioStreams.Find( filename );
		audioSystemObject.audioThread->audioStreams.Remove( index );

		stream = audioSystemObject.audioThread->defaultStream;
		OG_ASSERT( stream->StartStream( src, false ) );
	}
}

/*
==============================================================================

  AudioSystemEx

==============================================================================
*/
/*
================
AudioSystemEx::AudioSystemEx
================
*/
AudioSystemEx::AudioSystemEx() {
	volume = 1.0f;
	focusVolume = 1.0f;
	windowFocus = true;
	maxVariations = 0;
	
	context = NULL;
	device = NULL;

	audioThread = NULL;
}

/*
================
AudioSystemEx::Init
================
*/
bool AudioSystemEx::Init( const char *defaultFilename, const char *deviceName ) {
	if ( audioFS == NULL )
		return false;
	if ( audioThread != NULL )
		return true;

	device = deviceName == NULL ? NULL : alcOpenDevice( deviceName );
	if ( device == NULL ) {
		if ( deviceName != NULL )
			User::Warning("Failed to open audio device, trying default.");
		device = alcOpenDevice( NULL );
		if ( device == NULL ) {
			User::Error( ERR_AUDIO_INIT, "Failed to open default audio device.");
			return false;
		}
	}

	context = alcCreateContext( device, NULL );
	if ( context == NULL ) {
		alcCloseDevice( device );
		User::Error(ERR_AUDIO_INIT, "Failed to create audio context.");
		return false;
	}
	alcMakeContextCurrent( context );
	alDistanceModel( AL_LINEAR_DISTANCE );

	if ( !CheckAlErrors() ) {
		Shutdown();
		return false;
	}

	AudioStream *defaultStream = AudioStream::Open( defaultFilename );
	if ( defaultStream == NULL ) {
		Shutdown();
		return false;
	}

	volume = 1.0f;
	windowFocus = true;
	maxVariations = 0;

	audioThread = new AudioThread( defaultStream );
	audioThread->Start("AudioThread");

	return true;
}

/*
================
AudioSystemEx::Shutdown
================
*/
void AudioSystemEx::Shutdown( void ) {
	if ( audioThread ) {
		audioThread->Stop();
		audioThread = NULL;
	}

	audioEmitters.Clear();

	if ( context ) {
		alcMakeContextCurrent( NULL );
		alcDestroyContext( context );
		context = NULL;
	}
	if ( device ) {
		alcCloseDevice( device );
		device = NULL;
	}
}

/*
================
AudioSystemEx::SetVolume
================
*/
void AudioSystemEx::SetVolume( float value ) {
	volume = value;
	if ( device )
		alListenerf( AL_GAIN, focusVolume * volume );
}

/*
================
AudioSystemEx::SetListener
================
*/
void AudioSystemEx::SetListener( const Vec3 &origin, const Vec3 &forward, const Vec3 &up, const Vec3 &velocity ) {
	if ( device ) {
		float orientation[6] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
		alListener3f( AL_POSITION, origin.y, origin.x, origin.z );
		alListenerfv( AL_ORIENTATION, orientation );
		alListenerfv( AL_VELOCITY, &velocity.x );
	}
}

/*
================
AudioSystemEx::SetDopplerFactor
================
*/
void AudioSystemEx::SetDopplerFactor( float factor ) {
	alDopplerFactor( factor );
}

/*
================
AudioSystemEx::SetUnitLength
================
*/
void AudioSystemEx::SetUnitLength( float value ) {
	alListenerf( AL_METERS_PER_UNIT, value );
}

/*
================
AudioSystemEx::SetSpeedOfSound
================
*/
void AudioSystemEx::SetSpeedOfSound( float speed ) {
	alSpeedOfSound( speed );
}

/*
================
AudioSystemEx::CreateEmitter
================
*/
AudioEmitter *AudioSystemEx::CreateEmitter( int channels ) {
	emitterLock.lock();
	AudioEmitterEx *emt = &audioEmitters.Alloc();
	emitterLock.unlock();

	emt->node = audioEmitters.GetLastNode();
	if ( channels > 0 )
		emt->Init( channels );
	return emt;
}

/*
================
AudioSystemEx::FreeEmitter
================
*/
void AudioSystemEx::FreeEmitter( AudioEmitter *emitter ) {
	OG_ASSERT( emitter != NULL );
	
	emitterLock.lock();
	audioEmitters.Remove( static_cast<AudioEmitterEx *>(emitter)->node );
	emitterLock.unlock();
}

/*
================
AudioSystemEx::CreateEffect
================
*/
AudioEffect *AudioSystemEx::CreateEffect( void ) {
	effectLock.lock();
	AudioEffectEx *effect = &audioEffects.Alloc();
	effectLock.unlock();
	effect->node = audioEffects.GetLastNode();
	effect->Init();
	return effect;
}

/*
================
AudioSystemEx::FreeEffect
================
*/
void AudioSystemEx::FreeEffect( AudioEffect *effect ) {
	OG_ASSERT( effect != NULL );

	effectLock.lock();
	audioEffects.Remove( static_cast<AudioEffectEx *>(effect)->node );
	effectLock.unlock();
}

/*
==============================================================================

  AudioSystem

==============================================================================
*/
bool AudioSystem::GetDeviceList( StringList &deviceList ) {
	const char *result = NULL;
	if( alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") )
		result = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
	else if( alcIsExtensionPresent( NULL, "ALC_ENUMERATION_EXT" ) )
		result = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

	if( result == NULL || *result == '\0' )
		return false;

	do {
		deviceList.Append( result );
		result += strlen(result) + 1;
	} while( *result != '\0' );

	return true;
}
bool AudioSystem::Init( FileSystemCore *fileSystem, const char *defaultFilename, const char *deviceName ) {
	OG_ASSERT( fileSystem != NULL );
	OG_ASSERT( defaultFilename != NULL );
	audioFS = fileSystem;
	return audioSystemObject.Init(defaultFilename, deviceName);
}
void AudioSystem::Shutdown( void )					{ audioSystemObject.Shutdown(); audioFS = NULL; }
void AudioSystem::SetWindowFocus( bool hasFocus )	{ audioSystemObject.windowFocus = hasFocus; }

}
