#include "Voice.h"

CWGVoice::CWGVoice(GeneratorInfo x) {
	voiceInfo = x;
	Timer::startTimer(voiceInfo.grainDensity);
	grains.reserve(100);
	createGrain();
	voiceInfo.adsr.noteOn();
}

CWGVoice::~CWGVoice() {
	Timer::stopTimer();
}

void CWGVoice::timerCallback() {
	if (grains.size() < grains.capacity())
		createGrain();
}

void CWGVoice::processGrains(juce::AudioBuffer<float>& buffer) {
	for (int i = 0; i < grains.size(); ++i)
	{
		auto* grain = grains.at(i);
		grain->process(buffer);

		//Handling special cases
		if (grain->getBufferPos() >= voiceInfo.file->getNumSamples()) {
			grains.erase(grains.begin() + i);
		}
		else if (grain->getBufferPos() <= 0) {
			grains.erase(grains.begin() + i);
		}

		//ADSR ended 
		if (!grain->grainInfo.adsr.isActive()) {
			grains.erase(grains.begin() + i);
		}
	}
}

void CWGVoice::createGrain() {
	double sampleSkipAmount = voiceInfo.pitch + (std::pow(2.0, voiceInfo.note / 12.0) / 32);
	if (grains.size() < grains.capacity())
		grains.push_back(new CWGGrainProcessor(voiceInfo));
}

void CWGVoice::setAdsrOff() {
	voiceInfo.adsr.noteOff();
	for each (auto * grain in grains)
	{
		grain->grainInfo.adsr.noteOff();
	}
}