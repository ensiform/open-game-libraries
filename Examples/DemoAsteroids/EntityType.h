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

#ifndef __OG_ENTITYTYPE_H__
#define __OG_ENTITYTYPE_H__

/*
==============================================================================

  ogEntityType

==============================================================================
*/
class ogEntity;
typedef void ( ogEntity::*entitySpawnFunc_t )( void );

class ogEntityType {
public:
	ogEntityType( const char *name, const char *super, ogEntity *(*CreateEntity )( void ), entitySpawnFunc_t Spawn );

	void		Init( const og::List<entitySpawnFunc_t> &superSpawnFuncList );

	static ogEntity *SpawnEntity( const char *className, const og::Dict *additionalSettings=NULL );
	static void	DestroyEntity( ogEntity *ent );

	int			typeNum;
	int			typeNumLastChild;

	const char *typeName;
	const char *superTypeName;

	ogEntityType *superType;

	ogEntity *	(*CreateEntity )( void );
	entitySpawnFunc_t spawnFunc;
	og::List<entitySpawnFunc_t> spawnFuncList;

	static void RegisterEntityTypes( void );

private:
	static void InitEntityTypes( void );

	static og::DictEx<ogEntityType *> typeDict;
	static int	activeTypes;
};

#define DECLARE_ENTITY_TYPE() \
public:\
	static void		RegisterEntityType( void );\
	static ogEntity *CreateEntity( void );\
	static const ogEntityType &GetStaticType( void );\
	virtual bool	IsType( const ogEntityType &other ) const;\
	virtual const ogEntityType &GetType( void ) const;\
\
private:\
	static ogEntityType *entityType;

#define DEFINE_ENTITY_TYPE(className, superClassName) \
ogEntityType *className::entityType;\
void className::RegisterEntityType( void ) {\
	static ogEntityType type( #className, #superClassName, className::CreateEntity, ( void ( ogEntity::* )( void ) )&className::Spawn );\
	entityType = &type;\
}\
ogEntity *className::CreateEntity( void ) {\
	return new className;\
}\
void Register##className( void ) {\
	className::RegisterEntityType();\
}\
bool className::IsType( const ogEntityType &other ) const {\
	return entityType->typeNum >= other.typeNum && entityType->typeNum <= other.typeNumLastChild; \
}\
const ogEntityType &className::GetType( void ) const{\
	return *entityType;\
}\
const ogEntityType &className::GetStaticType( void ) {\
	return *entityType;\
}

#endif
