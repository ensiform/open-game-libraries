// ==============================================================================
//! @file
//! @brief	Unix Plugin Library
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

#ifdef OG_LINUX

#include <dlfcn.h>

namespace og {

#define OG_DECL 
typedef PluginImportBase * ( OG_DECL *plugin_entry_t )( PluginExportBase *pluginExport );

/*
==============================================================================

  PluginUnix

==============================================================================
*/
class PluginUnix : public Plugin {
private:
	void *	handle;
	String	filename;

public:
	PluginUnix( void *handle, const char *filename ) {
		this->handle = handle;
		this->filename = filename;
	}
	PluginImportBase *Connect( const char *name, PluginExportBase *pluginExport ) {
		plugin_entry_t pluginEntry = (plugin_entry_t)dlsym( handle, name ); 
		if( pluginEntry ) {
			return pluginEntry( pluginExport );
		}
		return NULL;
	}
	void Unload() {
		dlclose( dllHandle );
		const char *err = dlerror();
		if ( err != NULL ) {
			//fixme: better error id
			User::Error( ERR_SYSTEM_REQUIREMENTS, "Failed to free library", filename.c_str() );
		}
		delete this;
	}
};

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
Plugin *Plugin::Load( const char *filename ) {
	String filepath = filename;
	filepath += ".so";
	void *handle = dlopen( fn, RTLD_NOW );
	if ( handle ) {
		return new PluginUnix( handle, filepath.c_str() );
	}
	return NULL;
}

}

#endif
