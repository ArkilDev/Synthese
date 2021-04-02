/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "MidiProcessor.h"
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
	)
#endif
{
	pFormatManager.registerBasicFormats();
}

CWGAudioProcessor::~CWGAudioProcessor()
{
	pFormatReader = nullptr;
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

	if (hasFile) {
		//temporary buffer to copy from pGrainController more safely
		auto tempBuffer = controller.getProcessedBuffer(&buffer, &midiMessages);

		for (int i = 0; i < totalNumOutputChannels; ++i) {
			buffer.clear(i, 0, buffer.getNumSamples());
			buffer.copyFrom(i, 0, tempBuffer.getReadPointer(i), buffer.getNumSamples(), 1);
		}
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
}

void CWGAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new CWGAudioProcessor();
}

//User-made function ===========================================================

//File systems
void CWGAudioProcessor::loadFile(const juce::String& filePath) {
	pFormatReader = pFormatManager.createReaderFor(filePath);

	//make buffer big enough and clear buffer related variables
	pFileBuffer.setSize((int)pFormatReader->numChannels, (int)pFormatReader->lengthInSamples);

	//Add file to buffer
	pFormatReader->read(&pFileBuffer, 0, pFileBuffer.getNumSamples(), 0, true, true);
	controller.instantiate(pFileBuffer);
	hasFile = true;
}

//get file with button
void CWGAudioProcessor::loadFile() {
	juce::FileChooser chooser{ "Please select a file to load" };
	if (chooser.browseForFileToOpen()) {
		auto file = chooser.getResult();
		CWGAudioProcessor::loadFile(file.getFullPathName());
	}
}

void CWGAudioProcessor::switchLoop() {
	controller.switchLoop();
}