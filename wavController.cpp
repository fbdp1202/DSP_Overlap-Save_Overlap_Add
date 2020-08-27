#include "wavController.h"

wavController::wavController()
{
	this->wavFile = nullptr;
	this->wavHeader = nullptr;
}

wavController::wavController(std::string fileName)
{
	std::cout << "    READ WAV FILE : '" << fileName << "'" << std::endl;
	wavFile = fopen(fileName.c_str(), "rb");
	if (wavFile == nullptr)
	{
		std::cerr << "Unable to open wave file: " << fileName << std::endl;
		return;
	}

	this->wavHeader = new wavHdr;
	int headerSize = sizeof(wavHdr);
	size_t bytesRead = fread(this->wavHeader, 1, headerSize, wavFile);
	this->printWavHeader();
}

wavController::wavController(wavController* target)
{
	this->wavHeader = new wavHdr;

	memcpy(this->wavHeader->RIFF, target->wavHeader->RIFF, sizeof(target->wavHeader->RIFF));
	this->wavHeader->ChunkSize = target->wavHeader->ChunkSize;
	memcpy(this->wavHeader->WAVE, target->wavHeader->WAVE, sizeof(target->wavHeader->WAVE));

 	memcpy(this->wavHeader->fmt, target->wavHeader->fmt, sizeof(target->wavHeader->fmt));
 	this->wavHeader->Subchunk1Size = target->wavHeader->Subchunk1Size;
 	this->wavHeader->AudioFormat = target->wavHeader->AudioFormat;
 	this->wavHeader->NumOfChan = target->wavHeader->NumOfChan;
 	this->wavHeader->SamplesPerSec = target->wavHeader->SamplesPerSec;
 	this->wavHeader->bytesPerSec = target->wavHeader->bytesPerSec;
 	this->wavHeader->blockAlign = target->wavHeader->blockAlign;
 	this->wavHeader->bitsPerSample = target->wavHeader->bitsPerSample;

 	/* "data" sub-chunk */
 	memcpy(this->wavHeader->Subchunk2ID, target->wavHeader->Subchunk2ID, sizeof(target->wavHeader->Subchunk2ID));
 	this->wavHeader->Subchunk2Size = target->wavHeader->Subchunk2Size;
}

wavController::~wavController()
{
	if (this->wavFile != nullptr)
		fclose(this->wavFile);

	if (this->wavHeader != nullptr)
		delete this->wavHeader;
}

void wavController::printWavHeader(void)
{
	std::cout << "	RIFF header                :" << this->wavHeader->RIFF[0] << this->wavHeader->RIFF[1] << this->wavHeader->RIFF[2] << this->wavHeader->RIFF[3] << std::endl;
	std::cout << "	WAVE header                :" << this->wavHeader->WAVE[0] << this->wavHeader->WAVE[1] << this->wavHeader->WAVE[2] << this->wavHeader->WAVE[3] << std::endl;
	std::cout << "	FMT                        :" << this->wavHeader->fmt[0] << this->wavHeader->fmt[1] << this->wavHeader->fmt[2] << this->wavHeader->fmt[3] << std::endl;
	std::cout << "	Data size                  :" << this->wavHeader->ChunkSize << std::endl;

	// Display the sampling Rate from the header
	std::cout << "	Subchunk1Size              :" << this->wavHeader->Subchunk1Size << std::endl;
	std::cout << "	Sampling Rate              :" << this->wavHeader->SamplesPerSec << std::endl;
	std::cout << "	Number of bits used        :" << this->wavHeader->bitsPerSample << std::endl;
	std::cout << "	Number of channels         :" << this->wavHeader->NumOfChan << std::endl;
	std::cout << "	Number of bytes per second :" << this->wavHeader->bytesPerSec << std::endl;
	std::cout << "	Data length                :" << this->wavHeader->Subchunk2Size << std::endl;
	std::cout << "	Audio Format               :" << this->wavHeader->AudioFormat << std::endl;
	// Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM

	std::cout << "	Block align                :" << this->wavHeader->blockAlign << std::endl;
	std::cout << "	Data string                :" << this->wavHeader->Subchunk2ID[0] << this->wavHeader->Subchunk2ID[1] << this->wavHeader->Subchunk2ID[2] << this->wavHeader->Subchunk2ID[3] << std::endl;
}

uint16_t wavController::getBitsPerSample()
{
	return this->wavHeader->bitsPerSample;
}

FILE* wavController::getFilePtr()
{
	return this->wavFile;
}

void wavController::setDataLength(int length)
{
	uint32_t ChunkToSubLength = this->wavHeader->ChunkSize - this->wavHeader->Subchunk2Size; // 36 byte
	this->wavHeader->ChunkSize = length + ChunkToSubLength;
	this->wavHeader->Subchunk2Size = length;
}

wavHdr* wavController::getWavHeader()
{
	return this->wavHeader;
}

void wavController::writeWavFile(std::string outFileName, wavHdr* wavHeader, uint8_t* wavBuf, int length)
{
	std::cout << "    WRITE WAV FILE : '" << outFileName << "'" << std::endl;
	this->printWavHeader();
	FILE* outWavFile = fopen(outFileName.c_str(), "wb");
	fwrite(wavHeader, 1, sizeof wavHeader[0], outWavFile);
	fwrite(wavBuf, 1, length, outWavFile);
	fclose(outWavFile);
}