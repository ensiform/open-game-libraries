// ==============================================================================
//! @file
//! @brief	Public Gloot Interface (Input only)
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2002-2006 Camilla Berglund
//! @note	Copyright (C) 2007-2010 Lusito Software
//! @note	Some of the code is based on GLFW 2.6 (See Readme.txt)
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

#ifndef __GLOOT_INPUT_H__
#define __GLOOT_INPUT_H__

#include <og/Setup.h>

//! Open Game Libraries
namespace og {
	//! @defgroup Gloot Gloot (Library)
	//! @{

	//! @defgroup GlootInput Gloot Input
	//! @{

	//! Keyboard special keys
	namespace Key {
		enum {
			UNKNOWN			= -1,
			MAX_SPECIALKEYS	= 256,

			// Special Keys are negative so they are easily identified, and there is no collision with unicode keys.
			// Function Keys
			F1				= -MAX_SPECIALKEYS,
			F2,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			F10,
			F11,
			F12,
			F13,
			F14,
			F15,
			F16,
			F17,
			F18,
			F19,
			F20,
			F21,
			F22,
			F23,
			F24,
			F25,

			// 3 Block (non-mac)
			PRINT,
			SCROLL,
			PAUSE,

			// 6 Block
			INSERT,
			DEL,
			HOME,
			END,
			PAGEUP,
			PAGEDOWN,

			// Arrow Keys
			UP,
			DOWN,
			LEFT,
			RIGHT,

			// Keypad
			KP_0,
			KP_1,
			KP_2,
			KP_3,
			KP_4,
			KP_5,
			KP_6,
			KP_7,
			KP_8,
			KP_9,
			KP_NUMLOCK,
			KP_EQUAL,
			KP_DIVIDE,
			KP_MULTIPLY,
			KP_SUBTRACT,
			KP_ADD,
			KP_ENTER,
			KP_DECIMAL,

			// Modifiers & Menu Keys
			LSHIFT,
			RSHIFT,
			LCTRL,
			RCTRL,
			LALT,
			RALT,
			LSUPER,
			RSUPER,
			MENU,

			// Misc Keys
			ESC,
			TAB,
			CAPSLOCK,
			ENTER,
			BACKSPACE,
			SPACE,

			LAST
		};
	}

	//! Mouse buttons
	namespace Mouse {
		enum {
			Button1,
			Button2,
			Button3,
			Button4,
			Button5,
			Button6,
			Button7,
			Button8,

			MaxButtons,

			// Alternate names:
			ButtonLeft		= Button1,
			ButtonRight		= Button2,
			ButtonMiddle	= Button3,
		};
	}
//! @}
//! @}
}

#endif
