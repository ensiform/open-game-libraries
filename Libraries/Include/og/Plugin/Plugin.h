// ==============================================================================
//! @file
//! @brief	Plugin Library
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

#ifndef __OG_PLUGIN_H__
#define __OG_PLUGIN_H__

//! Open Game Libraries
namespace og {
	//! @defgroup Plugin Plugin (Library)
	//! You need to include <og/Plugin.h>
	//!
	//! @todo remove the need for ogCommon ?
	//! @{

	// ==============================================================================
	//! Plugin Import Base
	//!
	//! The base interface for the plugin to import methods from the executable
	// ==============================================================================
	class PluginImportBase {
	public:
		virtual int GetApiVersion( void ) const = 0;
	};

	// ==============================================================================
	//! Plugin Export Base
	//!
	//! The base interface for the plugin to export methods to the executable
	// ==============================================================================
	class PluginExportBase {
	public:
		virtual int GetApiVersion( void ) const = 0;
		virtual void OnLoad( PluginImportBase *pluginImport ) = 0;
		virtual void OnUnload( void ) = 0;
	};

	// ==============================================================================
	//! Plugin namespace
	//!
	//! Load/Unload Plugins
	// ==============================================================================
	namespace Plugin {
		// ==============================================================================
		//! Connect to the plugin
		//!
		//! @param	filename		The filename of the plugin
		//! @param	pluginImport	The interface the plugin wants to import
		//!
		//! @return	NULL on failure, otherwise the returned PluginExportBase object from the plugin
		// ==============================================================================
		PluginExportBase *Load( const char *filename, PluginImportBase *pluginImport );

		// ==============================================================================
		//! Unload this plugin
		//!
		//! @param	pluginExport	The export object of the plugin to unload
		// ==============================================================================
		void Unload( PluginExportBase *pluginExport );
	}
}

#endif
