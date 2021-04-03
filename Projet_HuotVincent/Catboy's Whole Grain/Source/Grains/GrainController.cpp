#include "GrainController.h"
#include <iostream>

CWGGrainController::CWGGrainController() {
}

void CWGGrainController::instantiate(juce::AudioBuffer<float>& buffer) {
	cFileBuffer.makeCopyOf(buffer, true);
	cBufferPos = 0;
	cMaxBufferPos = cFileBuffer.getNumSamples();
}

juce::AudioBuffer<float> CWGGrainController::getProcessedBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer& midi) {

	adsrParam.attack = 1.0f;;
	adsrParam.decay = 1.0f;
	adsrParam.sustain = 1.0f;
	adsrParam.release = 1.0f;
	adsr.setParameters(adsrParam);

	cProcessedBuffer.clear();
	cProcessedBuffer.setSize(buffer->getNumChannels(), buffer->getNumSamples());

	midiMessages = midi;
	mIt = new juce::MidiBuffer::Iterator(midiMessages);

	while (mIt->getNextEvent(currentMessage, samplePos)) {
		if (currentMessage.isNoteOn()) {
			adsr.noteOn();
			cBufferPos = 0;
		}

		if (currentMessage.isNoteOff()) {
			adsr.noteOff();
		}
	}

	float* filePointer = 0;
	float currentVal = 0;
	for (auto i = 0; i < buffer->getNumSamples(); ++i) {
		for (auto channel = 0; channel < buffer->getNumChannels(); ++channel) {
			filePointer = cFileBuffer.getWritePointer(channel);

			if (cBufferPos != std::trunc(cBufferPos)) {
				currentVal = (filePointer[(int)std::floor(cBufferPos)] + filePointer[(int)std::ceil(cBufferPos)]) / 2;
			}
			else {
				currentVal = filePointer[(int)cBufferPos];
			}

			cProcessedBuffer.setSample(channel, i, currentVal * (master * adsr.getNextSample()));
		}

		cBufferPos += pitch;

		if (cBufferPos >= cFileBuffer.getNumSamples()) {
			if (isLooping) {
				cBufferPos = 0;
			}
			else {
				cBufferPos = cFileBuffer.getNumSamples();
			}
		}
	}

	return cProcessedBuffer;
}

void CWGGrainController::setADSR(float attack, float decay, float sustain, float release) {
	adsrParam.attack = attack;
	adsrParam.decay = decay;
	adsrParam.sustain = sustain;
	adsrParam.release = release;
	adsr.setParameters(adsrParam);
}

void CWGGrainController::switchLoop() {
	isLooping = !isLooping;
}