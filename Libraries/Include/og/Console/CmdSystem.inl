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

#ifndef __OG_CMDSYSTEM_INL__
#define __OG_CMDSYSTEM_INL__

namespace og {

/*
==============================================================================

  ConUsage

==============================================================================
*/

/*
================
ConUsageString::ShowUsage
================
*/
OG_INLINE void ConUsageString::ShowUsage( void ) const {
	//! @todo Since this .inl should be useable from other modules, this need to be done differently.
	Console::Print( Format( "Usage: $*\n" ) << strUsage );
}

/*
================
ConUsageFunc::ShowUsage
================
*/
OG_INLINE void ConUsageFunc::ShowUsage( void ) const {
	usageFunc();
}

/*
==============================================================================

  ConArgComplete

==============================================================================
*/

/*
================
ConArgCompleteBoolean::Complete
================
*/
OG_INLINE void ConArgCompleteBoolean::Complete( const CmdArgs &args, argCompletionCB_t callback ) const {
	callback( Format( "$* 0" ) << args.Argv( 0 ) );
	callback( Format( "$* 1" ) << args.Argv( 0 ) );
}

/*
================
ConArgCompleteInteger::Complete
================
*/
OG_INLINE void ConArgCompleteInteger::Complete( const CmdArgs &args, argCompletionCB_t callback ) const {
	Format param( "$* $*" );
	for ( int i = min; i <= max; i++ ) {
		callback( param << args.Argv( 0 ) << i );
		param.Reset();
	}
}

/*
================
ConArgCompleteString::ConArgCompleteString
================
*/
OG_INLINE ConArgCompleteString::ConArgCompleteString( const char **strings ) {
	while ( *strings ) {
		list.Append(*strings);
		strings++;
	}
}

/*
================
ConArgCompleteString::Complete
================
*/
OG_INLINE void ConArgCompleteString::Complete( const CmdArgs &args, argCompletionCB_t callback ) const {
	Format param( "$* $*" );
	int num = list.Num();
	for ( int i=0; i<num; i++ ) {
		callback( param << args.Argv( 0 ) << list[i] );
		param.Reset();
	}
}

/*
================
ConArgCompleteFile::Complete
================
*/
OG_INLINE void ConArgCompleteFile::Complete( const CmdArgs &args, argCompletionCB_t callback ) const {
	if ( FS == NULL )
		return;
	FileList *files = FS->GetFileList( dir.c_str(), extension.c_str(), findFlags );

	if ( !files )
		return;

	Format param( "$* $*" );
	do {
		callback( Format( "$* $*" ) << args.Argv( 0 ) << files->GetName() );
		param.Reset();
	} while ( files->GetNext() );

	FS->FreeFileList( files );
}

}

#endif
