// ==============================================================================
//! @file
//! @brief	Configuration / Settings
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

#ifndef __OG_CONFIG_H__
#define __OG_CONFIG_H__

#define OG_HAVE_VISUAL_LEAK_DETECTOR	1	//!< Visual Leak Detector (2.0b) available ( only has effect on visual c++ )
#define OG_HAVE_STD_THREAD				0	//!< C++0x threads available
#define OG_HAVE_USER_ASSERT_FAILED		1	//!< Forward failed asserts in release mode to the user
#define OG_FTOI_USE_SSE					1	//!< Use SSE extensions for Math::Ftoi
#define OG_SHOW_MORE_WARNINGS			0	//!< See more the warnings we disabled on visual c++
											//! In detail that would be: 'signed/unsigned mismatch'
											//! and 'function was declared deprecated'

#endif
