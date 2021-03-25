/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
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

private:
	juce::TextButton btnLoad{ "browse" };
	juce::ToggleButton btnLoop{ "Loop" };

	std::vector<float> eSampleVal;
	bool eUpdateWaveDisplay{ false };

	juce::Slider eMasterSlider;
	juce::Label eMasterLabel;

	juce::Slider eAttackSlider, eDecaySlider, eSustainSlider, eReleaseSlider;
	juce::Label eAttackLabel, eDecayLabel, eSustainLabel, eReleaseLabel;

	CWGAudioProcessor& audioProcessor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CWGAudioProcessorEditor)
};
