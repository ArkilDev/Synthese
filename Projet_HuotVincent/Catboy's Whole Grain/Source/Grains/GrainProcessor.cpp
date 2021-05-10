#include "GrainProcessor.h"

CWGGrainProcessor::CWGGrainProcessor(GeneratorInfo x) {
	grainInfo = x;
	grainInfo.adsr.setSampleRate(grainInfo.sampleRate);
	grainInfo.grainAdsrParam.sustain = 0;
	grainInfo.grainAdsrParam.release = 0;
	grainInfo.adsr.setParameters(grainInfo.grainAdsrParam);
	grainInfo.adsr.noteOn();
	samplePos = grainInfo.start;
	maxSampleCount = grainInfo.grainLength * grainInfo.sampleRate / 1000;
}

void CWGGrainProcessor::process(juce::AudioBuffer<float>& buffer) {
	const float* filePointer = 0;
	float currentVal = 0;
	float sampleLeft = 0;
	for (int i = 0; i < buffer.getNumSamples(); ++i) {

		//If position not at the end of the sample nor the grain length
		if (samplePos <= grainInfo.file->getNumSamples() && samplePos - grainInfo.start < maxSampleCount) {

			//Linear interpolation
			for (auto channel = 0; channel < buffer.getNumChannels(); ++channel) {
				filePointer = grainInfo.file->getReadPointer(channel);

				if (samplePos != std::trunc(samplePos)) {
					currentVal = (filePointer[(int)std::floor(samplePos)] + filePointer[(int)std::ceil(samplePos)]) / 2;
				}
				else {
					currentVal = filePointer[(int)samplePos];
				}

				//Set value in output buffer
				buffer.setSample(channel, i, buffer.getSample(channel, i) + (currentVal * grainInfo.adsr.getNextSample() * grainInfo.volume));
			}
			samplePos += grainInfo.pitch;

		}
		else {
			samplePos = grainInfo.start;
		}
	}

	//Pan handling
	if (grainInfo.pan > 0) {
		buffer.applyGain(0, 0, buffer.getNumSamples(), 1 - grainInfo.pan);
	}
	else {
		buffer.applyGain(1, 0, buffer.getNumSamples(), 1 - (grainInfo.pan * -1));
	}
}