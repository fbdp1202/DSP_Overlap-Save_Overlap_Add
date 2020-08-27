#include "kiss_dtft_manager.h"

dtftManger::dtftManger()
{
	this->nfft = 0;
	this->mode = "";
	st = sti = nullptr;
    dt = dti = nullptr;
}

dtftManger::dtftManger(int N, std::string mode)
{
	this->nfft = N;
	this->mode = mode;
    st = kiss_fft_alloc(N, 0, 0, 0);
    sti = kiss_fft_alloc(N, 1, 0, 0);
    dt = new kiss_dft_state(N, 0);
    dti = new kiss_dft_state(N, 1);
}

dtftManger::~dtftManger()
{
    if (st != nullptr) free(st), st = nullptr;
    if (sti != nullptr) free(sti), sti = nullptr;
    if (dt != nullptr) delete dt, dt = nullptr;
    if (dti != nullptr) delete dti, dti = nullptr;
}

void dtftManger::runDTFT(kiss_fft_cpx* inBuf, kiss_fft_cpx* outBuf, int inverse)
{
    if (this->mode == "dft")
    {
        if (inverse == 0)
            this->dt->runDFT(inBuf, outBuf);
		else
		{
			this->dti->runDFT(inBuf, outBuf);
			for (int i = 0; i < this->nfft; i++)
				outBuf[i].r /= this->nfft, outBuf[i].i /= this->nfft;
		}
    }
    else if (this->mode == "fft")
    {
        if (inverse == 0)
            kiss_fft(this->st, inBuf, outBuf);
		else
		{
			kiss_fft(this->sti, inBuf, outBuf);
			for (int i = 0; i < this->nfft; i++)
				outBuf[i].r /= this->nfft, outBuf[i].i /= this->nfft;
		}
    }
    else
        std::cerr << "Unknown mode : " << mode << std::endl;
}