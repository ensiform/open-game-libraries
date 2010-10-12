// ==============================================================================
//! @file
//! @brief	Text Formatting
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

#ifndef __OG_FORMAT_H__
#define __OG_FORMAT_H__

//! Open Game Libraries
namespace og {
//! @defgroup Shared Shared (Library)
//! @{

	// ==============================================================================
	//! Set precision
	// ==============================================================================
	class SetPrecision {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	value	The precision value for the next input
		// ==============================================================================
		SetPrecision( int value ) : floatPrecision(value) {}

	private:
		friend class Format;
		int floatPrecision;	//!< The float precision
	};

	// ==============================================================================
	//! Set fill character
	// ==============================================================================
	class SetFill {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	fillCh	The new fill character
		// ==============================================================================
		SetFill( char fillCh ) : ch(fillCh) {}

	private:
		friend class Format;
		char	ch;		//!< The new fill character
	};

	// ==============================================================================
	//! Format
	//!
	//! Type safe alternative to VA ( sprintf ).
	//! @note
	//! @li $*		=> Non-formatted value
	//! @li $+23*	=> Left aligned text, fill up 23 fillChars on the right
	//! @li $-23*	=> Right aligned text, fill up 23 fillChars on the left
	//! @li $?*		=> Accepts an extra input value, but works like the 2 above
	// ==============================================================================
	class Format {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	fmt	Describes the format to use
		// ==============================================================================
		Format( const char *fmt = NULL );

		// ==============================================================================
		//! Destructor
		// ==============================================================================
		~Format();
		
		// ==============================================================================
		//! Reset the values already entered and (optional) the format
		//!
		//! @param	keep	true to keep the old format
		//! @param	fmt		Describes the format to use
		// ==============================================================================
		void	Reset( bool keep = true, const char *fmt = NULL );

		// ==============================================================================
		//! Byte length
		//!
		//! @return	How many bytes have been written already
		// ==============================================================================
		int		ByteLength( void ) { return offset; }

		// ==============================================================================
		//! GetFloatPrecision
		//!
		//! @return	The float precision
		// ==============================================================================
		int		GetFloatPrecision( void ) { return floatPrecision; }

		// ==============================================================================
		//! Get the formatted buffer
		//!
		//! @return	Pointer to the formatted buffer
		// ==============================================================================
		const char *c_str( void )const { return buffer; }

		// Return the formated buffer
		operator const char *( void ) const { return c_str(); }

		// ==============================================================================
		//! Try to print some value into the buffer, call Error if failed
		//!
		//! @note	Make sure you know what this is before you call it!
		//! @param	fmt	Describes the format to use
		// ==============================================================================
		void	TryPrint( const char *fmt, ... );

		// ==============================================================================
		//! Finishes appending a value
		//!
		//! @note	Make sure you know what this is before you call it!
		//! @return	self
		// ==============================================================================
		Format &	Finish( void ) {
			floatPrecision = -1;
			if ( paramCount != -1 )
				OnAppend();
			return *this;
		}

		// Fill with input
		Format &operator << ( int value );
		Format &operator << ( uInt value );
		Format &operator << ( short value );
		Format &operator << ( uShort value );
		Format &operator << ( char value );
		Format &operator << ( byte value );
		Format &operator << ( float value );
		Format &operator << ( const char *value );

		// Manipulators
		Format &operator << ( const SetFill &value ) { fillChar = value.ch; return *this; }
		Format &operator << ( const SetPrecision &value ) { floatPrecision = value.floatPrecision; return *this; }

	protected:

		// ==============================================================================
		//! Remove all format entries
		// ==============================================================================
		void	DeleteFormatEntries( void );

		// ==============================================================================
		//! Finished the formating after appending a value
		// ==============================================================================
		void	OnAppend( void );
		
		// ==============================================================================
		//! Check for a variable input ( currently only for fieldWidth filling )
		// ==============================================================================
		bool	CheckVariableInput( void );

		// ==============================================================================
		//! Format entry, used to define what input is expected and what to append after it
		//!
		//! @todo	Move float precision here too ?
		// ==============================================================================
		struct FormatEntry {
			FormatEntry() : append(NULL), fieldWidth(0), takeInput(0), next(NULL) {}

			const char *append;		//!< What to append after getting input
			int			fieldWidth;	//!< Field width ( fill up the rest with fillChar, align right if negative )
			int			takeInput;	//!< Accept variable input instead of a fixed value
			FormatEntry *next;		//!< The next format entry
		};
		FormatEntry *GetFormatEntry( int index );

		static const int bufferSize = 16384;	//!< Size of the buffer
		char	buffer[bufferSize];	//!< The buffer
		int		offset;				//!< The offset into the buffer

		char	fmtBuffer[256];		//!< Format buffer
		FormatEntry *firstFormatEntry;//!< First format entry
		FormatEntry *lastFormatEntry;//!< Last format entry
		int		numFormatEntries;	//!< Number of format entries
		bool	hasFormat;			//!< true if it has format
		int		paramCount;			//!< Number of parameters to be expected
		char	fillChar;			//!< The character to use to fill up

		int		floatPrecision;		//!< The float precision
	};

//! @}
}

#endif
