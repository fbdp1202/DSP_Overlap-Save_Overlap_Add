#include "sampler.h"

Sampler::Sampler(int bitsPerSample)
{
    this->samples = new std::vector<int16_t>;
    this->length = 0;
    this->bitsPerSample = bitsPerSample;
}

Sampler::~Sampler()
{
    if (this->samples != nullptr) delete this->samples, this->samples = nullptr;
}

void Sampler::appendInput(uint8_t* inBuf, size_t length)
{
    int perSampleByte = this->bitsPerSample / 8;
    int numSamples = length / perSampleByte;
    for (int i = 0; i < numSamples; i++)
    {
        int16_t tmp, ret = 0;
        for (int j = 0; j < perSampleByte; j++)
        {
            tmp = inBuf[i * perSampleByte + j];
            tmp <<= (8 * j);
            ret += tmp;
        }
        this->samples->push_back(ret);
        this->length++;
    }
}

int16_t Sampler::getSample(int idx)
{
    if (idx < 0 || idx >= this->length) return 0;
    return this->samples->at(idx);
}

void Sampler::addSample(int16_t x, int idx)
{
    if (idx >= this->length)
        this->samples->push_back(x), this->length++;
    else
        this->samples->at(idx) += x;
}

uint32_t Sampler::getLength()
{
    return this->length;
}

void Sampler::getWavData(uint8_t *outBuf)
{
    int perSampleByte = this->bitsPerSample / 8;
    for (int i = 0; i < this->length; i++)
        for (int j = 0; j < perSampleByte; j++)
            outBuf[i * perSampleByte + j] = ((uint16_t)(this->samples->at(i)) >> (j * 8)) & 0xFF;
}
