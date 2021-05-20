#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"

class CWGFXViewerPanel : public juce::Component {
public:
	CWGFXViewerPanel() {
		setPaintingIsUnclipped(true);
	}

	void initiate(CWGGrainController* c) {
		controller = c;
	}

	void paint(juce::Graphics& g) override {
		g.setColour(juce::Colours::darkgrey);
		g.drawRect(getLocalBounds());
	}

	void resized() override {

	}

private:
	CWGGrainController* controller;
};