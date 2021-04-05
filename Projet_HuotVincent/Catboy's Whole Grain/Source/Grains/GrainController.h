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

	//Controls
	float pitch = 1;
	float master = 1;

private:
	//OwnedArray is an array made by JUCE specifically made to hold Objects
	//Many parts of the code inspired directely from juce::Synthesiser class
	juce::OwnedArray<CWGGrainProcessor> voices;

	CWGMidiProcessor cMidiProcessor;
	juce::MidiBuffer midiMessages;
	juce::MidiBuffer::Iterator* mIt;
	juce::MidiMessage currentMessage;
	double lastNoteHz = 440;
	int samplePos = 0;

	double sampleRate;

	juce::AudioBuffer<float> cFileBuffer;
	juce::AudioBuffer<float> cProcessedBuffer;

	bool isLooping = false;

	juce::ADSR::Parameters adsrParam;
};