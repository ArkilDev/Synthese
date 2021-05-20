#pragma once
#include <JuceHeader.h>

class CWGFx {
public:
	enum Type {
		Distortion,
		Bitcrush
	};

	CWGFx(std::string name) {
		this->name = name;
	}

	virtual void process(juce::AudioBuffer<float>* buffer) = 0;

	float wet;
	std::string name;
	Type type;
};
