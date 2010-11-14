// ==============================================================================
//! @file
//! @brief	OS specific stuff and defines
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

#ifndef __OG_SETUP_H__
#define __OG_SETUP_H__

#include <og/Config.h>

typedef unsigned char		byte;		//!< unsigned char
typedef unsigned short		uShort;		//!< unsigned short
typedef unsigned long		uLong;		//!< unsigned long
typedef unsigned int		uInt;		//!< unsigned int
typedef unsigned long long	uLongLong;	//!< unsigned long long

/*!
@def OG_LITTLE_ENDIAN
@brief Little or big endian architecture

@def		OG_WIN32
@brief		This is set on Windows platforms

@def		OG_LINUX
@brief		This is set on Linux platforms

@def		OG_MACOS_X
@brief		This is set on Mac platforms

@def		OG_ASM_MSVC
@brief		This is set when a Microsoft assembler is used

@def		OG_ASM_GNU
@brief		This is set when a GNU assembler is used

@def		OG_INLINE
@brief		__forceinline on Microsoft compilers, otherwise inline

@def		OG_ASSERT(x)
@brief		Make sure a condition is met
@details	In debug mode it triggers a breakpoint,
in release mode it calls og::User::AssertFailed()
@see		OG_HAVE_USER_ASSERT_FAILED

@def		BIT(num)
@brief		returns an integer with just one specified bit set
*/

// Operating System
#if defined( WIN32 ) || defined( _WIN32 ) || defined( _WINDOWS ) || defined( __WIN32__ )
	#define OG_WIN32 1
#elif defined ( __linux__ )
	#warning "Linux build is not done yet, but we're working on it."
	#define OG_LINUX 1
#elif defined( MACOS_X )
	#error "Sorry, we don't have MAC code yet, if you are interested in helping out, contact us."
	#define OG_MACOS_X 1
#else
	#error "Invalid OS!"
#endif

// Win32 Specific
#if OG_WIN32
	// Pragma warnings for msvc
	#if defined(_MSC_VER)
		#pragma warning( error : 4002 )	// Too many actual parameters for macro: promoted to be an error
		#pragma warning( error : 4715 ) // Not all control paths return a value: promoted to be an error

		// Enable some useful ones that are disabled by default
		// http://msdn2.microsoft.com/en-us/library/23k5d385(VS.80).aspx
		#pragma warning( default : 4062 )	// enumerator 'identifier' in switch of enum 'enumeration' is not handled
		//#pragma warning( default : 4265 )	// class has virtual functions, but destructor is not virtual
		#pragma warning( default : 4431 )	// missing type specifier - int assumed. Note: C no longer supports default-int

		// Disable if these get annoying.
		//#pragma warning( default: 4710 )	// function '...' not inlined
		#pragma warning( default: 4711 )	// function '...' selected for automatic inline expansion

		#if OG_SHOW_MORE_WARNINGS == 0
			#pragma warning( disable : 4389 )	// signed/unsigned mismatch
			#pragma warning( disable : 4018 )	// signed/unsigned mismatch
			#pragma warning( disable : 4996 )	// 'function': was declared deprecated
		#endif
	#endif

	// Visual Leak Detector 2.0a
	#if OG_HAVE_VISUAL_LEAK_DETECTOR && defined(_DEBUG)
		#include <vld.h>
	#endif

	// Fix for MinGW
	#if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0501)
		#undef _WIN32_WINNT
		#define _WIN32_WINNT 0x0501
	#endif
	#if !defined(WINVER) || (WINVER < 0x0501)
		#undef WINVER
		#define WINVER 0x0501
	#endif
#endif

// (Mac OSX && PPC) = big endian, rest = little endian
#if OG_MACOS_X && defined(__ppc__)
	#define OG_LITTLE_ENDIAN 0
#else
	#define OG_LITTLE_ENDIAN 1
#endif

// Which Assembler to use
#if defined(_MSC_VER)
	#define OG_ASM_MSVC 1
#elif defined(__GNUC__)
	#define OG_ASM_GNU 1
#else
	#error "could not find suitable assembler"
#endif

#if defined(_MSC_VER)
	#define OG_INLINE __forceinline
#elif defined(__GNUC__)
	#define OG_INLINE __inline__
#else
	#define OG_INLINE inline
#endif


// define OG_DEBUG_BREAK
#if _MSC_VER
	#define OG_DEBUG_BREAK() { __debugbreak(); }
#elif __GNUC__
	#define OG_DEBUG_BREAK() { __asm__ __volatile__ ("int $0x03"); }
#elif OG_MACOS_X //! @todo	compiler specific ?
	#define OG_DEBUG_BREAK() { kill( getpid(), SIGINT ); }
#endif

// define OG_ASSERT()
#ifdef _DEBUG
	#define OG_ASSERT(x) { if ( !(x) ) OG_DEBUG_BREAK() }
#elif OG_HAVE_USER_ASSERT_FAILED
	#define OG_ASSERT(x) { if ( !(x) ) og::User::AssertFailed( #x, __FUNCTION__ ); }
#else
	#define OG_ASSERT(x) {}
#endif

#ifndef BIT
	#define BIT( num ) ( 1 << ( num ) )
#endif

#ifndef NULL
	#define NULL 0
#endif

#endif
