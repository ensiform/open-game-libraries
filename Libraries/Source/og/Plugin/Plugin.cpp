// ==============================================================================
//! @file
//! @brief	Win32 Plugin Library
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

#include <og/Plugin.h>

#ifdef OG_WIN32
#include <windows.h>
#include <fcntl.h>
#define OG_DECL __cdecl
#elif defined(OG_LINUX)
#include <dlfcn.h>
#define OG_DECL 
#endif

namespace og {

typedef PluginExportBase * ( OG_DECL *plugin_entry_t )( void );

/*
==============================================================================

  PluginHandle

==============================================================================
*/
class PluginHandle {
private:
#ifdef OG_WIN32
	HINSTANCE	handle;
#elif defined(OG_LINUX)
	void *		handle;
#endif
	String		filename;
	PluginExportBase *pluginExport;

public:
#ifdef OG_WIN32
	PluginHandle( HINSTANCE handle, const char *filename ) {
#elif defined(OG_LINUX)
	PluginUnix( void *handle, const char *filename ) {
#endif
		this->handle = handle;
		this->filename = filename;
		pluginExport = NULL;
	}
	PluginExportBase *Connect( void ) {
#ifdef OG_WIN32
		plugin_entry_t pluginEntry = (plugin_entry_t)GetProcAddress( handle, "getPluginExport" ); 
#elif defined(OG_LINUX)
		plugin_entry_t pluginEntry = (plugin_entry_t)dlsym( handle, "getPluginExport" ); 
#endif
		if( pluginEntry ) {
			pluginExport = pluginEntry();
			return pluginExport;
		}
		return NULL;
	}
	void Unload() {
#ifdef OG_WIN32
		if ( !FreeLibrary( handle ) ) {
#elif defined(OG_LINUX)
		dlclose( handle );
		const char *err = dlerror();
		if ( err != NULL ) {
#endif
			//fixme: better error id
			User::Error( ERR_SYSTEM_REQUIREMENTS, "Failed to free library", filename.c_str() );
		}
		delete this;
	}
	OG_INLINE PluginExportBase *GetExport( void ) const {
		return pluginExport;
	}
};

static og::List<PluginHandle *> loadedPlugins;

/*
==============================================================================

  Plugin

==============================================================================
*/
/*
================
Plugin::Load
================
*/
PluginExportBase *Plugin::Load( const char *filename, PluginImportBase *pluginImport ) {
#ifdef OG_WIN32
	DynBuffer<wchar_t> strPath;
	StringToWide( filename, strPath );
	HINSTANCE handle = LoadLibraryW( strPath.data );
#elif defined(OG_LINUX)
	void *handle = dlopen( filename, RTLD_NOW );
#endif
	if ( handle == NULL ) {
		return NULL;
	}
	PluginHandle *plugin = new PluginHandle( handle, filename );
	PluginExportBase *pluginExport = plugin->Connect();
	if( pluginExport == NULL ) {
		plugin->Unload();
		// fixme: error message
		return NULL;
	}
	if( pluginExport->GetApiVersion() != pluginImport->GetApiVersion() ) {
		plugin->Unload();
		// fixme: error message
		return NULL;
	}
	pluginExport->OnLoad( pluginImport );
	loadedPlugins.Append(plugin);
	return plugin->GetExport();
}

/*
================
Plugin::Unload
================
*/
void Plugin::Unload( PluginExportBase *pluginExport ) {
	for( int i=0; i<loadedPlugins.Num(); i++ ) {
		if( loadedPlugins[i]->GetExport() == pluginExport ) {
			pluginExport->OnUnload();
			loadedPlugins[i]->Unload();
			loadedPlugins.Remove(i);
			return;
		}
	}
}

}
