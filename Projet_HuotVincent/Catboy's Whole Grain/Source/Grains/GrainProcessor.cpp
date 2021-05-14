#include "GrainProcessor.h"

CWGGrainProcessor::CWGGrainProcessor(GeneratorInfo x) : maxSampleCount(x.grainLength* x.sampleRate / 1000) {
	grainInfo = x;
	samplePos = grainInfo.start;
	sampleLeft = maxSampleCount;

	nbSampleSkip = grainInfo.pitch + (std::pow(2.0, grainInfo.note / 12.0) / 32);

	grainInfo.pan = (float)(rand() % 200 - 100) / 100.0f;

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

		float adsrSample = 1 - grainInfo.adsr.getNextSample();
		//If position not at the end of the sample nor the grain length
		if (samplePos <= grainInfo.file->getNumSamples() && sampleLeft > 0) {

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
				bufferWritePointer[channel][i] += currentVal * grainInfo.volume;
				//buffer->setSample(channel, i, buffer->getSample(channel, i) + (currentVal * grainInfo.volume));
			}
			samplePos += nbSampleSkip;
			--sampleLeft;
		}
		else {
			grainInfo.adsr.noteOff();
		}
	}

	grainInfo.adsr.applyEnvelopeToBuffer(*buffer, 0, buffer->getNumSamples());
}