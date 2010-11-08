/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Basic Library stuff
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

#include <og/Common.h>
#include <stdlib.h>

namespace og {

FileSystemCore *commonFS = NULL;

Format &operator << ( Format &fmt, const String &value ) {
	fmt.TryPrint( "%s", value.c_str() );
	return fmt.Finish();
}

void CommonSetFileSystem( FileSystemCore *fileSystem ) {
	commonFS = fileSystem;
}

int StringToWide( const char *in, DynBuffer<wchar_t> &buffer ) {
	uInt numBytes = String::ByteLength( in ) + 1;
	uInt size = Max( 1, String::ToWide( in, numBytes, NULL, 0 ) );
	buffer.CheckSize( size );
	if ( size == 1 )
		buffer.data[0] = L'\0';
	else
		String::ToWide( in, numBytes, buffer.data, buffer.size );
	return size;
}
#if OG_LINUX
class CompareWrapper {
public:
	CompareWrapper( void *ct, int ( *cmp )(void *, const void *, const void *) )
		:context(ct), compare(cmp){}

	static int Compare(const void *a, const void *b, void *context) {
		CompareWrapper *wrap = reinterpret_cast<CompareWrapper *>(context);
		wrap->compare( wrap->context, a, b );
	}
private:
	void *context;
	int ( *compare )(void *, const void *, const void *);
};
#endif

void QuickSort( void *base, size_t num, size_t width, void *context,
							int ( *compare )(void *, const void *, const void *) ) {
#if OG_WIN32
	qsort_s( base, num, width, compare, context );
#elif OG_MACOS_X
	qsort_r( base, num, width, context, compare );
#elif OG_LINUX
	CompareWrapper data(context, compare);
	qsort_r( base, num, width, CompareWrapper::Compare, &data );
#endif
}

}
