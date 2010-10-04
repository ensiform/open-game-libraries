// ==============================================================================
//! @file
//! @brief	Fowler/Noll/Vo Hash algorithm
//! @author	chongo <Landon Curt Noll>
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2007-2010 Lusito Software
//! @see	http://www.isthe.com/chongo/tech/comp/fnv/
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

#include <og/Common/Common.h>
#include <ctype.h>

namespace og {

/*
================
Hash::Compute32
================
*/
uInt Hash::Compute32( const char *value, bool caseSensitive ) {
	uInt hval = 0x811c9dc5;
	const byte *s = reinterpret_cast<const byte *>(value);
	if ( caseSensitive ) {
		while ( *s != '\0' ) {
			hval ^= static_cast<uInt>(*s++);
			hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
		}
	} else {
		while ( *s != '\0' ) {
			hval ^= static_cast<uInt>(tolower(*s++));
			hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
		}
	}
	return hval;
}

/*
================
Hash::Compute64
================
*/
uLongLong Hash::Compute64( const char *value, bool caseSensitive ) {
	uLongLong hval = ((uLongLong)0x84222325 << 32 | (uLongLong)0x1b3);
	const byte *s = reinterpret_cast<const byte *>(value);
	if ( caseSensitive ) {
		while ( *s != '\0' ) {
			hval ^= static_cast<uLongLong>(*s++);
			hval += (hval<<1) + (hval<<4) + (hval<<5) + (hval<<7) + (hval<<8) + (hval<<40);
		}
	} else {
		while ( *s != '\0' ) {
			hval ^= static_cast<uLongLong>(tolower(*s++));
			hval += (hval<<1) + (hval<<4) + (hval<<5) + (hval<<7) + (hval<<8) + (hval<<40);
		}
	}
	return hval;
}

}
