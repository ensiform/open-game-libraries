// ==============================================================================
//! @file
//! @brief	Secure Hash Algorithm (SHA1)
//! @author	Adrien Pinet ( original code )
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2007-2010 Lusito Software
//! @note	Original code by Adrien Pinet, speed improved by over 70%
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

#ifndef __OG_SECUREHASH_H__
#define __OG_SECUREHASH_H__

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

//! @defgroup CommonHash Hashing
//! @{

	// ==============================================================================
	//! Secure Hash Algorithm
	// ==============================================================================
	class SecureHash {
	public:
		// ==============================================================================
		//! Default constructor
		// ==============================================================================
		SecureHash() { Reset(); }

		// ==============================================================================
		//! Reset the Algorithm
		// ==============================================================================
		void	Reset( void );

		// ==============================================================================
		//! Finish the algorithm, calculate the result
		// ==============================================================================
		void	Finish( void );

		// ==============================================================================
		//! Create a hash for the specified file
		//!
		//! @param	fileName	Filename of the file
		//!
		//! @return	true if it succeeds, false if it fails
		//!
		//! @note	Calls Reset and Finish automatically
		// ==============================================================================
		bool	ComputeFile( const char *fileName );

		// ==============================================================================
		//! Add buffer to the hash algorithm
		//!
		//! @param	buffer		The buffer. 
		//! @param	bufferSize	Size of the buffer. 
		//!
		//! @return	true if it succeeds, false if it fails. 
		// ==============================================================================
		bool	AddBuffer( const byte* buffer, uInt bufferSize );

		// ==============================================================================
		//! Get the result as byte array
		//!
		//! @return	Pointer to the byte array
		// ==============================================================================
		byte *	GetByteResult( void ) { return byteResult; }

		// ==============================================================================
		//! Get the result as a hexadecimal string
		//!
		//! @return	Null terminated ascii string
		// ==============================================================================
		char *	GetHexResult( void ) { return hexResult; }

	private:
		// ==============================================================================
		//! Process the message block
		//!
		//! @param	buffer	The buffer. 
		// ==============================================================================
		void	ProcessMessageBlock( const byte* buffer );

		// ==============================================================================
		//! Process the remaining part of the messageBlock
		// ==============================================================================
		void	ProcessRemaining( void );

		uInt		intermediateHash[5];	//!< The intermediate hash
		uLongLong	lengthLow;				//!< The length (low part)
		uLongLong	lengthHigh;				//!< The length (high part)

		uInt		messageBlockSize;		//!< Size of the message block
		byte		messageBlock[64];		//!< The message block

		byte		byteResult[20];			//!< The byte result
		char		hexResult[41];			//!< The hexadecimal result
	};
//! @}
//! @}
}

#endif
