#pragma once
#include <JuceHeader.h>
#include "../Source/MidiProcessor.h"
#include "../Grains/GrainProcessor.h"

class CWGGrainController {
public:
	CWGGrainController();
	void instantiate(juce::AudioBuffer<float>& buffer);

	juce::AudioBuffer<float> getProcessedBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer& midi);
	void switchLoop();

	//gets and sets
	juce::AudioBuffer<float>& getWaveform() { return cFileBuffer; };
	void setSampleRate(double rate) { sampleRate = rate; };
	void setADSR(float attack, float decay, float sustain, float release);
	void setMaster(float x) { master = x; };
	void setPitch(float x);

	void setGrainLen(float x) { grainLength = x; };
	void setSampleStart(float x) { startRatio = x; };
	void setGrainAttack(float x) { grainARParam.attack = x; };
	void setGrainRelease(float x) { grainARParam.release = x; };

	//OwnedArray is an array made by JUCE specifically made to hold Objects
	//Many parts of the code inspired directely from juce::Synthesiser class
	juce::OwnedArray<CWGGrainProcessor> voices;

private:
	//Controls
	float pitch = 0;
	float master = 1;

	juce::MidiBuffer midiMessages;
	juce::MidiBuffer::Iterator* mIt;
	juce::MidiMessage currentMessage;
	double lastNoteHz = 440;

	double sampleRate;

	juce::AudioBuffer<float> cFileBuffer;
	juce::AudioBuffer<float> cProcessedBuffer;
	float startRatio = 0;
	int samplePos = 0;
	float grainLength = 1;

	bool isLooping = false;

	juce::ADSR::Parameters adsrParam;
	juce::ADSR::Parameters grainARParam;

	double getNotePitch(int note);
};