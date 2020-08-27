#define _CRT_SECURE_NO_WARNINGS
#include "block_conv.h"

#include "_kiss_fft_guts.h"
#include "LPF_hamming_512.h"

Block_conv::Block_conv()
{
	this->nfft = this->blkLen = this->fltLen = -1;
	this->inWav = this->outWav = nullptr;
}

Block_conv::Block_conv(std::string fileName, std::string method, std::string mode, int N, int M)
{
	std::cout << "Block_conv(" << fileName << ", " << method << ", " << mode << ", " << N << ", " << M << ")" << std::endl;

	this->nfft = N;
	this->fltLen = M;
	this->blkLen = N - M + 1;
	
	this->initMethod(method);
	this->mode = mode;
	this->inFileName = fileName;
	this->outFileName = this->getOutFileName(this->inFileName, this->method, this->mode);

	this->inWav = new wavController(fileName);
	this->transformer = new dtftManger(this->nfft, this->mode);
	this->initFilter();
	this->initSampler();

	this->initInputs();

	std::chrono::high_resolution_clock::time_point start = getTime();

	this->runOverlapMethod();

	std::chrono::high_resolution_clock::time_point end = getTime();

	this->writeOutWav(this->outFileName);
	std::cout << "It took ("<< this->method << ", " << this->mode << "): " << getDiffTime(start, end) << " milliseconds." << std::endl << std::endl;
}

Block_conv::~Block_conv()
{
	this->delWav();
	this->delTransformer();
	this->delFilter();
	this->delSampler();
	this->delInputs();
}

std::string Block_conv::getOutFileName(std::string inFileName, std::string method, std::string mode)
{
	const std::string toReplace = ".wav";
	const std::string replaceWith = "_" + method + "_" + mode + ".wav";
	std::size_t pos = inFileName.find(".wav");
	if (pos == std::string::npos) return inFileName;
	return inFileName.replace(pos, toReplace.length(), replaceWith);
}

void Block_conv::initMethod(std::string method)
{
	this->method = method;
	if (this->method == "overlap_add")
	{
		this->inFcnPtr = [=](kiss_fft_cpx* inBuf, int start, int length) {
			this->overlapAddInput(inBuf, start, length);
		};
		this->outFcnPtr = [=](kiss_fft_cpx* inBuf, int start) {
			this->overlapAddOutput(inBuf, start);
		};
	}
	else if (this->method == "overlap_save")
	{
		this->inFcnPtr = [=](kiss_fft_cpx* inBuf, int start, int length) {
			this->overlapSaveInput(inBuf, start, length);
		};
		this->outFcnPtr = [=](kiss_fft_cpx* inBuf, int start) {
			this->overlapSaveOutput(inBuf, start);
		};
	}
	else
		std::cerr << "Unknown method : " << method << std::endl;
}

void Block_conv::initFilter()
{
	int nbytes = this->nfft * sizeof(kiss_fft_cpx);
    this->h_FltBuf = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
	this->H_FltBuf = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
    memset_cpx(this->h_FltBuf, 0, this->nfft);
    memset_cpx(this->H_FltBuf, 0, this->nfft);

    for (int i = 0; i < this->fltLen; i++)
		this->h_FltBuf[i].r = LPF_coeff[i];

	this->transformer->runDTFT(this->h_FltBuf, this->H_FltBuf, 0);
}

void Block_conv::runOverlapMethod()
{
	size_t bytesRead = 0;
	int bytesPerSample = this->inWav->getBitsPerSample() / 8;
	int nbytes = this->blkLen * bytesPerSample;
	uint8_t *tmpBuf = new uint8_t[nbytes];

	uint32_t i = 0;
	while ((bytesRead = fread(tmpBuf, sizeof tmpBuf[0], nbytes, this->inWav->getFilePtr())) != 0)
	{
		this->inSampler->appendInput(tmpBuf, bytesRead);
		this->inFcnPtr(this->x_WavBuf, i, bytesRead);
		this->transformer->runDTFT(this->x_WavBuf, this->X_WavBuf, 0);
		this->cpxMultiply(this->Y_WavBuf, this->X_WavBuf, this->H_FltBuf, this->nfft);
		this->transformer->runDTFT(this->Y_WavBuf, this->y_WavBuf, 1);
		this->outFcnPtr(this->y_WavBuf, i);
		i += (nbytes / bytesPerSample);
	}
}

void Block_conv::writeOutWav(std::string outFileName)
{
	this->outWav = new wavController(this->inWav);
	int outWavBufLen = this->outSampler->getLength() * (this->outWav->getBitsPerSample() / 8);
	uint8_t* outWavBuf = new uint8_t[outWavBufLen];
	this->outSampler->getWavData(outWavBuf);
	this->outWav->setDataLength(outWavBufLen);
	this->outWav->writeWavFile(outFileName, this->outWav->getWavHeader(), outWavBuf, outWavBufLen);
	delete outWavBuf;
}

void Block_conv::overlapAddInput(kiss_fft_cpx* inBuf, int start, int length)
{
	memset_cpx(inBuf, 0, this->nfft);
	for (int i = 0; i < length; i++)
		inBuf[i].r = this->inSampler->getSample(start + i);
}

void Block_conv::overlapAddOutput(kiss_fft_cpx* inBuf, int start)
{
	for (int i = 0; i < this->nfft; i++)
		this->outSampler->addSample(int16_t(inBuf[i].r), start + i);
}

void Block_conv::overlapSaveInput(kiss_fft_cpx* inBuf, int start, int length)
{
	memset_cpx(inBuf, 0, this->nfft);
	for (int i = 0; i < this->nfft; i++)
		inBuf[i].r = this->inSampler->getSample(start + i - (this->fltLen - 1));
}

void Block_conv::overlapSaveOutput(kiss_fft_cpx* inBuf, int start)
{
	for (int i = 0; i < this->blkLen; i++)
		this->outSampler->addSample(int16_t(inBuf[i + this->fltLen - 1].r), start + i);
}

void Block_conv::cpxMultiply(kiss_fft_cpx* dst, kiss_fft_cpx* src1, kiss_fft_cpx* src2, int length)
{
	for (int i = 0; i < length; i++)
		C_MUL(dst[i], src1[i], src2[i]);
}

void Block_conv::initSampler()
{
	int bitsPerSamples = this->inWav->getBitsPerSample();
	this->inSampler = new Sampler(bitsPerSamples);
	this->outSampler = new Sampler(bitsPerSamples);
}

void Block_conv::initInputs()
{
	int nbytes = this->nfft * sizeof(kiss_fft_cpx);
	this->x_WavBuf = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
	this->X_WavBuf = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
	this->y_WavBuf = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
	this->Y_WavBuf = (kiss_fft_cpx*)KISS_FFT_MALLOC(nbytes);
}

void Block_conv::delInputs()
{
	if (this->x_WavBuf != nullptr) free(this->x_WavBuf), this->x_WavBuf = nullptr;
	if (this->X_WavBuf != nullptr) free(this->X_WavBuf), this->X_WavBuf = nullptr;
	if (this->y_WavBuf != nullptr) free(this->y_WavBuf), this->y_WavBuf = nullptr;
	if (this->Y_WavBuf != nullptr) free(this->Y_WavBuf), this->Y_WavBuf = nullptr;
}

void Block_conv::delSampler()
{
	if (this->inSampler != nullptr) delete this->inSampler, this->inSampler = nullptr;
	if (this->outSampler != nullptr) delete this->outSampler, this->outSampler = nullptr;
}

void Block_conv::delWav()
{
	if (this->inWav != nullptr) delete this->inWav, this->inWav = nullptr;
	if (this->outWav != nullptr) delete this->outWav, this->outWav = nullptr;
}

void Block_conv::delTransformer()
{
	if (this->transformer != nullptr) delete this->transformer, this->transformer = nullptr;
}

void Block_conv::delFilter()
{
	if (this->h_FltBuf != nullptr) free(this->h_FltBuf), this->h_FltBuf = nullptr;
	if (this->H_FltBuf != nullptr) free(this->H_FltBuf), this->H_FltBuf = nullptr;
}