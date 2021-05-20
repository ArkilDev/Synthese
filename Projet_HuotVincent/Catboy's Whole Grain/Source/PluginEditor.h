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

	//File management
	bool isInterestedInFileDrag(const juce::StringArray& files) override;
	void filesDropped(const juce::StringArray& files, int x, int y) override;

	//Slider management
	void sliderValueChanged(juce::Slider* slider) override;
	float getStartValue() { return eStartSlider.getValue(); };

private:
	CWGWaveformPanel waveformPanel;
	CWGFXListPanel fxListPanel;
	CWGFXViewerPanel fxPanel;
	juce::ImageComponent eLogoImage;

	//Main controls
	juce::Slider eStartSlider, eMasterSlider, ePitchSlider, ePanSlider;
	juce::Label eMasterLabel, ePitchLabel, ePanLabel;

	//ADSR
	juce::Slider eAttackSlider, eDecaySlider, eSustainSlider, eReleaseSlider;
	juce::Label eAttackLabel, eDecayLabel, eSustainLabel, eReleaseLabel;

	//Grain main controls
	juce::Slider eGrainLengthSlider, eGrainDensitySlider, eGrainAttackSlider, eGrainReleaseSlider;
	juce::Label eGrainLengthLabel, eGrainDensityLabel, eGrainAttackLabel, eGrainReleaseLabel;

	//Grain randomizer controls
	//Knobs are also handled by the Slider class :v
	juce::Slider ePosRandKnob, ePanRandKnob, eVolRandKnob;
	juce::Label ePosRandLabel, ePanRandLabel, eVolRandLabel;

	CWGAudioProcessor& audioProcessor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CWGAudioProcessorEditor)
};
