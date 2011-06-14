/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Sound Manager
-----------------------------------------

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
===========================================================================
*/

#ifndef __DEMO_SOUND_MANAGER_H__
#define __DEMO_SOUND_MANAGER_H__

#include <og/Audio.h>

/*
==============================================================================

  ogSoundManager

==============================================================================
*/
class ogSoundManager {
public:
	ogSoundManager();

	virtual bool			Init( const char *defaultFilename );
	virtual bool			InitReverbs( void );
	virtual void			Clear( void );
	const og::Sound *		Find( const char *name ) const;

	// Reverbs
	const og::AudioEffectReverb *FindReverb( const char *name ) { return &reverbs[name]; }
	const og::AudioEffectReverb *GetReverb( int index ) { return &reverbs[index]; }
	int						GetNumReverbs( void ) const { return reverbs.Num(); }
	const og::String &		GetReverbName( int index ) const { return reverbs.GetKey(index); }

private:
	og::DictEx<og::Sound>	sounds;
	og::Sound				defaultSound;
	og::DictEx<og::AudioEffectReverb> reverbs;
};

/*
==============================================================================

  ogSoundManager2D

  Note: This is not how you should handle sound in a 3D game!
  In a 3D game, the emitter should be bound to the entity,
  getting velocity and position from its owner.

==============================================================================
*/
class ogSoundManager2D : public ogSoundManager {
public:
	ogSoundManager2D() : nextEmitter(0), ambienceEmitter(NULL) {}

	bool			SetupEmitters( uInt maxSounds );
	virtual void	Clear( void );

	void			Play( const char *name, const og::Vec2 &origin );
	void			StopAll( void );
	void			StartAmbience( const char *name );
	void			StopAmbience( void );

private:
	int							nextEmitter;
	og::List<og::AudioEmitter *>audioEmitters;
	og::AudioEmitter *			ambienceEmitter;

};
#endif
