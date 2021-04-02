#pragma once
#include <JuceHeader.h>
#include "../Source/MidiProcessor.h"

class CWGGrainController {
public:
	CWGGrainController();
	void instantiate(juce::AudioBuffer<float>& buffer);

	juce::AudioBuffer<float> getProcessedBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer* midi);
	void loadFileToBuffer(juce::AudioBuffer<float>& file);
	void switchLoop();

	//gets and sets
	juce::AudioBuffer<float>& getWaveform() { return cFileBuffer; };
	float& getSampleCount() { return cBufferPos; };

	//Controls
	float pitch = 1;
	float master = 1;

private:
	CWGMidiProcessor cMidiProcessor;
	juce::AudioBuffer<float> cFileBuffer;
	juce::AudioBuffer<float> cProcessedBuffer;
	bool isLooping = false;
	float cBufferPos = 0; //Buffer position is in sample, which is an int, but float is used when pitch shifting occurs
	int cMaxBufferPos = 0;
};