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

#include <og/Console.h>
#include "CvarSystemEx.h"

extern const char *cvarFlagNames[];
namespace og {
extern bool inEngineStartup; //! @todo	do this differently

const int MAX_INTEGER_COMPLETION = 30; //! @todo	what value is good ?
const int MAX_ENUM_LIST_SIZE = 35;

CVar com_developer( "com_developer", "0", CVAR_ENGINE|CVAR_BOOL, "Developer Mode" );
CVar net_allowCheats( "net_allowCheats", "0", CVAR_ENGINE|CVAR_BOOL|CVAR_NETSYNC, "Allow cheats in network game" );

// The object and global pointer
static CVarSystemEx CVarSystemObject;
CVarSystem *cvarSystem = &CVarSystemObject;
CVarSystemEx *cvarSystemEx = &CVarSystemObject;

/*
==============================================================================

  CVarDataEx

==============================================================================
*/

/*
================
CVarDataEx::SetString
================
*/
void CVarDataEx::SetString( const char *newValue ) {
	if ( flags & CVAR_BOOL ) {
		SetBool( String::ToInt(newValue) != 0 );
		return;
	} else if ( flags & CVAR_INTEGER ) {
		SetInt( String::ToInt(newValue) );
		return;
	} else if ( flags & CVAR_FLOAT ) {
		SetFloat( String::ToFloat(newValue) );
		return;
	} else if ( flags & CVAR_ENUM ) {
		// We have a set of values, check if one fits
		int count = 0;
		while ( enumValues[count] ) {
			if ( String::Icmp( newValue, enumValues[count] ) == 0 )
				break;
			count++;
		}
		// Not found
		if ( enumValues[count] == NULL )
			return;

		if ( !initialized )
			initialized = true;
		else if ( String::Cmp( enumValues[count], strValue ) == 0 )
			return;

		value		= enumValues[count];
		strValue	= value.c_str();
		iValue		= count;
		fValue		= static_cast<float>(count);
		flags		|= CVAR_MODIFIED;

		if ( callback )
			callback();
		return;
	}

	if ( !initialized )
		initialized = true;
	else if ( String::Cmp( newValue, strValue ) == 0 )
		return;

	value		= newValue;
	strValue	= value.c_str();
	iValue		= String::ToInt(newValue);
	fValue		= String::ToFloat(newValue);
	flags		|= CVAR_MODIFIED;

	if ( callback )
		callback();
}

/*
================
CVarDataEx::SetBool
================
*/
void CVarDataEx::SetBool( bool newValue ) {
	if ( flags & (CVAR_INTEGER|CVAR_ENUM) ) {
		SetInt( newValue );
		return;
	} else if ( flags & CVAR_FLOAT ) {
		SetFloat( static_cast<float>(newValue) );
		return;
	} else if ( !(flags & CVAR_BOOL) ) {
		SetString( newValue?"1":"0" );
		return;
	}
	if ( !initialized )
		initialized = true;
	else if ( newValue == (iValue != 0) )
		return;

	value		= newValue?"1":"0";
	strValue	= value.c_str();
	iValue		= newValue?1:0;
	fValue		= newValue?1.0f:0.0f;
	flags		|= CVAR_MODIFIED;

	if ( callback )
		callback();
}

/*
================
CVarDataEx::SetInt
================
*/
void CVarDataEx::SetInt( int newValue ) {
	if ( flags & CVAR_BOOL ) {
		SetBool( newValue != 0 );
		return;
	} else if ( flags & CVAR_FLOAT ) {
		SetFloat( static_cast<float>(newValue) );
		return;
	} else if ( flags & CVAR_ENUM ) {
		if ( newValue < iMinValue )
			newValue = iMinValue;
		else if ( newValue > iMaxValue )
			newValue = iMaxValue;

		if ( !initialized )
			initialized = true;
		else if ( newValue == iValue )
			return;

		value		= enumValues[newValue];
		strValue	= value.c_str();
		iValue		= newValue;
		fValue		= static_cast<float>(newValue);
		flags		|= CVAR_MODIFIED;

		if ( callback )
			callback();
		return;
	} else if ( !(flags & CVAR_INTEGER) ) {
		SetString( Format() << newValue );
		return;
	}

	if ( newValue < iMinValue )
		newValue = iMinValue;
	else if ( newValue > iMaxValue )
		newValue = iMaxValue;

	if ( !initialized )
		initialized = true;
	else if ( newValue == iValue )
		return;

	value = Format() << newValue;
	strValue	= value.c_str();
	iValue		= newValue;
	fValue		= static_cast<float>(newValue);
	flags		|= CVAR_MODIFIED;
	
	if ( callback )
		callback();
}

/*
================
CVarDataEx::SetFloat
================
*/
void CVarDataEx::SetFloat( float newValue ) {
	if ( flags & CVAR_BOOL ) {
		SetBool( newValue != 0.0f );
		return;
	} else if ( flags & (CVAR_INTEGER|CVAR_ENUM) ) {
		SetInt( Math::FtoiFast(newValue) );
		return;
	} else if ( !(flags & CVAR_FLOAT) ) {
		SetString( Format() << SetPrecision(CVAR_FLOAT_PRECISION) << newValue );
		return;
	}

	if ( newValue < fMinValue )
		newValue = fMinValue;
	else if ( newValue > fMaxValue )
		newValue = fMaxValue;

	if ( !initialized )
		initialized = true;
	else if ( newValue == fValue )
		return;

	value		= Format() << SetPrecision(CVAR_FLOAT_PRECISION) << newValue;
	strValue	= value.c_str();
	iValue		= Math::FtoiFast(newValue);
	fValue		= newValue;
	flags		|= CVAR_MODIFIED;
	
	if ( callback )
		callback();
}

/*
================
CVarDataEx::Init
================
*/
void CVarDataEx::Init( const CVarData *initData ) {
	flags		= initData->flags;
	name		= initData->strName;
	description	= initData->strDescription;
	defValue	= initData->strDefaultValue;
	completion	= initData->completion;
	fMinValue	= initData->fMinValue;
	fMaxValue	= initData->fMaxValue;
	iMinValue	= initData->iMinValue;
	iMaxValue	= initData->iMaxValue;
	enumValues	= initData->enumValues;
	callback	= initData->callback;

	strName			= name.c_str();
	strDescription	= description.c_str();
	strDefaultValue	= defValue.c_str();
	
	// Check for double types
	int activeTypes = 0;
	for( int i=0; i<CVAR_NUM_TYPES; i++ ) {
		if ( flags & BIT(i) )
			activeTypes++;
	}
	if ( activeTypes > 1 )
		User::Error( ERR_CVAR_INIT, "CVar has multiple cvar types, make sure you only have one specified\n", strName);

	// Check if enum has been initialized correctly..
	if ( flags & CVAR_ENUM ) {
		iMinValue = 0;
		iMaxValue = -1;
		if ( enumValues ) {
			while ( enumValues[iMaxValue+1] )
				iMaxValue++;
		}
		if ( iMaxValue < 1 )
			User::Error( ERR_CVAR_INIT, "CVar is of type ENUM, but does not have enough values (2 min)\n", strName);
	}
}

/*
==============================================================================

  CVarSystemEx

==============================================================================
*/

/*
================
CVarSystemEx::CVarSystemEx
================
*/
CVarSystemEx::CVarSystemEx() {
}

/*
================
CVarSystemEx::~CVarSystemEx
================
*/
CVarSystemEx::~CVarSystemEx() {
	Clear();
}

/*
================
CVarSystemEx::ResetToDefault
================
*/
void CVarSystemEx::ResetToDefault( int flags ) {
	int num = dataList.Num();
	for ( int i = 0; i<num; i++ ) {
		if ( dataList[i].flags & flags )
			dataList[i].SetString( dataList[i].strDefaultValue );
	}
}

/*
================
CVarSystemEx::IsModified
================
*/
bool CVarSystemEx::IsModified( int flags ) {
	int num = dataList.Num();
	for ( int i = 0; i<num; i++ ) {
		if ( dataList[i].flags & flags )
			return true;
	}
	return false;
}

/*
================
CVarSystemEx::SetModified
================
*/
void CVarSystemEx::SetModified( int flags, bool modified ) {
	int num = dataList.Num();
	for ( int i = 0; i<num; i++ ) {
		if ( dataList[i].flags & flags ) {
			if ( modified ) {
				dataList[i].flags |= CVAR_MODIFIED;
				if ( dataList[i].callback )
					dataList[i].callback();
			} else if ( dataList[i].flags & CVAR_MODIFIED ) {
				dataList[i].flags &= ~CVAR_MODIFIED;
			}
		}
	}
}

/*
================
CVarSystemEx::Find
================
*/
CVar *CVarSystemEx::Find( const char *key ) const {
	if ( key == NULL || key[0] == '\0' )
		return NULL;

	int index = dataList.Find( key );
	if ( index != -1 )
		return dataList[index].cvar;

	return NULL;
}

/*
================
CVarSystemEx::CreateUnlinkedCVar
================
*/
void CVarSystemEx::CreateUnlinkedCVar( const char *key, const char *value ) {
	CVarDataEx &data = dataList[key];
	data.flags = CVAR_UNLINKED;
	data.name = key;
	data.cvar = new CVar(&data);
	data.enumValues = NULL;
	data.completion = NULL;
	data.callback = NULL;
	data.name = key;
	data.defValue = value;
	data.strDescription = data.description.c_str();
	data.strName = data.name.c_str();
	data.strDefaultValue = data.defValue.c_str();

	// just make sure setstring has something to compare
	data.strValue = "";
	data.SetString( value );
}

/*
================
CVarSystemEx::SetString
================
*/
void CVarSystemEx::SetString( const char *key, const char *value, bool force ) {
	if ( key == NULL || key[0] == '\0' )
		return;

	CVar *cv = Find( key );
	if ( cv ) {
		if ( !force && ( cv->data->flags & (CVAR_ROM|CVAR_INIT) ) ) {
			User::Warning( Format("$* is readonly" ) << cv->data->strName );
			return;
		}
		if ( !force && ( cv->data->flags & CVAR_CHEAT && !net_allowCheats.GetBool() ) ) {
			User::Warning( Format( "$* is cheat protected" ) << cv->data->strName );
			return;
		}
		cv->data->SetString( value );
		return;
	}
	CreateUnlinkedCVar( key, value );
}

/*
================
CVarSystemEx::GetString
================
*/
const char *CVarSystemEx::GetString( const char *key ) const {
	CVar *cv = Find( key );
	if ( cv )
		return cv->data->strValue;
	return NULL;
}

/*
================
CVarSystemEx::SetBool
================
*/
void CVarSystemEx::SetBool( const char *key, bool value, bool force ) {
	if ( key == NULL || key[0] == '\0' )
		return;

	CVar *cv = Find( key );
	if ( cv ) {
		if ( !force && ( cv->data->flags & (CVAR_ROM|CVAR_INIT) ) ) {
			User::Warning( Format( "$* is readonly" ) << cv->data->strName );
			return;
		}
		if ( !force && ( cv->data->flags & CVAR_CHEAT && !net_allowCheats.GetBool() ) ) {
			User::Warning( Format( "$* is cheat protected" ) << cv->data->strName );
			return;
		}
		cv->data->SetBool( value );
		return;
	}
	CreateUnlinkedCVar( key, value?"1":"0" );
}

/*
================
CVarSystemEx::GetBool
================
*/
bool CVarSystemEx::GetBool( const char *key ) const {
	CVar *cv = Find( key );
	if ( cv )
		return cv->data->iValue != 0;
	return false;
}

/*
================
CVarSystemEx::SetInt
================
*/
void CVarSystemEx::SetInt( const char *key, int value, bool force ) {
	CVar *cv = Find( key );
	if ( cv ) {
		if ( !force && ( cv->data->flags & (CVAR_ROM|CVAR_INIT) ) ) {
			User::Warning( Format("$* is readonly" ) << cv->data->strName );
			return;
		}
		if ( !force && ( cv->data->flags & CVAR_CHEAT && !net_allowCheats.GetBool() ) ) {
			User::Warning( Format("$* is cheat protected" ) << cv->data->strName );
			return;
		}
		cv->data->SetInt( value );
		return;
	}
	CreateUnlinkedCVar( key, Format() << value );
}

/*
================
CVarSystemEx::GetInt
================
*/
int CVarSystemEx::GetInt( const char *key ) const {
	CVar *cv = Find( key );
	if ( cv )
		return cv->data->iValue;
	return 0;
}

/*
================
CVarSystemEx::SetFloat
================
*/
void CVarSystemEx::SetFloat( const char *key, float value, bool force ) {
	CVar *cv = Find( key );
	if ( cv ) {
		if ( !force && ( cv->data->flags & (CVAR_ROM|CVAR_INIT) ) ) {
			User::Warning( Format("$* is readonly" ) << cv->data->strName );
			return;
		}
		if ( !force && ( cv->data->flags & CVAR_CHEAT && !net_allowCheats.GetBool() ) ) {
			User::Warning( Format("$* is cheat protected" ) << cv->data->strName );
			return;
		}
		cv->data->SetFloat( value );
		return;
	}
	CreateUnlinkedCVar( key, Format() << value );
}

/*
================
CVarSystemEx::GetFloat
================
*/
float CVarSystemEx::GetFloat( const char *key ) const {
	CVar *cv = Find( key );
	if ( cv )
		return cv->data->fValue;
	return 0.0f;
}

/*
================
CVarSystemEx::ToDict
================
*/
void CVarSystemEx::ToDict( int flags, Dict *dict ) const {
	OG_ASSERT( dict != NULL );
	dict->Clear();
	int num = dataList.Num();
	for ( int i = 0; i<num; i++ ) {
		if ( dataList[i].flags & flags )
			dict->Set( dataList[i].strName, dataList[i].strValue );
	}
}

/*
================
CVarSystemEx::FromDict
================
*/
void CVarSystemEx::FromDict( const Dict *dict ) {
	int num = dict->Num();
	const KeyValue *kv;
	for ( int i=0; i<num; i++ ) {
		kv = dict->GetKeyValue(i);
		SetString( kv->GetKey().c_str(), kv->GetValue() );
	}
}

/*
================
CVarICmp
================
*/
OG_INLINE int CVarICmp( const int &a, const int &b, void *param ) {
	DictEx<CVarDataEx> &dataList = *(DictEx<CVarDataEx> *)param;
	return String::Icmp( dataList[a].strName, dataList[b].strName );
}

/*
================
CVarSystemEx::ExportToHTML
================
*/
bool CVarSystemEx::ExportToHTML( const char *filename, int flags ) const {
	if ( FS == NULL )
		return false;

	byte *buffer = NULL;
	FS->LoadFile("exportTemplate.html", &buffer);
	if ( !buffer ) 
		return false;

	String templateBuffer = reinterpret_cast<char *>(buffer);
	FS->FreeFile(buffer);

	File *file = FS->OpenWrite(filename);
	if ( !file )
		return false;

	String table = "<table width=\"900\">\r\n<tr><th width=\"25%\" style=\"text-align:left\"><b>Name</b></th><th width=\"100\"><b>Default</b></th><th width=\"140\"><b>Range/Values</b></th><th style=\"text-align:left\"><b>Description</b></th></tr>\r\n";

#if 0 // Unsorted
	int num = dataList.Num();
	for ( int i = 0; i<num; i++ ) {
		const CVarDataEx &cd = dataList[i];
#else // Sorted
	List<int> cvars;
	int i, num = dataList.Num();
	for ( i = 0; i<num; i++ ) {
		if ( dataList[i].flags & flags )
			cvars.Append(i);
	}
	cvars.SortEx( CVarICmp, (void *)&dataList, false );
	num = cvars.Num();
	Format out("<tr><td>$*</td><td align=\"center\">$*</td><td align=\"center\">$*$*$*</td><td>$*</td></tr>\r\n");
	for ( i = 0; i<num; i++ ) {
		const CVarDataEx &cd = dataList[cvars[i]];
#endif
		if ( cd.flags & flags ) {
			if ( cd.flags & CVAR_FLOAT )
				table += out << cd.strName << cd.strDefaultValue << cd.fMinValue << " -> " << cd.fMaxValue << cd.strDescription;
			else if ( cd.flags & CVAR_INTEGER )
				table += out << cd.strName << cd.strDefaultValue << cd.iMinValue << " -> " << cd.iMaxValue << cd.strDescription;
			else if ( cd.flags & CVAR_BOOL )
				table += out << cd.strName << cd.strDefaultValue << "0" << " / " << "1" << cd.strDescription;
			else if ( cd.flags & CVAR_ENUM ) {
				String values;
				const char **strings = cd.enumValues;
				while ( *strings ) {
					values += "\"";
					values += *strings;
					values += "\"";
					strings++;
					if ( strings )
						values += ", ";
				}
				values.StripTrailingOnce(", ");
				table += out << cd.strName << cd.strDefaultValue << "" << values << "" << cd.strDescription;
			} else
				table += out << cd.strName << cd.strDefaultValue << "" << "-" << "" << cd.strDescription;
			out.Reset();
		}
	}

	table += "</table>\r\n";
	String result;
	result.FromBitFlags( flags & ~CVAR_MODIFIED, cvarFlagNames );
	templateBuffer.Replace( "{{TITLE}}", Format("Exported CVars with flags ($*)" ) << result );
	templateBuffer.Replace( "{{TABLE}}", table.c_str() );

	file->Write( templateBuffer.c_str(), templateBuffer.ByteLength() );
	file->Close();
	return true;
}

/*
================
CVarSystemEx::WriteToConfig
================
*/
bool CVarSystemEx::WriteToConfig( const char *filename, int flags ) const {
	if ( FS == NULL )
		return false;

	File *file = FS->OpenWrite(filename);
	if ( !file )
		return false;

	String header(  "// **********************************************************\r\n"
					"// This file was generated and will be overwritten!\r\n"
					"// **********************************************************\r\n\r\n" );
	file->Write( header.c_str(), header.ByteLength() );

#if 0 // Unsorted
	int num = dataList.Num();
	for ( int i = 0; i<num; i++ ) {
		if ( dataList[i].flags & flags )
			file->Printf( "seta %-32s \"%s\"\r\n", dataList[i].strName, dataList[i].strValue );
	}
#else // Sorted
	List<int> cvars;
	int i, num = dataList.Num();
	for ( i = 0; i<num; i++ ) {
		if ( dataList[i].flags & flags )
			cvars.Append(i);
	}
	cvars.SortEx( CVarICmp, (void *)&dataList, false );
	num = cvars.Num();
	int index;
	Format out( "SetA $+32* \"$*\"\r\n" );
	for ( i = 0; i<num; i++ ) {
		index = cvars[i];
		out << dataList[index].strName << dataList[index].strValue;
		file->Write( out, out.ByteLength() );
		out.Reset();
	}
#endif

	file->Close();
	return true;
}

/*
================
CVarSystemEx::OnCommand
================
*/
bool CVarSystemEx::OnCommand( const CmdArgs &args ) {
	CVar *cv = cvarSystem->Find(args.Argv(0));
	if ( cv ) {
		if ( args.Argc() > 1 ) {
			if ( cv->data->flags & (CVAR_ROM|CVAR_INIT) )
				User::Warning( Format("$* is readonly" ) << cv->data->strName );
			else if ( cv->data->flags & CVAR_CHEAT && !net_allowCheats.GetBool() )
				User::Warning( Format("$* is cheat protected" ) << cv->data->strName );
			else
				cv->SetString( args.Argv(1) );
		}
		else
			if ( cv->data->strDescription[0] )
				Console::Print( Format( "$* is: \"$*\", default: \"$*\"\n- $*\n" ) << cv->data->strName << cv->data->strValue << cv->data->strDefaultValue << cv->data->strDescription );
			else
				Console::Print( Format( "$* is: \"$*\", default: \"$*\"\n" ) << cv->data->strName << cv->data->strValue << cv->data->strDefaultValue );

		return true;
	}
	return false;
}

/*
================
CVarSystemEx::CompleteCVar
================
*/
void CVarSystemEx::CompleteCVar( const CmdArgs &args, argCompletionCB_t callback ) {
	// Find the cvar and call its completion func
	CVar *cv = Find( args.Argv(0) );
	if ( cv ) {
		Format complete( "$* $*" );
		// Custom completion function
		if ( cv->data->completion ) {
			cv->data->completion->Complete(args, callback);
		// Custom string values
		} else if ( cv->data->flags & CVAR_ENUM ) {
			const char **strings = cv->data->enumValues;
			while ( *strings ) {
				callback( complete << args.Argv( 0 ) << *strings );
				complete.Reset();
				strings++;
			}
		// Automatic value completion
		} else if ( cv->data->flags & CVAR_BOOL ) {
			callback( complete << args.Argv( 0 ) << 0 );
			complete.Reset();
			callback( complete << args.Argv( 0 ) << 1 );
		} else if ( cv->data->flags & CVAR_INTEGER ) {
			if ( (cv->data->iMaxValue - cv->data->iMinValue) <= MAX_INTEGER_COMPLETION ) {
				for ( int i = cv->data->iMinValue; i <= cv->data->iMaxValue; i++ ) {
					callback( complete << args.Argv( 0 ) << i );
					complete.Reset();
				}
			}
		}
	}
}
void CVarSystemEx::CompleteCVar( const char *buf, int bufLen, StringList &completionList ) {
	for( int i = dataList.MatchPrefix( buf, bufLen ); i != -1; i = dataList.MatchPrefix( buf, bufLen, i ) )
		completionList.Append( dataList[i].strName );
}

/*
================
CVarSystemEx::GetCVarDataList
================
*/
const DictEx<CVarDataEx> &CVarSystemEx::GetCVarDataList( void ) const {
	return dataList;
}

/*
================
CVarSystemEx::Cmd_ListCVars_f
================
*/
ConUsageString Cmd_ListCVars_Usage("Lists all cvars currently available.", 0, "ListCVars [options] [searchstring]");
void CVarSystemEx::Cmd_ListCVars_f( const CmdArgs &args ) {
	DictEx<CVarDataEx> & cvarDataList = CVarSystemObject.dataList;
	bool match = false;
	const char *matchstr = NULL;

	int num = cvarDataList.Num();
	int disp = -1;
	if( args.Argc() > 1 ) {
		if( args.Argc() > 2 ) {
			match = true;
			matchstr = args.Argv(2);
		}
		if( String::Icmp(args.Argv(1), "-type") == 0 ) {
			disp = 0;
			Format outrange("$* $* [$*, $*]\n");
			Format out("$* $* [$*]\n");
			for ( int i=0; i<num; i++ ) {
				CVarDataEx &cd = cvarDataList[i];
				if ( match && cd.name.Find(matchstr, false) == String::INVALID_POSITION )
					continue;

				if ( cd.flags & CVAR_BOOL ) {
					Console::Print( outrange << "bool" << cd.strName << "0" << "1" );
					outrange.Reset();
				} else if ( cd.flags & CVAR_INTEGER ) {
					Console::Print( outrange << "int" << cd.strName << cd.iMinValue << cd.iMaxValue );
					outrange.Reset();
				} else if ( cd.flags & CVAR_FLOAT ) {
					Console::Print( outrange << "float" << cd.strName << cd.fMinValue << cd.fMaxValue );
					outrange.Reset();
				} else if ( cd.flags & CVAR_ENUM ) {
					String values;
					const char **strings = cd.enumValues;
					while ( *strings ) {
						values += "\"";
						values += *strings;
						values += "\"";
						strings++;
						if ( strings )
							values += ", ";
					}
					values.StripTrailingOnce(", ");
					values.CapLength(MAX_ENUM_LIST_SIZE, true);
					Console::Print( out << "enum" << cd.strName << values );
					out.Reset();
				} else {
					Console::Print( out << "string" << cd.strName << "?" );
					out.Reset();
				}

				disp++;
			}
		} else if( String::Icmp(args.Argv(1), "-flags") == 0 ) {
			disp = 0;
			Format out("$+32* $*\n");
			for ( int i=0; i<num; i++ ) {
				CVarDataEx &cd = cvarDataList[i];
				if ( match && cd.name.Find(matchstr, false) == String::INVALID_POSITION )
					continue;
				String result;
				result.FromBitFlags( cd.flags & ~CVAR_MODIFIED, cvarFlagNames );
				out << cd.strName << result;
				Console::Print( out );
				out.Reset();
				disp++;
			}
		} else if( String::Icmp(args.Argv(1), "-help") == 0 ) {
			disp = 0;
			Format out("$+32* $*\n");
			for ( int i=0; i<num; i++ ) {
				CVarDataEx &cd = cvarDataList[i];
				if ( match && cd.name.Find(matchstr, false) == String::INVALID_POSITION )
					continue;
				Console::Print( out << cd.strName << cd.strDescription );
				out.Reset();
				disp++;
			}
		} else {
			match = true;
			matchstr = args.Argv(1);
		}
	}
	if( disp == -1 ) {
		disp = 0;
		Format out("$+32* \"$*\"\n");
		for ( int i=0; i<num; i++ ) {
			CVarDataEx &cd = cvarDataList[i];
			if ( match && cd.name.Find(matchstr, false) == String::INVALID_POSITION )
				continue;
			Console::Print( out << cd.strName << cd.strValue );
			out.Reset();
			disp++;
		}
	}
	if( match )
		Console::Print( Format( "\n$* cvar$* matching \"$*\" listed\n\n" ) << disp << (disp == 1 ? "" : "s") << matchstr );
	else
		Console::Print( Format( "\n$* cvar$* listed\n\n" ) << disp << (disp == 1 ? "" : "s") );
	Console::Print( "ListCVars [search string]        = list cvar values\n" );
	Console::Print( "ListCVars -help [search string]  = list cvar descriptions\n" );
	Console::Print( "ListCVars -type [search string]  = list cvar types\n" );
	Console::Print( "ListCVars -flags [search string] = list cvar flags\n" );
}

/*
================
CVarSystemEx::Cmd_ExportCVars_f
================
*/
ConUsageString Cmd_ExportCVars_Usage("Export all cvars to an HTML file.", 0, "ExportCVars");
void CVarSystemEx::Cmd_ExportCVars_f( const CmdArgs &args ) {
	Console::Print( "Exporting all CVars to \"cvars.html\"..\n" );
	if ( cvarSystem->ExportToHTML() )
		Console::Print( "Done!\n" );
	else
		Console::Print( "Failed!\n" );
}

/*
================
CVarSystemEx::Cmd_Toggle_f
================
*/
ConUsageString Cmd_Toggle_Usage("Toggles a CVar's value.", 1, "Toggle <variable> [...]");
void CVarSystemEx::Cmd_Toggle_f( const CmdArgs &args ) {
	CVar *cv = cvarSystem->Find(args.Argv(1));
	if ( !cv ) {
		User::Warning( Format("Cmd_Toggle_f: cvar \"$*\" not found!" ) << args.Argv(1) );
		return;
	}

	if ( cv->data->flags & (CVAR_ROM|CVAR_INIT) ) {
		User::Warning( Format("Cmd_Toggle_f: $* is readonly" ) << cv->data->strName );
		return;
	}
	if ( cv->data->flags & CVAR_CHEAT && !net_allowCheats.GetBool() ) {
		User::Warning( Format("Cmd_Toggle_f: $* is cheat protected" ) << cv->data->strName );
		return;
	}

	// Bool got no other states, so we can ignore all other params
	if( (cv->data->flags & CVAR_BOOL) || args.Argc() == 2 ) {
		const char *curValue = cv->GetString();
		if ( cv->data->flags & CVAR_ENUM ) {
			// We have a set of values, check if one fits
			int currentIndex = 0;
			while ( cv->data->enumValues[currentIndex] ) {
				if ( String::Icmp( curValue, cv->data->enumValues[currentIndex] ) == 0 )
					break;
				currentIndex++;
			}
			// Not found or the last item
			if ( cv->data->enumValues[currentIndex] == NULL || currentIndex == cv->data->iMaxValue )
				cv->SetString(cv->data->enumValues[0]);
			else
				cv->SetString(cv->data->enumValues[currentIndex+1]);
		}
		else
			cv->SetBool(!cv->GetBool());
	} else if ( args.Argc() == 3 ) {
		/*
		NOTE:	Limits will cause this to fail if 0
				is out of range or argv(2) is out of range.
				The user has to take care of good values.

				A safety check is not needed here,
				the cvar will take care of it.
		*/

		const char *curValue = cv->GetString();
		const char *argValue = args.Argv(2);
		if ( argValue[0] ) {
			if ( cv->data->flags & CVAR_ENUM ) {
				if ( String::Cmp(curValue, cv->data->enumValues[0]) == 0 || String::Icmp(curValue, argValue) != 0 )
					cv->SetString(argValue);
				else
					cv->SetString(cv->data->enumValues[0]);
			} else {
				if ( String::Cmp(curValue, "0") == 0 || String::Icmp(curValue, argValue) != 0 )
					cv->SetString(argValue);
				else
					cv->SetString("0");
			}
		}
	} else {
		/*
		NOTE:	Same problems as with the code above.
				Nothing to worry about.
		*/

		int numArgs = args.Argc();

		const char *curValue = cv->GetString();
		int currentIndex = -1;
		for( int i=2; i<numArgs; i++ ) {
			if ( String::Icmp( args.Argv(i), curValue ) == 0 ) {
				currentIndex = i;
				break;
			}
		}
		// Not found or the last item
		if ( currentIndex == -1 || currentIndex == numArgs-1 )
			cv->SetString(args.Argv(2));
		else
			cv->SetString(args.Argv(currentIndex+1));
	}
	Console::Print( Format( "set $* = \"$*\"\n" ) << cv->data->strName << cv->data->strValue );
}

/*
================
CVarSystemEx::Cmd_Set_f
================
*/
ConUsageString Cmd_Set_Usage("Sets a CVar.", 2, "Set <variable> <value...>");
void CVarSystemEx::Cmd_Set_f( const CmdArgs &args ) {
	String rest;
	args.Args( rest, 2 );
	cvarSystem->SetString( args.Argv(1), rest.c_str() );

	// Do not print Set Commands in Startup
	if ( inEngineStartup )
		return;

	//! @todo	When SetString fails, this will print something different
	//!			than what was entered by the user, is this intended?
	CVar *cv = cvarSystem->Find(args.Argv(1));
	if ( !cv )
		return;
	Console::Print( Format( "$* $* = \"$*\"\n" ) << args.Argv(0) << cv->data->strName << cv->data->strValue );
}

#ifdef CMD_CVAR_SETU
/*
================
CVarSystemEx::Cmd_SetInfo_f
================
*/
ConUsageString Cmd_SetInfo_Usage("Sets a CVar and flags it as Userinfo.", 2, "SetInfo <variable> <value...>");
void CVarSystemEx::Cmd_SetInfo_f( const CmdArgs &args ) {
	Cmd_Set_f(args);
	CVar *cv = cvarSystem->Find(args.Argv(1));
	if ( !cv )
		return;
	cv->data->flags |= CVAR_USERINFO;
}
#endif

/*
================
CVarSystemEx::Cmd_SetA_f
================
*/
ConUsageString Cmd_SetA_Usage("Sets a CVar and flags it as Archived.", 2, "SetA <variable> <value...>");
void CVarSystemEx::Cmd_SetA_f( const CmdArgs &args ) {
	Cmd_Set_f(args);
	CVar *cv = cvarSystem->Find(args.Argv(1));
	if ( !cv )
		return;
	if ( !(cv->data->flags & (CVAR_ROM|CVAR_INIT)) )
		cv->data->flags |= CVAR_ARCHIVE;
}

/*
================
CVarSystemEx::Cmd_Inc_f
================
*/
ConUsageString Cmd_Inc_Usage("Increases a CVar by an amount(1 per default).", 1, "Inc <variable> [amount]");
void CVarSystemEx::Cmd_Inc_f( const CmdArgs &args ) {
	CVar *cv = cvarSystem->Find(args.Argv(1));
	if ( !cv )
		return;

	float amount = 1.0f;
	if ( args.Argc() == 3 )
		amount = String::ToFloat( args.Argv(2) );

	cv->SetFloat( cv->GetFloat() + amount );

	// Do not print Set Commands in Startup
	if ( inEngineStartup )
		return;
	Console::Print( Format( "set $* = \"$*\"\n" ) << cv->data->strName << cv->data->strValue );
}

/*
================
CVarSystemEx::Cmd_Dec_f
================
*/
ConUsageString Cmd_Dec_Usage("Decreases a CVar by an amount(1 per default).", 1, "Dec <variable> [amount]");
void CVarSystemEx::Cmd_Dec_f( const CmdArgs &args ) {
	CVar *cv = cvarSystem->Find(args.Argv(1));
	if ( !cv )
		return;

	float amount = 1.0f;
	if ( args.Argc() == 3 )
		amount = String::ToFloat( args.Argv(2) );

	cv->SetFloat( cv->GetFloat() - amount );

	// Do not print Set Commands in Startup
	if ( inEngineStartup )
		return;
	Console::Print( Format( "set $* = \"$*\"\n" ) << cv->data->strName << cv->data->strValue );
}

/*
================
CVarSystemEx::Cmd_Reset_f
================
*/
ConUsageString Cmd_Reset_Usage("Resets a CVar it's the default value.", 1, "Reset <variable>");
void CVarSystemEx::Cmd_Reset_f( const CmdArgs &args ) {
	if ( args.Argc() > 2 ) {
		Cmd_Reset_Usage.ShowUsage();
		return;
	}

	CVar *cv = cvarSystem->Find(args.Argv(1));
	if ( !cv )
		return;

	// Silently return if read only
	if ( cv->data->flags & (CVAR_ROM|CVAR_INIT) )
		return;

	cv->ResetToDefault();
}

/*
================
CVarSystemEx::LinkCVar
================
*/
void CVarSystemEx::LinkCVar( CVar *cvar ) {
	CVarData *initData = cvar->data;
	CVar *cv = Find(initData->strName);
	if ( cv ) {
		if ( !(cv->data->flags & CVAR_UNLINKED) ) {
			User::Error( ERR_CVAR_LINK, "Trying to link cvar, but it is already linked.", initData->strName );
			return;
		}

		CVarDataEx *savedData = static_cast<CVarDataEx *>(cv->data);
		cvar->data = savedData;
		savedData->cvar = cvar;
		savedData->Init( initData );

		// Some CVars that are not changeable
		if ( (initData->flags & (CVAR_ROM|CVAR_INIT)) || (initData->flags & CVAR_CHEAT && !net_allowCheats.GetBool()) )
			savedData->SetString(initData->strDefaultValue);
		else if ( savedData->flags & CVAR_FLOAT ) {
			// Make sure the correct precision is used
			savedData->value		= Format() << SetPrecision(CVAR_FLOAT_PRECISION) << savedData->fValue;
			savedData->strValue		= savedData->value.c_str();
			savedData->fValue		= String::ToFloat(savedData->strValue);
			savedData->iValue		= Math::FtoiFast(savedData->fValue);
		}

		delete cv; // delete temporary cvar
	} else {
		CVarDataEx &newData = dataList[initData->strName];
		cvar->data = &newData;
		newData.cvar = cvar;
		newData.flags = initData->flags;
		newData.Init( initData );

		// just make sure setstring has something to compare
		newData.strValue = "";
		if ( newData.flags & CVAR_ENUM )
			newData.iValue = -1;
		newData.SetString(initData->strDefaultValue);
	}
}

/*
================
CVarSystemEx::UnlinkCVar
================
*/
void CVarSystemEx::UnlinkCVar( CVar *cvar ) {
	CVarData *initData = cvar->data;
	if ( initData->flags & CVAR_UNLINKED ) {
		User::Error( ERR_CVAR_LINK, "Trying to unlink cvar, but it is already unlinked.", initData->strName );
		return;
	}
	CVar *cv = Find(initData->strName);

	if ( cv != cvar ) {
		User::Error( ERR_CVAR_LINK, "Trying to unlink cvar, but it does not match the internal cvar.", initData->strName );
		return;
	}
	// Create a temporary cvar
	CVarDataEx *data = static_cast<CVarDataEx *>(cv->data);
	data->flags |= CVAR_UNLINKED;
	data->cvar = new CVar(data);
	data->completion = NULL;
	data->enumValues = NULL;
	data->callback = NULL;
}

/*
================
CVarSystemEx::Clear
================
*/
void CVarSystemEx::Clear( void ) {
	int num = dataList.Num();
	for ( int i = 0; i<num; i++ ) {
		if ( dataList[i].flags & CVAR_UNLINKED )
			delete dataList[i].cvar; // delete temporary cvar
	}
	dataList.Clear();
}

}

