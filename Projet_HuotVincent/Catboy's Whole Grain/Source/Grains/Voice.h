#pragma once
#include <JuceHeader.h>
#include "../Grains/GrainProcessor.h"
#include "../Grains/infoStruct.h"

class CWGVoice : juce::Timer {
public:
	CWGVoice(GeneratorInfo x);
	~CWGVoice();
	void timerCallback() override;
	void processGrains(juce::AudioBuffer<float>& buffer);

	void setPitch(float x) { voiceInfo.pitch = x; };
	void setAdsrOff();

	int getNote() { return voiceInfo.note; };
	bool isAdsrActive() { return voiceInfo.adsr.isActive(); };

private:
	GeneratorInfo voiceInfo;

	std::vector<CWGGrainProcessor*> grains;
	juce::ADSR::Parameters grainARParam;
	float grainLength = 1;
	float grainStartSample;

	float clockDelay = 100;

	void createGrain();
};