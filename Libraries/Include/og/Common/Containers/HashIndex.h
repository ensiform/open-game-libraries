// ==============================================================================
//! @file
//! @brief	Hash Index ( Fast access to string based indices )
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

#ifndef __OG_HASHINDEX_H__
#define __OG_HASHINDEX_H__

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	// ==============================================================================
	//! Hash index
	// ==============================================================================
	class HashIndex {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	hashSize	Size of the hash
		// ==============================================================================
		HashIndex( int hashSize=128 );

		// ==============================================================================
		//! Destructor
		// ==============================================================================
		~HashIndex();

		// ==============================================================================
		//! Free all memory and reset to new size
		//!
		//! @param	newHashSize	Size of the new hash
		// ==============================================================================
		void	Clear( int newHashSize=-1 );

		// ==============================================================================
		//! Add a new entry
		//!
		//! @param	hash	The hash entry
		//! @param	index	Zero-based index
		// ==============================================================================
		void	Add( int hash, int index );

		// ==============================================================================
		//! Remove this entry
		//!
		//! @param	hash	The hash entry
		//! @param	index	Zero-based index
		// ==============================================================================
		void	Remove( int hash, int index );

		// ==============================================================================
		//! Get the first entry with the specified hash value
		//!
		//! @param	hash	The hash value
		//!
		//! @return	-1 if not found, otherwise the index of the first match
		// ==============================================================================
		int		First( int hash ) const;

		// ==============================================================================
		//! Get the next entry
		//!
		//! @return	-1 if not found, otherwise the index of the first match
		//! @see	First
		// ==============================================================================
		int		Next( void ) const;

		// ==============================================================================
		//! Generate a hash key
		//!
		//! @param	value			The string to hash
		//! @param	caseSensitive	true to create a case sensitive hash
		//!
		//! @return	The key. 
		// ==============================================================================
		static int GenerateKey( const char *value, bool caseSensitive );

	private:

		// ==============================================================================
		//! Initialize the hash list
		// ==============================================================================
		void	Init( void );

		int		hashSize;		//!< Size of the hash list, bigger hashsize = less collisions, but more memory
		int		mask;			//!< The hash mask
		bool	initialized;	//!< true when the hash list is initialized

		// ==============================================================================
		//! Hash index node
		// ==============================================================================
		struct HashIndexNode_s {
			HashIndexNode_s *next;
			int				index;
		};
		HashIndexNode_s **nodeList;			//!< List of nodelists
		List<HashIndexNode_s *> allnodes;	//!< List of all nodes
		mutable HashIndexNode_s *findNode;	//!< The find node
	};
//! @}
}

#endif
