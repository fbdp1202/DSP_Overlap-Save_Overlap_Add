#ifndef UTIL_H
#define UTIL_H

#include "kiss_fft.h"

#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>

#define MAX(x,y) ( ( (x)<(y) )?(y):(x) )
#define MIN(x,y) ( ( (x)<(y) )?(x):(y) )

void memset_cpx(kiss_fft_cpx* in, float v, uint32_t length);
std::chrono::high_resolution_clock::time_point getTime();
double getDiffTime(std::chrono::high_resolution_clock::time_point t1, std::chrono::high_resolution_clock::time_point t2);

#endif // !UTIL_H