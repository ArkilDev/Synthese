#pragma once
#include "FXBase.h"

class CWGBitcrush : public CWGFx {
public:
	CWGBitcrush(juce::String name) : CWGFx(name) {
		type = Type::Bitcrush;
	}

	void process(juce::AudioBuffer<float>* buffer) override {
		float** data = buffer->getArrayOfWritePointers();
		double pi = 2 * acos(0.0);
		for (int i = 0; i < buffer->getNumSamples(); ++i) {
			for (int j = 0; j < buffer->getNumChannels(); ++j) {
				float wetData = (std::round(data[j][i] * crush)) / crush;
				data[j][i] = (data[j][i] * (1 - wet)) + (wetData * wet);
			}
		}
	}

	float crush = 16;
};