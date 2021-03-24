/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CWGAudioProcessorEditor::CWGAudioProcessorEditor(CWGAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	btnLoad.onClick = [&]() {audioProcessor.loadFile(); };
	addAndMakeVisible(btnLoad);

	btnLoop.onClick = [&]() {audioProcessor.switchLoop(); };
	addAndMakeVisible(btnLoop);

	eAttackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eAttackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eAttackSlider.setRange(.0f, 5.0f, 0.01f);
	addAndMakeVisible(eAttackSlider);

	eAttackLabel.setFont(10.0f);
	eAttackLabel.setText("Attack", juce::NotificationType::dontSendNotification);

	eDecaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eDecaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eDecaySlider.setRange(.0f, 5.0f, 0.01f);
	addAndMakeVisible(eDecaySlider);

	eSustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eSustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eSustainSlider.setRange(.0f, 5.0f, 0.01f);
	addAndMakeVisible(eSustainSlider);

	eReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eReleaseSlider.setRange(.0f, 5.0f, 0.01f);
	addAndMakeVisible(eReleaseSlider);

	setSize(1000, 600);
}

CWGAudioProcessorEditor::~CWGAudioProcessorEditor()
{
}

//==============================================================================
void CWGAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::black);
	g.setColour(juce::Colours::white);
	g.setFont(15.0f);

	if (eUpdateWaveDisplay) {
		juce::Path p;
		eSampleVal.clear();

		//draw waveform
		auto wave = audioProcessor.getWaveform();
		auto ratio = wave.getNumSamples() / getWidth();
		auto buffer = wave.getReadPointer(0);

		//scale x axis
		for (int i = 0; i < wave.getNumSamples(); i += ratio) {
			eSampleVal.push_back(buffer[i]); //get point in sample at ratio interval to make the length fit the window
		}

		//scale y axis and draw path
		p.startNewSubPath(0, getHeight() / 2);
		for (int i = 0; i < eSampleVal.size(); ++i) {
			auto point = juce::jmap<float>(eSampleVal[i], -1.0f, 1.0f, getHeight(), 0); //Map amplitude from (-1, 1) to (maxHeight, minHeight)
			p.lineTo(i, point);
		}

		g.strokePath(p, juce::PathStrokeType(2));

		eUpdateWaveDisplay = false;
	}

	g.drawText(std::to_string(audioProcessor.getSampleCount()), 0, 50, 50, 50, juce::Justification::centred, true);
}

void CWGAudioProcessorEditor::resized()
{
	//ADSR Slider Placements
	const auto sliderX = 0.6f,
		sliderY = 0.5f,
		sliderWidth = 0.07f,
		sliderHeight = 0.35f;
	/*
	eAttackSlider.setBoundsRelative(sliderX, sliderY, sliderWidth, sliderHeight);
	eDecaySlider.setBoundsRelative(sliderX + sliderWidth, sliderY, sliderWidth, sliderHeight);
	eSustainSlider.setBoundsRelative(sliderX + (sliderWidth * 2), sliderY, sliderWidth, sliderHeight);
	eReleaseSlider.setBoundsRelative(sliderX + (sliderWidth * 3), sliderY, sliderWidth, sliderHeight);
	*/

	btnLoad.setBoundsRelative(0, 0, 0.1f, 0.1f);
	btnLoop.setBoundsRelative(0.1f, 0.0f, 0.1f, 0.1f);
}

bool CWGAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files) {

	for (auto file : files) {
		if (file.contains(".wav") || file.contains(".mp3") || file.contains(".aif")) {
			return true;
		}
	}
	return false;
}

void CWGAudioProcessorEditor::filesDropped(const juce::StringArray& files, int x, int y) {
	for (auto file : files) {
		if (isInterestedInFileDrag(file)) {
			eUpdateWaveDisplay = true;
			audioProcessor.loadFile(file);
		}
	}
	repaint();
}