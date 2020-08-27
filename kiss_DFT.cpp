#include "kiss_dft.h"

kiss_dft_state::kiss_dft_state()
{
    this->nfft = this->inverse = 0;
    this->matrix = nullptr;
}

kiss_dft_state::kiss_dft_state(int nfft, int inverse)
{
    int i, j;
    this->nfft = nfft;
    this->inverse = inverse;

    matrix = new kiss_fft_cpx*[this->nfft];
    for (i = 0; i < this->nfft; i++)
    {
        matrix[i] = new kiss_fft_cpx[this->nfft];
        for (j = 0; j < this->nfft; j++)
        {
            matrix[i][j].r = matrix[i][j].i = 0;
        }
    }

    for (i = 0; i < this->nfft; i++)
    {
        matrix[i][0].r = 1;
        matrix[0][i].r = 1;
    }

    const double pi = 3.141592653589793238462643383279502884197169399375105820974944;
    double phase = -2 * pi / this->nfft;
    if (this->inverse)
        phase *= -1;

    twiddles.r = cos(phase);
    twiddles.i = sin(phase);

    for (i = 1; i < this->nfft; i++)
    {
        C_MUL(matrix[i][1], matrix[i-1][1], twiddles);
    }
    for (i = 1; i < this->nfft; i++)
    {
        for (j = 2; j < this->nfft; j++)
        {
            C_MUL(matrix[i][j], matrix[i][j-1], matrix[i][1]);
        }
    }
}

kiss_dft_state::~kiss_dft_state()
{
    if (matrix != nullptr)
    {
        int i;
        for (i = 0; i < this->nfft; i++)
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

void kiss_dft_state::runDFT(kiss_fft_cpx* inBuf, kiss_fft_cpx* outBuf)
{
    for (int i = 0; i < this->nfft; i++)
    {
        outBuf[i].r = outBuf[i].i = 0;
        kiss_fft_cpx tmp;
        for (int j = 0; j < this->nfft; j++)
        {
            C_MUL(tmp, matrix[i][j], inBuf[j]);
            C_ADDTO(outBuf[i], tmp);
        }
    }
}