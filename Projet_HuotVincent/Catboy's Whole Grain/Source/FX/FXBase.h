#pragma once
#include <JuceHeader.h>

class CWGFx {
public:
	enum Type {
		Distortion,
		Bitcrush
	};

	CWGFx(juce::String name) {
		uid = juce::Uuid();
		this->name = name;
	}

	//virtual void fromValueTree(juce::ValueTree tree) = 0;

	virtual void process(juce::AudioBuffer<float>* buffer) = 0;

	juce::Uuid uid;
	juce::String name;
	float wet = 1;
	Type type;
};