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

#ifndef __OG_AUDIOSOURCE_H__
#define __OG_AUDIOSOURCE_H__

namespace og {
	class AudioEffectEx;
	class AudioEmitterEx;
	class AudioStreamData;

	const int AUDIOSRC_BUFFERS = 4;

	struct AudioSourceSetup {
		bool	relative;
		Vec3	origin;
		Vec3	velocity;
		Vec3	direction;
		float	innerAngle;
		float	outerAngle;
		float	outerVolume;
	};

	/*
	==============================================================================

	  AudioSource

	==============================================================================
	*/
	class AudioSource {
	public:
		AudioSource( AudioSource *previous, uInt sourceNum );
		~AudioSource();

		bool	Play( AudioEmitterEx *emitter, int channel, const Sound *sound, bool allowLoop );
		void	Pause( void );
		void	Stop( void );

		void	Frame( void );
		void	OnUpdate( const AudioSourceSetup *setup );

		bool	IsActive( void ) const { return emitter != OG_NULL; }

		bool	SetEffect( AudioEffectEx *effect );

	private:
		friend class AudioThread;
		friend class AudioStream;

		AudioSource *next;
		AudioSource *prev;
		int	safeLevel;

		AudioEmitterEx *emitter;
		int		emitterChannel;

		uInt	alSourceNum;

		AudioStreamData *streamData;
		uInt	alBuffers[AUDIOSRC_BUFFERS];
	};
}

#endif
