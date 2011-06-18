// ==============================================================================
//! @file
//! @brief	File Interface
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

#ifndef __OG_FILE_H__
#define __OG_FILE_H__

#include <time.h>
#include <og/Setup.h>

//! Open Game Libraries
namespace og {
//! @defgroup FileSystem FileSystem (Library)
//! @{

	// ==============================================================================
	//! File read write error
	//!
	//! Thrown on file I/O errors
	// ==============================================================================
	class FileReadWriteError {
	public:
		// ==============================================================================
		//! The currently used error type ids
		// ==============================================================================
		enum ErrorType {
			CUSTOM,			//!< Set if you use the string constructor. Do not use manually
			DECOMPRESS,		//!< Error while decompressing a zip file
			READ,			//!< Read failed
			WRITE,			//!< Writing failed
			REWIND,			//!< Rewind failed
			SEEK,			//!< Seek failed
			FLUSH,			//!< Couldn't flush data (writing to a file)
			END_OF_FILE		//!< Unexpected end of file
		};
		ErrorType type;			//!< The error type id
		String customMsg;	//!< The custom error message

		// ==============================================================================
		//! Constructor
		//!
		//! @param	msg		The custom error message
		// ==============================================================================
		FileReadWriteError( const char *msg );

		// ==============================================================================
		//! Constructor
		//!
		//! @param	type	The error type id
		// ==============================================================================
		FileReadWriteError( ErrorType type );

		// ==============================================================================
		//! Get a string representation of the error type
		//! @return	A string representation of the error type
		// ==============================================================================
		const char *ToString( void );
	};

	// ==============================================================================
	//! File interface
	//!
	//! Used to read from or write to a file.
	// ==============================================================================
	class File {
	public:
		// ==============================================================================
		//! Virtual Destructor
		// ==============================================================================
		virtual ~File() {}

		// ==============================================================================
		//! Close the file
		//!
		//! Cleans up everything, beware after calling this, the file pointer is no longer valid!
		// ==============================================================================
		virtual void	Close( void ) = 0;

		// ==============================================================================
		//! Jump to a position
		//!
		//! @param	offset	Offset from origin.
		//! @param	origin	SEEK_SET, SEEK_CUR or SEEK_END
		// ==============================================================================
		virtual void	Seek( long offset, int origin ) = 0;

		// ==============================================================================
		//! Returns read/write position
		//!
		//! @return	Current position in file
		// ==============================================================================
		virtual long	Tell( void ) = 0;

		// ==============================================================================
		//! Returns the filesize
		//!
		//! @return	filesize
		// ==============================================================================
		virtual long	Size( void ) = 0;

		// ==============================================================================
		//! Rewind to the beginning
		// ==============================================================================
		virtual void	Rewind( void ) = 0;

		// ==============================================================================
		//! Buffered write data will be written to the file.
		// ==============================================================================
		virtual void	Flush( void ) = 0;

		// ==============================================================================
		//! Do automatic flushing after each write
		//!
		//!  @param enable true to enable auto flush
		// ==============================================================================
		virtual void	SetAutoFlush( bool enable ) = 0;

		// ==============================================================================
		//! Check if we are at the end of the file
		//!
		//! @return	true if it at end of file, otherwise false
		// ==============================================================================
		virtual bool	Eof( void ) = 0;

		// ==============================================================================
		//! Reads from the file.
		//!
		//! @param	buffer	Buffer to read into
		//! @param	len		Number of bytes to read
		// ==============================================================================
		virtual void	Read( void *buffer, uInt len ) = 0;

		// ==============================================================================
		//! Writes to the file
		//!
		//! @param	buffer	Data to write
		//! @param	len		Number of bytes to write
		// ==============================================================================
		virtual void	Write( const void *buffer, uInt len ) = 0;

	// Reading Data (Endian independent -> file must be little endian)
		// ==============================================================================
		//! Read an integer
		//!
		//! @return Integer read from file
		//! @exception FileReadWriteError	Thrown when not enough data available
		// ==============================================================================
		int				ReadInt( void );

		// ==============================================================================
		//! Read an integer array
		//!
		//! @param values	Integer array
		//! @param num		Number of integers to read
		//!
		//! @exception FileReadWriteError	Thrown when not enough data available
		// ==============================================================================
		void			ReadIntArray( int *values, int num );

		// ==============================================================================
		//! Read an unsigned integer
		//!
		//! @return Unsigned integer read from file
		//!
		//! @exception FileReadWriteError	Thrown when not enough data available
		// ==============================================================================
		uInt			ReadUint( void );

		// ==============================================================================
		//! Read a short
		//!
		//! @return Short read from file
		//!
		//! @exception FileReadWriteError	Thrown when not enough data available
		// ==============================================================================
		short			ReadShort( void );

		// ==============================================================================
		//! Read a short array
		//!
		//! @param values	Short array
		//! @param num		Number of shorts to read
		//!
		//! @exception FileReadWriteError	Thrown when not enough data available
		// ==============================================================================
		void			ReadShortArray( short *values, int num );

		// ==============================================================================
		//! Read an unsigned short
		//!
		//! @return Unsigned short read from file
		//!
		//! @exception FileReadWriteError	Thrown when not enough data available
		// ==============================================================================
		uShort			ReadUshort( void );

		// ==============================================================================
		//! Read a char
		//!
		//! @return Char read from file
		//!
		//! @exception FileReadWriteError	Thrown when not enough data available
		// ==============================================================================
		char			ReadChar( void );

		// ==============================================================================
		//! Read a byte
		//!
		//! @return Byte read from file
		//!
		//! @exception FileReadWriteError	Thrown when not enough data available
		// ==============================================================================
		byte			ReadByte( void );

		// ==============================================================================
		//! Read a float
		//!
		//! @return Float read from file
		//!
		//! @exception FileReadWriteError	Thrown when not enough data available
		// ==============================================================================
		float			ReadFloat( void );

		// ==============================================================================
		//! Read a float
		//!
		//! @param values	Float array
		//! @param num		Number of floats to read
		//!
		//! @exception FileReadWriteError	Thrown when not enough data available
		// ==============================================================================
		void			ReadFloatArray( float *values, int num );

		// ==============================================================================
		//! Read a boolean
		//!
		//! @return Boolean read from file
		//!
		//! @exception FileReadWriteError	Thrown when not enough data available
		// ==============================================================================
		bool			ReadBool( void );


	// Writing Data (Endian independent -> file will be little endian)
		// ==============================================================================
		//! Write an integer
		//!
		//! @param	value	The integer
		//!
		//! @exception FileReadWriteError	Thrown when writing failed
		// ==============================================================================
		void			WriteInt( int value );

		// ==============================================================================
		//! Write an integer array
		//!
		//! @param	values	The integer array
		//! @param	num		Number of integers to write
		//!
		//! @exception FileReadWriteError	Thrown when writing failed
		// ==============================================================================
		void			WriteIntArray( const int *values, int num );

		// ==============================================================================
		//! Write an unsigned integer
		//!
		//! @param	value	The unsigned integer
		//!
		//! @exception FileReadWriteError	Thrown when writing failed
		// ==============================================================================
		void			WriteUint( uInt value );

		// ==============================================================================
		//! Write a short
		//!
		//! @param	value	The short
		//!
		//! @exception FileReadWriteError	Thrown when writing failed
		// ==============================================================================
		void			WriteShort( short value );

		// ==============================================================================
		//! Write an unsigned short
		//!
		//! @param	value	The unsigned short
		//!
		//! @exception FileReadWriteError	Thrown when writing failed
		// ==============================================================================
		void			WriteUshort( uShort value );

		// ==============================================================================
		//! Write a character
		//!
		//! @param	value	The character
		//!
		//! @exception FileReadWriteError	Thrown when writing failed
		// ==============================================================================
		void			WriteChar( char value );

		// ==============================================================================
		//! Write a byte
		//!
		//! @param	value	The byte
		//!
		//! @exception FileReadWriteError	Thrown when writing failed
		// ==============================================================================
		void			WriteByte( byte value );

		// ==============================================================================
		//! Write a float
		//!
		//! @param	value	The float
		//!
		//! @exception FileReadWriteError	Thrown when writing failed
		// ==============================================================================
		void			WriteFloat( float value );

		// ==============================================================================
		//! Write a float array
		//!
		//! @param	values	The float array
		//! @param	num		Number of floats to write
		//!
		//! @exception FileReadWriteError	Thrown when writing failed
		// ==============================================================================
		void			WriteFloatArray( const float *values, int num );

		// ==============================================================================
		//! Write a boolean
		//!
		//! @param	value	The boolean
		//!
		//! @exception FileReadWriteError	Thrown when writing failed
		// ==============================================================================
		void			WriteBool( bool value );

		// ==============================================================================
		//! Write a (null-terminated) c-string
		//!
		//! @param	value	The c-string
		//!
		//! @exception FileReadWriteError	Thrown when writing failed
		// ==============================================================================
		void			WriteCStr( const char *value );

		// ==============================================================================
		//! Get the filename
		//!
		//! @return	The filename
		// ==============================================================================
		virtual const char *GetFileName( void ) = 0;

		// ==============================================================================
		//! Get the full filepath
		//!
		//! @return	The path + filename
		// ==============================================================================
		virtual const char *GetFullPath( void ) = 0;

		// ==============================================================================
		//! Get which pak file this file is in ( empty if none )
		//!
		//! @return	The path to the pak file
		// ==============================================================================
		virtual const char *GetPakFileName( void ) = 0;

		// ==============================================================================
		//! Get the file modification time
		//!
		//! @return	The time in seconds (since 1.1.1970), when the file has been modified last
		// ==============================================================================
		virtual time_t	GetTime( void ) = 0;

	private:
		byte		endianBuf[4];	// buffer for reading/writing endian independent
	};
	//! @}
}
#endif
