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

#include "SoundManager.h"
#include <og/FileSystem/FileSystem.h>

ogSoundManager::ogSoundManager() {
	defaultSound.minDistance	= 0.0f;
	defaultSound.maxDistance	= 9999.0f;
	defaultSound.volume			= 1.0f;
	defaultSound.loop			= false;
}

/*
================
ogSoundManager::Init

Read in all sound declarations
================
*/
bool ogSoundManager::Init( const char *defaultFilename ) {
	Clear();
	if ( og::FS == NULL )
		return false;
	og::FileList *files = og::FS->GetFileList( "decls/sounds", ".decl" );
	if ( !files )
		return false;

	defaultSound.filenames.Append( defaultFilename );

	og::DeclType soundDecls("soundDecl");
	og::DeclParser parser;
	parser.AddDeclType( &soundDecls );
	do {
		parser.LoadFile( files->GetName() );
	} while ( files->GetNext() );

	parser.SolveInheritance();
	og::FS->FreeFileList( files );

	const char *value;
	int num = soundDecls.declList.Num();
	for( int i=0; i<num; i++ ) {
		og::Dict &dict		= soundDecls.declList[i];
		og::Sound &sound	= sounds[soundDecls.declList.GetKey(i).c_str()];
		sound.minDistance	= dict.GetFloat("minDistance");
		sound.maxDistance	= dict.GetFloat("maxDistance");
		sound.volume		= dict.GetFloat("volume");
		sound.loop			= dict.GetBool("loop");
		for( int j = dict.MatchPrefix( "sound", 5 ); j != -1; j = dict.MatchPrefix( "sound", 5, j ) ) {
			value = dict.GetKeyValue(j)->GetValue().c_str();
			if ( value[0] != '\0' )
				sound.filenames.Append( value );
		}
		if ( sound.filenames.IsEmpty() )
			og::User::Warning( og::Format("Sound '$*' has no filenames defined." ) << soundDecls.declList.GetKey(i) );
	}
	return sounds.Num() > 0;
}

/*
================
ogSoundManager::Clear
================
*/
void ogSoundManager::Clear( void ) {
	defaultSound.filenames.Clear();
	sounds.Clear();
}

/*
================
ogSoundManager::Find
================
*/
const og::Sound *ogSoundManager::Find( const char *name ) const {
	int index = sounds.Find( name );
	if ( index == -1 ) {
		og::User::Warning( og::Format("Sound '$*' not found." ) << name );
		return &defaultSound;
	}
	return &sounds[index];
}


/*
==============================================================================

  ogSoundManager2D

==============================================================================
*/
/*
================
ogSoundManager2D::SetupEmitters
================
*/
bool ogSoundManager2D::SetupEmitters( uInt maxSounds ) {
	// create audio emitters
	for( int i=0; i<maxSounds; i++ )
		audioEmitters.Append( og::AS->CreateAudioEmitter( 1 ) );
	nextEmitter = 0;
	ambienceEmitter = og::AS->CreateAudioEmitter( 1 );
	ambienceEmitter->SetPosition( og::c_vec3::origin );
	return true;
}

/*
================
ogSoundManager2D::Clear
================
*/
void ogSoundManager2D::Clear( void ) {
	int max = audioEmitters.Num();
	for( int i=0; i<max; i++ )
		og::AS->FreeAudioEmitter( audioEmitters[i] );
	audioEmitters.Clear();
	if ( ambienceEmitter ) {
		og::AS->FreeAudioEmitter( ambienceEmitter );
		ambienceEmitter = NULL;
	}
	nextEmitter = 0;

	ogSoundManager::Clear();
}

/*
================
ogSoundManager2D::Play
================
*/
void ogSoundManager2D::Play( const char *name, const og::Vec2 &origin ) {
	int max = audioEmitters.Num();
	for( int i=0; i<max && audioEmitters[nextEmitter]->IsOccupied(0); i++ ) {
		nextEmitter++;
		if ( nextEmitter >= max )
			nextEmitter = 0;
	}
	og::Vec3 audioOrigin( origin.x, origin.y, 10.0f );
	audioOrigin *= 0.01f;
	audioEmitters[nextEmitter]->Play( 0, Find( name ) );
	audioEmitters[nextEmitter]->SetPosition( audioOrigin );
	nextEmitter++;
	if ( nextEmitter >= max )
		nextEmitter = 0;
}

/*
================
ogSoundManager2D::StartAmbience
================
*/
void ogSoundManager2D::StartAmbience( const char *name ) {
	const og::Sound *sound = Find( name );
	if ( sound )
		ambienceEmitter->Play( 0, sound );
}

/*
================
ogSoundManager2D::StopAll
================
*/
void ogSoundManager2D::StopAll( void ) {
	int max = audioEmitters.Num();
	for( int i=0; i<max; i++ ) {
		if ( audioEmitters[i] )
			audioEmitters[i]->Stop(0);
	}
}

/*
================
ogSoundManager2D::StopAmbience
================
*/
void ogSoundManager2D::StopAmbience( void ) {
	if ( ambienceEmitter )
		ambienceEmitter->Stop(0);
}