#include "GrainProcessor.h"
#include <random>

CWGGrainProcessor::CWGGrainProcessor(GeneratorInfo x) : maxSampleCount(x.grainLength* x.sampleRate / 1000) {
	grainInfo = x;
	sampleLeft = maxSampleCount;

	nbSampleSkip = grainInfo.pitch + (std::pow(2.0, grainInfo.note / 12.0) / 32);

	//Random
	std::default_random_engine randEngine(rand());
	std::uniform_int_distribution<int> randDist(0, grainInfo.file->getNumSamples() * grainInfo.randStart);

	grainInfo.pan = (-1.0f + float(rand()) / float(RAND_MAX / 2)) * grainInfo.randPan;
	grainInfo.volume = 1 - (float(rand()) / float(RAND_MAX) * grainInfo.randVol);
	grainInfo.start += randDist(randEngine) - (grainInfo.file->getNumSamples() * grainInfo.randStart) / 2;

	if (grainInfo.start < 0)
		grainInfo.start = 0;
	if (grainInfo.start > grainInfo.file->getNumSamples())
		grainInfo.start = grainInfo.file->getNumSamples() - grainInfo.grainLength;
	samplePos = grainInfo.start;

	//ADSR
	grainInfo.adsr.reset();
	grainInfo.adsr.setSampleRate(grainInfo.sampleRate);
	grainInfo.grainAdsrParam.sustain = 0;
	grainInfo.grainAdsrParam.release = 0;
	grainInfo.grainAdsrParam.attack *= (maxSampleCount / grainInfo.sampleRate);
	grainInfo.grainAdsrParam.decay *= (maxSampleCount / grainInfo.sampleRate);
	grainInfo.adsr.setParameters(grainInfo.grainAdsrParam);
	grainInfo.adsr.noteOn();
}

void CWGGrainProcessor::process(juce::AudioBuffer<float>* buffer) {
	float** bufferWritePointer = buffer->getArrayOfWritePointers();
	const float* filePointer = 0;
	float currentVal = 0;
	auto temp = buffer->getNumSamples();

	for (int i = 0; i < buffer->getNumSamples(); ++i) {
		float adsrSample = grainInfo.adsr.getNextSample();

		//If position not at the end of the sample nor the grain length
		if (samplePos < grainInfo.file->getNumSamples() && sampleLeft > 0) {

			//Linear interpolation
			for (auto channel = 0; channel < buffer->getNumChannels(); ++channel) {
				filePointer = grainInfo.file->getReadPointer(channel);

				if (samplePos != std::trunc(samplePos)) {
					currentVal = (filePointer[(int)std::floor(samplePos)] + filePointer[(int)std::ceil(samplePos)]) / 2;
				}
				else {
					currentVal = filePointer[(int)samplePos];
				}

				//Pan handling
				switch (channel) {
				default: grainInfo.pan > 0 ? currentVal *= 1 - grainInfo.pan : currentVal; break;
				case 1: grainInfo.pan < 0 ? currentVal *= 1 - std::abs(grainInfo.pan) : currentVal; break;
				}

				//Declick
				if (sampleLeft < 25) {
					currentVal /= 25 - sampleLeft;
				}
				else if (maxSampleCount - sampleLeft < 25) {
					currentVal /= 25 - (maxSampleCount - sampleLeft);
				}

				//Set value in output buffer
				bufferWritePointer[channel][i] += currentVal * adsrSample * grainInfo.volume;
			}
			samplePos += nbSampleSkip;
			--sampleLeft;
		}
		else {
			grainInfo.adsr.noteOff();
		}
	}
}