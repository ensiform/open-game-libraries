// ==============================================================================
//! @file
//! @brief	Command System Public Interface (shared with game module)
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

#ifndef __OG_CMDSYSTEM_H__
#define __OG_CMDSYSTEM_H__

//! Open Game Libraries
namespace og {
	//! @addtogroup Console
	//! @{
	//! @defgroup CmdSystem CmdSystem (Library)
	//! @see	Console
	//! @{

	//! @todo	which ones are needed ?
	typedef enum {
		CMD_ALL			= -1,

		// Commands that can only be called under Certain conditions
		CMD_INIT		= BIT(0),	//!< Must be called from the commandline
		CMD_CHEAT		= BIT(1),	//!< Cheats must be enabled to call this
		CMD_DEVELOPER	= BIT(2),	//!< Must be in developer mode to see and call this command

		//Cmd relations
		CMD_ENGINE		= BIT(3),	//!< Related to the engine
		CMD_RENDERER	= BIT(4), 	//!< Related to the renderer
		CMD_SOUND		= BIT(5),	//!< Related to the sound system
		CMD_INPUT		= BIT(6),	//!< Related to the input system
		CMD_NETWORK		= BIT(7),	//!< Related to the network settings
		CMD_GAME		= BIT(8),	//!< Related to the game module

		CMD_CUSTOM_BIT	= 9			//!< The first free bit for custom Cmd flags.\n
									//!  Keep in mind, there are only 32 bits available ( BIT(0)-BIT(31) )
	} cmdFlags_t;

	// ==============================================================================
	//! Command function
	//!
	//! @param	args	The command line arguments passed
	// ==============================================================================
	typedef void ( *cmdFunc_t )( const CmdArgs &args );

	// ==============================================================================
	//! Command completion callback
	//!
	//! @param	str	The completed command
	// ==============================================================================
	typedef void( *argCompletionCB_t )( const char *str );

	// ==============================================================================
	//! Usage Function
	// ==============================================================================
	typedef void ( *cmdUsage_t )( void );

	// ==============================================================================
	//! Console command usage base class
	//!
	//! This baby is the base class for showing usage in console commands.\n
	//! In this form though, it only contains a description for the help command.\n
	//! Use it if your command does not need arguments.
	//! 
	//! @see	ConUsageString, ConUsageFunc
	// ==============================================================================
	class ConUsage {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	description	The description printed by the Help command.
		//!						This is \b not the usage string, but rather
		//!						a short description of what the function does.
		//! @param	minArgs		The minimum number of arguments to display usage
		// ==============================================================================
		ConUsage( const char *description, int minArgs=0 )
			: strDescription(description), minArguments(minArgs) {}

		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~ConUsage() {}

		// ==============================================================================
		//! Shows the usage string
		//!
		//! Implement this if you want to show usage text
		// ==============================================================================
		virtual void ShowUsage( void ) const {};

		friend class CmdSystemEx;
	protected:
		int			minArguments;		//!< The minimum number of arguments to display usage
		String		strDescription;		//!< The description printed by the Help command
	};

	// ==============================================================================
	//! Console command usage string printer
	//!
	//! This one prints a simple string if the number of arguments did not match.
	// ==============================================================================
	class ConUsageString : public ConUsage {
	public:

		// ==============================================================================
		//! @copydoc ConUsage::ConUsage
		//! @param	usage	The string to display for usage
		// ==============================================================================
		ConUsageString( const char *description, int minArgs, const char *usage )
			: ConUsage( description, minArgs ), strUsage(usage) {}

		void ShowUsage( void ) const;

	private:
		String strUsage;	//!< The string to display for usage
	};

	// ==============================================================================
	//! Console command usage function call
	//!
	//! Calls a function instead of printing a string
	// ==============================================================================
	class ConUsageFunc : public ConUsage {
	public:
		// ==============================================================================
		//! @copydoc ConUsage::ConUsage
		//! @param	func	The function to call for usage
		// ==============================================================================
		ConUsageFunc( const char *description, int minArgs, cmdUsage_t func )
			: ConUsage( description, minArgs ), usageFunc(func) {}

		void ShowUsage( void ) const;

	private:
		cmdUsage_t usageFunc;	//!< The function to call for usage
	};

	// ==============================================================================
	//! Console command argument completion
	//!
	//! Base class, useless in this form
	// ==============================================================================
	class ConArgComplete {
	public:
		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~ConArgComplete() {}

		// ==============================================================================
		//! Completes the command argument
		//!
		//! @param	args		The currently typed arguments
		//! @param	callback	Call it to display the different options
		// ==============================================================================
		virtual void Complete( const CmdArgs &args, argCompletionCB_t callback ) const = 0;
	};

	// ==============================================================================
	//! Console command argument completion for booleans
	// ==============================================================================
	class ConArgCompleteBoolean : public ConArgComplete {
	public:
		ConArgCompleteBoolean() {}
		void Complete( const CmdArgs &args, argCompletionCB_t callback ) const;
	};

	// ==============================================================================
	//! Console command argument completion for integers
	// ==============================================================================
	class ConArgCompleteInteger : public ConArgComplete {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	iMin	The lower limit
		//! @param	iMax	The upper limit
		// ==============================================================================
		ConArgCompleteInteger( int iMin, int iMax ) : min(iMin), max(iMax) {}

		void Complete( const CmdArgs &args, argCompletionCB_t callback ) const;

	private:
		int		min;	//!< The lower limit
		int		max;	//!< The upper limit
	};

	// ==============================================================================
	//! Console command argument completion for strings
	// ==============================================================================
	class ConArgCompleteString : public ConArgComplete {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	strings	The available strings to choose from
		// ==============================================================================
		ConArgCompleteString( const char **strings );
		void Complete( const CmdArgs &args, argCompletionCB_t callback ) const;

	private:
		StringList list;	//!< The available strings to choose from
	};

	// ==============================================================================
	//! Console command argument completion for strings
	//!
	//! @todo	Might want to be able to specify multiple extensions here?
	// ==============================================================================
	class ConArgCompleteFile : public ConArgComplete {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	folder	The directory to search in
		//! @param	ext		The file extension to look for
		//! @param	flags	The flags to use.
		//!
		//! @see	ListFlags, FileSystem::GetFileList
		// ==============================================================================
		ConArgCompleteFile( const char *folder, const char *ext, int flags=LF_DEFAULT )
			: dir(folder), extension(ext), findFlags(flags) {}

		void Complete( const CmdArgs &args, argCompletionCB_t callback ) const;

	private:
		String	dir;		//!< The directory to search in
		String	extension;	//!< The file extension to look for
		int		findFlags;	//!< The flags to use.
	};

	// ==============================================================================
	//! CmdSystem interface
	//!
	//! Global command access
	// ==============================================================================
	class CmdSystem {
	public:

		// ==============================================================================
		//! Register a console command
		//!
		//! @param	cmd			The command name
		//! @param	func		The function to call
		//! @param	flags		The command flags
		//! @param	usage		Pointer to the usage object, must not be NULL
		//! @param	completion	Pointer to the completion object, can be NULL
		//!
		//! @see	AddCmd
		// ==============================================================================
		virtual void	AddCmd( const char *cmd, cmdFunc_t func, int flags, const ConUsage *usage, const ConArgComplete *completion = NULL ) = 0;

		// ==============================================================================
		//! Unregister a command previously registered by AddCmd
		//!
		//! @param	cmd	The command name
		//!
		//! @see	AddCmd
		// ==============================================================================
		virtual void	RemoveCmd( const char *cmd ) = 0;

		// ==============================================================================
		//! Unregister commands by flags
		//!
		//! @param	flags	The flags to look for
		// ==============================================================================
		virtual void	RemoveCmdsByFlags( int flags ) = 0;

		// ==============================================================================
		//! Execute the command
		//!
		//! @param	cmd		The command to execute
		//! @param	force	true to execute immediately, otherwise it will be added to the command queue
		// ==============================================================================
		virtual void	ExecuteCmd( const char *cmd, bool force=false ) = 0;

		// ==============================================================================
		//! Load and execute the specified config file
		//!
		//! @param	filename	The (relative) file path
		// ==============================================================================
		virtual void	ExecuteConfig( const char *filename ) = 0;

		// ==============================================================================
		//! Tries to complete a command and its arguments
		//!
		//! @param	buf			The already typed in characters
		//! @param	callback	The callback to show the values
		//!
		//! @return	All matching characters, or if no match was found, \a buf
		//!
		//! @see ConArgComplete::Complete
		// ==============================================================================
		virtual const char *CompleteCmd( const char *buf, argCompletionCB_t callback ) = 0;

		// ==============================================================================
		//! Print usage for a command
		//!
		//! @param	buf	The command name (if arguments exist, they will be ignored)
		// ==============================================================================
		virtual void	PrintUsage( const char *buf ) const = 0;

		// ==============================================================================
		//! Export flagged Commands to HTML
		//!
		//! @param	filename	The (relative) file path
		//! @param	flags		The flags to search for
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		virtual bool	ExportToHTML( const char *filename="cmds.html", int flags=CMD_ALL ) const = 0;
	};
	// ==============================================================================
	//! CmdSystem interface pointer (can be passed to the game module)
	//!
	//! @note	This pointer is automatically available in the executable linking to this library,\n
	//!			but needs to be created (and assigned the value of the original) for other module that need access to it.
	// ==============================================================================
	extern CmdSystem *cmdSystem;

//! @}
//! @}
}

#endif
