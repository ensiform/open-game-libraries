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
#include <og/FileSystem.h>

/*
================
ogSoundManager::ogSoundManager
================
*/
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
	for( int i=0; i<files->Num(); i++ )
		parser.LoadFile( files->GetName(i) );

	parser.SolveInheritance();
	og::FS->FreeFileList( files );

	int num = soundDecls.declList.Num();
	for( int i=0; i<num; i++ ) {
		og::Dict &dict		= soundDecls.declList[i];
		og::Sound &sound	= sounds[soundDecls.declList.GetKey(i).c_str()];
		sound.minDistance	= dict["minDistance"];
		sound.maxDistance	= dict["maxDistance"];
		sound.volume		= dict["volume"];
		sound.loop			= dict["loop"];
		for( int j = dict.MatchPrefix( "sound", 5 ); j != -1; j = dict.MatchPrefix( "sound", 5, j ) ) {
			const og::String &value = dict.GetKeyValue(j)->GetStringValue();
			if ( !value.IsEmpty() )
				sound.filenames.Append( value );
		}
		if ( sound.filenames.IsEmpty() )
			og::User::Warning( og::Format("Sound '$*' has no filenames defined." ) << soundDecls.declList.GetKey(i) );
	}
	return sounds.Num() > 0;
}

/*
================
ogSoundManager::InitReverbs

Read in all reverb declarations
================
*/
bool ogSoundManager::InitReverbs( void ) {
	og::FileList *files = og::FS->GetFileList( "decls/reverbs", ".decl" );
	if ( !files )
		return false;

	og::DeclType reverbPresets("reverbPreset");
	og::DeclParser parser;
	parser.AddDeclType( &reverbPresets );
	for( int i=0; i<files->Num(); i++ )
		parser.LoadFile( files->GetName(i) );

	parser.SolveInheritance();
	og::FS->FreeFileList( files );

	int num = reverbPresets.declList.Num();
	for( int i=0; i<num; i++ ) {
		const og::Dict &from = reverbPresets.declList[i];
		og::AudioEffectReverb &reverb = reverbs[ reverbPresets.declList.GetKey(i).c_str() ];

		reverb.density				= from["density"];
		reverb.diffusion			= from["diffusion"];
		reverb.gain					= from["gain"];
		reverb.gainHF				= from["gainHF"];
		reverb.gainLF				= from["gainLF"];
		reverb.decayTime			= from["decayTime"];
		reverb.decayHFRatio			= from["decayHFRatio"];
		reverb.decayLFRatio			= from["decayLFRatio"];
		reverb.reflectionsGain		= from["reflectionsGain"];
		reverb.reflectionsDelay		= from["reflectionsDelay"];
		reverb.reflectionsPan		= from["reflectionsPan"];
		reverb.lateReverbGain		= from["lateReverbGain"];
		reverb.lateReverbDelay		= from["lateReverbDelay"];
		reverb.lateReverbPan		= from["lateReverbPan"];
		reverb.echoTime				= from["echoTime"];
		reverb.echoDepth			= from["echoDepth"];
		reverb.modulationTime		= from["modulationTime"];
		reverb.modulationDepth		= from["modulationDepth"];
		reverb.airAbsorptionGainHF	= from["airAbsorptionGainHF"];
		reverb.referenceHF			= from["referenceHF"];
		reverb.referenceLF			= from["referenceLF"];
		reverb.roomRolloffFactor	= from["roomRolloffFactor"];
		reverb.decayHFLimit			= from["decayHFLimit"];
	}
	return true;
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
		audioEmitters.Append( og::AS->CreateEmitter( 1 ) );
	nextEmitter = 0;
	ambienceEmitter = og::AS->CreateEmitter( 1 );
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
		og::AS->FreeEmitter( audioEmitters[i] );
	audioEmitters.Clear();
	if ( ambienceEmitter ) {
		og::AS->FreeEmitter( ambienceEmitter );
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