// ==============================================================================
//! @file
//! @brief	Audio Effect
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

#ifndef __OG_AUDIO_EFFECT_EX_H__
#define __OG_AUDIO_EFFECT_EX_H__

namespace og {
	/*
	==============================================================================

	  AudioEffectEx

	==============================================================================
	*/
	class AudioEffectEx : public AudioEffect {
	public:
		// ---------------------- Public AudioEffect Interface -------------------

		bool	Load( const AudioEffectReverb *effect );
		bool	Load( const AudioEffectEcho *effect );
		void	Clear( void );

		// ---------------------- Internal AudioEffectEx Members -------------------

	public:
		AudioEffectEx();
		~AudioEffectEx();

		bool	Init( void );

	private:
		friend class AudioSource;
		friend class AudioSystemEx;

		bool	CreateEffect( int type );
		void	DeleteEffect( void );

		uInt	alEffectSlotNum;
		int		alEffectType;
		uInt	alEffectNum;
		LinkedList<AudioEffectEx>::nodeType *node;
	};

}

#endif
