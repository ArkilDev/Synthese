#pragma once
#include <JuceHeader.h>
#include "../Grains/infoStruct.h"

class CWGGrainProcessor {

public:
	CWGGrainProcessor(GeneratorInfo x);

	void process(juce::AudioBuffer<float>* const& buffer);

	//Getters / setters
	float getBufferPos() { return samplePos; };
	void setBufferPos(float position) { samplePos = position; };
	int getNote() { return grainInfo.note; };
	void setPitch(double x) { grainInfo.pitch = x; };
	void setVol(float x) { grainInfo.volume = x; };
	void setPan(float x) { grainInfo.pan = x; };

	GeneratorInfo grainInfo;

private:
	float samplePos;
	const float maxSampleCount;
	float nbSampleSkip;
	float sampleLeft;
};