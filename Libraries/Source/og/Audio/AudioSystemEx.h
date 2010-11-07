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

#ifndef __OG_AUDIOSYSTEMEX_H__
#define __OG_AUDIOSYSTEMEX_H__

#include <og/Audio.h>
#include <og/FileSystem.h>
#include <og/Common/Thread/EventQueue.h>
#include "AudioSource.h"
#include "AudioEmitterEx.h"
#include "AudioStream.h"
#include <al/al.h>
#include <al/alc.h>

namespace og {
	extern FileSystemCore *audioFS;
	bool CheckAlErrors( void );

	/*
	==============================================================================

	  AudioThread

	==============================================================================
	*/
	class AudioThread : public Thread {
	public:
		AudioThread( AudioStream *stream ) : firstAudioSource(NULL), defaultStream(stream) {}

		void	AddEvent( QueuedEvent *evt ) { eventQueue.Add( evt ); WakeUp(); }

		AudioSource *FindFreeAudioSource( void );

		void	PlayStream( AudioSource *src, const char *filename, bool loop );

	protected:
		void	Run( void );

	private:
		EventQueue	eventQueue;
		AudioSource *firstAudioSource;

		AudioStream *defaultStream;
		DictEx<AudioStream *> audioStreams;
	};

	/*
	==============================================================================

	  AudioSystemEx

	==============================================================================
	*/
	class AudioSystemEx : public AudioSystem {
		// ---------------------- Public AudioSystem Interface -------------------

		void				SetVolume( float value );
		void				SetMaxVariations( int num ) { maxVariations = num; }
		void				SetListener( const Vec3 &origin, const Vec3 &forward, const Vec3 &up, const Vec3 &velocity );
		void				SetDopplerFactor( float factor );
		void				SetSpeedOfSound( float speed );

		AudioEmitter *		CreateAudioEmitter( int channels=0 );
		void				FreeAudioEmitter( AudioEmitter *emitter );

		// ---------------------- Internal AudioSystemEx Members -------------------

	public:
		AudioSystemEx();

	private:
		friend class AudioSystem;
		friend class AudioSource;
		friend class AudioThread;

		bool		Init( const char *defaultFilename, const char *deviceName );
		void		Shutdown( void );

		bool		windowFocus;
		float		focusVolume;
		float		volume;
		int			maxVariations;
		LinkedList<AudioEmitterEx> audioEmitters;
		ogst::mutex	emitterLock;

		ALCcontext	*context;
		ALCdevice	*device;

	public:
		AudioThread *audioThread;
	};
	extern AudioSystemEx audioSystemObject;
}
#endif
