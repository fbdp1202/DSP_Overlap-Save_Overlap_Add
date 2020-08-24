// reference https://stackoverrun.com/ko/q/3685205
#ifndef WAV_HEADER_H
#define WAV_HEADER_H
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <cstdint>

typedef struct  WAV_HEADER
{
	/* RIFF Chunk Descriptor */
	uint8_t         RIFF[4];        // RIFF Header Magic header
	uint32_t        ChunkSize;      // RIFF Chunk Size
	uint8_t         WAVE[4];        // WAVE Header
	/* "fmt" sub-chunk */
	uint8_t         fmt[4];         // FMT header
	uint32_t        Subchunk1Size;  // Size of the fmt chunk
	uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
	uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Sterio
	uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
	uint32_t        bytesPerSec;    // bytes per second
	uint16_t        blockAlign;     // 2=16-bit mono, 4=16-bit stereo
	uint16_t        bitsPerSample;  // Number of bits per sample
	/* "data" sub-chunk */
	uint8_t         Subchunk2ID[4]; // "data"  string
	uint32_t        Subchunk2Size;  // Sampled data length
} wav_hdr;

typedef struct WAV_BUFFER
{
	uint32_t 		length;
	uint8_t*		buffer;
} wav_buf;

typedef struct WAV_DATA
{
	wav_hdr*		wavHeader;
	wav_buf*		wavBuffer;
} wav_data;

typedef struct SAMPLE_BUFFER
{
	uint32_t		length;
	int16_t*		buffer;
} sample_16b_buf;

int getFileSize(FILE* inFile);

wav_data readWaveData(char *fname);
wav_data setWaveHeader(wav_data wavData);
void writeWaveData(char *outFileName, wav_data wavData);
void freeWaveData(wav_data wavData);
sample_16b_buf getSampleData(wav_data wavData);
void freeSampleData(sample_16b_buf sample_buf);

sample_16b_buf makeOutSampleData(sample_16b_buf inSampleData, uint32_t blockSize, uint32_t pad);
wav_data makeOutWavData(wav_data inWavData, sample_16b_buf OutSampleData);

#endif // WAV_HEADER_H