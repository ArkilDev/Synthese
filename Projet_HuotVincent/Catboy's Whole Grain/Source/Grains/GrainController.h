#ifndef GRAIN_CONTROLLER_H
#define GRAIN_CONTROLLER_H

#pragma once
#include <JuceHeader.h>
#include "../Grains/infoStruct.h"
#include "../Grains/Voice.h"

#include "../FX/FXBase.h"
#include "../FX/Distortion.h"
#include "../FX/Bitcrush.h"

class CWGGrainController : juce::HighResolutionTimer {
public:


	CWGGrainController();
	~CWGGrainController();
	void instantiate(juce::AudioBuffer<float>* buffer);
	void hiResTimerCallback() override;
	void fromValueTree(juce::ValueTree tree);

	juce::AudioBuffer<float> getProcessedBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer& midi);
	void addFx(int id, bool callLambda = true);
	void removeFx(int id);

	std::function<void()> onFxAdd;
	std::function<void(juce::Uuid uid)> onFxRemove;
	std::function<void(juce::String path)> onFileLoad;

	//gets and sets
	void setSampleRate(double rate) { controllerInfo.sampleRate = rate; };
	void setPitch(float x);
	void loadFile(const juce::String& path, bool callLambda = true);
	bool isFileLoaded() { return hasFile; };

	juce::AudioBuffer<float> getFileBuffer() { return *controllerInfo.file; };
	void setADSR(float attack, float decay, float sustain, float release);
	void setSampleStart(float x) { startRatio = x; };

	GeneratorInfo controllerInfo;
	std::vector<CWGVoice*> voices;
	std::vector<CWGFx*> FXs;

	bool isReady = false;

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