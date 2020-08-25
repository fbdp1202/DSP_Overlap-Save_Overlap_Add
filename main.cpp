/*
 *  Copyright (c) 2003-2010, Mark Borgerding. All rights reserved.
 *  This file is part of KISS FFT - https://github.com/mborgerding/kissfft
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *  See COPYING file for more information.
 */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <typeinfo.h>

#include "WaveHeader.h"
#include "block_conv.h"

int main(int argc, char ** argv)
{
	char fileName[] = "sa1.wav";
	wav_data wavData = readWaveData(fileName);

	char outFileNameSaveFFT[] = "sa1_overlap_save_fft.wav";
	wav_data outWavData = do_overlap_save(wavData, 0);
	writeWaveData(outFileNameSaveFFT, outWavData);
	freeWaveData(outWavData);

	char outFileNameSaveDFT[] = "sa1_overlap_save_dft.wav";
	outWavData = do_overlap_save(wavData, 1);
	writeWaveData(outFileNameSaveDFT, outWavData);
	freeWaveData(outWavData);

	char outFileNameAddFFT[] = "sa1_overlap_add_fft.wav";
	outWavData = do_overlap_add(wavData, 0);
	writeWaveData(outFileNameAddFFT, outWavData);
	freeWaveData(outWavData);

	char outFileNameAddDFT[] = "sa1_overlap_add_dft.wav";
	outWavData = do_overlap_add(wavData, 1);
	writeWaveData(outFileNameAddDFT, outWavData);
	freeWaveData(outWavData);

	freeWaveData(wavData);
	return 0;
}