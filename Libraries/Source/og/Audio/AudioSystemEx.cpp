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
FileSystemCore *audioFS = OG_NULL;
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
	AudioSource *source = OG_NULL;
	uInt alSourceNum;
	for( int i=0; i<MAX_AUDIOSOURCES; i++ ) {
		alGenSources( 1, &alSourceNum );
		if ( CheckAlErrors() == false )
			break;
		source = new AudioSource( source, alSourceNum );
		if ( firstAudioSource == OG_NULL )
			firstAudioSource = source;
	}/*
	if ( firstAudioSource == OG_NULL ) {
		Shutdown();
		return false;
	}*/
	wakeUpEvent.Lock();
	while( keepRunning ) {
		eventQueue.ProcessAll();

		// update audio sources
		for( AudioSource *source = firstAudioSource; source != OG_NULL; source = source->next ) {
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
		for( AudioSource *source = firstAudioSource; source != OG_NULL; source = source->next ) {
			if ( !source->IsActive() || level >= source->safeLevel ) {
				if ( source->IsActive() )
					source->Stop();
				return source;
			}
		}
	}
	return OG_NULL;
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
	
	context = OG_NULL;
	device = OG_NULL;

	audioThread = OG_NULL;
}

/*
================
AudioSystemEx::Init
================
*/
bool AudioSystemEx::Init( const char *defaultFilename, const char *deviceName ) {
	if ( audioFS == OG_NULL )
		return false;
	if ( audioThread != OG_NULL )
		return true;

	device = deviceName == OG_NULL ? OG_NULL : alcOpenDevice( deviceName );
	if ( device == OG_NULL ) {
		if ( deviceName != OG_NULL )
			User::Warning("Failed to open audio device, trying default.");
		device = alcOpenDevice( OG_NULL );
		if ( device == OG_NULL ) {
			User::Error( ERR_AUDIO_INIT, "Failed to open default audio device.");
			return false;
		}
	}

	context = alcCreateContext( device, OG_NULL );
	if ( context == OG_NULL ) {
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
	if ( defaultStream == OG_NULL ) {
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
		audioThread = OG_NULL;
	}

	audioEmitters.Clear();

	if ( context ) {
		alcMakeContextCurrent( OG_NULL );
		alcDestroyContext( context );
		context = OG_NULL;
	}
	if ( device ) {
		alcCloseDevice( device );
		device = OG_NULL;
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
	OG_ASSERT( emitter != OG_NULL );
	
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
	OG_ASSERT( effect != OG_NULL );

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
	const char *result = OG_NULL;
	if( alcIsExtensionPresent(OG_NULL, "ALC_ENUMERATE_ALL_EXT") )
		result = alcGetString(OG_NULL, ALC_ALL_DEVICES_SPECIFIER);
	else if( alcIsExtensionPresent( OG_NULL, "ALC_ENUMERATION_EXT" ) )
		result = alcGetString(OG_NULL, ALC_DEVICE_SPECIFIER);

	if( result == OG_NULL || *result == '\0' )
		return false;

	do {
		deviceList.Append( result );
		result += strlen(result) + 1;
	} while( *result != '\0' );

	return true;
}
bool AudioSystem::Init( FileSystemCore *fileSystem, const char *defaultFilename, const char *deviceName ) {
	OG_ASSERT( fileSystem != OG_NULL );
	OG_ASSERT( defaultFilename != OG_NULL );
	audioFS = fileSystem;
	return audioSystemObject.Init(defaultFilename, deviceName);
}
void AudioSystem::Shutdown( void )					{ audioSystemObject.Shutdown(); audioFS = OG_NULL; }
void AudioSystem::SetWindowFocus( bool hasFocus )	{ audioSystemObject.windowFocus = hasFocus; }

}
