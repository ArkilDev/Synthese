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
	//Logo
	auto logoImage = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);

	if (!logoImage.isNull())
		eLogoImage.setImage(logoImage, juce::RectanglePlacement::stretchToFit);
	addAndMakeVisible(eLogoImage);

	waveformPanel.initiate(&audioProcessor, &eStartSlider);
	addAndMakeVisible(waveformPanel);

	//Main controls
	eStartSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	eStartSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
	eStartSlider.setRange(.0f, 1.0f, 0.001f);
	eStartSlider.setValue(0);
	eStartSlider.addListener(this);
	addAndMakeVisible(eStartSlider);

	eMasterSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	eMasterSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eMasterSlider.setRange(.0f, 1.0f, 0.01f);
	eMasterSlider.setValue(1);
	eMasterSlider.addListener(this);
	addAndMakeVisible(eMasterSlider);

	ePitchSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	ePitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	ePitchSlider.setRange(.0f, 1.0f, 0.01f);
	ePitchSlider.setValue(0);
	ePitchSlider.addListener(this);
	addAndMakeVisible(ePitchSlider);

	ePanSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	ePanSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	ePanSlider.setRange(-1.0f, 1.0f, 0.01f);
	ePanSlider.setValue(0);
	ePanSlider.addListener(this);
	addAndMakeVisible(ePanSlider);

	//ADSR 
	eAttackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eAttackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eAttackSlider.setRange(.0f, 5.0f, 0.01f);
	eAttackSlider.addListener(this);
	addAndMakeVisible(eAttackSlider);

	eDecaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eDecaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eDecaySlider.setRange(.0f, 5.0f, 0.01f);
	eDecaySlider.addListener(this);
	addAndMakeVisible(eDecaySlider);

	eSustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eSustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eSustainSlider.setRange(.0f, 1.0f, 0.01f);
	eSustainSlider.setValue(1);
	eSustainSlider.addListener(this);
	addAndMakeVisible(eSustainSlider);

	eReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eReleaseSlider.setRange(.0f, 5.0f, 0.01f);
	eReleaseSlider.addListener(this);
	addAndMakeVisible(eReleaseSlider);

	//Grain controls
	eGrainLengthSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	eGrainLengthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
	eGrainLengthSlider.setTextValueSuffix(" ms");
	eGrainLengthSlider.setRange(1, 500, 1);
	eGrainLengthSlider.setValue(150);
	eGrainLengthSlider.addListener(this);
	eGrainLengthLabel.setText("Length", juce::dontSendNotification);
	eGrainLengthLabel.attachToComponent(&eGrainLengthSlider, false);
	addAndMakeVisible(eGrainLengthSlider);

	eGrainDensitySlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	eGrainDensitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
	eGrainDensitySlider.setRange(20, 500, 1);
	eGrainDensitySlider.setTextValueSuffix(" ms");
	eGrainDensitySlider.setValue(75);
	eGrainDensitySlider.addListener(this);
	eGrainDensityLabel.setText("Density", juce::dontSendNotification);
	eGrainDensityLabel.attachToComponent(&eGrainDensitySlider, false);
	addAndMakeVisible(eGrainDensitySlider);

	eGrainAttackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eGrainAttackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eGrainAttackSlider.setRange(.0f, 1.0f, 0.01f);
	eGrainAttackSlider.addListener(this);
	addAndMakeVisible(eGrainAttackSlider);

	eGrainReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eGrainReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eGrainReleaseSlider.setRange(.0f, 1.0f, 0.01f);
	eGrainReleaseSlider.addListener(this);
	addAndMakeVisible(eGrainReleaseSlider);

	//Grain randomizer controls
	eLengthRandKnob, ePosRandKnob, ePanRandKnob, eVolRandKnob;

	setSize(1000, 600);
}

CWGAudioProcessorEditor::~CWGAudioProcessorEditor() {

}

//==============================================================================
void CWGAudioProcessorEditor::paint(juce::Graphics& g)
{
	auto limit = getLocalBounds();
	g.setFont(15.0f);

	g.setColour(juce::Colours::black);
	g.fillRect(limit);
}



void CWGAudioProcessorEditor::resized()
{
	eLogoImage.setBoundsRelative(0.01, 0.8, 0.1, 0.2);

	//Main controls
	ePanSlider.setBoundsRelative(0.75, 0.3, 0.1, 0.15);
	eMasterSlider.setBoundsRelative(0.85, 0.3, 0.15, 0.1);
	ePitchSlider.setBoundsRelative(0.85, 0.4, 0.15, 0.1);
	eStartSlider.setBoundsRelative(0.01, 0.22, 0.8, 0.04);

	//ADSR Slider Placements
	const float sliderX = 0.72,
		sliderY = 0.65,
		sliderWidth = 0.07,
		sliderHeight = 0.35;

	eAttackSlider.setBoundsRelative(sliderX, sliderY, sliderWidth, sliderHeight);
	eDecaySlider.setBoundsRelative(sliderX + sliderWidth, sliderY, sliderWidth, sliderHeight);
	eSustainSlider.setBoundsRelative(sliderX + (sliderWidth * 2), sliderY, sliderWidth, sliderHeight);
	eReleaseSlider.setBoundsRelative(sliderX + (sliderWidth * 3), sliderY, sliderWidth, sliderHeight);

	//Grain controls
	eGrainLengthSlider.setBoundsRelative(0.81, 0.05, 0.19, 0.05);
	eGrainDensitySlider.setBoundsRelative(0.81, 0.15, 0.19, 0.05);
	eGrainAttackSlider.setBoundsRelative(0.25, 0.8, 0.05, 0.2);
	eGrainReleaseSlider.setBoundsRelative(0.3, 0.8, 0.05, 0.2);

	waveformPanel.setBoundsRelative(0.01, 0.01, 0.8, 0.2);
}

//Controllers ==============================================
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
			audioProcessor.controller.loadFile(file);
		}
	}
	repaint();
}

void CWGAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {

	//Main controls
	if (slider == &eMasterSlider)
		audioProcessor.controller.setMaster(slider->getValue());
	if (slider == &ePitchSlider)
		audioProcessor.controller.setPitch(slider->getValue());
	if (slider == &eStartSlider) {
		audioProcessor.controller.setSampleStart(slider->getValue());
		waveformPanel.repaint();
	}
	if (slider == &ePanSlider)
		audioProcessor.controller.setMainPan(slider->getValue());

	//ADSR
	if (slider == &eAttackSlider || slider == &eDecaySlider
		|| slider == &eSustainSlider || slider == &eReleaseSlider)
		audioProcessor.controller.setADSR(eAttackSlider.getValue(),
			eDecaySlider.getValue(),
			eSustainSlider.getValue(),
			eReleaseSlider.getValue());

	//Grain controls
	if (slider == &eGrainLengthSlider)
		audioProcessor.controller.setGrainLen(slider->getValue());
	if (slider == &eGrainDensitySlider)
		audioProcessor.controller.setGrainDensity(slider->getValue());
	if (slider == &eGrainAttackSlider)
		audioProcessor.controller.setGrainAttack(slider->getValue());
	if (slider == &eGrainReleaseSlider)
		audioProcessor.controller.setGrainRelease(slider->getValue());
}