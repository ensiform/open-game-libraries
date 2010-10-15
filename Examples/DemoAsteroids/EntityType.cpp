/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: EntityType Class
-----------------------------------------

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
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

#include "main.h"


/*
==============================================================================

  ogEntityType

==============================================================================
*/
og::DictEx<ogEntityType *> ogEntityType::typeDict;
int ogEntityType::activeTypes = 0;

/*
================
ogEntityType::ogEntityType
================
*/
ogEntityType::ogEntityType( const char *name, const char *super, ogEntity *(*_CreateEntity )( void ), entitySpawnFunc_t Spawn ) {
	OG_ASSERT( name != NULL && super != NULL );

	typeNum = -1;
	typeNumLastChild = -1;

	typeName = name;
	superTypeName = super;
	superType = NULL;
	CreateEntity = _CreateEntity;
	spawnFunc = Spawn;

	typeDict[typeName] = this;
}

/*
================
ogEntityType::Init
================
*/
void ogEntityType::Init( const og::List<entitySpawnFunc_t> &superSpawnFuncList ) {
	typeNum = activeTypes++;
	spawnFuncList = superSpawnFuncList;

	if ( spawnFuncList.Find( spawnFunc ) == -1 )
		spawnFuncList.Append( spawnFunc );

	// Init all children of this type
	ogEntityType *type;
	int num = typeDict.Num();
	for( int i=0; i<num; i++ ) {
		type = typeDict[i];
		if ( type->superType == this )
			type->Init( spawnFuncList );
	}
	typeNumLastChild = (activeTypes-1);
}

/*
================
ogEntityType::SpawnEntity
================
*/
ogEntity *ogEntityType::SpawnEntity( const char *className, const og::Dict *additionalSettings ) {
	int index = demoWindow.game.entityDecls.declList.Find( className );
	if ( index == -1 )
		throw ogError( og::Format("Classname '$*' does not exist!" ) << className );

	const og::Dict &baseDict = demoWindow.game.entityDecls.declList[index];

	const char *entityType = baseDict["entityType"];

	if ( entityType[0] == '\0' )
		throw ogError( og::Format("Classname '$*' did not define an entityType!" ) << className );

	index = typeDict.Find( entityType );
	if ( index == -1 )
		throw ogError( og::Format("EntityType '$*' not found!" ) << entityType );

	ogEntityType *type = typeDict[index];

	ogEntity *ent = type->CreateEntity();
	
	ent->settings = baseDict;

	if ( additionalSettings ) {
		ent->settings.Append( *additionalSettings, true );
		ent->name = additionalSettings->Get( "name", og::Format( "unnamed_entity_$*" ) << demoWindow.game.spawnCount );
	}
	else
		ent->name = og::Format("unnamed_entity_$*") << demoWindow.game.spawnCount;

	demoWindow.game.RegisterEntity( ent );

	int num = type->spawnFuncList.Num();
	for( int i=0; i<num; i++ )
		( ent->*(type->spawnFuncList[i]) )();
	return ent;
}

/*
================
ogEntityType::DestroyEntity
================
*/
void ogEntityType::DestroyEntity( ogEntity *ent ) {
	demoWindow.game.UnregisterEntity( ent );
	delete ent;
}

/*
================
ogEntityType::RegisterEntityTypes
================
*/
void ogEntityType::RegisterEntityTypes( void ) {
	if ( activeTypes ) {
		throw ogError("RegisterEntityTypes called multiple times!");
		return;
	}

#define REGISTER(className) void Register##className( void ); Register##className();

	REGISTER(ogEntity);
	REGISTER(ogPlayer);
	REGISTER(ogBlast);
	REGISTER(ogAsteroid);
	REGISTER(ogSplitter);
	REGISTER(ogItem);

#undef REGISTER

	InitEntityTypes();
}

/*
================
ogEntityType::InitEntityTypes
================
*/
void ogEntityType::InitEntityTypes( void ) {
	ogEntityType *type;

	// Get Base Type
	int superIndex = typeDict.Find( "ogEntity" );
	if ( superIndex == -1 )
		throw ogError( "Base Type ogEntity not defined!");

	ogEntityType *baseType = typeDict[superIndex];

	// Link all super classes
	int num = typeDict.Num();
	for( int i=0; i<num; i++ ) {
		type = typeDict[i];

		// Ignore base Type, since it doesn't have any super class
		if ( type == baseType )
			continue;

		superIndex = typeDict.Find( type->superTypeName );
		if ( superIndex == -1 )
			throw ogError( og::Format("EntityClass '$*' has unknown SuperClass '$*'!" ) << type->typeName << type->superTypeName );

		type->superType = typeDict[superIndex];
	}

	// Init the base class, which will init it's children, and they will init their children, etc..
	og::List<entitySpawnFunc_t> temp;
	baseType->Init(temp);
}
