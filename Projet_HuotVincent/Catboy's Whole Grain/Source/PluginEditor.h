/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Visuals/WaveformPanel.h"
#include "Visuals/FXListPanel.h"
#include "Visuals/FXViewerPanel.h"

//==============================================================================

class CWGAudioProcessorEditor :
	public juce::AudioProcessorEditor,
	public juce::FileDragAndDropTarget,
	public juce::Slider::Listener
{
public:
	CWGAudioProcessorEditor(CWGAudioProcessor&);
	~CWGAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;
	void setupView();

	//File management
	bool isInterestedInFileDrag(const juce::StringArray& files) override;
	void filesDropped(const juce::StringArray& files, int x, int y) override;

	//Slider management
	void sliderValueChanged(juce::Slider* slider) override;
	float getStartValue() { return eStartSlider.getValue(); };

private:
	bool isReady = false;
	CWGWaveformPanel waveformPanel;
	CWGFXListPanel fxListPanel;
	CWGFXViewerPanel fxPanel;
	juce::ImageComponent eLogoImage;

	//Main controls
	juce::Slider eStartSlider, eMasterSlider, ePitchSlider, ePanSlider;
	juce::Label	eMasterLabel, ePitchLabel, ePanLabel;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> startAth, masterAth, pitchAth, panAth;

	//ADSR
	juce::Slider eAttackSlider, eDecaySlider, eSustainSlider, eReleaseSlider;
	juce::Label eAttackLabel, eDecayLabel, eSustainLabel, eReleaseLabel;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAth, decayAth, sustainAth, releaseAth;

	//Grain main controls
	juce::Slider eGrainLengthSlider, eGrainDensitySlider, eGrainAttackSlider, eGrainReleaseSlider;
	juce::Label eGrainLengthLabel, eGrainDensityLabel, eGrainAttackLabel, eGrainReleaseLabel;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> grainLenAth, grainDensAth, grainAtkAth, grainRelAth;

	//Grain randomizer controls
	//Knobs are also handled by the Slider class for some reasons :v
	juce::Slider ePosRandKnob, ePanRandKnob, eVolRandKnob;
	juce::Label ePosRandLabel, ePanRandLabel, eVolRandLabel;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> posRandAth, panRandAth, volRandAth;

	CWGAudioProcessor& audioProcessor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CWGAudioProcessorEditor)
};
