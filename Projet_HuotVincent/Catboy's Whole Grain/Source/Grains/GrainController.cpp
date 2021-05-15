#include "GrainController.h"
#include <iostream>

CWGGrainController::CWGGrainController() {
	//callback at every milisecond and let individual voices desides to run or not
	HighResolutionTimer::startTimer(1);
	voices.clear();
	voices.reserve(8);
	formatManager.registerBasicFormats();
}

CWGGrainController::~CWGGrainController() {
	formatReader = nullptr;
}

void CWGGrainController::instantiate(juce::AudioBuffer<float>* buffer) {
	controllerInfo.file = buffer;
}

void CWGGrainController::hiResTimerCallback() {
	for (auto* voice : voices) {
		voice->fakeTimerCallback();
	}

}

//File systems
void CWGGrainController::loadFile(const juce::String& filePath) {
	fileBuffer.clear();
	formatReader = formatManager.createReaderFor(filePath);

	//make buffer big enough and clear buffer related variables
	fileBuffer.setSize((int)formatReader->numChannels, (int)formatReader->lengthInSamples);

	//Add file to buffer
	formatReader->read(&fileBuffer, 0, fileBuffer.getNumSamples(), 0, true, true);
	controllerInfo.file = &fileBuffer;
	hasFile = true;
}

juce::AudioBuffer<float> CWGGrainController::getProcessedBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer midi) {
	controllerInfo.processBuffer.clear();
	controllerInfo.processBuffer.setSize(buffer->getNumChannels(), buffer->getNumSamples());

	//Midi Handling
	mIt = new juce::MidiBuffer::Iterator(midi);
	while (mIt->getNextEvent(currentMessage, iteratorPos)) {
		if (currentMessage.isNoteOn()) {
			controllerInfo.note = currentMessage.getNoteNumber();
			controllerInfo.start = std::floor(startRatio * controllerInfo.file->getNumSamples());
			voices.push_back(new CWGVoice(controllerInfo));
		}

		if (currentMessage.isNoteOff()) {
			for (int i = 0; i < voices.size(); ++i)
				if (voices.at(i)->voiceInfo.note == currentMessage.getNoteNumber())
					voices.at(i)->voiceInfo.adsr.noteOff();
		}
	}

	//Voice handling
	for (int i = 0; i < voices.size(); ++i)
	{
		auto* voice = voices.at(i);
		voice->processGrains(&controllerInfo.processBuffer);

		if (!voice->voiceInfo.adsr.isActive())
			voices.erase(voices.begin() + i);
	}

	//Pan handling
	if (controllerInfo.pan > 0) {
		controllerInfo.processBuffer.applyGain(0, 0, controllerInfo.processBuffer.getNumSamples(), 1 - controllerInfo.pan);
	}
	else {
		controllerInfo.processBuffer.applyGain(1, 0, controllerInfo.processBuffer.getNumSamples(), 1 - std::abs(controllerInfo.pan));
	}

	controllerInfo.processBuffer.applyGain(controllerInfo.volume);

	return controllerInfo.processBuffer;
}

void CWGGrainController::setADSR(float attack, float decay, float sustain, float release) {
	controllerInfo.adsrParam.attack = attack;
	controllerInfo.adsrParam.decay = decay;
	controllerInfo.adsrParam.sustain = sustain;
	controllerInfo.adsrParam.release = release;
	controllerInfo.adsr.setParameters(controllerInfo.adsrParam);
}

void CWGGrainController::setPitch(float x) {
	controllerInfo.pitch = x;
	for (auto* voice : voices)
		voice->setPitch(x);
}