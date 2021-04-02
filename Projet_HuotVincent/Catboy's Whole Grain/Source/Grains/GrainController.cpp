#include "GrainController.h"
#include <iostream>

CWGGrainController::CWGGrainController() {
}

void CWGGrainController::instantiate(juce::AudioBuffer<float>& buffer) {
	cFileBuffer.makeCopyOf(buffer, true);
	cBufferPos = 0;
	cMaxBufferPos = cFileBuffer.getNumSamples();
}

juce::AudioBuffer<float> CWGGrainController::getProcessedBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer* midi) {
	cProcessedBuffer.clear();
	cProcessedBuffer.setSize(buffer->getNumChannels(), buffer->getNumSamples());

	cMidiProcessor.setMidiMessage(*midi);
	cMidiProcessor.process();

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

			cProcessedBuffer.setSample(channel, i, currentVal * (master * cMidiProcessor.getVelocity()));
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

void CWGGrainController::loadFileToBuffer(juce::AudioBuffer<float>& file) {

}

void CWGGrainController::switchLoop() {
	isLooping = !isLooping;
}