/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: FileSystem Utilities
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

#ifndef __OG_FS_UTILITIES_H__
#define __OG_FS_UTILITIES_H__

namespace og {
#if OG_WIN32
	FILE *fopenwin32( const char *filename, const char *mode );
	#define fopen fopenwin32
#endif

	/*
	==============================================================================

	  FileListEx

	==============================================================================
	*/
	class FileListEx : public FileList {
	public:
		// ---------------------- Public FileList Interface -------------------

		bool		GetNext( void );
		void		Reset( void ) { position = 0; }
		const char *GetName( void ) { return files[position].c_str(); }
		int			Num( void ) { return files.Num(); }

		// ---------------------- Internal FileListEx Members -------------------

	public:
		FileListEx() { position = 0; }

		uInt			position;
		StringList		files;
	};
	
	bool LocalFileSearch( const char *baseDir, const char *dir, const char *extension, StringList *list, int flags );
}

#endif
