/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"

class CWGWaveformPanel : public juce::Component, juce::Timer {
public:
	CWGWaveformPanel() {
		setPaintingIsUnclipped(true);
		juce::Timer::startTimerHz(30);
	};

	void timerCallback() override {
		repaint();
	}

	void initiate(CWGAudioProcessor* p, juce::Slider* start) {
		processor = p;
		startSlider = start;
		controller = &processor->controller;
		btnLoad.onClick = [&]() {processor->loadFile(); };
		addAndMakeVisible(btnLoad);
	};

	void paint(juce::Graphics& g) override {
		g.setColour(juce::Colours::darkgrey);
		g.drawRect(getLocalBounds());

		if (controller->isFileLoaded()) {
			drawWaveform(g);
			btnLoad.setVisible(false);
		}
		else {
			btnLoad.setVisible(true);
		}
	}

	void resized() override {
		btnLoad.setBoundsRelative(0.4f, 0.3f, 0.2f, 0.4f);
	}

	void drawWaveform(juce::Graphics& g) {
		//get waveform
		g.setColour(juce::Colours::white);
		wave = controller->getFileBuffer();
		ratio = wave.getNumSamples() / getWidth();
		auto buffer = wave.getArrayOfReadPointers();


		//Get waveform if empty
		if (waveform.isEmpty()) {
			eSampleVal.clear();
			for (int i = 0; i < wave.getNumSamples(); i += ratio) {
				eSampleVal.push_back(buffer[0][i]); //get point in sample at ratio interval to make the length fit the window
			}

			//scale y axis
			waveform.startNewSubPath(0, getHeight() / 2);
			for (int i = 0; i < eSampleVal.size(); ++i) {
				auto point = juce::jmap<float>(eSampleVal[i], -1.0f, 1.0f, getHeight(), 0);
				waveform.lineTo(i, point);
			}
		}
		g.strokePath(waveform, juce::PathStrokeType(2));

		//Draw start line
		g.setColour(juce::Colour::fromFloatRGBA(0, 0.5, 1, 0.5));
		g.fillRect(startSlider->getValue() * getWidth(), 0, 5, getHeight());

		//Draw grains
		g.setColour(juce::Colour::fromFloatRGBA(0, 0.5, 1, 0.5));
		if (controller->isFileLoaded()) {
			int count = 0;
			for (int i = 0; i < controller->voices.size(); ++i) {
				CWGVoice* voice = controller->voices.at(i);
				for (int j = 0; j < voice->grains.size(); ++j) {
					++count;
					if (count < 30) {	//Limit grain drawing count for speed purpose
						CWGGrainProcessor* grain = voice->grains.at(j);
						float Xpos = std::floor(grain->getBufferPos()) / controller->getFileBuffer().getNumSamples() * getWidth();
						float Ypos = (getHeight() / 2) + grain->grainInfo.pan * ((getHeight() - 20) / 2);
						g.fillRect(Xpos, Ypos, grain->grainInfo.grainLength / 500 * 20, 5.0f);
					}
				}
			}
		}
	}

	juce::Slider* startSlider;

private:
	CWGAudioProcessor* processor;
	CWGGrainController* controller;
	bool updateWaveDisplay = false;

	juce::Path waveform;
	std::vector<float> eSampleVal;
	bool eUpdateWaveDisplay = false;

	juce::Label startLabel;
	juce::TextButton btnLoad{ "browse" };
	juce::AudioBuffer<float> wave;
	float ratio;
};