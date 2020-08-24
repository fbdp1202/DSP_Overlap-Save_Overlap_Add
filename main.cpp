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

	char outFileName1[] = "sa1_overlap_save.wav";
	wav_data outWavData1 = do_overlap_save(wavData);
	writeWaveData(outFileName1, outWavData1);
	freeWaveData(outWavData1);

	char outFileName2[] = "sa1_overlap_add.wav";
	wav_data outWavData2 = do_overlap_add(wavData);
	writeWaveData(outFileName2, outWavData2);
	freeWaveData(outWavData2);

	freeWaveData(wavData);
	return 0;
}