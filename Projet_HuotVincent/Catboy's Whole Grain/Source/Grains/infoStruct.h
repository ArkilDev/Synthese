#pragma once
#include <JuceHeader.h>

/*
	Note : When processing a voice or a grain, it is generally preferable
	to use a new buffer and then merge it with the output buffer.
	It allows easier per voice modifications such as volume enveloppe,
	whereas applying an ADSR to the output buffer directly would affect
	every voices and grains.

	It is however not actually used in GrainProcessor.cpp since the grains
	values are written directly into the buffer's array and uses no
	buffer-wide functions.
*/

struct GeneratorInfo {
	juce::AudioBuffer<float>* file;			//File to read
	juce::AudioBuffer<float> processBuffer; //Buffer to modify
	double sampleRate = 44100;
	int note = 0;							//Midi note number
	float pitch = 0;
	float volume = 1, randVol = 0;
	float start = 0, randStart = 0;			//Starting sample
	float pan = 0, randPan = 0;
	float grainLength = 150;
	int grainDensity = 75;					//Timer delay to create new grain
	juce::ADSR adsr;
	juce::ADSR::Parameters adsrParam;		//Separated parameters for voice and grain adsr because
	juce::ADSR::Parameters grainAdsrParam;	//CWGVoice needs both to create new grains and for itself
};