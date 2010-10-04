/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Text Streaming & Formatting
-----------------------------------------

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
===========================================================================
*/

#include <og/Common/Common.h>
#include <stdarg.h>

namespace og {

/*
================
vsnPrintf
================
*/
int vsnPrintf( char *dest, int size, const char *fmt, va_list list ) {
	int ret = vsnprintf(dest, size, fmt, list);
	dest[size-1] = '\0';
	return (ret >= 0 && ret < size) ? ret : -1;
}

/*
================
TS::Reset
================
*/
void TS::Reset( bool keep, const char *fmt ) {
	floatPrecision = -1;
	fillChar = ' ';

	if ( keep ) {
		if ( !hasFormat ) {
			offset = 0;
			paramCount = -1;
		} else {
			offset = strlen( fmtBuffer );
			memcpy( buffer, fmtBuffer, offset );
			if ( paramCount > 0 )
				paramCount = 0;
			int max = fmtList.Num();
			for( int i=0; i<max; i++ ) {
				if ( fmtList[i].takeInput == 1 )
					fmtList[i].takeInput = 2;
			}
		}
		buffer[offset] = '\0';
		return;
	}
	fmtList.Clear();

	hasFormat = (fmt != NULL);
	if ( !hasFormat ) {
		offset = 0;
		paramCount = -1;
		fmtBuffer[0] = '\0';
	} else {
		strcpy( fmtBuffer, fmt );
		fmtBuffer[255] = '\0';

		int end;
		offset = -1;
		for( int r=0, w=0; fmtBuffer[r] != '\0'; r++, w++ ) {
			if ( fmtBuffer[r] == '$' ) {
				if ( fmtBuffer[r+1] == '$' )
					r++;
				else {
					fmtEntry_t &fmt = fmtList.Alloc();
					fmt.fieldWidth = 0;
					end = r;
					if ( fmtBuffer[r+1] == '?' ) {
						fmt.takeInput = 2;
						r++;
					} else if ( fmtBuffer[r+1] == '+' || fmtBuffer[r+1] == '-' ) {
						bool negative = fmtBuffer[r+1] == '-';
						fmt.takeInput = 0;
						for ( r += 2; String::IsDigit(fmtBuffer[r]); r++ )
							fmt.fieldWidth = 10 * fmt.fieldWidth + (fmtBuffer[r] - '0');
						if ( negative )
							fmt.fieldWidth *= -1;
						r--;
					}
					if ( fmtBuffer[r+1] == '*' ) {
						fmtBuffer[w] = '\0';
						if ( offset == -1 ) {
							memcpy( buffer, fmtBuffer, r );
							offset = end;
						}
						r++;
						fmt.append = fmtBuffer + r + 1;
						w = r;
						continue;
					}
					OG_ASSERT( false );
					//! @todo	error ?
				}
			}
			fmtBuffer[w] = fmtBuffer[r];
		}
		// This assert gets triggered when you have no parameters specified in the format.
		//! @todo	add an error here ?
		OG_ASSERT( offset != -1 );
		if ( offset != -1 ) {
			paramCount = 0;
		} else {
			// no params specified, act like this is the first parameter
			offset = strlen( fmtBuffer );
			memcpy( buffer, fmtBuffer, offset );
			paramCount = -1;
		}
	}
	buffer[offset] = '\0';
}

/*
================
TS::TryPrint
================
*/
void TS::TryPrint( const char *fmt, ... ) {
	int size = bufferSize - offset;
	if ( size > 1 ) {
		va_list	list;
		va_start(list, fmt);
		int ret = vsnPrintf( buffer+offset, size, fmt, list );
		if ( ret > 0 ) {
			int fieldWidth = (paramCount == -1 || paramCount >= fmtList.Num()) ? 0 : fmtList[paramCount].fieldWidth;
			bool alignLeft = true;
			if ( fieldWidth < 0 ) {
				fieldWidth *= -1;
				alignLeft = false;
			}
			if ( ret >= fieldWidth )
				offset += ret;
			else {
				if ( !alignLeft ) {
					int max = Math::Min( offset + fieldWidth - ret, bufferSize-1 );
					for( ; offset < max; offset++ )
						buffer[offset] = fillChar;
					size = bufferSize - offset;
					if ( size > 1 ) {
						ret = vsnPrintf( buffer+offset, size, fmt, list );
						if ( ret > 0 )
							offset += ret;
					}
				} else {
					offset += ret;
					int max = Math::Min( offset + fieldWidth - ret, bufferSize-1 );
					for( ; offset < max; offset++ )
						buffer[offset] = fillChar;
					buffer[offset] = '\0';
				}
			}
		}
		va_end(list);
		if ( ret == -1 )
			User::Error( ERR_BUFFER_OVERFLOW, "TS::TryPrint", TS() << size );
	}
}

/*
================
TS::OnAppend
================
*/
void TS::OnAppend( void ) {
	// This assert gets triggered when you add more parameters than where specified in the format.
	//! @todo	add an error here ?
	OG_ASSERT( paramCount < fmtList.Num() );
	if ( paramCount < fmtList.Num() ) {
		paramCount++;
		TryPrint( "%s", fmtList[paramCount-1] );
	}
}

/*
================
TS::CheckVariableInput
================
*/
bool TS::CheckVariableInput( void ) {
	// This assert gets triggered when you add more parameters than where specified in the format.
	//! @todo	add an error here ?
	OG_ASSERT( paramCount != -1 && paramCount < fmtList.Num() );
	if ( paramCount < fmtList.Num() ) {
		if ( fmtList[paramCount].takeInput == 2 ) {
			fmtList[paramCount].takeInput = 1;
			return true;
		}
	}
	return false;
}

/*
================
TS::operator <<
================
*/
TS &TS::operator << ( int value ) {
	if ( paramCount != -1 && CheckVariableInput() ) {
		fmtList[paramCount].fieldWidth = value;
		return *this;
	}
	TryPrint( "%d", value );
	return Finish();
}

TS &TS::operator << ( uInt value ) {
	if ( paramCount != -1 && CheckVariableInput() ) {
		fmtList[paramCount].fieldWidth = value;
		return *this;
	}
	TryPrint( "%u", value );
	return Finish();
}

TS &TS::operator << ( short value ) {
	if ( paramCount != -1 && CheckVariableInput() ) {
		fmtList[paramCount].fieldWidth = value;
		return *this;
	}
	TryPrint( "%d", value );
	return Finish();
}

TS &TS::operator << ( uShort value ) {
	if ( paramCount != -1 && CheckVariableInput() ) {
		fmtList[paramCount].fieldWidth = value;
		return *this;
	}
	TryPrint( "%u", value );
	return Finish();
}

TS &TS::operator << ( char value ) {
	TryPrint( "%c", value );
	return Finish();
}

TS &TS::operator << ( byte value ) {
	TryPrint( "%u", value );
	return Finish();
}

TS &TS::operator << ( float value ) {
	if ( floatPrecision >= 0 )
		TryPrint( "%.*f", floatPrecision, value );
	else
		TryPrint( "%f", value );
	return Finish();
}

TS &TS::operator << ( const char *value ) {
	TryPrint( "%s", value );
	return Finish();
}

TS &TS::operator << ( const String &value ) {
	TryPrint( "%s", value.c_str() );
	return Finish();
}

TS &TS::operator << ( const Vec2 &value ) {
	if ( floatPrecision >= 0 )
		TryPrint( "%.*f %.*f",
				floatPrecision, value.x, floatPrecision, value.y );
	else
		TryPrint( "%f %f",
				value.x, value.y );
	return Finish();
}

TS &TS::operator << ( const Vec3 &value ) {
	if ( floatPrecision >= 0 )
		TryPrint( "%.*f %.*f %.*f",
				floatPrecision, value.x, floatPrecision, value.y, floatPrecision, value.z );
	else
		TryPrint( "%f %f %f",
				value.x, value.y, value.z );
	return Finish();
}

TS &TS::operator << ( const Vec4 &value ) {
	if ( floatPrecision >= 0 )
		TryPrint( "%.*f %.*f %.*f %.*f",
				floatPrecision, value.x, floatPrecision, value.y, floatPrecision, value.z, floatPrecision, value.w );
	else
		TryPrint( "%f %f %f %f",
				value.x, value.y, value.z, value.w );
	return Finish();
}

TS &TS::operator << ( const Angles &value ) {
	if ( floatPrecision >= 0 )
		TryPrint( "%.*f %.*f %.*f",
				floatPrecision, value.pitch, floatPrecision, value.yaw, floatPrecision, value.roll );
	else
		TryPrint( "%f %f %f",
				value.pitch, value.yaw, value.roll );
	return Finish();
}

TS &TS::operator << ( const Rect &value ) {
	if ( floatPrecision >= 0 )
		TryPrint( "%.*f %.*f %.*f %.*f",
				floatPrecision, value.x, floatPrecision, value.y, floatPrecision, value.w, floatPrecision, value.h );
	else
		TryPrint( "%f %f %f %f",
				value.x, value.y, value.w, value.h );
	return Finish();
}

TS &TS::operator << ( const Quat &value ) {
	if ( floatPrecision >= 0 )
		TryPrint( "%.*f %.*f %.*f %.*f",
				floatPrecision, value.x, floatPrecision, value.y, floatPrecision, value.z, floatPrecision, value.w );
	else
		TryPrint( "%f %f %f %f",
				value.x, value.y, value.z, value.w );
	return Finish();
}

TS &TS::operator << ( const Mat2 &value ) {
	if ( floatPrecision >= 0 )
		TryPrint( "%.*f %.*f %.*f %.*f",
				floatPrecision, value[0].x, floatPrecision, value[0].y, floatPrecision, value[1].x, floatPrecision, value[1].y );
	else
		TryPrint( "%f %f %f %f",
				value[0].x, value[0].y, value[1].x, value[1].y );
	return Finish();
}

TS &TS::operator << ( const Mat3 &value ) {
	if ( floatPrecision >= 0 )
		TryPrint( "%.*f %.*f %.*f %.*f %.*f %.*f %.*f %.*f %.*f",
				floatPrecision, value[0].x, floatPrecision, value[0].y, floatPrecision, value[0].z,
				floatPrecision, value[1].x, floatPrecision, value[1].y, floatPrecision, value[1].z,
				floatPrecision, value[2].x, floatPrecision, value[2].y, floatPrecision, value[2].z );
	else
		TryPrint( "%f %f %f %f %f %f %f %f %f",
				value[0].x, value[0].y, value[0].z,
				value[1].x, value[1].y, value[1].z,
				value[2].x, value[2].y, value[2].z );
	return Finish();
}

TS &TS::operator << ( const Color &value ) {
	byte r, g, b, a;
	value.ToBytes( r, g, b, a );
	TryPrint( "#%X%X%X%X", r, g, b, a );
	return Finish();
}

}
