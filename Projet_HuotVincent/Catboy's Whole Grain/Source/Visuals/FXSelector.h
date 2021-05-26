#pragma once
#include <JuceHeader.h>
#include "../Visuals/FXListPanel.h"
#include "../PluginProcessor.h"

class CWGFXSelector : public juce::Component {
public:
	CWGFXSelector(CWGGrainController* c, int i) {
		setPaintingIsUnclipped(true);
		controller = c;

		CWGFx* fx = controller->FXs[controller->FXs.size() - 1];
		name = fx->name;
		id = i;

		btnView.setButtonText("View");
		btnView.onClick = [&] { changeView(id); };
		addAndMakeVisible(btnView);

		btnRemove.setButtonText("Remove");
		btnRemove.onClick = [&] { controller->removeFx(id);  id = -1; removed(); };
		addAndMakeVisible(btnRemove);
	};

	void paint(juce::Graphics& g) override {
		g.setColour(juce::Colours::darkgrey);
		g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 10);

		g.setColour(juce::Colours::black);
		g.drawText(name, juce::Rectangle<float>(10, 0, getWidth(), getHeight()), juce::Justification::left, true);
		g.drawText(std::to_string(id), juce::Rectangle<float>(getWidth() / 2, 0, getWidth(), getHeight()), juce::Justification::left, true);
	};

	void resized() override {
		btnView.setBoundsRelative(0.6, 0, 0.2, 1);
		btnRemove.setBoundsRelative(0.8, 0, 0.2, 1);
	};

	std::function<void()> removed;
	std::function<void(int id)> changeView;

	juce::String name;
	int id;
	juce::TextButton btnView;
	juce::TextButton btnRemove;

	CWGGrainController* controller;
};