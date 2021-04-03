#pragma once
#include <JuceHeader.h>
#include "../Source/MidiProcessor.h"

class CWGGrainController {
public:
	CWGGrainController();
	void instantiate(juce::AudioBuffer<float>& buffer);

	juce::AudioBuffer<float> getProcessedBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer& midi);
	void switchLoop();

	//gets and sets
	juce::AudioBuffer<float>& getWaveform() { return cFileBuffer; };
	float& getSampleCount() { return cBufferPos; };
	void setSampleRate(double rate) { sampleRate = rate; adsr.setSampleRate(rate); };
	void setADSR(float attack, float decay, float sustain, float release);

	//Controls
	float pitch = 1;
	float master = 1;
	juce::ADSR adsr;

private:
	CWGMidiProcessor cMidiProcessor;
	juce::MidiBuffer midiMessages;
	juce::MidiBuffer::Iterator* mIt;
	juce::MidiMessage currentMessage;
	int samplePos = 0;

	double sampleRate;

	juce::AudioBuffer<float> cFileBuffer;
	juce::AudioBuffer<float> cProcessedBuffer;

	bool isLooping = false;
	float cBufferPos = 0; //Buffer position is in sample, which is an int, but float is used when pitch shifting occurs
	int cMaxBufferPos = 0;

	juce::ADSR::Parameters adsrParam;
};