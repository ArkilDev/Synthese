#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "../Visuals/FXViewerPanel.h"
#include "../Visuals/FXSelector.h"

class CWGFXListPanel : public juce::Component {
public:
	CWGFXListPanel() {
		setPaintingIsUnclipped(true);
		selector.reserve(16);

		fxList.addItem("Distortion", 1);
		fxList.addItem("Bitcrush", 2);
		fxList.setSelectedItemIndex(0, false);
		addAndMakeVisible(fxList);

		btnAdd.setButtonText("Add");
		addAndMakeVisible(btnAdd);
	};

	void initiate(CWGAudioProcessor* processor, CWGFXViewerPanel* p) {
		controller = &processor->controller;
		fxPanel = p;


		btnAdd.onClick = [&] {
			controller->addFx(fxList.getSelectedId() - 1);
			createNewSelector(controller->FXs.size() - 1);
		};
	};

	void paint(juce::Graphics& g) override {
		g.setColour(juce::Colours::darkgrey);
		g.drawRect(getLocalBounds());

		if (selector.size() != controller->FXs.size())
			refresh();
	};

	void resized() override {
		int fxCount = selector.size();
		for (int i = 0; i < fxCount; ++i) {
			selector.at(i)->setBoundsRelative(0, (float)i / 10, 1, 0.1);
		}

		btnAdd.setBoundsRelative(0, (float)fxCount / 10, 0.2, 0.1);
		fxList.setBoundsRelative(0.2, (float)fxCount / 10, 0.8, 0.1);
	};

	void createNewSelector(int id) {
		selector.push_back(new CWGFXSelector(controller, id));
		selector.at(selector.size() - 1)->removed = [&] { updateFxList(); };
		selector.at(selector.size() - 1)->changeView = [&](int id) { fxPanel->setViewedFx(id); };
		addAndMakeVisible(selector.at(selector.size() - 1));
		resized();
	}

	void refresh() {
		if (controller->FXs.size() != selector.size()) {
			for (int i = 0; i < controller->FXs.size(); ++i) {
				createNewSelector(i);
			}
		}
	}

	void updateFxList() {
		fxPanel->setViewedFx(-1);
		for (int i = 0; i < selector.size(); ++i) {
			if (selector.at(i)->id == -1) {
				delete selector.at(i);
				selector.erase(selector.begin() + i);
				--i;
			}
			else
				selector.at(i)->id = i;
		}
		resized();
	}

private:
	std::vector<CWGFXSelector*> selector;
	juce::TextButton btnAdd;
	juce::ComboBox fxList;

	CWGGrainController* controller;
	CWGFXViewerPanel* fxPanel;
};