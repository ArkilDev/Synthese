/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "MidiProcessor.h"

//==============================================================================
CWGAudioProcessorEditor::CWGAudioProcessorEditor(CWGAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	//Logo
	auto logoImage = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);

	if (!logoImage.isNull())
		eLogoImage.setImage(logoImage, juce::RectanglePlacement::stretchToFit);
	addAndMakeVisible(eLogoImage);

	btnLoad.onClick = [&]() {audioProcessor.loadFile(); };
	addAndMakeVisible(btnLoad);

	btnLoop.onClick = [&]() {audioProcessor.switchLoop(); };
	addAndMakeVisible(btnLoop);

	//Master Knob
	eMasterSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	eMasterSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eMasterSlider.setRange(.0f, 1.0f, 0.1f);
	eMasterSlider.setValue(1);
	eMasterSlider.addListener(this);
	addAndMakeVisible(eMasterSlider);


	//Pitch slider
	ePitchSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	ePitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	ePitchSlider.setRange(-1.0f, 10.0f, 0.01f);
	ePitchSlider.setValue(0);
	ePitchSlider.addListener(this);
	addAndMakeVisible(ePitchSlider);

	//ADSR 
	eAttackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eAttackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eAttackSlider.setRange(.0f, 5.0f, 0.01f);
	eAttackSlider.addListener(this);
	addAndMakeVisible(eAttackSlider);

	eAttackLabel.setFont(10.0f);
	eAttackLabel.setText("Attack", juce::NotificationType::dontSendNotification);

	eDecaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eDecaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eDecaySlider.setRange(.0f, 5.0f, 0.01f);
	eDecaySlider.addListener(this);
	addAndMakeVisible(eDecaySlider);

	eSustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eSustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eSustainSlider.setRange(.0f, 5.0f, 0.01f);
	eSustainSlider.addListener(this);
	addAndMakeVisible(eSustainSlider);

	eReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eReleaseSlider.setRange(.0f, 5.0f, 0.01f);
	eReleaseSlider.addListener(this);
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
		auto wave = audioProcessor.getFileBuffer();
		float ratio = wave.getNumSamples() / getWidth();
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
}

void CWGAudioProcessorEditor::resized()
{
	eLogoImage.setBoundsRelative(0.01f, 0.8f, 0.1f, 0.2f);

	eMasterSlider.setBoundsRelative(0.85f, 0.0f, 0.15f, 0.1f);
	ePitchSlider.setBoundsRelative(0.85f, 0.1f, 0.15f, 0.1f);

	//ADSR Slider Placements
	const auto sliderX = 0.6f,
		sliderY = 0.5f,
		sliderWidth = 0.07f,
		sliderHeight = 0.35f;

	eAttackSlider.setBoundsRelative(sliderX, sliderY, sliderWidth, sliderHeight);
	eDecaySlider.setBoundsRelative(sliderX + sliderWidth, sliderY, sliderWidth, sliderHeight);
	eSustainSlider.setBoundsRelative(sliderX + (sliderWidth * 2), sliderY, sliderWidth, sliderHeight);
	eReleaseSlider.setBoundsRelative(sliderX + (sliderWidth * 3), sliderY, sliderWidth, sliderHeight);

	btnLoad.setBoundsRelative(0, 0, 0.1f, 0.1f);
	btnLoop.setBoundsRelative(0.1f, 0.0f, 0.1f, 0.1f);
}

//File management ==============================================
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

void CWGAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
	if (slider == &eMasterSlider)
		audioProcessor.controller.master = eMasterSlider.getValue();

	if (slider == &ePitchSlider)
		audioProcessor.controller.pitch = ePitchSlider.getValue();

	//ADSR
	if (slider == &eAttackSlider || slider == &eDecaySlider
		|| slider == &eSustainSlider || slider == &eReleaseSlider)
		audioProcessor.controller.setADSR(eAttackSlider.getValue(),
			eDecaySlider.getValue(),
			eSustainSlider.getValue(),
			eReleaseSlider.getValue());
}