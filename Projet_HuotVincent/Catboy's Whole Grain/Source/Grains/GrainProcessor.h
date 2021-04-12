#pragma once
#include <JuceHeader.h>

class CWGGrainProcessor {

public:
	CWGGrainProcessor(juce::AudioBuffer<float> file, int note, double pitch, juce::ADSR::Parameters param, double rate, float len);

	void process(juce::AudioBuffer<float>& buffer);

	//Getters / setters
	float getBufferPos() { return gBufferPos; };
	void setBufferPos(float position) { gBufferPos = position; };
	int getNote() { return gNote; };
	void setPitch(double x) { gNotePitch = x; };
	void setStart(int x) { start = x; gBufferPos = x; };
	void setVol(float x) { grainVol = x; };
	void setPan(float x) { grainPan = x; };

	juce::ADSR adsr;

private:
	int start = 0;
	float gBufferPos = start; //Buffer position is in sample, which is usually an int, but float is used when pitch shifting occurs

	double sampleRate;
	int gNote;
	double gNotePitch;
	float length;
	float grainVol = 1;
	float grainPan = 0;

	juce::AudioBuffer<float> gFileBuffer;
};