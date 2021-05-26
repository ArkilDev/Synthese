#pragma once
#include "FXBase.h"

class CWGDistortion : public CWGFx {
public:
	CWGDistortion(juce::String name) : CWGFx(name) {
		type = Type::Distortion;
	}

	void process(juce::AudioBuffer<float>* buffer) override {
		float** data = buffer->getArrayOfWritePointers();
		double pi = 2 * acos(0.0);
		for (int i = 0; i < buffer->getNumSamples(); ++i) {
			for (int j = 0; j < buffer->getNumChannels(); ++j) {
				float wetData = data[j][i] * drive * range;
				wetData = ((2.f / pi) * atan(wetData));
				data[j][i] = (data[j][i] * (1 - wet)) + (wetData * wet);;
			}
		}
	}

	float drive = 1;
	float range = 1;
};