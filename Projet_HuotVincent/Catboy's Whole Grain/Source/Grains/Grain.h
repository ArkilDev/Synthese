#pragma once
#include <JuceHeader.h>

class CWGVoiceProcessor {

public:
	CWGVoiceProcessor(juce::AudioBuffer<float>* file,
		int note,
		double pitch,
		juce::ADSR::Parameters param,
		double rate,
		float len,
		int startSample,
		float pan);

	void process(juce::AudioBuffer<float>* buffer);

	//Getters / setters
	float getBufferPos() { return gBufferPos; };
	void setBufferPos(float position) { gBufferPos = position; };
	int getNote() { return gNote; };
	float getPan() { return grainPan; };

	void setPitch(double x) { gNotePitch = x; };
	void setStart(int x) { start = x; gBufferPos = x; };
	void setVol(float x) { grainVol = x; };
	void setPan(float x) { grainPan = x; };
	void setLooping(bool x) { persistantGrain = x; };

	juce::ADSR adsr;

private:
	int start = 0;
	float gBufferPos; //Buffer position is in sample, which is usually an int, but float is used when pitch shifting occurs

	double sampleRate;
	int gNote;
	double gNotePitch;
	int length;
	int sampleLeft;
	float grainVol = 1;
	float grainPan = 0;
	bool persistantGrain = false;
	int fileSize = 0;


	juce::AudioBuffer<float>* gFileBuffer;
	juce::ADSR::Parameters gParam;
};