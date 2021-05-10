#ifndef GRAIN_CONTROLLER_H
#define GRAIN_CONTROLLER_H

#pragma once
#include <JuceHeader.h>
#include "../Grains/infoStruct.h"
#include "../Grains/Voice.h"

class CWGGrainController {
public:


	CWGGrainController();
	~CWGGrainController();
	void instantiate(juce::AudioBuffer<float>* buffer);

	juce::AudioBuffer<float> getProcessedBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer& midi);

	//gets and sets
	juce::AudioBuffer<float> getWaveform() { return *controllerInfo.file; };
	void setSampleRate(double rate) { controllerInfo.sampleRate = rate; };
	void setADSR(float attack, float decay, float sustain, float release);
	void setMaster(float x) { master = x; };
	void setPitch(float x);
	void setMainPan(float x) { controllerInfo.pan = x; };

	void setGrainLen(float x) { controllerInfo.grainLength = x; };
	void setSampleStart(float x) { startRatio = x; };
	void setGrainDensity(int x) { controllerInfo.grainDensity = x; };
	void setGrainAttack(float x) { controllerInfo.grainAdsrParam.attack = x; };
	void setGrainRelease(float x) { controllerInfo.grainAdsrParam.decay = x; };

	std::vector<CWGVoice*> voices;

private:
	GeneratorInfo controllerInfo;
	float master = 1;

	juce::MidiMessage currentMessage;
	juce::MidiBuffer::Iterator* mIt;
	int iteratorPos = 0;

	juce::AudioBuffer<float> cProcessedBuffer;
	float startRatio = 0;
};

#endif GRAIN_CONTROLLER_H