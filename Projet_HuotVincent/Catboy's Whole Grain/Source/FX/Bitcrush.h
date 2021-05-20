#pragma once
#include "FXBase.h"

class CWGBitcrush : public CWGFx {
public:
	CWGBitcrush(std::string name) : CWGFx(name) {
		type = Type::Bitcrush;
	}

	void process(juce::AudioBuffer<float>* buffer) override {
		DBG("booya");
	}
};