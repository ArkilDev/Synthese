#ifndef GRAIN_CONTROLLER_H
#define GRAIN_CONTROLLER_H

#pragma once
#include <JuceHeader.h>
#include "../Grains/infoStruct.h"
#include "../Grains/Voice.h"

class CWGGrainController : juce::HighResolutionTimer {
public:


	CWGGrainController();
	~CWGGrainController();
	void instantiate(juce::AudioBuffer<float>* buffer);
	void hiResTimerCallback() override;

	juce::AudioBuffer<float> getProcessedBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer midi);

	//gets and sets
	void setSampleRate(double rate) { controllerInfo.sampleRate = rate; };
	void setADSR(float attack, float decay, float sustain, float release);
	void setMaster(float x) { master = x; };
	void setPitch(float x);
	void setMainPan(float x) { controllerInfo.pan = x; };
	void loadFile(const juce::String& path);
	bool isFileLoaded() { return hasFile; };

	juce::AudioBuffer<float> getFileBuffer() { return *controllerInfo.file; };
	void setGrainLen(float x) { controllerInfo.grainLength = x; };
	void setSampleStart(float x) { startRatio = x; };
	void setGrainDensity(int x) { controllerInfo.grainDensity = x; };
	void setGrainAttack(float x) { controllerInfo.grainAdsrParam.attack = x; };
	void setGrainRelease(float x) { controllerInfo.grainAdsrParam.decay = x; };

	std::vector<CWGVoice*> voices;

private:
	//General
	GeneratorInfo controllerInfo;
	float master = 1;

	//File related
	juce::AudioBuffer<float> fileBuffer;
	juce::AudioFormatManager formatManager;
	juce::AudioFormatReader* formatReader;
	juce::AudioBuffer<float> processedBuffer;
	bool hasFile = false;
	float startRatio = 0;

	//Midi related
	juce::MidiMessage currentMessage;
	juce::MidiBuffer::Iterator* mIt;
	int iteratorPos = 0;

};

#endif GRAIN_CONTROLLER_H