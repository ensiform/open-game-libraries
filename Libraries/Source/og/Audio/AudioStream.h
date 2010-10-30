// ==============================================================================
//! @file
//! @brief	Audio File Streaming
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

#ifndef __OG_AUDIOFILE_H__
#define __OG_AUDIOFILE_H__

namespace og {
	class AudioSource;
	class AudioStream;

	class AudioStreamData {
	public:
		AudioStreamData( AudioStream *stream, bool loop );
		virtual ~AudioStreamData();

		uInt		pos;
		bool		isDone;
		bool		loop;
		AudioStream *stream;
	};

	/*
	==============================================================================

	  AudioStream

	==============================================================================
	*/
	class AudioStream {
	public:
		AudioStream() : numInUse(0), format(0), freq(0), data(NULL), dataSize(0) {}
		virtual ~AudioStream() {}

		virtual bool	LoadFile( const char *filename ) = 0;

		virtual AudioStreamData *CreateStreamData( bool loop ) = 0;
		virtual void	ClearStreamData( AudioStreamData *streamData ) = 0;
		virtual bool	UploadData( AudioStreamData *streamData, int buffer, uInt maxSize ) = 0;

	public:

		bool			StartStream( AudioSource *src, bool loop );
		void			UpdateStream( AudioSource *src );

		static	AudioStream *Open( const char *filename );

	protected:
		friend class VorbisStream;
		friend class WavStream;

		int		numInUse;
		uInt	format;
		uInt	freq;

		byte	*data;
		uInt	dataSize;
		uInt	outSize;
	};
}

#endif
