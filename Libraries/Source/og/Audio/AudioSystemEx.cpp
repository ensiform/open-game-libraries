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
		User::Warning( TS("OpenAL Error: $*!" ) << alErrorMessage(error) );
		return false;
	}
	return true;
}

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
	while( !selfDestruct ) {
		wakeUpEvent.Wait(20);

		EmitterEvent *evt = NULL;
		while( (evt = eventQueue.Consume()) != NULL ) {
			evt->emitter->OnEvent( evt );
			delete evt;
		}
		// update audio sources
		for( AudioSource *source = firstAudioSource; source != NULL; source = source->next ) {
			if ( source->IsActive() )
				source->Frame();
		}
	}
	if ( firstAudioSource ) {
		delete firstAudioSource;
		firstAudioSource = NULL;
	}

	if ( defaultStream ) {
		delete defaultStream;
		defaultStream = NULL;
	}
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

	// Read in all sound Decls
	soundDecls.Clear();
	FileList *files = audioFS->GetFileList( "decls/sounds", ".decl" );
	if ( files ) {
		DeclType sndDecls("soundDecl");
		DeclParser parser;
		parser.AddDeclType( &sndDecls );
		do {
			parser.LoadFile( files->GetName() );
		} while ( files->GetNext() );

		parser.SolveInheritance();
		audioFS->FreeFileList( files );

		const char *value;
		int num = sndDecls.declList.Num();
		for( int i=0; i<num; i++ ) {
			Dict &dict = sndDecls.declList[i];
			SoundDeclEx &decl	= soundDecls[sndDecls.declList.GetKey(i).c_str()];
			decl.minDistance	= dict.GetFloat("minDistance");
			decl.maxDistance	= dict.GetFloat("maxDistance");
			decl.volume			= dict.GetFloat("volume");
			decl.loop			= dict.GetBool("loop");
			decl.safeLevel		= dict.GetInt("safeLevel");
			for( int j = dict.MatchPrefix( "sound", 5 ); j != -1; j = dict.MatchPrefix( "sound", 5, j ) ) {
				value = dict.GetKeyValue(j)->GetValue().c_str();
				if ( value[0] != '\0' )
					decl.sounds.Append( value );
			}
			if ( decl.sounds.IsEmpty() )
				User::Warning( TS("Sound '$*' has no filenames defined." ) << sndDecls.declList.GetKey(i) );
		}
	}

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

	soundDecls.Clear();
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
		alListenerf( AL_GAIN, windowFocus ? volume : 0.0f );
}

/*
================
AudioSystemEx::SetListener
================
*/
void AudioSystemEx::SetListener( const Vec3 &origin, const Vec3 &forward, const Vec3 &up ) {
	if ( device ) {
		float orientation[6] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
		alListener3f( AL_POSITION, origin.y, origin.x, origin.z );
		alListenerfv( AL_ORIENTATION, orientation );
	}
}

/*
================
AudioSystemEx::Find
================
*/
const SoundDecl *AudioSystemEx::Find( const char *name ) const {
	int index = soundDecls.Find( name );
	if ( index == -1 ) {
		User::Warning( TS("Sound '$*' not found." ) << name );
		return NULL;
	}
	return &soundDecls[index];
}

/*
================
AudioSystemEx::CreateAudioEmitter
================
*/
AudioEmitter *AudioSystemEx::CreateAudioEmitter( void ) {
	emitterLock.Lock();
	AudioEmitter *emt = &audioEmitters.Alloc();
	emitterLock.Unlock();

	return emt;
}

/*
================
AudioSystemEx::FreeAudioEmitter
================
*/
void AudioSystemEx::FreeAudioEmitter( AudioEmitter *emitter ) {
	OG_ASSERT( emitter != NULL );
	
	emitterLock.Lock();
	LinkedList<AudioEmitterEx>::nodeType *node = audioEmitters.FindByAddress( static_cast<AudioEmitterEx *>(emitter) );
	if ( node != NULL )
		audioEmitters.Remove( node );
	else
		User::Warning("Trying to free a AudioEmitter, which has not been allocated or already been freed!");
	emitterLock.Unlock();
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
	if ( !Shared::Init() )
		return false;
	audioFS = fileSystem;
	return audioSystemObject.Init(defaultFilename, deviceName);
}
void AudioSystem::Shutdown( void )					{ audioSystemObject.Shutdown(); audioFS = NULL; }
void AudioSystem::SetWindowFocus( bool hasFocus )
	{ audioSystemObject.windowFocus = hasFocus; AS->SetVolume( audioSystemObject.volume ); }

}
