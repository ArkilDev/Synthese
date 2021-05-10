#include "GrainController.h"
#include <iostream>

CWGGrainController::CWGGrainController() {
	voices.clear();
	voices.reserve(16);
}

CWGGrainController::~CWGGrainController() {
	//Deconstructor
}

void CWGGrainController::instantiate(juce::AudioBuffer<float>* buffer) {
	controllerInfo.file = buffer;
}

juce::AudioBuffer<float> CWGGrainController::getProcessedBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer& midi) {
	cProcessedBuffer.clear();
	cProcessedBuffer.setSize(buffer->getNumChannels(), buffer->getNumSamples());

	//Midi Handling
	mIt = new juce::MidiBuffer::Iterator(midi);
	while (mIt->getNextEvent(currentMessage, iteratorPos)) {
		if (currentMessage.isNoteOn()) {
			controllerInfo.adsr.noteOn();
			controllerInfo.note = currentMessage.getNoteNumber();
			controllerInfo.start = std::floor(startRatio * controllerInfo.file->getNumSamples());
			voices.push_back(new CWGVoice(controllerInfo));
		}

		if (currentMessage.isNoteOff()) {
			for (int i = 0; i < voices.size(); ++i)
				if (voices.at(i)->getNote() == currentMessage.getNoteNumber()) voices.at(i)->setAdsrOff();
		}
	}

	//Grain handling
	for (int i = 0; i < voices.size(); ++i)
	{
		auto* voice = voices.at(i);
		//Process
		voice->processGrains(cProcessedBuffer);

		if (!voice->isAdsrActive()) {
			voices.erase(voices.begin() + i);
		}
	}

	cProcessedBuffer.applyGain(master);
	controllerInfo.adsr.applyEnvelopeToBuffer(cProcessedBuffer, 0, cProcessedBuffer.getNumSamples());
	return cProcessedBuffer;
}

void CWGGrainController::setADSR(float attack, float decay, float sustain, float release) {
	controllerInfo.adsrParam.attack = attack;
	controllerInfo.adsrParam.decay = decay;
	controllerInfo.adsrParam.sustain = sustain;
	controllerInfo.adsrParam.release = release;
	controllerInfo.adsr.setParameters(controllerInfo.adsrParam);
}

void CWGGrainController::setPitch(float x) {
	for (auto* voice : voices)
		voice->setPitch(x);
}