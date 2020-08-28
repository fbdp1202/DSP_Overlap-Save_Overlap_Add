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
#include <string>

#include "block_conv.h"

int main(int argc, char ** argv)
{
    std::string fileName = "sa1.wav";

    const int N = 1024;
    const int M = 512;

    Block_conv* tests[4];
    std::string strOverlapAdd = "overlap_add";
    std::string strOverlapSave = "overlap_save";
    std::string strFFT = "fft";
    std::string stdDFT = "dft";

    tests[0] = new Block_conv(fileName, strOverlapAdd, strFFT, N, M);
    tests[1] = new Block_conv(fileName, strOverlapAdd, stdDFT, N, M);
    tests[2] = new Block_conv(fileName, strOverlapSave, strFFT, N, M);
    tests[3] = new Block_conv(fileName, strOverlapSave, stdDFT, N, M);
    return 0;
}