#ifndef BLOCK_CONV_H
#define BLOCK_CONV_H

#include "kiss_fft.h"
#include "kiss_dft.h"
#include "WaveHeader.h"
#include "util.h"

void do_kiss_ft(kiss_fft_cpx* inBuf, kiss_fft_cpx* outBuf, int nfft, int inverse, int flag);
sample_16b_buf run_overlap_save_fft(sample_16b_buf inSampleData, int flag);
sample_16b_buf run_overlap_add_fft(sample_16b_buf inSampleData, int flag);
wav_data do_overlap_save(wav_data inWavData, int flag);
wav_data do_overlap_add(wav_data inWavData, int flag);

#endif // !BLOCK_CONV_H
