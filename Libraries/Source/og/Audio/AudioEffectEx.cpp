// ==============================================================================
//! @file
//! @brief	Audio Effects
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

#define AL_ALEXT_PROTOTYPES
#include <al/efx.h>

namespace og {

/*
================
alEffectv3
================
*/
OG_INLINE void alEffectv3( uInt effect, int param, const Vec3 &v ) {
	float value[3] = { v.x, v.y, v.z};
	alEffectfv( effect, param, value );
}

/*
==============================================================================

  AudioEffectEx

==============================================================================
*/

/*
================
AudioEffectEx::AudioEffectEx
================
*/
AudioEffectEx::AudioEffectEx() : alEffectType(-1) {
}

/*
================
AudioEffectEx::~AudioEffectEx
================
*/
AudioEffectEx::~AudioEffectEx() {
	// Delete Auxiliary Effect Slot
	alDeleteAuxiliaryEffectSlots( 1, &alEffectSlotNum );
}
/*
================
AudioEffectEx::Init
================
*/
bool AudioEffectEx::Init( void ) {
	CheckAlErrors();

	// Generate an auxiliary effect slot
	alGenAuxiliaryEffectSlots( 1, &alEffectSlotNum );
	return CheckAlErrors();
}

/*
================
AudioEffectEx::CreateEffect
================
*/
bool AudioEffectEx::CreateEffect( int type ) {
	if ( alEffectType == type )
		return true;

	DeleteEffect();
	CheckAlErrors();

	// Generate an effect
	alGenEffects( 1, &alEffectNum );
	if ( !CheckAlErrors() )
		return false;

	// Set the effect type
	alEffecti( alEffectNum, AL_EFFECT_TYPE, type );
	if ( !CheckAlErrors() ) {
		alDeleteEffects( 1, &alEffectNum );
		return false;
	}
	return true;
}

/*
================
AudioEffectEx::DeleteEffect
================
*/
void AudioEffectEx::DeleteEffect( void ) {
	if ( alEffectType != -1 ) {
		alDeleteEffects( 1, &alEffectNum );
		alEffectType = -1;
	}
}

/*
================
AudioEffectEx::Load
================
*/
bool AudioEffectEx::Load( const AudioEffectReverb *effect ) {
	if ( !CreateEffect( AL_EFFECT_EAXREVERB ) )
		return false;

	alEffectf( alEffectNum,		AL_EAXREVERB_DENSITY,				effect->density );
	alEffectf( alEffectNum,		AL_EAXREVERB_DIFFUSION,				effect->diffusion );
	alEffectf( alEffectNum,		AL_EAXREVERB_GAIN,					effect->gain );
	alEffectf( alEffectNum,		AL_EAXREVERB_GAINHF,				effect->gainHF );
	alEffectf( alEffectNum,		AL_EAXREVERB_GAINLF,				effect->gainLF );
	alEffectf( alEffectNum,		AL_EAXREVERB_DECAY_TIME,			effect->decayTime );
	alEffectf( alEffectNum,		AL_EAXREVERB_DECAY_HFRATIO,			effect->decayHFRatio );
	alEffectf( alEffectNum,		AL_EAXREVERB_DECAY_LFRATIO,			effect->decayLFRatio );
	alEffectf( alEffectNum,		AL_EAXREVERB_REFLECTIONS_GAIN,		effect->reflectionsGain );
	alEffectf( alEffectNum,		AL_EAXREVERB_REFLECTIONS_DELAY,		effect->reflectionsDelay );
	alEffectv3( alEffectNum,	AL_EAXREVERB_REFLECTIONS_PAN,		effect->reflectionsPan );
	alEffectf( alEffectNum,		AL_EAXREVERB_LATE_REVERB_GAIN,		effect->lateReverbGain );
	alEffectf( alEffectNum,		AL_EAXREVERB_LATE_REVERB_DELAY,		effect->lateReverbDelay );
	alEffectv3( alEffectNum,	AL_EAXREVERB_LATE_REVERB_PAN,		effect->lateReverbPan );
	alEffectf( alEffectNum,		AL_EAXREVERB_ECHO_TIME,				effect->echoTime );
	alEffectf( alEffectNum,		AL_EAXREVERB_ECHO_DEPTH,			effect->echoDepth );
	alEffectf( alEffectNum,		AL_EAXREVERB_MODULATION_TIME,		effect->modulationTime );
	alEffectf( alEffectNum,		AL_EAXREVERB_MODULATION_DEPTH,		effect->modulationDepth );
	alEffectf( alEffectNum,		AL_EAXREVERB_AIR_ABSORPTION_GAINHF,	effect->airAbsorptionGainHF );
	alEffectf( alEffectNum,		AL_EAXREVERB_HFREFERENCE,			effect->referenceHF );
	alEffectf( alEffectNum,		AL_EAXREVERB_LFREFERENCE,			effect->referenceLF );
	alEffectf( alEffectNum,		AL_EAXREVERB_ROOM_ROLLOFF_FACTOR,	effect->roomRolloffFactor );
	alEffecti( alEffectNum,		AL_EAXREVERB_DECAY_HFLIMIT,			effect->decayHFLimit );

	alAuxiliaryEffectSloti( alEffectSlotNum, AL_EFFECTSLOT_EFFECT, alEffectNum );

	return CheckAlErrors();
}

/*
================
AudioEffectEx::Load
================
*/
bool AudioEffectEx::Load( const AudioEffectEcho *effect ) {
	if ( !CreateEffect( AL_EFFECT_ECHO ) )
		return false;

	alEffectf( alEffectNum,	AL_ECHO_DELAY,		effect->delay );
	alEffectf( alEffectNum,	AL_ECHO_LRDELAY,	effect->delayLR );
	alEffectf( alEffectNum,	AL_ECHO_DAMPING,	effect->damping );
	alEffectf( alEffectNum,	AL_ECHO_FEEDBACK,	effect->feedback );
	alEffectf( alEffectNum,	AL_ECHO_SPREAD,		effect->spread );

	return CheckAlErrors();
}
/*
================
AudioEffectEx::Clear
================
*/
void AudioEffectEx::Clear( void ) {
	CheckAlErrors();
	alAuxiliaryEffectSloti(alEffectSlotNum, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
	CheckAlErrors();
}

}
