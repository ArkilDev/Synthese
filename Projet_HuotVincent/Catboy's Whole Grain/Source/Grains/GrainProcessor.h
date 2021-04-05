#pragma once
#include <JuceHeader.h>

class CWGGrainProcessor {

public:
	CWGGrainProcessor(juce::AudioBuffer<float> file, int note, double pitch, juce::ADSR::Parameters param, double rate);

	void process(juce::AudioBuffer<float>& buffer);

	//Getters / setters
	float getBufferPos() { return gBufferPos; };
	void setBufferPos(float position) { gBufferPos = position; };
	int getNote() { return gNote; };

	juce::ADSR adsr;

private:
	float gBufferPos = 0; //Buffer position is in sample, which is usually an int, but float is used when pitch shifting occurs

	double sampleRate;
	int gNote;
	double gNotePitch;

	juce::AudioBuffer<float> gFileBuffer;
};