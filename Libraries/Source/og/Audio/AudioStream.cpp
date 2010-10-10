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

#include "AudioSystemEx.h"
#include "AudioStream.h"
#include <vorbis/vorbisfile.h>

namespace og {

const uInt AUDIOSRC_BUFFERSIZE = 16384;
const uInt AUDIOSRC_STREAM_LIMIT = AUDIOSRC_BUFFERSIZE * AUDIOSRC_BUFFERS;

/*
================
ovErrorMessage
================
*/
static const char *ovErrorMessage( int error ) {
	switch( error ){
		case OV_EREAD:
			return "Read from media.";
		case OV_ENOTVORBIS:
			return "Not Vorbis data.";
		case OV_EVERSION:
			return "Vorbis version mismatch.";
		case OV_EBADHEADER:
			return "Invalid Vorbis header.";
		case OV_EFAULT:
			return "Internal lic fault (bug or heap/stack corruption.";
		default:
			return "Unknown Ogg error.";
	}
}


AudioStreamData::AudioStreamData( AudioStream *_stream, bool _loop ) : pos(0), isDone(false), loop(_loop), stream(_stream) {

}

AudioStreamData::~AudioStreamData() {
	stream->ClearStreamData( this );
}

class VorbisStreamData : public AudioStreamData {
public:
	VorbisStreamData( AudioStream *stream, bool loop ) : AudioStreamData(stream, loop) {}

	OggVorbis_File	file;
};

/*
==============================================================================

  VorbisStream

==============================================================================
*/
class VorbisStream : public AudioStream {
public:
	~VorbisStream();

	bool	LoadFile( const char *filename );

    AudioStreamData *CreateStreamData( bool loop );
    void	ClearStreamData( AudioStreamData *streamData );
	bool	UploadData( AudioStreamData *streamData, int buffer, uInt maxSize );

	// Vorbis Callbacks
	static size_t	cbRead( void *buffer, size_t byteSize, size_t sizeToRead, void *srcPtr );
	static int		cbSeek( void *srcPtr, ogg_int64_t offset, int origin );
	static int		cbClose( void *srcPtr );
	static long		cbTell( void *srcPtr );
};
ov_callbacks vorbisCallbacks = { VorbisStream::cbRead, VorbisStream::cbSeek, VorbisStream::cbClose, VorbisStream::cbTell };

/*
================
VorbisStream::~VorbisStream
================
*/
VorbisStream::~VorbisStream() {
	if ( data ) {
		OG_ASSERT( numInUse == 0 );
		audioFS->FreeFile( data );
		data = NULL;
	}
}

/*
================
VorbisStream::LoadFile
================
*/
bool VorbisStream::LoadFile( const char *filename ) {
	dataSize = audioFS->LoadFile( filename, &data );
	if ( dataSize == -1 )
		return false;

	// Create a fake stream data so we can get some info.
	VorbisStreamData *streamData = static_cast<VorbisStreamData *>( CreateStreamData( false ) );
	if ( streamData == NULL )
		return false;
	vorbis_info *fileInfo = ov_info( &streamData->file, -1 );

	if ( fileInfo->channels == 1 )
        format = AL_FORMAT_MONO16;
    else
        format = AL_FORMAT_STEREO16;
	freq = fileInfo->rate;
	outSize = freq * fileInfo->channels * 2;

	delete streamData;

	// load once for information parsing.
	return true;
}

/*
================
VorbisStream::CreateStreamData
================
*/
AudioStreamData *VorbisStream::CreateStreamData( bool loop ) {
	VorbisStreamData *streamData = new VorbisStreamData( this, loop );
	if ( ov_open_callbacks( streamData, &streamData->file, NULL, 0, vorbisCallbacks ) != 0 ) {
		//common->Warning("Failed to read ogg file.");
		delete streamData;
		return NULL;
	}

	numInUse++;
	return streamData;
}

/*
================
VorbisStream::ClearStreamData
================
*/
void VorbisStream::ClearStreamData( AudioStreamData *streamData ) {
	ov_clear( &static_cast<VorbisStreamData *>(streamData)->file );
	
	numInUse--;
}

/*
================
VorbisStream::UploadData
================
*/
bool VorbisStream::UploadData( AudioStreamData *streamData, int buffer, uInt maxSize ) {
	VorbisStreamData *streamDataVorbis = static_cast<VorbisStreamData *>(streamData);

	if ( streamData->isDone )
		return false;

	DynBuffer<char> wavData(maxSize);
	int  size = 0;
	int  bitsteam, result;

	while( size < maxSize ) {
		result = ov_read( &streamDataVorbis->file, wavData.data + size, maxSize - size, 0, 2, 1, &bitsteam );

		if( result < 0 ) {
			//common->Warning("Vorbis read error: '%s'", ovErrorMessage( result ) );
			return false;
		}
		if( result > 0 )
			size += result;

		if( result == 0 )
			break;
	}

	if( size == 0 )
		return false;

	if( result == 0 ) {
		if ( !streamData->loop )
			streamData->isDone = true;
		else {
			ov_clear( &static_cast<VorbisStreamData *>(streamData)->file );
			streamData->pos = 0;
			if ( ov_open_callbacks( streamData, &static_cast<VorbisStreamData *>(streamData)->file, NULL, 0, vorbisCallbacks ) != 0 ) {
				//common->Warning("Failed to read ogg file.");
				return false;
			}
		}
	}
	alBufferData( buffer, format, wavData.data, size, freq );

	return CheckAlErrors();
}

/*
================
VorbisStream::cbRead
================
*/
size_t VorbisStream::cbRead( void *buffer, size_t byteSize, size_t sizeToRead, void *srcPtr ) {
	VorbisStreamData *streamData = reinterpret_cast<VorbisStreamData *>(srcPtr);

	int readSize = byteSize * sizeToRead;
	if ( (streamData->pos + readSize) > streamData->stream->dataSize )
		readSize = streamData->stream->dataSize - streamData->pos;

	if ( readSize > 0 ) {
		memcpy( buffer, (streamData->stream->data + streamData->pos), readSize );
		streamData->pos += readSize;
	}
	return readSize;
}

/*
================
VorbisStream::cbSeek
================
*/
int VorbisStream::cbSeek( void *srcPtr, ogg_int64_t offset, int origin ) {
	VorbisStreamData *streamData = reinterpret_cast<VorbisStreamData *>(srcPtr);

	int iOffset = static_cast<int>(offset);
	switch ( origin) {
		case SEEK_SET:
			if ( streamData->stream->dataSize < iOffset)
				streamData->pos = streamData->stream->dataSize;
			else
				streamData->pos = iOffset;
			break;
		case SEEK_CUR:
			if ( (streamData->pos + iOffset) > streamData->stream->dataSize )
				streamData->pos = streamData->stream->dataSize;
			else
				streamData->pos += iOffset;
			break;
		case SEEK_END:
			streamData->pos = streamData->stream->dataSize+1;
			break;
	};

	return 0;
}

/*
================
VorbisStream::cbClose
================
*/
int VorbisStream::cbClose( void *srcPtr ) {
	// Not used, we'll close them differently
	return 1;
}

/*
================
VorbisStream::cbTell
================
*/
long VorbisStream::cbTell( void *srcPtr ) {
	return reinterpret_cast<VorbisStreamData *>(srcPtr)->pos;
}

/*
==============================================================================

  WavStream

==============================================================================
*/
class WavStream : public AudioStream {
public:
	~WavStream();

	bool	LoadFile( const char *filename );

   AudioStreamData *CreateStreamData( bool loop );
    void	ClearStreamData( AudioStreamData *streamData );
	bool	UploadData( AudioStreamData *streamData, int buffer, uInt maxSize );
};

/*
================
WavStream::~WavStream
================
*/
WavStream::~WavStream() {
	if ( data ) {
		OG_ASSERT( numInUse == 0 );
		delete[] data;
		data = NULL;
	}
}

/*
================
WavStream::LoadFile
================
*/
const uInt chunkID_fmt = 0x20746D66;  // "fmt "
const uInt chunkID_data = 0x61746164; // "data"
const uInt chunkID_RIFF = 0x46464952;  // "RIFF"
const uInt fileType_WAVE = 0x45564157; // "WAVE"

bool WavStream::LoadFile( const char *filename ) {
	File *file = audioFS->OpenRead( filename );
	if ( !file )
		return false;

	short formatTag;
	unsigned short channels, bitsPerSample;
	data = NULL;

	try {
		uInt chunkID	= file->ReadUint();
		uInt chunkSize	= file->ReadUint();
		uInt fileType	= file->ReadUint();

		bool hasFormat = false;
		bool failed = false;
		if ( chunkID != chunkID_RIFF || fileType != fileType_WAVE )
			failed = true;

		while ( !failed && !file->Eof() ) {
			chunkID = file->ReadUint();
			chunkSize = file->ReadUint();

			switch( chunkID ) {
				case chunkID_fmt:
					formatTag	= file->ReadShort();
					channels	= file->ReadUshort();
					freq		= file->ReadUint();

					file->Seek( 6, SEEK_CUR ); // AvgBytesPerSec(4), BlockAlign(2)

					bitsPerSample = file->ReadUshort();

					if ( channels == 1 )
						format = (bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
					else
						format = (bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;

					// Must be PCM/uncompressed
					if ( formatTag != 1 )
						failed = true;

					hasFormat = true;
					break;
				case chunkID_data:
					if ( hasFormat ) {
						outSize = dataSize = chunkSize;
						data = new byte[chunkSize];
						file->Read( data, dataSize );
						return true;
					}
					failed = true;
					break;
				default:
					// We don't need other chunks
					file->Seek( chunkSize, SEEK_CUR );
					break;
			}
		}
		file->Close();
		User::Error( ERR_BAD_FILE_FORMAT, "File is no PCM wave file.", filename );
		return false;
	}
	catch( FileReadWriteError err ) {
		if ( data ) {
			delete[] data;
			data = NULL;
		}
		file->Close();
		User::Error( ERR_FILE_CORRUPT, Format("Wave: $*" ) << err.ToString(), filename );
		return false;
	}
}

/*
================
WavStream::CreateStreamData
================
*/
AudioStreamData *WavStream::CreateStreamData( bool loop ) {
	numInUse++;
	return new AudioStreamData( this, loop );
}

/*
================
WavStream::ClearStreamData
================
*/
void WavStream::ClearStreamData( AudioStreamData *streamData ) {
	numInUse--;
}

/*
================
WavStream::UploadData
================
*/
bool WavStream::UploadData( AudioStreamData *streamData, int buffer, uInt maxSize ) {
	if ( streamData->isDone )
		return false;

	uInt start = streamData->pos;
	uInt size = maxSize;

	// Only read as much as we have
	if ( (streamData->pos + size) <= dataSize )
		streamData->pos += size;
	else {
		size = dataSize - streamData->pos;
		if ( !streamData->loop )
			streamData->isDone = true;
		else
			streamData->pos = 0; // start from the beginning.
	}

	alBufferData( buffer, format, (data+start), size, freq );

	return CheckAlErrors();
}


/*
==============================================================================

  AudioStream

==============================================================================
*/

/*
================
AudioStream::StartStream
================
*/
bool AudioStream::StartStream( AudioSource *src, bool loop ) {
	src->streamData = CreateStreamData( loop );

	// streaming not needed, as size too small
	if ( outSize <= AUDIOSRC_STREAM_LIMIT ) {
		if( !UploadData( src->streamData, src->alBuffers[0], AUDIOSRC_STREAM_LIMIT ) ) {
			delete src->streamData;
			src->streamData = NULL;
			return false;
		}
		src->streamData->isDone = true;
		alSourcei( src->alSourceNum, AL_BUFFER, src->alBuffers[0] );
		alSourcei( src->alSourceNum, AL_LOOPING, loop );
	} else {
		// Upload Data
		for( int i=0; i<AUDIOSRC_BUFFERS; i++ ) {
			if( !UploadData( src->streamData, src->alBuffers[i], AUDIOSRC_BUFFERSIZE ) ) {
				delete src->streamData;
				src->streamData = NULL;
				return false;
			}
		}
    
		alSourcei( src->alSourceNum, AL_LOOPING, false ); // solved differently when streaming
		alSourceQueueBuffers( src->alSourceNum, AUDIOSRC_BUFFERS, src->alBuffers );
	}
    alSourcePlay( src->alSourceNum );

	return true;
}

/*
================
AudioStream::UpdateStream
================
*/
void AudioStream::UpdateStream( AudioSource *src ) {
	OG_ASSERT( src->streamData != NULL );

	if ( outSize > AUDIOSRC_STREAM_LIMIT ) {
		int processed;
		alGetSourcei( src->alSourceNum, AL_BUFFERS_PROCESSED, &processed );

		ALuint buffer;
		while( processed-- ) {
			alSourceUnqueueBuffers( src->alSourceNum, 1, &buffer );
			CheckAlErrors();

			if ( !UploadData( src->streamData, buffer, AUDIOSRC_BUFFERSIZE ) )
				return;

			alSourceQueueBuffers( src->alSourceNum, 1, &buffer );
			CheckAlErrors();
		}
	}

	return;
}

/*
================
AudioStream::Open
================
*/
AudioStream *AudioStream::Open( const char *filename ) {
	String extension = String::GetFileExtension( filename, String::Length(filename) );

	AudioStream *stream;
	if ( extension.Icmp("ogg") == 0 )
		stream = new VorbisStream;
	else if ( extension.Icmp("wav") == 0 )
		stream = new WavStream;
	else {
		//common->Warning("Unknown extension '%s'", extension.c_str() );
		return NULL;
	}

	if ( !stream->LoadFile( filename ) ) {
		delete stream;
		return NULL;
	}
	return stream;
}

}
