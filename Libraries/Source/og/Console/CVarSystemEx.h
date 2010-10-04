/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: CVar System Interface (engine part)
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

#ifndef __OG_CVARSYSTEM_EX_H__
#define __OG_CVARSYSTEM_EX_H__

namespace og {

/*
==============================================================================

  CVarDataEx

==============================================================================
*/
class CVarDataEx : public CVarData {
public:
	CVarDataEx() { initialized = false; }

	void		SetString( const char *newValue );
	void		SetBool( bool newValue );
	void		SetInt( int newValue );
	void		SetFloat( float newValue );

private:
	void		Init( const CVarData *initData );

	friend class CVarSystemEx;
	bool		initialized;
	CVar *	cvar;
	String	name;
	String	description;
	String	value;
	String	defValue;
};

/*
==============================================================================

  CVarSystemEx

==============================================================================
*/
class CVarSystemEx : public CVarSystem {
public:
	// ---------------------- Public CVarSystem Interface -------------------

	void		ResetToDefault( int flags );
	bool		IsModified( int flags );
	void		SetModified( int flags, bool modified=true );

	CVar *	Find( const char *key ) const;

	// String
	void		SetString( const char *key, const char *value, bool force=false );
	const char *GetString( const char *key ) const;

	// Bool
	void		SetBool( const char *key, bool value, bool force=false );
	bool		GetBool( const char *key ) const;

	// Integer
	void		SetInt( const char *key, int value, bool force=false );
	int			GetInt( const char *key ) const;

	// Float
	void		SetFloat( const char *key, float value, bool force=false );
	float		GetFloat( const char *key ) const;

	void		ToDict( int flags, Dict *dict ) const;
	void		FromDict( const Dict *dict );

	bool		ExportToHTML( const char *filename="cvars.html", int flags=CVAR_ALL ) const;
	bool		WriteToConfig( const char *filename, int flags=CVAR_ARCHIVE ) const;

	bool		OnCommand( const CmdArgs &args );

	void		CompleteCVar( const CmdArgs &args, argCompletionCB_t callback );
	void		CompleteCVar( const char *buf, int bufLen, StringList &completionList );

protected:
	void		LinkCVar( CVar *cvar );
	void		UnlinkCVar( CVar *cvar );

	// ---------------------- Internal CVarSystemEx Members -------------------

public:
	CVarSystemEx();
	~CVarSystemEx();

	void		Clear( void );

	static void Cmd_ListCVars_f( const CmdArgs &args );
	static void Cmd_ExportCVars_f( const CmdArgs &args );
	static void Cmd_Toggle_f( const CmdArgs &args );
	static void Cmd_Set_f( const CmdArgs &args );
#ifdef CMD_CVAR_SETU
	static void Cmd_SetInfo_f( const CmdArgs &args );
#endif
	static void Cmd_SetA_f( const CmdArgs &args );
	static void Cmd_Inc_f( const CmdArgs &args );
	static void Cmd_Dec_f( const CmdArgs &args );
	static void Cmd_Reset_f( const CmdArgs &args );

	const DictEx<CVarDataEx> & GetCVarDataList( void ) const;

private:
	void		CreateUnlinkedCVar( const char *key, const char *value );

	DictEx<CVarDataEx> dataList;
};

extern CVarSystemEx *cvarSystemEx;

}
#endif
