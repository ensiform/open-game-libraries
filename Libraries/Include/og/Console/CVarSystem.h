// ==============================================================================
//! @file
//! @brief	CVar System Public Interface (shared with game module)
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

#ifndef __OG_CVARSYSTEM_H__
#define __OG_CVARSYSTEM_H__

#include "CmdSystem.h"

//! Open Game Libraries
namespace og {
//! @addtogroup Console
//! @{
//! @defgroup CVarSystem CVarSystem (Library)
//! @see	Console
//! @{

	/*
	recommendations for cvar naming prefixes:
	(fixme: unfinished.. those who need thought are marked with //)

	info_*:	Informations, like game version, build date, etc.. flagged CVAR_ROM(non-exclusive)
	sys_*:	Detected hardware info, like cpu, gpu, ram, etc.. flagged CVAR_ROM(non-exclusive)
	com_*:	CVars flagged with CVAR_ENGINE but not one of the following:
//	dev_*:	CVars flagged with CVAR_DEVELOPER
	r_*:	CVars flagged with CVAR_RENDERER, except images
//	img_*:	CVars flagged with CVAR_RENDERER image (quality,..) settings
	s_*:	CVars flagged with CVAR_SOUND
	in_*:	CVars flagged with CVAR_INPUT
	net_*:	CVars flagged with CVAR_NETWORK
	g_*:	CVars flagged with CVAR_GAME, except physics
//	phys_*:	CVars flagged with CVAR_GAME, physics
//	ns_*:	CVars flagged with CVAR_NETSYNC
	feed_*:	CVars flagged with CVAR_BROWSERFEED
	si_*:	CVars flagged with CVAR_SERVERINFO
	cl_*:	CVars flagged with CVAR_CLIENT
	fs_*:	Filesystem settings
	gui_*:	Graphical User Interface
	hud_*:	Heads Up Display
	ai_*:	Artificial Intelligence

	fixme: more ?
	*/

	// ==============================================================================
	//! CVar Flags
	// ==============================================================================
	typedef enum {
		CVAR_ALL		= -1,			//!< Covers all CVar flags

		// CVar Types( default is String )
		CVAR_BOOL		= BIT(0),		//!< Boolean value, can only be 0 or 1
		CVAR_INTEGER	= BIT(1),		//!< Integer value, can have an upper and a lower limit
		CVAR_FLOAT		= BIT(2),		//!< Floating point number, can have an upper and a lower limit
		CVAR_ENUM		= BIT(3),		//!< Enumeration, can be accessed as both string or integer value.

		CVAR_NUM_TYPES	= 4,			//!< Number of Types (excluding string), for internal use

		CVAR_UNLINKED	= BIT(4),		//!< Not linked to a cvar object, for internal use
		CVAR_MODIFIED	= BIT(5),		//!< This CVar has been modified (set internally)\n
										//!  Use cvar->Set/GetModified()

		CVAR_ARCHIVE	= BIT(6),		//!< This CVar gets stored in the config.

		// CVars that can only be changed under Certain conditions
		CVAR_ROM		= BIT(7),		//!< The end user can not change this CVar
		CVAR_INIT		= BIT(8),		//!< Must be set via commandline arguments during startup
		CVAR_CHEAT		= BIT(9),		//!< Cheats must be enabled to change this CVar
		CVAR_DEVELOPER	= BIT(10),		//!< Must be in developer mode to see and change this CVar
										//!  @todo	Needs to be checked like CVAR_CHEAT

		// CVars that do not affect changes immediately 
		CVAR_MAPRESTART	= BIT(11),		//!< Requires a world/map restart
		CVAR_VIDRESTART	= BIT(12),		//!< Requires video restart
		CVAR_SNDRESTART	= BIT(13),		//!< Requires sound system restart

		//CVar relations
		CVAR_ENGINE		= BIT(14),		//!< Related to the engine
		CVAR_RENDERER	= BIT(15),		//!< Related to the renderer
		CVAR_SOUND		= BIT(16),		//!< Related to the sound system
		CVAR_INPUT		= BIT(17),		//!< Related to the input system
		CVAR_NETWORK	= BIT(18),		//!< Related to the network settings
		CVAR_GAME		= BIT(19),		//!< Related to the game module

		// CVars that get transfered via network
		CVAR_NETSYNC	= BIT(20),		//!< Synced over network instantly
		CVAR_BROWSERFEED= BIT(21),		//!< Information for the Serverbrowser only
		CVAR_SERVERINFO	= BIT(22),		//!< General server information
		CVAR_CLIENT	= BIT(23),			//!< General client/user information

		CVAR_CUSTOM_BIT	= 24			//!< The first free bit for custom CVar flags.\n
										//!  Keep in mind, there are only 32 bits available ( BIT(0)-BIT(31) )
	} cvarFlags_t;

	// ==============================================================================
	//! CVar on change callback
	// ==============================================================================
	typedef void ( *cvarCallback_t )( void );

	const int CVAR_FLOAT_PRECISION = 4;	//!< The float precision used in CVars

	// ==============================================================================
	//! CVar data
	//!
	//! The object behind the CVar
	// ==============================================================================
	class CVarData {
	public:
		CVarData() {}
		virtual ~CVarData() {}

		const char *strName;				//!< Name of the CVar
		const char *strDescription;			//!< Description 
		const char *strValue;				//!< String representation of the value
		const char *strDefaultValue;		//!< Default (initial) value
		int			flags;					//!< CVar flags
		int			iValue;					//!< Integer representation of the value
		float		fValue;					//!< Float representation of the value
		float		fMinValue;				//!< Float minimum (CVAR_FLOAT)
		float		fMaxValue;				//!< Float maximum (CVAR_FLOAT)
		int			iMinValue;				//!< Integer minimum (CVAR_INT)
		int			iMaxValue;				//!< Integer maximum (CVAR_INT)
		const char **enumValues;			//!< List of possible values (CVAR_ENUM)
		const ConArgComplete *completion;	//!< Completion callback class
		cvarCallback_t callback;			//!< Modification callback

		// ==============================================================================
		//! @copydoc	CVar::SetString
		// ==============================================================================
		virtual void		SetString( const char *value ) {}

		// ==============================================================================
		//! @copydoc	CVar::SetBool
		// ==============================================================================
		virtual void		SetBool( bool value ) {}

		// ==============================================================================
		//! @copydoc	CVar::SetInt
		// ==============================================================================
		virtual void		SetInt( int value ) {}

		// ==============================================================================
		//! @copydoc	CVar::SetFloat
		// ==============================================================================
		virtual void		SetFloat( float value ) {}
	};

	// ==============================================================================
	//! Console Variable
	//!
	//! A versatile, user changeable variable for easy use in game
	// ==============================================================================
	class CVar {
	public:
	// Static Interface
	//! @addtogroup CVarSystem
	//! @{

		// ==============================================================================
		//! Connect all CVars to the CVarSystem
		//!
		//! This adds CVars to the CVarSystem's list, so they can be used.\n
		//! Should be called only once on startup.
		//!
		//! @see	UnlinkCVars
		// ==============================================================================
		static void	LinkCVars( void );

		// ==============================================================================
		//! Disconnect all CVars from the CVarSystem
		//!
		//! This removes CVars to the CVarSystem's list.\n
		//! Should be called only once on shutdown.
		//!
		//! @see	LinkCVars
		// ==============================================================================
		static void	UnlinkCVars( void );
	//! @}

	private:
		// ==============================================================================
		//! Private Constructor for internal use
		//!
		//! @param	data	CVarData object
		// ==============================================================================
		CVar( CVarData *data ) { this->data = data; }

		// ==============================================================================
		//! Initialize this CVar
		//!
		//! @param	name		CVar name
		//! @param	defValue	The default value
		//! @param	flags		The flags
		//! @param	desc		The description
		//! @param	completion	The completion callback class
		//! @param	min			The lower limit for numeric types
		//! @param	max			The upper limit for numeric types
		//! @param	enumValues	The enum values (CVAR_ENUM)
		//!
		//! @see	cvarFlags_t
		// ==============================================================================
		void Init( const char *name, const char *defValue, int flags, const char *desc, const ConArgComplete *completion, float min, float max, const char **enumValues );

	public:

		// ==============================================================================
		//! Constructor for the string and boolean type
		//!
		//! @see	Init
		// ==============================================================================
		CVar( const char *name, const char *defValue, int flags, const char *desc, const ConArgComplete *completion=NULL );

		// ==============================================================================
		//! Constructor for numeric types
		//!
		//! @see	Init
		// ==============================================================================
		CVar( const char *name, const char *defValue, int flags, const char *desc, float min, float max );

		// ==============================================================================
		//! Constructor for the enum type
		//!
		//! @see	Init
		// ==============================================================================
		CVar( const char *name, const char *defValue, int flags, const char *desc, const char **enumValues );

		// ==============================================================================
		//! Resets to default value
		// ==============================================================================
		void		ResetToDefault( void );

		// ==============================================================================
		//! Check if the CVar has been modified
		//!
		//! @return	true if modified, false if not
		// ==============================================================================
		bool		IsModified( void ) const;

		// ==============================================================================
		//! Set the modified flag
		//!
		//! @param	modified	The new modified state
		// ==============================================================================
		void		SetModified( bool modified=true );

		// ==============================================================================
		//! Get the CVar flags
		//!
		//! @return	The CVar flags
		// ==============================================================================
		int			GetFlags( void );

		// ==============================================================================
		//! Set the CVar flags
		//!
		//! @param	flags	The new flags
		// ==============================================================================
		void		SetFlags( int flags );

		// ==============================================================================
		//! Set the CVar value via an input string
		//!
		//! @param	value	The new value
		//!
		//! All types will be set to the best matching conversion of the string
		// ==============================================================================
		void		SetString( const char *value );

		// ==============================================================================
		//! Get the string value
		//!
		//! @return	String representation of the value
		// ==============================================================================
		const char *GetString( void ) const;

		// ==============================================================================
		//! Set the CVar value via an input boolean
		//!
		//! @param	value	The new value
		//!
		//! All types will be set to the best matching conversion of the boolean
		// ==============================================================================
		void		SetBool( bool value );

		// ==============================================================================
		//! Get the boolean value
		//!
		//! @return	Boolean representation of the value
		// ==============================================================================
		bool		GetBool( void ) const;

		// ==============================================================================
		//! Set the CVar value via an input integer
		//!
		//! @param	value	The new value
		//!
		//! All types will be set to the best matching conversion of the integer
		// ==============================================================================
		void		SetInt( int value );

		// ==============================================================================
		//! Get the integer value
		//!
		//! @return	Integer representation of the value
		// ==============================================================================
		int			GetInt( void ) const;

		// ==============================================================================
		//! Set the CVar value via an input float
		//!
		//! @param	value	The new value
		//!
		//! All types will be set to the best matching conversion of the float
		// ==============================================================================
		void		SetFloat( float value );

		// ==============================================================================
		//! Get the float value
		//!
		//! @return	Float representation of the value
		// ==============================================================================
		float		GetFloat( void ) const;

		// ==============================================================================
		//! Set the modified callback
		//!
		//! @param	callback	The callback function, NULL to disable
		//!
		//! The supplied function will be called whenever the CVar gets changed
		// ==============================================================================
		void		SetModifiedCallback( cvarCallback_t callback );

	private:
		friend class CVarSystemEx;
		friend class CmdSystemEx;
		CVarData *	data;			//!< The data object
		CVar *		nextLink;		//!< The next CVar to be linked

		static bool	linkedToEngine;	//!< State of the linking process
		static CVar *firstLink;		//!< First CVar to be linked
	};

	// ==============================================================================
	//! CVarSystem Interface
	//!
	//! Access to all CVars
	// ==============================================================================
	class CVarSystem {
	public:

		// ==============================================================================
		//! Reset flagged CVars to their default values
		//!
		//! @param	flags	The flags to search for
		// ==============================================================================
		virtual void		ResetToDefault( int flags ) = 0;

		// ==============================================================================
		//! Check if one of the flagged CVars has been modified
		//!
		//! @param	flags	The flags to search for
		//!
		//! @return	true if one or more have been modified, false if not
		// ==============================================================================
		virtual bool		IsModified( int flags ) = 0;

		// ==============================================================================
		//! Set all flagged CVars' modified state
		//!
		//! @param	flags		The flags to search for
		//! @param	modified	The new modified state
		// ==============================================================================
		virtual void		SetModified( int flags, bool modified=true ) = 0;

		// ==============================================================================
		//! Find the CVar with the specified name
		//!
		//! @param	key	Name of the CVar
		//!
		//! @return	NULL if not found, otherwise a pointer to the CVar object
		// ==============================================================================
		virtual CVar *	Find( const char *key ) const = 0;

		// ==============================================================================
		//! Set the specified CVar value via an input string
		//!
		//! @param	key		Name of the CVar
		//! @param	value	The new value
		//! @param	force	Overwrite even read only CVars
		//!
		//! All types will be set to the best matching conversion of the string
		// ==============================================================================
		virtual void		SetString( const char *key, const char *value, bool force=false ) = 0;

		// ==============================================================================
		//! Get a CVar string
		//!
		//! @param	key		Name of the CVar
		//!
		//! @return	String representation of the value
		// ==============================================================================
		virtual const char *GetString( const char *key ) const = 0;

		// ==============================================================================
		//! Set the specified CVar value via an input boolean
		//!
		//! @param	key		Name of the CVar
		//! @param	value	The new value
		//! @param	force	Overwrite even read only CVars
		//!
		//! All types will be set to the best matching conversion of the boolean
		// ==============================================================================
		virtual void		SetBool( const char *key, bool value, bool force=false ) = 0;

		// ==============================================================================
		//! Get a CVar bool
		//!
		//! @param	key		Name of the CVar
		//!
		//! @return	Boolean representation of the value
		// ==============================================================================
		virtual bool		GetBool( const char *key ) const = 0;

		// ==============================================================================
		//! Set the specified CVar value via an input integer
		//!
		//! @param	key		Name of the CVar
		//! @param	value	The new value
		//! @param	force	Overwrite even read only CVars
		//!
		//! All types will be set to the best matching conversion of the integer
		// ==============================================================================
		virtual void		SetInt( const char *key, int value, bool force=false ) = 0;

		// ==============================================================================
		//! Get a CVar integer
		//!
		//! @param	key		Name of the CVar
		//!
		//! @return	Integer representation of the value
		// ==============================================================================
		virtual int			GetInt( const char *key ) const = 0;

		// ==============================================================================
		//! Set the specified CVar value via an input float
		//!
		//! @param	key		Name of the CVar
		//! @param	value	The new value
		//! @param	force	Overwrite even read only CVars
		//!
		//! All types will be set to the best matching conversion of the float
		// ==============================================================================
		// Float
		virtual void		SetFloat( const char *key, float value, bool force=false ) = 0;

		// ==============================================================================
		//! Get a CVar float
		//!
		//! @param	key		Name of the CVar
		//!
		//! @return	Float representation of the value
		// ==============================================================================
		virtual float		GetFloat( const char *key ) const = 0;

		// ==============================================================================
		//! Export flagged CVars to dict
		//!
		//! @param	flags	The flags to search for
		//! @param	dict	Pointer to the dictionary
		// ==============================================================================
		virtual void		ToDict( int flags, Dict *dict ) const = 0;

		// ==============================================================================
		//! Import CVars from dict
		//!
		//! @param	dict	The dictionary
		// ==============================================================================
		virtual void		FromDict( const Dict *dict ) = 0;

		// ==============================================================================
		//! Export flagged CVars to HTML
		//!
		//! @param	filename	The (relative) file path
		//! @param	flags		The flags to search for
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		virtual bool		ExportToHTML( const char *filename="cvars.html", int flags=CVAR_ALL ) const = 0;

	protected:
		friend class CVar;

		// ==============================================================================
		//! Link a CVar
		//!
		//! @param	cvar	The CVar object
		// ==============================================================================
		virtual void		LinkCVar( CVar *cvar ) = 0;

		// ==============================================================================
		//! Unlink a CVar
		//!
		//! @param	cvar	The CVar object
		// ==============================================================================
		virtual void		UnlinkCVar( CVar *cvar ) = 0;
	};
	// ==============================================================================
	//! CVarSystem interface pointer (can be passed to the game module)
	//!
	//! @note	This pointer is automatically available in the executable linking to this library,\n
	//!			but needs to be created (and assigned the value of the original) for other modules that need access to it.
	// ==============================================================================
	extern CVarSystem *cvarSystem;

//! @}
//! @}
}

#endif
