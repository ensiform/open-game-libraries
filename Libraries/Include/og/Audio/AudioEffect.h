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

#ifndef __OG_AUDIO_EFFECT_H__
#define __OG_AUDIO_EFFECT_H__

//! Open Game Libraries
namespace og {
//! @defgroup Audio Audio (Library)
//! @{

	// ==============================================================================
	//! Defines properties of an EFX reverb effect
	// ==============================================================================
	class AudioEffectReverb {
	public:
		float	density;
		float	diffusion;
		float	gain;
		float	gainHF;
		float	gainLF;
		float	decayTime;
		float	decayHFRatio;
		float	decayLFRatio;
		float	reflectionsGain;
		float	reflectionsDelay;
		Vec3	reflectionsPan;
		float	lateReverbGain;
		float	lateReverbDelay;
		Vec3	lateReverbPan;
		float	echoTime;
		float	echoDepth;
		float	modulationTime;
		float	modulationDepth;
		float	airAbsorptionGainHF;
		float	referenceHF;
		float	referenceLF;
		float	roomRolloffFactor;
		int		decayHFLimit;
	};

	// ==============================================================================
	//! Defines properties of an EFX echo effect
	// ==============================================================================
	class AudioEffectEcho {
	public:
		float	delay;
		float	delayLR;
		float	damping;
		float	feedback;
		float	spread;  
	};

	// ==============================================================================
	//! This is an effect, you can load reverbs or echoes into it ( only one )
	// ==============================================================================
	class AudioEffect {
	public:
		// ==============================================================================
		//! Load a reverb into the effect, overwrites the last effect
		//!
		//! @param	effect	The reverb effect
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		virtual bool	Load( const AudioEffectReverb *effect ) = 0;

		// ==============================================================================
		//! Load an echo into the effect, overwrites the last effect
		//!
		//! @param	effect	The echo effect
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		virtual bool	Load( const AudioEffectEcho *effect ) = 0;

		// ==============================================================================
		//! Clear the last effect
		// ==============================================================================
		virtual void	Clear( void ) = 0;
	};

//! @}
}

#endif
