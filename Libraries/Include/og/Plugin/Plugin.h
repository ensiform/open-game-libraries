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
	//! The base for the import interface
	// ==============================================================================
	class PluginImportBase {};

	// ==============================================================================
	//! Plugin Export Base
	//!
	//! The base for the export interface
	// ==============================================================================
	class PluginExportBase {};

	// ==============================================================================
	//! Plugin Interface
	//!
	//! Load/Unload Plugins
	// ==============================================================================
	class Plugin {
	public:
		// ==============================================================================
		//! Constructor. 
		// ==============================================================================
		Plugin() {}

		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~Plugin() {}

		// ==============================================================================
		//! Connect to the plugin
		//!
		//! @param	name	The entry point to call
		//!
		//! @return	NULL on failure, otherwise the returned PluginImportBase object from the plugin
		// ==============================================================================
		virtual PluginImportBase *Connect( const char *name, PluginExportBase *pluginExport ) = 0;

		// ==============================================================================
		//! Unload this plugin
		// ==============================================================================
		virtual void Unload() = 0;

		// ==============================================================================
		//! Load a plugin
		//!
		//! @param	filename	The file path
		//!
		//! @return	true if it succeeds, false if it fails
		// ==============================================================================
		static Plugin *	Load( const char *filename );
	};
}

#endif
