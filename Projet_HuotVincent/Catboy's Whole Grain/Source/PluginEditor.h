/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MidiProcessor.h"

//==============================================================================
/**
*/
class CWGAudioProcessorEditor :
	public juce::AudioProcessorEditor,
	public juce::FileDragAndDropTarget,
	public juce::Slider::Listener,
	public juce::Timer
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

	//Timer
	void timerCallback() override;

private:
	juce::Path eWaveform;

	juce::ImageComponent eLogoImage;

	juce::TextButton btnLoad{ "browse" };
	juce::ToggleButton btnLoop{ "Loop" };

	std::vector<float> eSampleVal;
	bool eUpdateWaveDisplay = false;

	//Main controls
	juce::Slider eStartSlider, eMasterSlider, ePitchSlider;
	juce::Label eStartLabel, eMasterLabel, ePitchLabel;

	//ADSR
	juce::Slider eAttackSlider, eDecaySlider, eSustainSlider, eReleaseSlider;
	juce::Label eAttackLabel, eDecayLabel, eSustainLabel, eReleaseLabel;

	//Grain controls
	juce::Slider eGrainLengthSlider, eGrainAttackSlider, eGrainReleaseSlider;
	juce::Label eGrainLengthLabel, eGrainAttackLabel, eGrainReleaseLabel;

	CWGAudioProcessor& audioProcessor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CWGAudioProcessorEditor)
};
