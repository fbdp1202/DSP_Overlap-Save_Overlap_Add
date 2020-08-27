#ifndef SAMPLER_H
#define SAMPLER_H

#include <iostream>
#include <vector>
#include <string>

class Sampler
{
	std::vector<int16_t>* samples;
	uint32_t length;
	int bitsPerSample;
public:
	Sampler(int bitsPerSample);
	~Sampler();
	void appendInput(uint8_t* inBuf, size_t length);
	int16_t getSample(int idx);
	void addSample(int16_t x, int idx);
	uint32_t getLength();
	void getWavData(uint8_t *outBuf);
};

#endif // !SAMPLER_H