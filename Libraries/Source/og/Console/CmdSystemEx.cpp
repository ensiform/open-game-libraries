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
#include "CmdSystemEx.h"
#include "CvarSystemEx.h"

extern const char *cmdFlagNames[];//! @todo	this will not work
namespace og {

extern CVar com_developer;
extern CVar net_allowCheats;

bool inEngineStartup = true; //! @todo	do this differently

// The object and global pointer
static CmdSystemEx CmdSystemObject;
CmdSystem *cmdSystem = &CmdSystemObject;
CmdSystemEx *cmdSystemEx = &CmdSystemObject;

/*
==============================================================================

  CmdSystemEx

==============================================================================
*/

/*
================
CmdSystemEx::CmdSystemEx
================
*/
CmdSystemEx::CmdSystemEx() {
	waitTime	= 0;
	minWaitTime	= 0;
}

/*
================
CmdSystemEx::AddCmd
================
*/
void CmdSystemEx::AddCmd( const char *cmd, cmdFunc_t func, int flags, const ConUsage *usage, const ConArgComplete *completion ) {
	if ( cmd == OG_NULL || cmd[0] == '\0' || usage == OG_NULL ) {
		User::Warning("AddCmd called with bad arguments");
		return;
	}

	int index = cmdList.Find( cmd );
	if ( index != -1 ) {
		User::Warning( Format( "Trying to add command '$*', which already exists!" ) << cmd );
		return;
	}
	ConsoleCmd &newCmd = cmdList[cmd];
	newCmd.func = func;
	newCmd.flags = flags;
	newCmd.usage = usage;
	newCmd.completion = completion;
}

/*
================
CmdSystemEx::RemoveCmd
================
*/
void CmdSystemEx::RemoveCmd( const char *cmd ) {
	cmdList.Remove( cmd );
}

/*
================
CmdSystemEx::RemoveCmdsByFlags
================
*/
void CmdSystemEx::RemoveCmdsByFlags( int flags ) {
	int num = cmdList.Num();
	for( int i=0; i<num; i++ ) {
		if ( cmdList[i].flags & flags ) {
			cmdList.Remove(i);
			num--;
			i--;
		}
	}
}

/*
================
CmdSystemEx::ExecuteCmd
================
*/
void CmdSystemEx::ExecuteCmd( const char *buf, bool force ) {
	if( !buf[0] )
		return;

	char ch[2] = {'\0', '\0'};
	StringList list;
	String temp;
	bool inquote = false;
	int i;
	for ( i=0; buf[i] != '\0'; i++ ) {
		if ( buf[i] == '\n' || (!inquote && buf[i] == ';') ) {
			if ( temp.IsEmpty() )
				continue;
			else if ( inquote ) {
				temp += "\"";
				inquote = false;
			}
			list.Append(temp);
			temp.Clear();
			continue;
		}
		if ( buf[i] == '\"' )
			inquote = !inquote;
		ch[0] = buf[i];
		temp += ch;
	}
	if ( !temp.IsEmpty() ) {
		if ( inquote ) {
			temp += "\"";
			inquote = false;
		}
		list.Append(temp);
	}

	if ( force ) {
		int num = list.Num();
		for ( i=0; i<num; i++ )
			ExecuteSingleCmd( list[i].c_str() );
	} else {
		int num = list.Num();
		for ( i=0; i<num; i++ )
			cmdQueue.Push( list[i] );
	}
}

/*
================
CmdSystemEx::ExecuteSingleCmd
================
*/
void CmdSystemEx::ExecuteSingleCmd( const char *cmd ) {
	CmdArgs args(cmd);
	if ( !args.Argc() )
		return;

	const char *arg0 = args.Argv(0);
	int index = cmdList.Find( arg0 );
	if ( index != -1 ) {
		ConsoleCmd &cCmd = cmdList[index];
		if( cCmd.flags & CMD_DEVELOPER && !com_developer.GetBool() ) {
			User::Warning( "Developer mode is not active!" );
			return;
		}
		if( cCmd.flags & CMD_CHEAT && !net_allowCheats.GetBool() ) {
			User::Warning( Format( "$* is cheat protected" ) << cmdList.GetKey(index) );
			return;
		}
		if ( cCmd.usage->minArguments > (args.Argc()-1) )
			cCmd.usage->ShowUsage();
		else
			cCmd.func(args);
		return;
	}
	if ( !cvarSystemEx->OnCommand(args) )
		User::Warning( Format( "Unknown command '$*'.\n" ) << arg0 );
}

/*
================
CmdSystemEx::ExecuteCmdQueue
================
*/
void CmdSystemEx::ExecuteCmdQueue( void ) {
	if ( waitTime <= SysInfo::GetTime() ) {
		while( cmdQueue.Num() ) {
			ExecuteSingleCmd( cmdQueue.Front().c_str() );
			cmdQueue.Pop();
		}
	}
}

/*
================
CmdSystemEx::ExecuteConfig
================
*/
void CmdSystemEx::ExecuteConfig( const char *filename ) {
	// was just full lines dunno why BOM error should be printed for plain text configs
	Lexer lexer(LEXER_FULL_LINES|LEXER_NO_BOM_WARNING);
	if ( !lexer.LoadFile(filename) )
		return;

	try {
		const Token *token;
		const char *p;
		while ( (token = lexer.ReadToken()) != OG_NULL ) {
			p = token->GetString();
			if ( p && *p )
				ExecuteCmd( p, inEngineStartup );
		}
	}
	catch( LexerError &err ) {
		String errStr;
		err.ToString( errStr );
		User::Error( ERR_LEXER_FAILURE, errStr.c_str(), filename );
	}
}

/*
================
CmdSystemEx::CompletionCallback

Called by argument completion
================
*/
void CmdSystemEx::CompletionCallback( const char *str ) {
	// check if all of the buffers chars match (str can be longer than the buffer)
	if ( CmdSystemObject.completionBuffer.Icmpn(str, CmdSystemObject.completionBuffer.Length()) == 0 )
		CmdSystemObject.completionList.Append(str);
}

/*
================
CmdSystemEx::CompleteCmd
================
*/
const char *CmdSystemEx::CompleteCmd( const char *buf, argCompletionCB_t callback ) {
	static String returnBuffer;
	if ( !buf )
		return "";

	completionBuffer = buf;
	completionBuffer.StripLeadingWhitespaces();

	if ( completionBuffer.IsEmpty() )
		return "";

	int bufLen = completionBuffer.Length();
	completionList.Clear();

	int i;
	if( completionBuffer.Find(" ") != String::INVALID_POSITION ) {
		CmdArgs args(buf);

		OG_ASSERT( args.Argc() > 0 );

		// Find the command and call its completion func
		const char *arg0 = args.Argv(0);
		i = cmdList.Find( arg0 );
		if ( i != -1 ) {
			if ( cmdList[i].completion )
				cmdList[i].completion->Complete(args, CompletionCallback);
		}
		else
			cvarSystemEx->CompleteCVar( args, CompletionCallback );
	} else {
		// Find all commands that start with buf
		for( i = cmdList.MatchPrefix( buf, bufLen ); i != -1; i = cmdList.MatchPrefix( buf, bufLen, i ) )
			completionList.Append( cmdList.GetKey(i) );

		// Find all cvars that start with buf
		cvarSystemEx->CompleteCVar(buf, bufLen, completionList);
	}
	if ( completionList.IsEmpty() )
		return completionBuffer.c_str();

	int numEntries = completionList.Num();

	// Only one option, so return it straight
	if ( numEntries == 1 )
		return completionList[0].c_str();

	// Convert to lowercase for faster comparison, then strip the input and calculate the shortest length
	int shortest = 1023;
	String lowerBuf = completionBuffer;
	lowerBuf.ToLower();
	StringList newList;
	for( i=0; i<numEntries; i++ ) {
		newList.Append( completionList[i] );
		newList[i].ToLower();
		newList[i].StripLeadingOnce(lowerBuf.c_str());
		if ( newList[i].Length() < shortest )
			shortest = newList[i].Length();
	}

	returnBuffer.Clear();
	if ( shortest == 0 )
		completionList[0].Left( completionBuffer.Length(), returnBuffer );
	else {
		// Find the first not matching char
		int pos;
		const String &firstStr = newList[0];
		for( pos=0; pos<shortest; pos++ ) {
			// Check if all chars at this position match
			for( i=1; i<numEntries; i++ ) {
				if ( firstStr.Cmpn( newList[i].c_str(), pos ) != 0 )
					break;
			}
			// One did not match
			if ( i < numEntries )
				break;
		}
		completionList[0].Left( completionBuffer.Length()+pos-1, returnBuffer );
	}

	// Send the list back and print it in the console
	Console::Print( Format( "]$*\n" ) << returnBuffer );
	Format complete( "    $*\n" );
	for( i=0; i<numEntries; i++ ) {
		if ( callback )
			callback( completionList[i].c_str() );
		Console::Print( complete << completionList[i] );
		complete.Reset();
	}
	return returnBuffer.c_str();
}

/*
================
CmdSystemEx::PrintUsage
================
*/
void CmdSystemEx::PrintUsage( const char *buf ) const {
	if ( !buf )
		return;

	String command = buf;
	command.StripLeadingWhitespaces();

	if ( command.IsEmpty() )
		return;

	CmdArgs args(buf);

	OG_ASSERT( args.Argc() > 0 );

	// Find the command and call its usage func
	int i = cmdList.Find( args.Argv(0) );
	if ( i != -1 )
		cmdList[i].usage->ShowUsage();
}

/*
================
CmdSystemEx::ExportToHTML
================
*/
bool CmdSystemEx::ExportToHTML( const char *filename, int flags ) const {
	if ( FS == OG_NULL )
		return false;

	byte *buffer = OG_NULL;
	FS->LoadFile("exportTemplate.html", &buffer);
	if ( !buffer ) 
		return false;

	String templateBuffer = reinterpret_cast<char *>(buffer);
	FS->FreeFile(buffer);

	File *file = FS->OpenWrite(filename);
	if ( !file )
		return false;

	String table = "<table width=\"900\">\r\n<tr><th width=\"30%\" style=\"text-align:left\"><b>Name</b></th><th style=\"text-align:left\"><b>Description</b></th></tr>\r\n";

	Format out("<tr><td>$*</td><td>$*</td></tr>\r\n");
	int num = cmdList.Num();
	for ( int i = 0; i<num; i++ ) {
		if ( cmdList[i].flags & flags ) {
			table += out << cmdList.GetKey(i) << cmdList[i].usage->strDescription;
			out.Reset();
		}
	}
	table += "</table>\r\n";
	String result;
	result.FromBitFlags( flags, cmdFlagNames );
	templateBuffer.Replace("{{TITLE}}", Format( "Exported Commands with flags ($*)" ) << result );
	templateBuffer.Replace("{{TABLE}}", table.c_str());

	file->Write( templateBuffer.c_str(), templateBuffer.ByteLength() );
	file->Close();
	return true;
}

/*
================
CmdSystemEx::GetCommandList
================
*/
const DictEx<ConsoleCmd> &CmdSystemEx::GetCommandList(void) const {
	return cmdList;
}

/*
================
CmdSystemEx::Cmd_ListCmds_f
================
*/
ConUsageString Cmd_ListCmds_Usage("Lists all commands currently available.", 0, "ListCmds [searchstring]");
void CmdSystemEx::Cmd_ListCmds_f( const CmdArgs &args ) {
	DictEx<ConsoleCmd> & cmdList = CmdSystemObject.cmdList;
	bool match = false;
	const char *matchstr = 0;

	if( args.Argc() > 1 ) {
		match = true;
		matchstr = args.Argv(1);
	}

	int num = cmdList.Num();
	int i = 0;
	int disp = 0;
	Format out("  $+18* - $*\n");
	for( i=0; i<num; i++ ) {
		if ( match && String::Find(cmdList.GetKey(i).c_str(), matchstr, false) == String::INVALID_POSITION )
			continue;
		out << cmdList.GetKey(i) << cmdList[i].usage->strDescription;
		Console::Print( out );
		out.Reset();
		disp++;
	}

	if( match )
		Console::Print( Format( "\n  $* command$* matching \"$*\"\n" ) << disp << (disp == 1 ? "" : "s") << matchstr );
	else
		Console::Print( Format( "\n  $* command$*\n" ) << disp << (disp == 1 ? "" : "s") );
}

/*
================
CmdSystemEx::Cmd_ExportCmds_f
================
*/
ConUsageString Cmd_ExportCmds_Usage("Export all commands to an HTML file.", 0, "ExportCmds");
void CmdSystemEx::Cmd_ExportCmds_f( const CmdArgs &args ) {
	Console::Print( "Exporting all commands to \"cmds.html\"..\n" );
	if ( cmdSystem->ExportToHTML() )
		Console::Print( "Done!\n" );
	else
		Console::Print( "Failed!\n" );
}

/*
================
CmdSystemEx::Cmd_Help_f
================
*/
ConUsageString Cmd_Help_Usage("Show help about a command or cvar.", 1, "Help <command/cvar>");
void CmdSystemEx::Cmd_Help_f( const CmdArgs &args ) {
	if ( args.Argc() > 2 ) {
		Cmd_Help_Usage.ShowUsage();
		return;
	}

	const char *cmd = args.Argv(1);
	// Try Command List
	int i = CmdSystemObject.cmdList.Find(cmd);
	if ( i != -1 ) {
		Console::Print( Format( "$*: $*\n" ) << CmdSystemObject.cmdList.GetKey(i) << CmdSystemObject.cmdList[i].usage->strDescription );
		CmdSystemObject.cmdList[i].usage->ShowUsage();
		return;
	}
	// Not a Command, is it a CVar?
	CVar *cv = cvarSystem->Find(cmd);
	if ( cv ) {
		Console::Print( Format( "$*: $*\n" ) << cv->data->strName << cv->data->strDescription );
		return;
	}
	Console::Print( Format( "Command/Cvar not found: '$*'.\n" ) << cmd );
}

/*
================
Cmd_Wait_f
================
*/
ConUsageString Cmd_Wait_Usage("Wait n milliseconds before executing the next command.", 0, "Echo [ms]");
void CmdSystemEx::Cmd_Wait_f( const CmdArgs &args ) {
	int ms = CmdSystemObject.minWaitTime;
	if ( args.Argc() > 1 ) {
		ms = String::ToInt(args.Argv(1));
		if ( ms < CmdSystemObject.minWaitTime )
			ms = CmdSystemObject.minWaitTime;
	}
	CmdSystemObject.waitTime = SysInfo::GetTime() + ms;
}

/*
================
Cmd_Exec_f
================
*/
ConUsageString Cmd_Exec_Usage("Execute a config.", 1, "Exec <filename[.cfg]>");
ConArgCompleteFile Cmd_Exec_Completion( "", ".cfg" );
void CmdSystemEx::Cmd_Exec_f( const CmdArgs &args ) {
	String name = args.Argv(1);
	name.DefaultFileExtension(".cfg");
	Console::Print( Format( "Executing '$*'\n" ) << name );
	cmdSystem->ExecuteConfig( name.c_str() );
}

/*
================
Cmd_Echo_f
================
*/
ConUsageString Cmd_Echo_Usage("Print text to console.", 1, "Echo <text>");
void CmdSystemEx::Cmd_Echo_f( const CmdArgs &args ) {
	String result;
	args.Args( result );
	Console::Print( Format( "$*\n" ) << result );
}

/*
================
Cmd_Quit_f
================
*/
ConUsage Cmd_Quit_Usage("Exit without questions.");
void CmdSystemEx::Cmd_Quit_f( const CmdArgs &args ) {
	User::OnForceExit();
}


class ConArgCompleteHelp : public ConArgComplete {
public:
	ConArgCompleteHelp() {}
	void Complete( const CmdArgs &args, argCompletionCB_t callback ) const {
		int num, i;
		StringList fullList;
		const DictEx<ConsoleCmd> &cmdList = CmdSystemObject.GetCommandList();
		num = cmdList.Num();
		for( i = 0; i < num; i++ )
			fullList.Append( cmdList.GetKey(i) );
		const DictEx<CVarDataEx> &cvarDataList = cvarSystemEx->GetCVarDataList();
		num = cvarDataList.Num();
		for( i = 0; i < num; i++ )
			fullList.Append( cvarDataList.GetKey(i) );

		fullList.Sort( StringListICmp, true );
		num = fullList.Num();
		Format complete( "$* $*" );
		for( i = 0; i < num; i++ ) {
			callback( complete << args.Argv( 0 ) << fullList[i] );
			complete.Reset();
		}
	}
};

class ConArgCompleteCVars : public ConArgComplete {
public:
	ConArgCompleteCVars() {}
	void Complete( const CmdArgs &args, argCompletionCB_t callback ) const {
		const DictEx<CVarDataEx> &cvarDataList = cvarSystemEx->GetCVarDataList();
		int num = cvarDataList.Num();
		Format complete( "$* $*" );
		for ( int i=0; i<num; i++ ) {
			callback( complete << args.Argv( 0 ) << cvarDataList[i].strName );
			complete.Reset();
		}
	}
};

ConArgCompleteHelp Cmd_Help_Completion;
ConArgCompleteCVars ArgComplete_CVars;

extern ConUsageString Cmd_ListCmds_Usage;
extern ConUsageString Cmd_ExportCmds_Usage;
extern ConUsageString Cmd_ListCVars_Usage;
extern ConUsageString Cmd_Toggle_Usage;
extern ConUsageString Cmd_Set_Usage;
extern ConUsageString Cmd_Inc_Usage;
extern ConUsageString Cmd_Dec_Usage;
extern ConUsageString Cmd_SetA_Usage;
extern ConUsageString Cmd_Reset_Usage;
extern ConUsageString Cmd_ExportCVars_Usage;
extern ConUsageString Cmd_Help_Usage;
extern ConUsageString Cmd_Wait_Usage;

/*
================
CmdSystemEx::InitConsoleCmds
================
*/
void CmdSystemEx::InitConsoleCmds( int minWaitTime ) {
	this->minWaitTime = minWaitTime;

	cmdSystem->AddCmd("ListCmds",		CmdSystemEx::Cmd_ListCmds_f,		CMD_ENGINE,		&Cmd_ListCmds_Usage );
	cmdSystem->AddCmd("ExportCmds",		CmdSystemEx::Cmd_ExportCmds_f,		CMD_ENGINE,		&Cmd_ExportCmds_Usage );
	cmdSystem->AddCmd("Help",			CmdSystemEx::Cmd_Help_f,			CMD_ENGINE,		&Cmd_Help_Usage, &Cmd_Help_Completion );
	cmdSystem->AddCmd("Wait",			CmdSystemEx::Cmd_Wait_f,			CMD_ENGINE,		&Cmd_Wait_Usage );
	cmdSystem->AddCmd("Exec",			CmdSystemEx::Cmd_Exec_f,			CMD_ENGINE,		&Cmd_Exec_Usage, &Cmd_Exec_Completion );
	cmdSystem->AddCmd("Echo",			CmdSystemEx::Cmd_Echo_f,			CMD_ENGINE,		&Cmd_Echo_Usage );
	cmdSystem->AddCmd("Quit",			CmdSystemEx::Cmd_Quit_f,			CMD_ENGINE,		&Cmd_Quit_Usage );

	cmdSystem->AddCmd("ListCVars",		CVarSystemEx::Cmd_ListCVars_f,		CMD_ENGINE,		&Cmd_ListCVars_Usage );
	cmdSystem->AddCmd("ExportCVars",	CVarSystemEx::Cmd_ExportCVars_f,	CMD_ENGINE,		&Cmd_ExportCVars_Usage );
	cmdSystem->AddCmd("Toggle",			CVarSystemEx::Cmd_Toggle_f,			CMD_ENGINE,		&Cmd_Toggle_Usage, &ArgComplete_CVars );

	cmdSystem->AddCmd("Set",			CVarSystemEx::Cmd_Set_f,			CMD_ENGINE,		&Cmd_Set_Usage );
	cmdSystem->AddCmd("SetA",			CVarSystemEx::Cmd_SetA_f,			CMD_ENGINE,		&Cmd_SetA_Usage );
	cmdSystem->AddCmd("Inc",			CVarSystemEx::Cmd_Inc_f,			CMD_ENGINE,		&Cmd_Inc_Usage );
	cmdSystem->AddCmd("Dec",			CVarSystemEx::Cmd_Dec_f,			CMD_ENGINE,		&Cmd_Dec_Usage );
	cmdSystem->AddCmd("Reset",			CVarSystemEx::Cmd_Reset_f,			CMD_ENGINE,		&Cmd_Reset_Usage, &ArgComplete_CVars );
}

/*
================
CmdSystemEx::ShutdownConsoleCmds
================
*/
void CmdSystemEx::ShutdownConsoleCmds( void ) {
	cmdSystem->RemoveCmdsByFlags(CMD_ENGINE);
}

}

