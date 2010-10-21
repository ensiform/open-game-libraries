// ==============================================================================
//! @file
//! @brief	String Pool ( avoiding too many duplicates of a string in memory )
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

#ifndef __OG_STRINGPOOL_H__
#define __OG_STRINGPOOL_H__

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	class StringPool;

	// ==============================================================================
	//! Pool string
	//!
	//! The String object that is used by StringPool
	// ==============================================================================
	class PoolString : public String {
	public:
		friend class StringPool;

		// ==============================================================================
		//! Constructor
		// ==============================================================================
		PoolString() { inUse = 0; }

		// ==============================================================================
		//! Initialize this object
		//!
		//! @param	pool	The parent StringPool
		//! @param	value	The string value
		// ==============================================================================
		void Init( StringPool *pool, const char *value ) {
			inUse = 1;
			this->pool = pool;
			size_t byteLen, len;
			BothLengths( value, &byteLen, &len );
			SetData( value, byteLen, len );
		}

	private:
		StringPool *	pool;	//!< The parent pool
		mutable int		inUse;	//!< How many times this object is in use
	};

	// ==============================================================================
	//! String pool
	//!
	//! Stores strings and optimizes memory usage
	// ==============================================================================
	class StringPool {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	granularity	The list granularity
		// ==============================================================================
		StringPool( int granularity = 1024 ) : strings( granularity ) { caseSensitive = true; }

		// ==============================================================================
		//! Clear all Strings
		// ==============================================================================
		void		Clear( void );

		// ==============================================================================
		//! Set case sensitive mode
		//!
		//! @param	caseSensitive	true to make it case sensitive
		// ==============================================================================
		void		SetCaseSensitive( bool caseSensitive ) { this->caseSensitive = caseSensitive; }

		// ==============================================================================
		//! Get the number of entries
		//!
		//! @return	The current number of entries
		// ==============================================================================
		int			Num( void ) const { return strings.Num(); }

		// ==============================================================================
		//! Returns the matching string object to the value, or a new one if no match exists
		//!
		//! @param	value	The string value
		//!
		//! @return	The string object
		// ==============================================================================
		const PoolString *	Alloc( const char *value );

		// ==============================================================================
		//! Free the poolstring, automaticly destroy the string object if not in use anymore
		//!
		//! @param	poolStr	The string object
		// ==============================================================================
		void				Free( const PoolString *poolStr );

		// ==============================================================================
		//! Same as Alloc, but takes an existing PoolString
		//!
		//! @param	poolStr	The pool string
		//!
		//! @return	The string object
		// ==============================================================================
		const PoolString *	Copy( const PoolString *poolStr );

	private:
		ListEx<PoolString>	strings;		//!< The list of pool strings
		HashIndex			hashIndex;		//!< Hash index
		bool				caseSensitive;	//!< true to be case sensitive
	};
//! @}
}

#endif
