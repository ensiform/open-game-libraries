/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Command System Interface (engine part)
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

#include <og/Console.h>

#ifndef __OG_CMDSYSTEM_EX_H__
#define __OG_CMDSYSTEM_EX_H__

namespace og {

/*
==============================================================================

  ConsoleCmd

==============================================================================
*/
class ConsoleCmd {
public:
	cmdFunc_t		func;
	int				flags;
	const ConUsage *usage;
	const ConArgComplete *completion;
};

/*
==============================================================================

  CmdSystemEx

==============================================================================
*/
class CmdSystemEx : public CmdSystem {
public:
	// ---------------------- Public CmdSystem Interface -------------------

	void	AddCmd( const char *cmd, cmdFunc_t func, int flags, const ConUsage *usage, const ConArgComplete *completion = NULL );
	void	RemoveCmd( const char *cmd );
	void	RemoveCmdsByFlags( int flags );

	void	ExecuteCmd( const char *cmd, bool force=false );
	void	ExecuteConfig( const char *filename );

	const char *CompleteCmd( const char *buf, argCompletionCB_t callback );
	void	PrintUsage( const char *buf ) const;

	bool	ExportToHTML( const char *filename="cmds.html", int flags=CMD_ALL ) const;

	// ---------------------- Internal CmdSystemEx Members -------------------

public:
	CmdSystemEx();

	static void Cmd_ListCmds_f( const CmdArgs &args );
	static void Cmd_ExportCmds_f( const CmdArgs &args );
	static void Cmd_Help_f( const CmdArgs &args );
	static void Cmd_Wait_f( const CmdArgs &args );
	static void Cmd_Exec_f( const CmdArgs &args );
	static void Cmd_Echo_f( const CmdArgs &args );
	static void Cmd_Quit_f( const CmdArgs &args );

	const DictEx<ConsoleCmd> & GetCommandList(void) const;

	void	ExecuteCmdQueue( void );
	void	ClearCmdQueue( void ) { cmdQueue.Clear(); }

	void	InitConsoleCmds( int minWaitTime );
	void	ShutdownConsoleCmds( void );

private:
	friend class CmdSystem;

	void	ExecuteSingleCmd( const char *cmd );

	static void	CompletionCallback( const char *str );
	StringList			completionList; // used to store values received with CompletionCallback
	String				completionBuffer; // needed to compare incomming callback values so we don't add values that are incorrect.

	DictEx<ConsoleCmd>	cmdList;
	int					waitTime;
	int					minWaitTime;

	Queue<String>		cmdQueue;
};

extern CmdSystemEx *cmdSystemEx;

}

#endif
