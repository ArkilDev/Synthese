#pragma once
#include <JuceHeader.h>

struct GeneratorInfo {
	juce::AudioBuffer<float>* file;			//File to read
	double sampleRate = 44100;
	int note = 0;							//Midi note number
	float volume = 1;
	float start = 0;						//Starting sample
	float grainLength = 150;
	float pan = 0;
	float pitch = 0;
	int grainDensity = 75;					//Timer delay to create new grain
	juce::ADSR adsr;
	juce::ADSR::Parameters adsrParam;		//Separated parameters for voice and grain adsr because
	juce::ADSR::Parameters grainAdsrParam;	//CWGVoice needs both to create new grains and for itself
};