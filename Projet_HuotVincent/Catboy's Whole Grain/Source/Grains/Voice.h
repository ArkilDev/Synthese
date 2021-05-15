#pragma once
#include <JuceHeader.h>
#include "../Grains/GrainProcessor.h"
#include "../Grains/infoStruct.h"

class CWGVoice {
public:
	CWGVoice(GeneratorInfo x);
	~CWGVoice();
	void processGrains(juce::AudioBuffer<float>* buffer);
	void fakeTimerCallback();
	void setPitch(float x) { voiceInfo.pitch = x; };

	std::vector<CWGGrainProcessor*> grains;
	GeneratorInfo voiceInfo;

private:
	int clock;
	juce::ADSR::Parameters grainARParam;
	std::vector<CWGGrainProcessor*>::const_iterator grainIterator;
	float grainLength = 1;
	float grainStartSample;

	void createGrain();
};