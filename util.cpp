#include "util.h"

std::chrono::high_resolution_clock::time_point getTime()
{
    return std::chrono::high_resolution_clock::now();
}

double getDiffTime(std::chrono::high_resolution_clock::time_point t1, std::chrono::high_resolution_clock::time_point t2)
{
    std::chrono::duration<double, std::milli> time_span = t2 - t1;
    return time_span.count();
}

void memset_cpx(kiss_fft_cpx* in, float v, uint32_t length)
{
    uint32_t i;
    for (i = 0; i < length; i++)
        in[i].r = in[i].i = v;
}