#pragma once
#include "FXBase.h"

class CWGDistortion : public CWGFx {
public:
	CWGDistortion(std::string name) : CWGFx(name) {
		type = Type::Distortion;
	}

	void process(juce::AudioBuffer<float>* buffer) override {
		DBG("booya");
	}
};