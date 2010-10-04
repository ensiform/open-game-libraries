/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Command System Public Interface (shared with game module)
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

#ifndef __OG_CVARSYSTEM_INL__
#define __OG_CVARSYSTEM_INL__

namespace og {

/*
==============================================================================

  CVar

==============================================================================
*/

/*
================
CVar::ResetToDefault
================
*/
OG_INLINE void CVar::ResetToDefault( void ) {
	OG_ASSERT( linkedToEngine );
	data->SetString(data->strDefaultValue);
}

/*
================
CVar::IsModified
================
*/
OG_INLINE bool CVar::IsModified( void ) const {
	OG_ASSERT( linkedToEngine );
	return (data->flags & CVAR_MODIFIED) != 0;
}

/*
================
CVar::SetModified
================
*/
OG_INLINE void CVar::SetModified( bool modified ) {
	OG_ASSERT( linkedToEngine );
	if ( modified != IsModified() )
		data->flags &= ~CVAR_MODIFIED;
}

/*
================
CVar::GetFlags
================
*/
OG_INLINE int CVar::GetFlags( void ) {
	OG_ASSERT( linkedToEngine );
	return data->flags;
}

/*
================
CVar::SetFlags
================
*/
OG_INLINE void CVar::SetFlags( int flags ) {
	OG_ASSERT( linkedToEngine );
	data->flags = flags;
}

/*
================
CVar::SetModifiedCallback
================
*/
OG_INLINE void CVar::SetModifiedCallback( cvarCallback_t callback ) {
	OG_ASSERT( linkedToEngine );
	data->callback = callback;
}

/*
================
CVar::SetString
================
*/
OG_INLINE void CVar::SetString( const char *value ) {
	OG_ASSERT( linkedToEngine );
	data->SetString(value);
}

/*
================
CVar::GetString
================
*/
OG_INLINE const char *CVar::GetString( void ) const {
	OG_ASSERT( linkedToEngine );
	return data->strValue;
}

/*
================
CVar::SetBool
================
*/
OG_INLINE void CVar::SetBool( bool value ) {
	OG_ASSERT( linkedToEngine );
	data->SetBool(value);
}

/*
================
CVar::GetBool
================
*/
OG_INLINE bool CVar::GetBool( void ) const {
	OG_ASSERT( linkedToEngine );
	return (data->iValue != 0);
}

/*
================
CVar::SetInt
================
*/
OG_INLINE void CVar::SetInt( int value ) {
	OG_ASSERT( linkedToEngine );
	data->SetInt(value);
}

/*
================
CVar::GetInt
================
*/
OG_INLINE int CVar::GetInt( void ) const {
	OG_ASSERT( linkedToEngine );
	return data->iValue;
}

/*
================
CVar::SetFloat
================
*/
OG_INLINE void CVar::SetFloat( float value ) {
	OG_ASSERT( linkedToEngine );
	data->SetFloat(value);
}

/*
================
CVar::GetFloat
================
*/
OG_INLINE float CVar::GetFloat( void ) const {
	OG_ASSERT( linkedToEngine );
	return data->fValue;
}

/*
================
CVar::Init
================
*/
OG_INLINE void CVar::Init( const char *name, const char *defValue, int flags, const char *desc, const ConArgComplete *completion, float min, float max, const char **enumValues ) {
	OG_ASSERT( !linkedToEngine );

	// Create a temporary object we can use when linking to the engine.
	data = new CVarData();
	data->strName = name;
	data->strDescription = desc;
	data->strDefaultValue = defValue;
	data->flags = flags;
	data->completion = completion;
	data->fMinValue = min;
	data->fMaxValue = max;
	data->iMinValue = Math::FtoiFast(min); // can't use Math::Ftoi here, sys is not initialized yet
	data->iMaxValue = Math::FtoiFast(max);
	data->enumValues = enumValues;
	
	// These are not used in the initial state
	data->strValue = NULL;
	data->iValue = 0;
	data->fValue = 0.0f;
	data->callback = NULL;

	nextLink = firstLink;
	firstLink = this;
}

/*
================
CVar::CVar
================
*/
OG_INLINE CVar::CVar( const char *name, const char *defValue, int flags, const char *desc, const ConArgComplete *completion ) {
	OG_ASSERT( !(flags & CVAR_ENUM) );
	Init( name, defValue, flags, desc, completion, -1.0f, 1.0f, NULL);
}
OG_INLINE CVar::CVar( const char *name, const char *defValue, int flags, const char *desc, float min, float max ) {
	OG_ASSERT( (flags & CVAR_INTEGER) || (flags & CVAR_FLOAT) );
	Init( name, defValue, flags, desc, NULL, min, max, NULL);
}
OG_INLINE CVar::CVar( const char *name, const char *defValue, int flags, const char *desc, const char **enumValues ) {
	OG_ASSERT( flags & CVAR_ENUM );
	Init( name, defValue, flags, desc, NULL, -1.0f, 1.0f, enumValues);
}

/*
================
CVar::LinkCVars
================
*/
OG_INLINE void CVar::LinkCVars( void ) {
	if ( linkedToEngine )
		return;

	CVarData *oldData;
	for ( CVar *cv=firstLink; cv != NULL; cv = cv->nextLink ) {
		// Store pointer to the old data object, so we can free it after linking
		oldData = cv->data;
		cvarSystem->LinkCVar( cv );
		delete oldData;
	}

	linkedToEngine = true;
}

/*
================
CVar::UnlinkCVars
================
*/
OG_INLINE void CVar::UnlinkCVars( void ) {
	if ( !linkedToEngine )
		return;

	for ( CVar *cv=firstLink; cv != NULL; cv = cv->nextLink )
		cvarSystem->UnlinkCVar( cv );

	linkedToEngine = false;
}

}

#endif
