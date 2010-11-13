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

#ifndef __OG_AUDIO_EMITTER_EX_H__
#define __OG_AUDIO_EMITTER_EX_H__

namespace og {
	/*
	==============================================================================

	  AudioEmitterEx

	==============================================================================
	*/
	class AudioEmitterEx : public AudioEmitter {
	public:
		// ---------------------- Public AudioEmitter Interface -------------------

		void	Init( int channels );
		int		NumChannels( void ) const { return numChannels; }

		void	Play( int channel, const Sound *sound, bool allowLoop=true );
		void	Pause( int channel );
		void	Stop( int channel );
		void	StopAll( void );
		bool	IsOccupied( int channel );

		void	SetRelative( bool value );
		void	SetPosition( const Vec3 &pos );
		void	SetVelocity( const Vec3 &vel );
		void	SetDirectional( const Vec3 &dir, float innerAngle, float outerAngle, float outerVolume );

		// ---------------------- Internal AudioEmitterEx Members -------------------

	public:
		AudioEmitterEx();
		~AudioEmitterEx();

		void	Clear( void );
		void	OnSourceStop( int channel );

	private:
		friend class EE_Play;
		friend class EE_Pause;
		friend class EE_Stop;
		friend class EE_Update;
		friend class EE_SetRelative;
		friend class EE_SetPosition;
		friend class EE_SetVelocity;
		friend class EE_SetDirectional;
		friend class AudioSystemEx;

		bool	IsValidChannel( int channel ) const;

		AudioSource **sndChannels;
		int		numChannels;

		AudioSourceSetup details;

		LinkedList<AudioEmitterEx>::nodeType *node;
	};

}

#endif
