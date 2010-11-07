// ==============================================================================
//! @file
//! @brief	Thread Local Storage
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

#ifndef __OG_THREAD_LOCAL_STORAGE_H__
#define __OG_THREAD_LOCAL_STORAGE_H__

#include <og/Shared.h>

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	// ==============================================================================
	//! TLS index
	// ==============================================================================
	class TLS_Index {
	public:
		// ==============================================================================
		//! Default constructor
		// ==============================================================================
		TLS_Index();

		// ==============================================================================
		//! Destructor
		// ==============================================================================
		~TLS_Index();

		// ==============================================================================
		//! Find out if the index allocation succeeded
		//!
		//! @return	true if a valid index has been allocated
		// ==============================================================================
		bool	IsValid( void ) const { return data != NULL; }

		// ==============================================================================
		//! Gets the value
		//!
		//! @return	The value of this index
		// ==============================================================================
		void *	GetValue( void ) const;

		// ==============================================================================
		//! Set the value
		//!
		//! @param	value	The new value
		// ==============================================================================
		void	SetValue( void *value ) const;

	private:
		void	*data;	//!< A pointer to the platform specific index data
	};

	// ==============================================================================
	//! Basic TLS data
	// ==============================================================================
	class TLS_Data {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	index	Pointer to the valid! TLS index
		// ==============================================================================
		TLS_Data( const TLS_Index *index );

		// ==============================================================================
		//! Destructor
		//! 
		//! Also deletes previous
		// ==============================================================================
		virtual ~TLS_Data();

	protected:
		friend void TLS_AtExit( void );
		friend class Thread;

		// ==============================================================================
		//! Register a new TLS_Data object
		//!
		//! @param	data	The TLS_Data object
		// ==============================================================================
		static void Register( TLS_Data *data );

		// ==============================================================================
		//! Delete all TLS data
		// ==============================================================================
		static void Cleanup( void );

	private:
		TLS_Data *			previous;	//!< The previous data object ( for chain deletion )
		const TLS_Index *	tlsIndex;	//!< The TLS index
	};

	// ==============================================================================
	//! Thread Local Storage
	//!
	//! @warning	Only use together with og::Thread
	// ==============================================================================
	template<typename type>
	class TLS {
	private:
		// ==============================================================================
		//! Extended TLS data
		// ==============================================================================
		class TLS_DataEx : public TLS_Data {
		public:
			// ==============================================================================
			//! Constructor
			//!
			//! @param	index			Pointer to the valid! TLS index
			//! @param	initialValue	The initial value
			// ==============================================================================
			TLS_DataEx( const TLS_Index *index, type initialValue ) : TLS_Data(index), value(initialValue) {}

		public:
			type value;	//!< The value
		};

	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	initial	The initial value
		// ==============================================================================
		TLS( type initial ) : initialValue(initial) {
			/*if ( !index.IsValid() ) 
				User::Error("TLS allocation failed");*/
		}

		// ==============================================================================
		//! Default constructor
		// ==============================================================================
		TLS() {
			/*if ( !index.IsValid() ) 
				User::Error("TLS allocation failed");*/
		}

		// ==============================================================================
		//! Get the thread local value
		//!
		//! @return	NULL if the TLS index was invalid
		// ==============================================================================
		type *Get( void ) const {
			OG_ASSERT( index.IsValid() );
			if ( index.IsValid() ) {
				TLS_DataEx *data = static_cast<TLS_DataEx *>( index.GetValue() );
				if ( data == NULL )
					data = new TLS_DataEx( &index, initialValue );
				return &data->value;
			}
			return NULL;
		}

		// ==============================================================================
		//! -> Operator ( Short version of Get() )
		//!
		//! @copydoc Get
		// ==============================================================================
		type *operator->( void ) const { return Get(); }

		// ==============================================================================
		//! * Operator ( Short version of *Get() )
		//!
		//! @copydoc Get
		// ==============================================================================
		type &operator*() const { return *Get(); }

	private:
		type		initialValue;	//!< The initial value
		TLS_Index	index;			//!< The TLS index
	};
	//! @}
}

#endif
