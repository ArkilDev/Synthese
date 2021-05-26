#include "GrainController.h"
#include <iostream>

CWGGrainController::CWGGrainController() {
	//callback at every milisecond and let individual voices desides to run or not
	HighResolutionTimer::startTimer(1);
	voices.clear();
	voices.reserve(8);
	FXs.reserve(16);
	formatManager.registerBasicFormats();
}

void CWGGrainController::fromValueTree(juce::ValueTree tree) {
	auto filePath = tree.getChildWithName("File").getPropertyAsValue("Path", nullptr).toString();
	if (filePath != "")
		loadFile(filePath, false);

	startRatio = tree.getChildWithProperty("id", "START").getPropertyAsValue("value", nullptr).getValue();
	controllerInfo.volume = tree.getChildWithProperty("id", "MASTER").getPropertyAsValue("value", nullptr).getValue();
	controllerInfo.pitch = tree.getChildWithProperty("id", "PITCH").getPropertyAsValue("value", nullptr).getValue();
	controllerInfo.pan = tree.getChildWithProperty("id", "PAN").getPropertyAsValue("value", nullptr).getValue();
	controllerInfo.adsrParam.attack = tree.getChildWithProperty("id", "ATTACK").getPropertyAsValue("value", nullptr).getValue();
	controllerInfo.adsrParam.decay = tree.getChildWithProperty("id", "DECAY").getPropertyAsValue("value", nullptr).getValue();
	controllerInfo.adsrParam.sustain = tree.getChildWithProperty("id", "SUSTAIN").getPropertyAsValue("value", nullptr).getValue();
	controllerInfo.adsrParam.release = tree.getChildWithProperty("id", "RELEASE").getPropertyAsValue("value", nullptr).getValue();
	controllerInfo.grainLength = tree.getChildWithProperty("id", "GRAIN_LEN").getPropertyAsValue("value", nullptr).getValue();
	controllerInfo.grainDensity = tree.getChildWithProperty("id", "GRAIN_DENS").getPropertyAsValue("value", nullptr).getValue();
	controllerInfo.grainAdsrParam.attack = tree.getChildWithProperty("id", "GRAIN_ATK").getPropertyAsValue("value", nullptr).getValue();
	controllerInfo.grainAdsrParam.decay = tree.getChildWithProperty("id", "GRAIN_REL").getPropertyAsValue("value", nullptr).getValue();
	controllerInfo.randStart = tree.getChildWithProperty("id", "RAND_POS").getPropertyAsValue("value", nullptr).getValue();
	controllerInfo.randPan = tree.getChildWithProperty("id", "RAND_PAN").getPropertyAsValue("value", nullptr).getValue();
	controllerInfo.randVol = tree.getChildWithProperty("id", "RAND_VOL").getPropertyAsValue("value", nullptr).getValue();

	for (auto& child : tree) {
		if (child.hasType("Fx")) {
			auto type = child.getPropertyAsValue("FxType", nullptr).getValue();
			if (type == juce::String(CWGFx::Type::Distortion)) {
				auto fx = new CWGDistortion(child.getPropertyAsValue("Name", nullptr).toString());
				fx->wet = child.getPropertyAsValue("Wet", nullptr).getValue();
				fx->uid = child.getPropertyAsValue("Uid", nullptr).getValue();
				fx->drive = child.getPropertyAsValue("Drive", nullptr).getValue();
				fx->range = child.getPropertyAsValue("Range", nullptr).getValue();
				FXs.push_back(fx);
			}

			if (type == juce::String(CWGFx::Type::Bitcrush)) {
				auto fx = new CWGBitcrush(child.getPropertyAsValue("Name", nullptr).toString());
				fx->wet = child.getPropertyAsValue("Wet", nullptr).getValue();
				fx->uid = child.getPropertyAsValue("Uid", nullptr).getValue();
				fx->crush = child.getPropertyAsValue("Crush", nullptr).getValue();
				FXs.push_back(fx);
			}
		}
	}

	isReady = true;
}

CWGGrainController::~CWGGrainController() {
	formatReader = nullptr;
	HighResolutionTimer::stopTimer();
}

void CWGGrainController::hiResTimerCallback() {
	for (auto* voice : voices) {
		voice->fakeTimerCallback();
	}
}

//File systems
void CWGGrainController::loadFile(const juce::String& filePath, bool callLambda) {
	fileBuffer.clear();
	controllerInfo.file = &fileBuffer;
	formatReader = formatManager.createReaderFor(filePath);

	//make buffer big enough and clear buffer related variables
	fileBuffer.setSize((int)formatReader->numChannels, (int)formatReader->lengthInSamples);

	//Add file to buffer
	formatReader->read(&fileBuffer, 0, fileBuffer.getNumSamples(), 0, true, true);
	controllerInfo.file = &fileBuffer;
	hasFile = true;

	if (callLambda)
		onFileLoad(filePath);
}

juce::AudioBuffer<float> CWGGrainController::getProcessedBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer& midi) {
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

	//FX
	for (int i = 0; i < FXs.size(); ++i) {
		FXs[i]->process(&controllerInfo.processBuffer);
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

void CWGGrainController::addFx(int id, bool callLambda) {
	std::string fxCountStr = std::to_string(FXs.size());
	switch ((CWGFx::Type)id) {
	default: break;
	case CWGFx::Type::Bitcrush: FXs.push_back(new CWGBitcrush("Crush" + fxCountStr)); break;
	case CWGFx::Type::Distortion: FXs.push_back(new CWGDistortion("Dist" + fxCountStr)); break;
	}

	if (callLambda)
		onFxAdd(); //Call Lambda in processor to add parameters to ValueTreeState
}

void CWGGrainController::removeFx(int id) {
	onFxRemove(FXs.at(id)->uid); //Call Lambda in processor to remove parameter from ValueTreeState
	FXs.erase(FXs.begin() + id);
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