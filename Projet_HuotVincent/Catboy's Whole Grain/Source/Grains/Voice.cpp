#include "Voice.h"

CWGVoice::CWGVoice(GeneratorInfo x) {
	voiceInfo = x;
	grains.reserve(100);
	createGrain();
	voiceInfo.adsr.reset();
	voiceInfo.adsr.setParameters(voiceInfo.adsrParam);
	voiceInfo.adsr.noteOn();
	clock = 1;
}

void CWGVoice::processGrains(juce::AudioBuffer<float>* buffer)
{
	voiceInfo.processBuffer = *buffer;
	for (int i = 0; i < grains.size(); ++i)
	{
		auto* grain = grains.at(i);
		grain->process(&voiceInfo.processBuffer);

		//Delete if ADSR ended 
		if (!grain->grainInfo.adsr.isActive()) {
			grains.erase(grains.begin() + i);
		}
	}

	voiceInfo.adsr.applyEnvelopeToBuffer(voiceInfo.processBuffer, 0, voiceInfo.processBuffer.getNumSamples());

	for (int i = 0; i < buffer->getNumChannels(); ++i)
		buffer->addFrom(i, 0, voiceInfo.processBuffer, i, 0, buffer->getNumSamples(), 1);
}

void CWGVoice::fakeTimerCallback() {
	if (grains.size() < grains.capacity() && clock % voiceInfo.grainDensity == 0) {
		createGrain();
		clock = 1;
	}
	clock++;
}

void CWGVoice::createGrain() {
	if (grains.size() < grains.capacity())
		grains.push_back(new CWGGrainProcessor(voiceInfo));
}