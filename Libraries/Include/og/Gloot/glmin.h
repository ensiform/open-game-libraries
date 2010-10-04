// ==============================================================================
//! @file
//! @brief	Minimal OpenGL Includes
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

#ifndef __OG_GLMIN_H__
#define __OG_GLMIN_H__

#if !defined(_WIN32) && (defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__))
	#define _WIN32
#endif

#ifndef APIENTRY
	#ifdef _WIN32
		#define APIENTRY __stdcall
	#else
		#define APIENTRY
	#endif
#endif

#ifndef NULL
  #define NULL 0
#endif

#if !defined(WINGDIAPI) && defined(_WIN32)
	#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__POCC__)
		#define WINGDIAPI __declspec(dllimport)
	#elif defined(__LCC__)
		#define WINGDIAPI __stdcall
	#else
		#define WINGDIAPI extern
	#endif
#define GL_WINGDIAPI_DEFINED
#endif

#if defined(__APPLE_CC__)
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#endif
