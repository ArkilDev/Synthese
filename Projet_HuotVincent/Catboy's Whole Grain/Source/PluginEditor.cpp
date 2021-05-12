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
	startTimerHz(60);

	//Logo
	auto logoImage = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);

	if (!logoImage.isNull())
		eLogoImage.setImage(logoImage, juce::RectanglePlacement::stretchToFit);
	addAndMakeVisible(eLogoImage);

	btnLoad.onClick = [&]() {audioProcessor.loadFile(); };
	addAndMakeVisible(btnLoad);

	//Main controls
	eStartSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	eStartSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
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
	eGrainLengthSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eGrainLengthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eGrainLengthSlider.setRange(1.0f, 500.0f, 0.1f);
	eGrainLengthSlider.addListener(this);
	addAndMakeVisible(eGrainLengthSlider);

	eGrainDensitySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	eGrainDensitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
	eGrainDensitySlider.setRange(1, 500, 1);
	eGrainDensitySlider.setValue(1);
	eGrainDensitySlider.addListener(this);
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

	setSize(1000, 600);
}

CWGAudioProcessorEditor::~CWGAudioProcessorEditor()
{
	stopTimer();
}

//==============================================================================
void CWGAudioProcessorEditor::paint(juce::Graphics& g)
{
	waveBox.setBounds(100, 100, 100, 100);
	g.setColour(juce::Colours::aliceblue);
	g.fillRect(waveBox);

	g.fillAll(juce::Colours::black);
	g.setColour(juce::Colours::white);
	g.setFont(15.0f);

	if (audioProcessor.controller.isFileLoaded()) {
		//get waveform
		auto wave = audioProcessor.controller.getFileBuffer();
		float ratio = wave.getNumSamples() / getWidth();


		if (eUpdateWaveDisplay) {
			auto buffer = wave.getReadPointer(0);
			eSampleVal.clear();


			//scale x axis
			for (int i = 0; i < wave.getNumSamples(); i += ratio) {
				eSampleVal.push_back(buffer[i]); //get point in sample at ratio interval to make the length fit the window
			}

			//scale y axis and draw path
			eWaveform.startNewSubPath(0, getHeight() / 2);
			for (int i = 0; i < eSampleVal.size(); ++i) {
				auto point = juce::jmap<float>(eSampleVal[i], -1.0f, 1.0f, getHeight(), 0); //Map amplitude from (-1, 1) to (minHeight, maxHeight)
				eWaveform.lineTo(i, point);
			}


			eUpdateWaveDisplay = false;
		}
		g.strokePath(eWaveform, juce::PathStrokeType(2));

		if (audioProcessor.controller.isFileLoaded()) {
			for (int i = 0; i < audioProcessor.controller.voices.size(); ++i) {
				CWGVoice* voice = audioProcessor.controller.voices.at(i);
				for (int j = 0; j < voice->grains.size(); ++j) {
					CWGGrainProcessor* grain = voice->grains.at(j);
					auto playHeadPosition = juce::jmap<int>(std::floor(grain->getBufferPos()), 0, audioProcessor.getFileBuffer().getNumSamples(), 0, getWidth());
					float Ypos = getHeight() / 2 + grain->grainInfo.pan * 100;
					g.setColour(juce::Colours::blue);
					g.fillRect((float)playHeadPosition, (float)Ypos, 20.0f, 10.0f);
				}
			}
		}
	}
}

void CWGAudioProcessorEditor::resized()
{
	eLogoImage.setBoundsRelative(0.01f, 0.8f, 0.1f, 0.2f);

	//Main controls
	ePanSlider.setBoundsRelative(0.75f, 0.0f, 0.1f, 0.15f);
	eMasterSlider.setBoundsRelative(0.85f, 0.0f, 0.15f, 0.1f);
	ePitchSlider.setBoundsRelative(0.85f, 0.1f, 0.15f, 0.1f);
	eStartSlider.setBoundsRelative(0, 0.5, 1, 0.1f);

	//ADSR Slider Placements
	const auto sliderX = 0.72f,
		sliderY = 0.65f,
		sliderWidth = 0.07f,
		sliderHeight = 0.35f;

	eAttackSlider.setBoundsRelative(sliderX, sliderY, sliderWidth, sliderHeight);
	eDecaySlider.setBoundsRelative(sliderX + sliderWidth, sliderY, sliderWidth, sliderHeight);
	eSustainSlider.setBoundsRelative(sliderX + (sliderWidth * 2), sliderY, sliderWidth, sliderHeight);
	eReleaseSlider.setBoundsRelative(sliderX + (sliderWidth * 3), sliderY, sliderWidth, sliderHeight);

	//Grain controls
	eGrainLengthSlider.setBoundsRelative(0.15f, 0.8, 0.05f, 0.2f);
	eGrainDensitySlider.setBoundsRelative(0.10f, 0.8, 0.05f, 0.2f);
	eGrainAttackSlider.setBoundsRelative(0.25f, 0.8, 0.05f, 0.2f);
	eGrainReleaseSlider.setBoundsRelative(0.3f, 0.8, 0.05f, 0.2f);

	//Other
	btnLoad.setBoundsRelative(0, 0, 0.1f, 0.1f);
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
	if (slider == &eStartSlider)
		audioProcessor.controller.setSampleStart(slider->getValue());
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

void CWGAudioProcessorEditor::timerCallback() {
	repaint();
}