// reference https://stackoverrun.com/ko/q/3685205
#ifndef WAV_CONTROLLER_H
#define WAV_CONTROLLER_H
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>

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
} wavHdr;

typedef struct SAMPLE_BUFFER
{
    uint32_t        length;
    int16_t*        buffer;
} sample_16b_buf;

class wavController
{
    FILE* wavFile;
    wavHdr* wavHeader;
public:
    wavController();
    wavController(std::string fileName);
    wavController(wavController* target);
    ~wavController();
    void printWavHeader(void);
    uint16_t getBitsPerSample();
    FILE* getFilePtr();
    void setDataLength(int length);
    wavHdr* getWavHeader();
    void writeWavFile(std::string outFileName, wavHdr* wavHeader, uint8_t* wavBuf, int length);
};

#endif // WAV_CONTROLLER_H