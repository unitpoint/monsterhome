#ifndef __WAV_READER_H__
#define __WAV_READER_H__

#include <AppCommon.h>

// Format tags
#define WAVE_FORMAT_UNKNOWN 0x0000
#define WAVE_FORMAT_PCM     0x0001
#define WAVE_FORMAT_ADPCM   0x0002

class WavReader
{
protected:

public:

	// Data structures
	struct RIFFHeader
	{
		char    typeID[4];          //!< ID string - Expect "RIFF"
		uint32  length;             //!< Length of data after this field (i.e. including rest of this struct)
		char    subTypeID[4];       //!< Sub-type ID string - Expect "WAVE"
	};

	struct RIFFChunkHeader
	{
		char    typeID[4];          //!< ID string - Either "fmt " or "data"
		uint32  length;             //!< Length of data after this field
	};

	struct WAVEFormatChunk
	{
		uint16  formatTag;          //!< Format category
		uint16  channels;           //!< Number of channels
		uint32  samplesPerSec;      //!< Sampling rate
		uint32  avgBytesPerSec;     //!< For buffer estimation
		uint16  blockAlign;         //!< Data block size
		uint16  bitsPerSample;      //!< Bits per sample - Should be 8 or 16
	};

	struct WAVEFormatChunkADPCM
	{
		uint16  formatTag;          //!< Format category
		uint16  channels;           //!< Number of channels
		uint32  samplesPerSec;      //!< Sampling rate
		uint32  avgBytesPerSec;     //!< For buffer estimation
		uint16  blockAlign;         //!< Data block size
		uint16  bitsPerSample;      //!< Bits per sample - Should be 8 or 16
		uint16  cbSize;             //!< The size in bytes of the extra information in the extended WAVE 'fmt' header. This should be 2 for adpcm.
		uint16  samplesPerBlock;    //!< The number of samples in each adpcm block
	};

	enum ESoundDataFormat
	{
		PCM_8BIT_MONO,              //!< Raw 8-bit per sample single-channel PCM data
		PCM_16BIT_MONO,             //!< Raw 16-bit per sample single-channel PCM data
		ADPCM_MONO                  //!< ADPCM compressed  single-channel data
	};

	struct SoundData
	{
		ESoundDataFormat format;
		int pitch;
		
		int8 * samples;
		int sample_count;

		int buffer_size;

		SoundData();
		SoundData(ESoundDataFormat, int pitch);
		virtual ~SoundData();

		// void setSampleCount(uint32);
		void setBufferSize(int);
		
		void switchDataSign();
	};

	WavReader();

	static SoundData * load(const char * filename);

protected:

	String filename;

	SoundData * loadInternal(const char * filename);

	bool readChunkHeader(RIFFChunkHeader& header, s3eFile& file);
	bool readChunkFormat(const RIFFChunkHeader& header, SoundData*& data, s3eFile& file);
	bool readChunkData(const RIFFChunkHeader& header, SoundData*& data, s3eFile& file);
	bool readChunkFact(const RIFFChunkHeader& header, SoundData*& data, s3eFile& file);
};

#endif // __WAV_READER_H__