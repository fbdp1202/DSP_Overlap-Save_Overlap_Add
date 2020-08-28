#ifndef BLOCK_CONV_H
#define BLOCK_CONV_H

#include "util.h"
#include "wavController.h"
#include "kiss_fft.h"
#include "kiss_dft.h"
#include "kiss_dtft_manager.h"
#include "sampler.h"

#include <iostream>
#include <cstdio>
#include <string>
#include <functional>

class Block_conv
{
    int nfft;
    int blkLen;
    int fltLen;

    wavController* inWav;
    wavController* outWav;

    dtftManger* transformer;

    Sampler* inSampler;
    Sampler* outSampler;

    std::string method;
    std::string mode;
    std::string inFileName;
    std::string outFileName;

    std::function<void(kiss_fft_cpx*, int, int)> inFcnPtr;
    std::function<void(kiss_fft_cpx*, int)> outFcnPtr;

    kiss_fft_cpx* h_FltBuf;
    kiss_fft_cpx* H_FltBuf;

    kiss_fft_cpx* x_WavBuf;
    kiss_fft_cpx* X_WavBuf;

    kiss_fft_cpx* y_WavBuf;
    kiss_fft_cpx* Y_WavBuf;

public:
    Block_conv();
    Block_conv(std::string fileName, std::string method, std::string mode, int N, int M);
    ~Block_conv();

    void runOverlapMethod();
    void overlapAddInput(kiss_fft_cpx* inBuf, int start, int length);
    void overlapAddOutput(kiss_fft_cpx* inBuf, int start);

    void overlapSaveInput(kiss_fft_cpx* inBuf, int start, int length);
    void overlapSaveOutput(kiss_fft_cpx* inBuf, int start);

    void writeOutWav(std::string outFileName);
    void writeWav(std::string outFileName, wavHdr* wavHeader, uint8_t* wavBuf, int length);

    std::string getOutFileName(std::string inFileName, std::string method, std::string mode);
    void cpxMultiply(kiss_fft_cpx* A, kiss_fft_cpx* B, kiss_fft_cpx* C, int length);

    void initMethod(std::string method);
    void initFilter();
    void initSampler();
    void initInputs();

    void delWav();
    void delTransformer();
    void delFilter();
    void delSampler();
    void delInputs();
};

#endif // !BLOCK_CONV_H
