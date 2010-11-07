// ==============================================================================
//! @file
//! @brief	Common Library
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2007-2010 Lusito Software
//! @todo	Unit tests ?
//! @todo	Check where Error and Warning Messages would be appropriate
//! @todo	Checks for out of memory stuff?
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

#ifndef __OG_COMMON_H__
#define __OG_COMMON_H__

#include <og/Shared/Shared.h>
#include <og/Shared/FileSystemCore.h>
#include <og/Common/Thread/ThreadLocalStorage.h>

#include <stdio.h>
#include <math.h>

// Public Library Includes
#include "Containers/List.h"
#include "Containers/LinkedList.h"
#include "Containers/Stack.h"
#include "Containers/HashIndex.h"
#include "Containers/StringPool.h"
#include "Containers/Dict.h"
#include "Containers/Pair.h"
#include "Containers/CmdArgs.h"
#include "Containers/DynBuffer.h"
#include "Containers/Allocator.h"
#include "Text/Lexer.h"
#include "Text/DeclParser.h"
#include "Text/XDeclParser.h"

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{
	
	// ==============================================================================
	//! Set the FileSystem for the Common library
	//!
	//! @param	fileSystem	The FileSystem to use
	//!
	//! @note	This is needed to allow file operations within the Common library
	// ==============================================================================
	void	CommonSetFileSystem( FileSystemCore *fileSystem );

	// ==============================================================================
	//! Convert an UTF-8 string to a wchar_t buffer
	//!
	//! @param	in		The UTF-8 input string
	//! @param	buffer	The buffer to store the converted string in
	//!
	//! @return	The size of the new buffer
	// ==============================================================================
	int StringToWide( const char *in, DynBuffer<wchar_t> &buffer );
	
	// ==============================================================================
	//! QuickSort
	//!
	//! @param	base	The start of the target array
	//! @param	num		The array size in elements
	//! @param	width	The element size in bytes
	//! @param	context	A pointer to a context, can be anything you like
	//! @param	compare	The comparison function
	// ==============================================================================
	void QuickSort( void *base, size_t num, size_t width, void *context,
								int ( *compare )(void *, const void *, const void *) );
//! @}
}

// We include .inl files last, so we can access all classes here.
#include "Containers/List.inl"
#include "Containers/LinkedList.inl"
#include "Containers/Stack.inl"
#include "Containers/HashIndex.inl"
#include "Containers/Dict.inl"
#include "Containers/Pair.inl"
#include "Containers/DynBuffer.inl"
#include "Containers/Allocator.inl"

#endif
