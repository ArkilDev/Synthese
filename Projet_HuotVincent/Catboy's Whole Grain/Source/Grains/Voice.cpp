#include "Voice.h"

CWGVoice::CWGVoice(GeneratorInfo x) {
	voiceInfo = x;
	grains.reserve(100);
	createGrain();
	voiceInfo.adsr.setParameters(voiceInfo.adsrParam);
	voiceInfo.adsr.noteOn();
	clock = 1;
}

CWGVoice::~CWGVoice() {
	//Deconstructor
}

void CWGVoice::fakeTimerCallback() {
	if (grains.size() < grains.capacity() && clock % voiceInfo.grainDensity == 0) {
		createGrain();
		clock = 1;
	}
	clock++;
}

void CWGVoice::processGrains(juce::AudioBuffer<float>* const& buffer) {
	for (int i = 0; i < grains.size(); ++i)
	{
		auto* grain = grains.at(i);
		auto temp = buffer->getSample(0, 255);
		grain->process(buffer);
		voiceInfo.adsr.applyEnvelopeToBuffer(*buffer, 0, buffer->getNumSamples());

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
	if (grains.size() < grains.capacity())
		grains.push_back(new CWGGrainProcessor(voiceInfo));
}

void CWGVoice::setAdsrOff() {
	voiceInfo.adsr.noteOff();
	for (int i = 0; i < grains.size(); ++i)
	{
		grains.at(i)->grainInfo.adsr.noteOff();
	}
}