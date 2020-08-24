#ifndef BLOCK_CONV_H
#define BLOCK_CONV_H

#include "kiss_fft.h"
#include "WaveHeader.h"
#include "util.h"

void do_kiss_fft(kiss_fft_cpx* inBuf, kiss_fft_cpx* outBuf, int nfft, int inverse);
sample_16b_buf run_overlap_save_fft(sample_16b_buf inSampleData);
sample_16b_buf run_overlap_add_fft(sample_16b_buf inSampleData);
wav_data do_overlap_save(wav_data inWavData);
wav_data do_overlap_add(wav_data inWavData);

#endif // !BLOCK_CONV_H
