#include "GrainController.h"
#include <iostream>

CWGGrainController::CWGGrainController() {
	//callback at every milisecond and let individual voices desides to run or not
	HighResolutionTimer::startTimer(1);
	voices.clear();
	voices.reserve(16);
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
	formatReader = formatManager.createReaderFor(filePath);

	//make buffer big enough and clear buffer related variables
	fileBuffer.setSize((int)formatReader->numChannels, (int)formatReader->lengthInSamples);

	//Add file to buffer
	formatReader->read(&fileBuffer, 0, fileBuffer.getNumSamples(), 0, true, true);
	controllerInfo.file = &fileBuffer;
	hasFile = true;
}

juce::AudioBuffer<float> CWGGrainController::getProcessedBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer midi) {
	processedBuffer.clear();
	processedBuffer.setSize(buffer->getNumChannels(), buffer->getNumSamples());

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
				if (voices.at(i)->voiceInfo.note == currentMessage.getNoteNumber())
					voices.at(i)->setAdsrOff();
		}
	}

	//Grain handling
	for (int i = 0; i < voices.size(); ++i)
	{
		auto* voice = voices.at(i);
		//Process
		auto temp = processedBuffer.getSample(0, 255);
		voice->processGrains(&processedBuffer);

		if (!voice->voiceInfo.adsr.isActive())
			voices.erase(voices.begin() + i);
	}

	processedBuffer.applyGain(master);
	controllerInfo.adsr.applyEnvelopeToBuffer(processedBuffer, 0, processedBuffer.getNumSamples());
	return processedBuffer;
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