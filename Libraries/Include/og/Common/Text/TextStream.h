// ==============================================================================
//! @file
//! @brief	Text Streaming & Formatting
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

#ifndef __OG_TEXT_STREAM_H__
#define __OG_TEXT_STREAM_H__

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

//! @defgroup CommonText Text
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
		friend class TS;
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
		friend class TS;
		char	ch;		//!< The new fill character
	};

	// ==============================================================================
	//! Text Stream
	//!
	//! Type safe alternative to VA ( sprintf ).
	//! @note
	//! @li $*		=> Non-formatted value
	//! @li $+23*	=> Left aligned text, fill up 23 fillChars on the right
	//! @li $-23*	=> Right aligned text, fill up 23 fillChars on the left
	//! @li $?*		=> Accepts an extra input value, but works like the 2 above
	// ==============================================================================
	class TS {
	public:
		// ==============================================================================
		//! Constructor
		//!
		//! @param	fmt	Describes the format to use
		// ==============================================================================
		TS( const char *fmt = NULL ) { Reset( false, fmt ); }

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
		//! Get the formatted buffer
		//!
		//! @return	Pointer to the formatted buffer
		// ==============================================================================
		const char *c_str( void )const { return buffer; }

		// Return the formated buffer
		operator const char *( void ) const { return c_str(); }

		// Fill with input
		TS &operator << ( int value );
		TS &operator << ( uInt value );
		TS &operator << ( short value );
		TS &operator << ( uShort value );
		TS &operator << ( char value );
		TS &operator << ( byte value );
		TS &operator << ( float value );
		TS &operator << ( const char *value );
		TS &operator << ( const String &value );
		TS &operator << ( const Vec2 &value );
		TS &operator << ( const Vec3 &value );
		TS &operator << ( const Vec4 &value );
		TS &operator << ( const Angles &value );
		TS &operator << ( const Rect &value );
		TS &operator << ( const Quat &value );
		TS &operator << ( const Mat2 &value );
		TS &operator << ( const Mat3 &value );
		TS &operator << ( const Color &value );

		// Manipulators
		TS &operator << ( const SetFill &value ) { fillChar = value.ch; return *this; }
		TS &operator << ( const SetPrecision &value ) { floatPrecision = value.floatPrecision; return *this; }

	protected:

		// ==============================================================================
		//! Finished the formating after appending a value
		// ==============================================================================
		void	OnAppend( void );
		
		// ==============================================================================
		//! Check for a variable input ( currently only for fieldWidth filling )
		// ==============================================================================
		bool	CheckVariableInput( void );

		// ==============================================================================
		//! Try to print some value into the buffer, call Error if failed
		//!
		//! @param	fmt	Describes the format to use
		// ==============================================================================
		void	TryPrint( const char *fmt, ... );

		// ==============================================================================
		//! Finishes appending a value
		//!
		//! @return	
		// ==============================================================================
		TS &	Finish( void ) {
			floatPrecision = -1;
			if ( paramCount != -1 )
				OnAppend();
			return *this;
		}

		// ==============================================================================
		//! Format entry, used to define what input is expected and what to append after it
		//!
		//! @todo	Move float precision here too ?
		// ==============================================================================
		struct fmtEntry_t {
			const char *append;		//!< What to append after getting input
			int			fieldWidth;	//!< Field width ( fill up the rest with fillChar, align right if negative )
			int			takeInput;	//!< Accept variable input instead of a fixed value
		};

		static const int bufferSize = 16384;	//!< Size of the buffer
		char	buffer[bufferSize];	//!< The buffer
		int		offset;				//!< The offset into the buffer

		char	fmtBuffer[256];		//!< Format buffer
		List<fmtEntry_t>fmtList;	//!< Pointers into the format buffer
		bool	hasFormat;			//!< true if it has format
		int		paramCount;			//!< Number of parameters to be expected
		char	fillChar;			//!< The character to use to fill up

		int		floatPrecision;		//!< The float precision
	};

//! @}
//! @}
}

#endif
