/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>

//==============================================================================
CWGAudioProcessor::CWGAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	), VTS(*this, nullptr, "APVTS", createParameters())
#endif
{

}

CWGAudioProcessor::~CWGAudioProcessor()
{

}

//==============================================================================
const juce::String CWGAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool CWGAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool CWGAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool CWGAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double CWGAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int CWGAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int CWGAudioProcessor::getCurrentProgram()
{
	return 0;
}

void CWGAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String CWGAudioProcessor::getProgramName(int index)
{
	return {};
}

void CWGAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void CWGAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
	controller.setSampleRate(sampleRate);

	controller.onFxAdd = [&] {
		auto fx = controller.FXs.at(controller.FXs.size() - 1);
		juce::ValueTree t("Fx");

		t.setProperty("Name", juce::String(fx->name), nullptr);
		t.setProperty("FxType", fx->type, nullptr);
		t.setProperty("Wet", fx->wet, nullptr);
		t.setProperty("Uid", fx->uid.toString(), nullptr);

		switch (fx->type) {
		case CWGFx::Type::Bitcrush:
			t.setProperty("Crush", dynamic_cast<CWGBitcrush*>(fx)->crush, nullptr);
			break;
		case CWGFx::Type::Distortion:
			t.setProperty("Drive", dynamic_cast<CWGDistortion*>(fx)->drive, nullptr);
			t.setProperty("Range", dynamic_cast<CWGDistortion*>(fx)->range, nullptr);
			break;
		}

		VTS.state.appendChild(t, nullptr);
	};

	controller.onFxRemove = [&](juce::Uuid uid) {
		VTS.state.removeChild(VTS.state.getChildWithProperty("Uid", uid.toString()), nullptr);
	};

	controller.onFileLoad = [&](juce::String path) {
		if (!VTS.state.getChildWithName("File").isValid()) {
			juce::ValueTree t("File");
			t.setProperty("Path", path, nullptr);
			VTS.state.appendChild(t, nullptr);
		}
		else {
			VTS.state.getChildWithName("File").setProperty("Path", path, nullptr);
		}
	};
}

void CWGAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CWGAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void CWGAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	if (controller.isFileLoaded()) {
		auto tempBuffer = controller.getProcessedBuffer(&buffer, midiMessages);
		for (int i = 0; i < totalNumOutputChannels; ++i)
			buffer.copyFrom(i, 0, tempBuffer.getReadPointer(i), buffer.getNumSamples(), 1);
	}
}

//==============================================================================
bool CWGAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CWGAudioProcessor::createEditor()
{
	return new CWGAudioProcessorEditor(*this);
}

//==============================================================================
void CWGAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.

	std::unique_ptr<juce::XmlElement> xml(VTS.state.createXml());
	copyXmlToBinary(*xml, destData);
}

void CWGAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.

	std::unique_ptr<juce::XmlElement> parameters(getXmlFromBinary(data, sizeInBytes));
	if (parameters != nullptr) {
		if (parameters->hasTagName(VTS.state.getType())) {
			VTS.state = juce::ValueTree::fromXml(*parameters);
			controller.fromValueTree(VTS.state);
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new CWGAudioProcessor();
}

//User-made function ===========================================================

//get file with button
void CWGAudioProcessor::loadFile() {
	juce::FileChooser chooser{ "Please select a file to load" };
	if (chooser.browseForFileToOpen()) {
		auto file = chooser.getResult();
		controller.loadFile(file.getFullPathName());
	}
}

juce::AudioProcessorValueTreeState::ParameterLayout CWGAudioProcessor::createParameters() {
	std::vector<std::unique_ptr<juce::RangedAudioParameter>> VTSParams;

	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("START", "Start", .0f, 1.f, 0.f));
	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("MASTER", "Master", .0f, 1.f, 1.f));
	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("PITCH", "Pitch", .0f, 1.f, 0.f));
	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("PAN", "Pan", -1.f, 1.f, 0.f));
	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("ATTACK", "Attack", .01f, 1.f, 0.f));
	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("DECAY", "Decay", 0, 1.f, 0.f));
	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("SUSTAIN", "Sustain", 0, 1.f, 1.f));
	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("RELEASE", "Release", .01f, 1.f, 0.f));
	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("GRAIN_LEN", "grainLen", 1.f, 500.f, 150.f));
	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("GRAIN_DENS", "grainDens", 20.f, 500.0f, 75.f));
	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("GRAIN_ATK", "grainAtk", .01f, 1.0f, 0.f));
	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("GRAIN_REL", "grainRel", .01f, 1.0f, 0.f));
	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("RAND_POS", "randPos", .0f, 1.0f, 0.f));
	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("RAND_PAN", "randPan", 0.f, 1.0f, 0.f));
	VTSParams.push_back(std::make_unique<juce::AudioParameterFloat>("RAND_VOL", "randVol", .0f, 1.0f, 0.f));

	return { VTSParams.begin(), VTSParams.end() };
}