#include "WaveHeader.h"

int getFileSize(FILE* inFile)
{
	int fileSize = 0;
	fseek(inFile, 0, SEEK_END);

	fileSize = ftell(inFile);

	fseek(inFile, 0, SEEK_SET);
	return fileSize;
}

wav_data readWaveData(char *fileName)
{
	wav_data wavData;
	memset(&wavData, 0, sizeof(wavData));

	wav_hdr *wavHeader = new wav_hdr;
	int headerSize = sizeof(wav_hdr), filelength = 0;
	memset(wavHeader, 0, sizeof(wavHeader));

	wav_buf *wavBuffer = new wav_buf;
	int wavBufferSize = sizeof(wav_buf);
	memset(wavBuffer, 0, sizeof(wavBuffer));

	wavData.wavHeader = wavHeader;
	wavData.wavBuffer = wavBuffer;

	FILE* wavFile = fopen(fileName, "rb");
	if (wavFile == nullptr)
	{
		fprintf(stderr, "Unable to open wave file: %s\n", fileName);
		return wavData;
	}
	size_t bytesRead = fread(wavData.wavHeader, 1, headerSize, wavFile);
	std::cout << "Header Read " << bytesRead << " bytes." << std::endl;
	if (bytesRead > 0)
	{
		//Read the data
		uint16_t bytesPerSample = wavData.wavHeader->bitsPerSample / 8;      //Number     of bytes per sample
		uint64_t numSamples = wavData.wavHeader->ChunkSize / bytesPerSample; //How many samples are in the wav file?

		wavData.wavBuffer->length = wavData.wavHeader->Subchunk2Size;
		wavData.wavBuffer->buffer = new uint8_t[wavData.wavBuffer->length];
		bytesRead = fread(wavData.wavBuffer->buffer, sizeof wavData.wavBuffer->buffer[0], wavData.wavBuffer->length, wavFile);
		std::cout << "Read " << bytesRead << " bytes." << std::endl;

//		static const uint16_t BUFFER_SIZE = 4096;
//		int8_t* buffer = new int8_t[BUFFER_SIZE];
		filelength = getFileSize(wavFile);

		std::cout << "File is                    :" << filelength << " bytes." << std::endl;
		std::cout << "RIFF header                :" << wavData.wavHeader->RIFF[0] << wavData.wavHeader->RIFF[1] << wavData.wavHeader->RIFF[2] << wavData.wavHeader->RIFF[3] << std::endl;
		std::cout << "WAVE header                :" << wavData.wavHeader->WAVE[0] << wavData.wavHeader->WAVE[1] << wavData.wavHeader->WAVE[2] << wavData.wavHeader->WAVE[3] << std::endl;
		std::cout << "FMT                        :" << wavData.wavHeader->fmt[0] << wavData.wavHeader->fmt[1] << wavData.wavHeader->fmt[2] << wavData.wavHeader->fmt[3] << std::endl;
		std::cout << "Data size                  :" << wavData.wavHeader->ChunkSize << std::endl;

		// Display the sampling Rate from the header
		std::cout << "Subchunk1Size              :" << wavData.wavHeader->Subchunk1Size << std::endl;
		std::cout << "Sampling Rate              :" << wavData.wavHeader->SamplesPerSec << std::endl;
		std::cout << "Number of bits used        :" << wavData.wavHeader->bitsPerSample << std::endl;
		std::cout << "Number of channels         :" << wavData.wavHeader->NumOfChan << std::endl;
		std::cout << "Number of bytes per second :" << wavData.wavHeader->bytesPerSec << std::endl;
		std::cout << "Data length                :" << wavData.wavHeader->Subchunk2Size << std::endl;
		std::cout << "Audio Format               :" << wavData.wavHeader->AudioFormat << std::endl;
		// Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM

		std::cout << "Block align                :" << wavData.wavHeader->blockAlign << std::endl;
		std::cout << "Data string                :" << wavData.wavHeader->Subchunk2ID[0] << wavData.wavHeader->Subchunk2ID[1] << wavData.wavHeader->Subchunk2ID[2] << wavData.wavHeader->Subchunk2ID[3] << std::endl;
	}
	fclose(wavFile);
	return wavData;
}

wav_data setWaveHeader(wav_data wavData)
{
	uint32_t ChunkToSubLength = wavData.wavHeader->ChunkSize - wavData.wavHeader->Subchunk2Size; // 36 byte
	wavData.wavHeader->ChunkSize = wavData.wavBuffer->length + ChunkToSubLength;
	wavData.wavHeader->Subchunk2Size = wavData.wavBuffer->length;
	return wavData;
}

void writeWaveData(char *outFileName, wav_data outWavData)
{
	std::cout << "write Wave File : " << outFileName << std::endl;
	FILE* wavFile = fopen(outFileName, "wb");
	fwrite(outWavData.wavHeader, 1, 44, wavFile);
	fwrite(outWavData.wavBuffer->buffer, 1, outWavData.wavBuffer->length, wavFile);
	fclose(wavFile);
}

void freeWaveData(wav_data wavData)
{
	if (wavData.wavHeader != nullptr) {
		free(wavData.wavHeader);
		wavData.wavHeader = nullptr;
	}
	if (wavData.wavBuffer != nullptr) {
		if (wavData.wavBuffer->buffer != nullptr) {
			free(wavData.wavBuffer->buffer);
			wavData.wavBuffer->buffer = nullptr;
		}
		free(wavData.wavBuffer);
		wavData.wavBuffer = nullptr;
	}
}

sample_16b_buf getSampleData(wav_data wavData)
{
	sample_16b_buf sampleData;
	memset(&sampleData, 0, sizeof(sampleData));
	if (wavData.wavHeader->bitsPerSample == 16 && wavData.wavHeader->NumOfChan == 1)
	{
		uint32_t numSample = wavData.wavHeader->Subchunk2Size / (wavData.wavHeader->bitsPerSample / 8);
		sampleData.length = numSample;
		sampleData.buffer = new int16_t[numSample];

		uint32_t i, j;
		for (i = 0; i < numSample; i++)
		{
			int16_t tmp, ret = 0;
			int16_t perSampleByte = wavData.wavHeader->bitsPerSample / 8;
			for (j = 0; j < perSampleByte; j++)
			{
				tmp = wavData.wavBuffer->buffer[i*perSampleByte + j];
				tmp <<= (8 * j);
				ret += tmp;
			}
			sampleData.buffer[i] = ret;
		}
	}
	return sampleData;
}

void freeSampleData(sample_16b_buf sample_buf)
{
	if (sample_buf.buffer != nullptr)
	{
		free(sample_buf.buffer);
		sample_buf.buffer = nullptr;
	}
}

sample_16b_buf makeOutSampleData(sample_16b_buf inSampleData, uint32_t blockSize, uint32_t pad)
{
	sample_16b_buf outSampleData;
	memset(&outSampleData, 0, sizeof(outSampleData));
	uint32_t numBlock = (inSampleData.length + blockSize - 1) / blockSize;
	outSampleData.length = numBlock * blockSize + pad;
	outSampleData.buffer = new int16_t[outSampleData.length];
	memset(outSampleData.buffer, 0, sizeof(outSampleData.buffer[0])*outSampleData.length);
	return outSampleData;
}

void copyOutSampleToOutWavBuf(sample_16b_buf outSampleData, wav_data outWavData)
{
	uint32_t i, j;
	for (i = 0; i < outSampleData.length; i++)
	{
		for (j = 0; j < 2; j++)
		{
			outWavData.wavBuffer->buffer[i * 2 + j] = ((uint16_t)(outSampleData.buffer[i]) >> (j * 8)) & 0xFF;
		}
		if (i == 0 || i == 1)
			printf("%04x : %02x %02x\n", (uint16_t)(outSampleData.buffer[i]), outWavData.wavBuffer->buffer[i * 2], outWavData.wavBuffer->buffer[i * 2 + 1]);
	}
}

void copyWavHeader(wav_data inWavData, wav_data outWavData)
{
	memcpy(outWavData.wavHeader->RIFF, inWavData.wavHeader->RIFF, sizeof(inWavData.wavHeader->RIFF));
	outWavData.wavHeader->ChunkSize = inWavData.wavHeader->ChunkSize;
	memcpy(outWavData.wavHeader->WAVE, inWavData.wavHeader->WAVE, sizeof(inWavData.wavHeader->WAVE));

	memcpy(outWavData.wavHeader->fmt, inWavData.wavHeader->fmt, sizeof(inWavData.wavHeader->fmt));
	outWavData.wavHeader->Subchunk1Size = inWavData.wavHeader->Subchunk1Size;
	outWavData.wavHeader->AudioFormat = inWavData.wavHeader->AudioFormat;
	outWavData.wavHeader->NumOfChan = inWavData.wavHeader->NumOfChan;
	outWavData.wavHeader->SamplesPerSec = inWavData.wavHeader->SamplesPerSec;
	outWavData.wavHeader->bytesPerSec = inWavData.wavHeader->bytesPerSec;
	outWavData.wavHeader->blockAlign = inWavData.wavHeader->blockAlign;
	outWavData.wavHeader->bitsPerSample = inWavData.wavHeader->bitsPerSample;

	/* "data" sub-chunk */
	memcpy(outWavData.wavHeader->Subchunk2ID, inWavData.wavHeader->Subchunk2ID, sizeof(inWavData.wavHeader->Subchunk2ID));
	outWavData.wavHeader->Subchunk2Size = inWavData.wavHeader->Subchunk2Size;
}

wav_data makeOutWavData(wav_data inWavData, sample_16b_buf OutSampleData)
{
	wav_data outWavData;
	memset(&outWavData, 0, sizeof(outWavData));
	outWavData.wavHeader = new wav_hdr;
	outWavData.wavBuffer = new wav_buf;

	copyWavHeader(inWavData, outWavData);
	std::cout << "makeOutWavData : " << outWavData.wavHeader->Subchunk2ID[0] << std::endl;
	std::cout << "OutSampleData.length : " << OutSampleData.length << std::endl;
	std::cout << "bits per samples : " << inWavData.wavHeader->bitsPerSample << std::endl;

	outWavData.wavBuffer->length = OutSampleData.length * (inWavData.wavHeader->bitsPerSample / 8);
	std::cout << "OutSampleData.length : " << outWavData.wavBuffer->length << std::endl;

	outWavData.wavBuffer->buffer = new uint8_t[outWavData.wavBuffer->length];
	copyOutSampleToOutWavBuf(OutSampleData, outWavData);
	setWaveHeader(outWavData);

	std::cout << "RIFF header                :" << outWavData.wavHeader->RIFF[0] << outWavData.wavHeader->RIFF[1] << outWavData.wavHeader->RIFF[2] << outWavData.wavHeader->RIFF[3] << std::endl;
	std::cout << "WAVE header                :" << outWavData.wavHeader->WAVE[0] << outWavData.wavHeader->WAVE[1] << outWavData.wavHeader->WAVE[2] << outWavData.wavHeader->WAVE[3] << std::endl;
	std::cout << "FMT                        :" << outWavData.wavHeader->fmt[0] << outWavData.wavHeader->fmt[1] << outWavData.wavHeader->fmt[2] << outWavData.wavHeader->fmt[3] << std::endl;
	std::cout << "Data size                  :" << outWavData.wavHeader->ChunkSize << std::endl;

	// Display the sampling Rate from the header
	std::cout << "Subchunk1Size              :" << outWavData.wavHeader->Subchunk1Size << std::endl;
	std::cout << "Sampling Rate              :" << outWavData.wavHeader->SamplesPerSec << std::endl;
	std::cout << "Number of bits used        :" << outWavData.wavHeader->bitsPerSample << std::endl;
	std::cout << "Number of channels         :" << outWavData.wavHeader->NumOfChan << std::endl;
	std::cout << "Number of bytes per second :" << outWavData.wavHeader->bytesPerSec << std::endl;
	std::cout << "Data length                :" << outWavData.wavHeader->Subchunk2Size << std::endl;
	std::cout << "Audio Format               :" << outWavData.wavHeader->AudioFormat << std::endl;
	// Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM

	std::cout << "Block align                :" << outWavData.wavHeader->blockAlign << std::endl;
	std::cout << "Data string                :" << outWavData.wavHeader->Subchunk2ID[0] << outWavData.wavHeader->Subchunk2ID[1] << outWavData.wavHeader->Subchunk2ID[2] << outWavData.wavHeader->Subchunk2ID[3] << std::endl;

	return outWavData;
}
