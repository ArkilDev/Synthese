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
	void setPitch(float x);
	void loadFile(const juce::String& path);
	bool isFileLoaded() { return hasFile; };

	juce::AudioBuffer<float> getFileBuffer() { return *controllerInfo.file; };
	void setADSR(float attack, float decay, float sustain, float release);
	void setSampleStart(float x) { startRatio = x; };

	GeneratorInfo controllerInfo;
	std::vector<CWGVoice*> voices;

private:
	//File related
	juce::AudioBuffer<float> fileBuffer;
	juce::AudioFormatManager formatManager;
	juce::AudioFormatReader* formatReader;
	bool hasFile = false;
	float startRatio = 0;

	//Midi related
	juce::MidiMessage currentMessage;
	juce::MidiBuffer::Iterator* mIt;
	int iteratorPos = 0;
};

#endif GRAIN_CONTROLLER_H