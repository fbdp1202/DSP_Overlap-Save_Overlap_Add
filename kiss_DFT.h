#ifndef KISS_DFT_H
#define KISS_DFT_H

#include <iostream>
#include "kiss_fft.h"
#include "_kiss_fft_guts.h"

class kiss_dft_state {
	int nfft;
	int inverse;
	kiss_fft_cpx twiddles;
	kiss_fft_cpx **matrix;
public:
	kiss_dft_state();
	kiss_dft_state(int nfft, int inverse);
	~kiss_dft_state();
	void runDFT(kiss_fft_cpx* inBuf, kiss_fft_cpx* outBuf);
};

#endif // !KISS_DFT_H
