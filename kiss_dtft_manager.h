#ifndef KISS_DTFT_MANAGER_H
#define KISS_DTFT_MANAGER_H

#include "kiss_dft.h"
#include "kiss_fft.h"
#include <iostream>
#include <string>

class dtftManger
{
    kiss_fft_state* st;
    kiss_fft_state* sti;

    kiss_dft_state* dt;
    kiss_dft_state* dti;

	int nfft;
	std::string mode;

public:
    dtftManger();
    dtftManger(int N, std::string mode);
    ~dtftManger();
    void runDTFT(kiss_fft_cpx* inBuf, kiss_fft_cpx* outBuf, int inverse);
};

#endif // !KISS_DTFT_MANAGER_H
