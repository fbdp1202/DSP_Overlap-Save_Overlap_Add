#include "block_conv.h"

#include "_kiss_fft_guts.h"
#include "LPF_hamming_512.h"

void do_kiss_fft(kiss_fft_cpx* inBuf, kiss_fft_cpx* outBuf, int nfft, int inverse)
{
	// copy filter data to buffer
	kiss_fft_cfg st = kiss_fft_alloc(nfft, inverse, 0, 0);
	kiss_fft(st, inBuf, outBuf);
	free(st);
	kiss_fft_cleanup();
}

void memset_cpx(kiss_fft_cpx* in, float v, uint32_t length)
{
	uint32_t i;
	for (i = 0; i < length; i++)
		in[i].r = in[i].i = v;
}

sample_16b_buf run_overlap_save_fft(sample_16b_buf inSampleData)
{
	kiss_fft_cpx * wavBuf, *fltBuf;
	kiss_fft_cpx * wavBufOut, *fltBufOut, *mulBufOut, *bufOut;

	uint32_t i, j;

	const int N = 1024; // number of fft
	const int L = 513; // Block Length
	const int M = 512; // impulse response Length

	int nbytes = N * sizeof(kiss_fft_cpx);

	sample_16b_buf outSampleData = makeOutSampleData(inSampleData, L, 0);

	// FFT of Low-pass Filter
	fltBuf = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
	fltBufOut = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
	memset_cpx(fltBuf, 0, N);
	memset_cpx(fltBufOut, 0, N);

	for (i = 0; i < M; i++)
		fltBuf[i].r = LPF_coeff[i];

	do_kiss_fft(fltBuf, fltBufOut, N, 0);

	wavBuf = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
	wavBufOut = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
	mulBufOut = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
	bufOut = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);

	// overlap-save
	uint32_t length = outSampleData.length;

	for (i = 0; i < length; i += L)
	{
		memset_cpx(wavBuf, 0, N);
		memset_cpx(wavBufOut, 0, N);
		memset_cpx(mulBufOut, 0, N);
		memset_cpx(bufOut, 0, N);
		for (j = 0; j < N; j++)
		{
			uint32_t bufIdx = i + j - (M - 1);
			if (bufIdx < 0 || bufIdx >= inSampleData.length) continue;
			wavBuf[j].r = (float)(inSampleData.buffer[bufIdx]);
		}
		do_kiss_fft(wavBuf, wavBufOut, N, 0);

		for (j = 0; j < N; j++)
			C_MUL(mulBufOut[j], wavBufOut[j], fltBufOut[j]);

		do_kiss_fft(mulBufOut, bufOut, N, 1);
//		do_kiss_fft(wavBufOut, bufOut, N, 1);

		for (j = 0; j < L; j++)
		{
			outSampleData.buffer[i + j] = (int16_t)(bufOut[j + M - 1].r / N);
		}
	}

	free(wavBuf); free(wavBufOut);
	free(fltBuf); free(fltBufOut);
	free(mulBufOut);
	free(bufOut);

	return outSampleData;
}

sample_16b_buf run_overlap_add_fft(sample_16b_buf inSampleData)
{
	kiss_fft_cpx * wavBuf, *fltBuf;
	kiss_fft_cpx * wavBufOut, *fltBufOut, *mulBufOut, *bufOut;

	uint32_t i, j;

	const int N = 1024; // number of fft
	const int L = 513; // Block Length
	const int M = 512; // impulse response Length

	int nbytes = N * sizeof(kiss_fft_cpx);

	sample_16b_buf outSampleData = makeOutSampleData(inSampleData, L, M-1);

	// FFT of Low-pass Filter
	fltBuf = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
	fltBufOut = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
	memset_cpx(fltBuf, 0, N);
	memset_cpx(fltBufOut, 0, N);

	for (i = 0; i < M; i++)
		fltBuf[i].r = LPF_coeff[i];

	do_kiss_fft(fltBuf, fltBufOut, N, 0);

	wavBuf = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
	wavBufOut = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
	mulBufOut = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
	bufOut = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);

	// overlap-add
	uint32_t length = outSampleData.length;

	for (i = 0; i < length; i += L)
	{
		memset_cpx(wavBuf, 0, N);
		memset_cpx(wavBufOut, 0, N);
		memset_cpx(mulBufOut, 0, N);
		memset_cpx(bufOut, 0, N);
		for (j = 0; j < L; j++)
		{
			uint32_t bufIdx = i + j;
			if (bufIdx < 0 || bufIdx >= inSampleData.length) continue;
			wavBuf[j].r = (float)(inSampleData.buffer[bufIdx]);
		}
		do_kiss_fft(wavBuf, wavBufOut, N, 0);

		for (j = 0; j < N; j++)
			C_MUL(mulBufOut[j], wavBufOut[j], fltBufOut[j]);

		do_kiss_fft(mulBufOut, bufOut, N, 1);

		for (j = 0; j < N; j++)
		{
			outSampleData.buffer[i + j] += (int16_t)(bufOut[j].r / N);
		}
	}

	free(wavBuf); free(wavBufOut);
	free(fltBuf); free(fltBufOut);
	free(mulBufOut);
	free(bufOut);

	return outSampleData;
}

wav_data do_overlap_save(wav_data inWavData)
{
	// convert wav_data to sample_data
	sample_16b_buf inSampleData = getSampleData(inWavData);
	sample_16b_buf OutSampleData = run_overlap_save_fft(inSampleData);

	wav_data outWavData = makeOutWavData(inWavData, OutSampleData);

	freeSampleData(inSampleData);
	freeSampleData(OutSampleData);
	return outWavData;
}

wav_data do_overlap_add(wav_data inWavData)
{
	// convert wav_data to sample_data
	sample_16b_buf inSampleData = getSampleData(inWavData);
	sample_16b_buf OutSampleData = run_overlap_add_fft(inSampleData);

	wav_data outWavData = makeOutWavData(inWavData, OutSampleData);

	freeSampleData(inSampleData);
	freeSampleData(OutSampleData);
	return outWavData;
}
