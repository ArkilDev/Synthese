/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Visuals/FXListPanel.h"
#include "Visuals/FXViewerPanel.h"

//==============================================================================
CWGAudioProcessorEditor::CWGAudioProcessorEditor(CWGAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	//Logo
	auto logoImage = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);

	if (!logoImage.isNull())
		eLogoImage.setImage(logoImage, juce::RectanglePlacement::stretchToFit);
	addAndMakeVisible(eLogoImage);

	setupView();
	addAndMakeVisible(waveformPanel);
	addAndMakeVisible(fxListPanel);
	addAndMakeVisible(fxPanel);

	//Main controls
	startAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "START", eStartSlider);
	eStartSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	eStartSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
	eStartSlider.addListener(this);
	addAndMakeVisible(eStartSlider);


	masterAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "MASTER", eMasterSlider);
	eMasterSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	eMasterSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
	eMasterSlider.addListener(this);
	eMasterLabel.setText("Volume", juce::dontSendNotification);
	eMasterLabel.attachToComponent(&eMasterSlider, false);
	eMasterLabel.setJustificationType(juce::Justification::left);
	addAndMakeVisible(eMasterSlider);

	pitchAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "PITCH", ePitchSlider);
	ePitchSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	ePitchSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 40, 20);
	ePitchSlider.addListener(this);
	ePitchLabel.setText("Pitch", juce::dontSendNotification);
	ePitchLabel.attachToComponent(&ePitchSlider, false);
	ePitchLabel.setJustificationType(juce::Justification::left);
	addAndMakeVisible(ePitchSlider);

	panAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "PAN", ePanSlider);
	ePanSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	ePanSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
	ePanSlider.addListener(this);
	addAndMakeVisible(ePanSlider);

	//ADSR 
	attackAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "ATTACK", eAttackSlider);
	eAttackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eAttackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
	eAttackSlider.addListener(this);
	eAttackLabel.setText("Attack", juce::dontSendNotification);
	eAttackLabel.attachToComponent(&eAttackSlider, false);
	eAttackLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(eAttackSlider);

	decayAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "DECAY", eDecaySlider);
	eDecaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eDecaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
	eDecaySlider.addListener(this);
	eDecayLabel.setText("Decay", juce::dontSendNotification);
	eDecayLabel.attachToComponent(&eDecaySlider, false);
	eDecayLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(eDecaySlider);

	sustainAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "SUSTAIN", eSustainSlider);
	eSustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eSustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
	eSustainSlider.addListener(this);
	eSustainLabel.setText("Sustain", juce::dontSendNotification);
	eSustainLabel.attachToComponent(&eSustainSlider, false);
	eSustainLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(eSustainSlider);

	releaseAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "RELEASE", eReleaseSlider);
	eReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
	eReleaseSlider.addListener(this);
	eReleaseLabel.setText("Release", juce::dontSendNotification);
	eReleaseLabel.attachToComponent(&eReleaseSlider, false);
	eReleaseLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(eReleaseSlider);

	//Grain controls
	grainLenAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "GRAIN_LEN", eGrainLengthSlider);
	eGrainLengthSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	eGrainLengthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
	eGrainLengthSlider.setTextValueSuffix(" ms");
	eGrainLengthSlider.addListener(this);
	eGrainLengthLabel.setText("Grain Length", juce::dontSendNotification);
	eGrainLengthLabel.attachToComponent(&eGrainLengthSlider, false);
	eGrainLengthLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(eGrainLengthSlider);

	grainDensAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "GRAIN_DENS", eGrainDensitySlider);
	eGrainDensitySlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	eGrainDensitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
	eGrainDensitySlider.setTextValueSuffix(" ms");
	eGrainDensitySlider.addListener(this);
	eGrainDensityLabel.setText("Grain Density", juce::dontSendNotification);
	eGrainDensityLabel.attachToComponent(&eGrainDensitySlider, false);
	eGrainDensityLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(eGrainDensitySlider);

	grainAtkAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "GRAIN_ATK", eGrainAttackSlider);
	eGrainAttackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eGrainAttackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
	eGrainAttackSlider.addListener(this);
	eGrainAttackLabel.setText("Grain Attack", juce::dontSendNotification);
	eGrainAttackLabel.attachToComponent(&eGrainAttackSlider, false);
	eGrainAttackLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(eGrainAttackSlider);

	grainRelAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "GRAIN_REL", eGrainReleaseSlider);
	eGrainReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eGrainReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
	eGrainReleaseSlider.addListener(this);
	eGrainReleaseLabel.setText("Grain release", juce::dontSendNotification);
	eGrainReleaseLabel.attachToComponent(&eGrainReleaseSlider, false);
	eGrainReleaseLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(eGrainReleaseSlider);

	//Grain randomizer controls
	posRandAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "RAND_POS", ePosRandKnob);
	ePosRandKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	ePosRandKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
	ePosRandKnob.addListener(this);
	ePosRandLabel.setText("Position random", juce::dontSendNotification);
	ePosRandLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(ePosRandLabel);
	addAndMakeVisible(ePosRandKnob);

	panRandAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "RAND_PAN", ePanRandKnob);
	ePanRandKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	ePanRandKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
	ePanRandKnob.addListener(this);
	ePanRandLabel.setText("Pan random", juce::dontSendNotification);
	ePanRandLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(ePanRandLabel);
	addAndMakeVisible(ePanRandKnob);

	volRandAth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.VTS, "RAND_VOL", eVolRandKnob);
	eVolRandKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	eVolRandKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
	eVolRandKnob.addListener(this);
	eVolRandLabel.setText("Volume random", juce::dontSendNotification);
	eVolRandLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(eVolRandLabel);
	addAndMakeVisible(eVolRandKnob);

	setSize(1000, 600);
}

void CWGAudioProcessorEditor::setupView() {
	waveformPanel.initiate(&audioProcessor, &eStartSlider);
	fxListPanel.initiate(&audioProcessor, &fxPanel);
	fxPanel.instantiate(&audioProcessor);
}

CWGAudioProcessorEditor::~CWGAudioProcessorEditor() {

}

//==============================================================================
void CWGAudioProcessorEditor::paint(juce::Graphics& g)
{
	//Re-setup once the controller is loaded
	if (audioProcessor.controller.isReady && !this->isReady) {
		setupView();
		isReady = true;
	}

	auto limit = getLocalBounds();
	g.setFont(15.0f);

	g.setColour(juce::Colours::black);
	g.fillRect(limit);
}

void CWGAudioProcessorEditor::resized()
{
	eLogoImage.setBoundsRelative(0.01, 0.8, 0.1, 0.2);

	//Main controls
	eMasterSlider.setBoundsRelative(0.81, 0.05, 0.19, 0.05);
	ePanSlider.setBoundsRelative(0.81, 0.15, 0.19, 0.05);
	ePitchSlider.setBoundsRelative(0.81, 0.25, 0.19, 0.05);
	eStartSlider.setBoundsRelative(0.01, 0.22, 0.8, 0.04);

	//ADSR Slider Placements
	const float sliderX = 0.25,
		sliderY = 0.35,
		sliderWidth = 0.07,
		sliderHeight = 0.35;

	eAttackSlider.setBoundsRelative(sliderX, sliderY, sliderWidth, sliderHeight);
	eDecaySlider.setBoundsRelative(sliderX + sliderWidth, sliderY, sliderWidth, sliderHeight);
	eSustainSlider.setBoundsRelative(sliderX + (sliderWidth * 2), sliderY, sliderWidth, sliderHeight);
	eReleaseSlider.setBoundsRelative(sliderX + (sliderWidth * 3), sliderY, sliderWidth, sliderHeight);

	//Grain controls
	eGrainLengthSlider.setBoundsRelative(0.62, 0.35, 0.38, 0.05);
	eGrainDensitySlider.setBoundsRelative(0.62, 0.45, 0.38, 0.05);

	eGrainAttackSlider.setBoundsRelative(0.52, 0.35, 0.1, 0.15);
	eGrainReleaseSlider.setBoundsRelative(0.52, 0.55, 0.1, 0.15);

	//Grain randomizer controls
	ePosRandKnob.setBoundsRelative(0.01, 0.3, 0.1, 0.2);
	ePanRandKnob.setBoundsRelative(0.14, 0.3, 0.1, 0.2);
	eVolRandKnob.setBoundsRelative(0.07, 0.5, 0.1, 0.2);

	ePosRandLabel.setBoundsRelative(0.035, 0.28, 0.05, 0.2);
	ePanRandLabel.setBoundsRelative(0.165, 0.28, 0.05, 0.2);
	eVolRandLabel.setBoundsRelative(0.095, 0.48, 0.05, 0.2);

	waveformPanel.setBoundsRelative(0.01, 0.01, 0.8, 0.2);
	fxListPanel.setBoundsRelative(0.62, 0.55, 0.37, 0.44);
	fxPanel.setBoundsRelative(0.12, 0.75, 0.48, 0.24);
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
		audioProcessor.controller.controllerInfo.volume = slider->getValue();
	if (slider == &ePitchSlider)
		audioProcessor.controller.setPitch(slider->getValue()); //use setPitch to change voices pitch dynamically
	if (slider == &eStartSlider) {
		audioProcessor.controller.setSampleStart(slider->getValue()); //Need to use setSampleStart instead of controllerInfo for starting position
		waveformPanel.repaint();
	}
	if (slider == &ePanSlider)
		audioProcessor.controller.controllerInfo.pan = slider->getValue();

	//ADSR
	if (slider == &eAttackSlider || slider == &eDecaySlider
		|| slider == &eSustainSlider || slider == &eReleaseSlider)
		audioProcessor.controller.setADSR(eAttackSlider.getValue(),
			eDecaySlider.getValue(),
			eSustainSlider.getValue(),
			eReleaseSlider.getValue());

	//Grain controls
	if (slider == &eGrainLengthSlider)
		audioProcessor.controller.controllerInfo.grainLength = slider->getValue();
	if (slider == &eGrainDensitySlider)
		audioProcessor.controller.controllerInfo.grainDensity = slider->getValue();
	if (slider == &eGrainAttackSlider)
		audioProcessor.controller.controllerInfo.grainAdsrParam.attack = slider->getValue();
	if (slider == &eGrainReleaseSlider)
		audioProcessor.controller.controllerInfo.grainAdsrParam.decay = slider->getValue();

	//Grain randomizer controls
	if (slider == &ePosRandKnob)
		audioProcessor.controller.controllerInfo.randStart = slider->getValue();
	if (slider == &ePanRandKnob)
		audioProcessor.controller.controllerInfo.randPan = slider->getValue();
	if (slider == &eVolRandKnob)
		audioProcessor.controller.controllerInfo.randVol = slider->getValue();
}