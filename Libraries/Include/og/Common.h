// ==============================================================================
//! @file
//! @brief	Includes for the Common library
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

#ifndef __OG_FULL_COMMON_H__
#define __OG_FULL_COMMON_H__

#include <stdio.h>
#include <math.h>

#include <og/Shared.h>
#include <og/Shared/FileSystemCore.h>

// Public Library Includes
#include <og/Common/Thread/ThreadLocalStorage.h>
#include <og/Common/Containers/List.h>
#include <og/Common/Containers/LinkedList.h>
#include <og/Common/Containers/Stack.h>
#include <og/Common/Containers/HashIndex.h>
#include <og/Common/Containers/StringPool.h>
#include <og/Common/Containers/Dict.h>
#include <og/Common/Containers/Pair.h>
#include <og/Common/Containers/CmdArgs.h>
#include <og/Common/Containers/DynBuffer.h>
#include <og/Common/Containers/Allocator.h>
#include <og/Common/Text/Lexer.h>
#include <og/Common/Text/DeclParser.h>
#include <og/Common/Text/XDeclParser.h>
#include <og/Common/Common.h>

// We include .inl files last, so we can access all classes here.
#include <og/Common/Containers/List.inl>
#include <og/Common/Containers/LinkedList.inl>
#include <og/Common/Containers/Stack.inl>
#include <og/Common/Containers/HashIndex.inl>
#include <og/Common/Containers/Dict.inl>
#include <og/Common/Containers/Pair.inl>
#include <og/Common/Containers/DynBuffer.inl>
#include <og/Common/Containers/Allocator.inl>

#endif
