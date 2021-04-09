#include "GrainController.h"
#include <iostream>

CWGGrainController::CWGGrainController() {
	voices.clear();
}

void CWGGrainController::instantiate(juce::AudioBuffer<float>& buffer) {
	cFileBuffer.makeCopyOf(buffer, true);
}

juce::AudioBuffer<float> CWGGrainController::getProcessedBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer& midi) {
	cProcessedBuffer.clear();
	cProcessedBuffer.setSize(buffer->getNumChannels(), buffer->getNumSamples());

	midiMessages = midi;
	mIt = new juce::MidiBuffer::Iterator(midiMessages);

	while (mIt->getNextEvent(currentMessage, samplePos)) {
		if (currentMessage.isNoteOn()) {
			double notePitch = pitch + getNotePitch(currentMessage.getNoteNumber());
			voices.add(new CWGGrainProcessor(cFileBuffer, currentMessage.getNoteNumber(), notePitch, adsrParam, sampleRate, grainLength));
		}

		if (currentMessage.isNoteOff()) {
			for (auto* grain : voices) {
				if (currentMessage.getNoteNumber() == grain->getNote())
					grain->adsr.noteOff();
			}
		}
	}

	if (!voices.isEmpty()) {
		for (auto* grain : voices)
		{
			grain->process(cProcessedBuffer);

			if (grain->getBufferPos() >= cFileBuffer.getNumSamples()) {
				if (isLooping) {
					grain->setBufferPos(0);
				}
				else {
					grain->setBufferPos(cFileBuffer.getNumSamples());
				}
			}

			if (!grain->adsr.isActive())
				voices.removeObject(grain);

			if (grain->getBufferPos() >= cFileBuffer.getNumSamples()) {
				if (isLooping) {
					grain->setBufferPos(0);
				}
				else {
					voices.removeObject(grain);
				}
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
}

void CWGGrainController::switchLoop() {
	isLooping = !isLooping;
}

void CWGGrainController::setPitch(float x) {
	pitch = x;
	for (auto* grain : voices) {
		grain->setPitch(pitch + getNotePitch(grain->getNote()));
	}
}

double CWGGrainController::getNotePitch(int note) {
	return (std::pow(2.0, note / 12.0)) / 32;
}