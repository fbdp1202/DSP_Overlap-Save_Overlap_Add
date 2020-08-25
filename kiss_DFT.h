#ifndef KISS_DFT_H
#define KISS_DFT_H

#include <iostream>
#include "kiss_fft.h"
#include "_kiss_fft_guts.h"

struct kiss_dft_state {
	int _nfft;
	int _inverse;
	kiss_fft_cpx twiddles;
	kiss_fft_cpx **matrix;
	kiss_dft_state()
	{
		_nfft = _inverse = 0;
		matrix = nullptr;
	}

	kiss_dft_state(int nfft, int inverse)
	{
		int i, j;
		_nfft = nfft;
		_inverse = inverse;

		matrix = new kiss_fft_cpx*[_nfft];
		for (i = 0; i < _nfft; i++)
		{
			matrix[i] = new kiss_fft_cpx[_nfft];
			for (j = 0; j < _nfft; j++)
			{
				matrix[i][j].r = matrix[i][j].i = 0;
			}
		}

		for (i = 0; i < _nfft; i++)
		{
			matrix[i][0].r = 1;
			matrix[0][i].r = 1;
		}

		const double pi = 3.141592653589793238462643383279502884197169399375105820974944;
		double phase = -2 * pi / _nfft;
		if (_inverse)
			phase *= -1;

		twiddles.r = cos(phase);
		twiddles.i = sin(phase);

		for (i = 1; i < _nfft; i++)
		{
			C_MUL(matrix[i][1], matrix[i-1][1], twiddles);
		}
		for (i = 1; i < _nfft; i++)
		{
			for (j = 2; j < _nfft; j++)
			{
				C_MUL(matrix[i][j], matrix[i][j-1], matrix[i][1]);
			}
		}
	}

	~kiss_dft_state() {
		if (matrix != nullptr)
		{
			int i;
			for (i = 0; i < _nfft; i++)
			{
				if (matrix[i] != nullptr)
				{
					free(matrix[i]);
					matrix[i] = nullptr;
				}
			}
			free(matrix);
			matrix = nullptr;
		}

	}

	void run_dft(kiss_fft_cpx* inBuf, kiss_fft_cpx* outBuf)
	{
		int i, j;
		for (i = 0; i < _nfft; i++)
		{
			outBuf[i].r = outBuf[i].i = 0;
			kiss_fft_cpx tmp;
			for (j = 0; j < _nfft; j++)
			{
				C_MUL(tmp, matrix[i][j], inBuf[j]);
				C_ADDTO(outBuf[i], tmp);
			}
		}
	}
};

#endif // !KISS_DFT_H
