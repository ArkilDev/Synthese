#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"

class CWGAbstractFxPanel : public juce::Component,
	public juce::ValueTree::Listener,
	public juce::Slider::Listener {
public:
	CWGAbstractFxPanel() {
		currentFx = nullptr;
		backgroundColour = juce::Colours::grey;
	}

	CWGAbstractFxPanel(CWGFx* fx, juce::ValueTree vt) {
		currentFx = fx;
		this->vt = vt;
		backgroundColour = juce::Colours::grey;
	}

	void paint(juce::Graphics& g) override {
		g.setColour(backgroundColour);
		g.fillRect(getLocalBounds());
	}

	void sliderValueChanged(juce::Slider* slider) override {

	}

	void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override {
		DBG(treeWhosePropertyHasChanged.toXmlString());
	}

	juce::Colour backgroundColour;
	CWGFx* currentFx;

protected:
	juce::ValueTree vt;
};

class CWGBitcrushViewer : public CWGAbstractFxPanel {
public:
	CWGBitcrushViewer(CWGFx* fx, juce::ValueTree vt) : CWGAbstractFxPanel(fx, vt) {
		backgroundColour = juce::Colours::darkblue;

		crushSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
		crushSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
		crushSlider.setRange(1, 50, 0.1f);
		crushSlider.setValue(100);
		crushSlider.setValue(vt.getPropertyAsValue("Crush", nullptr).getValue());
		crushSlider.addListener(this);
		addAndMakeVisible(crushSlider);

		wetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		wetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
		wetSlider.setRange(0, 1, 0.01f);
		wetSlider.setValue(1);
		wetSlider.setValue(vt.getPropertyAsValue("Wet", nullptr).getValue());
		wetSlider.addListener(this);
		addAndMakeVisible(wetSlider);
	}

	void resized() override {
		crushSlider.setBoundsRelative(0, 0, 0.25, 1);
		wetSlider.setBoundsRelative(0.85, 0.01, 0.15, 0.98);
	}

	void sliderValueChanged(juce::Slider* slider) override {
		if (slider == &crushSlider) {
			vt.setProperty("Crush", slider->getValue(), nullptr);
			dynamic_cast<CWGBitcrush*>(currentFx)->crush = slider->getValue();
		}
		if (slider == &wetSlider) {
			vt.setProperty("Wet", slider->getValue(), nullptr);
			currentFx->wet = slider->getValue();
		}
	}

private:
	juce::Slider crushSlider;
	juce::Slider wetSlider;
};

class CWGDistortionViewer : public CWGAbstractFxPanel {
public:
	CWGDistortionViewer(CWGFx* fx, juce::ValueTree vt) : CWGAbstractFxPanel(fx, vt) {
		backgroundColour = juce::Colours::darkred;

		driveSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
		driveSlider.setRange(0.01, 1, 0.01f);
		driveSlider.setValue(vt.getPropertyAsValue("Drive", nullptr).getValue());
		driveSlider.addListener(this);
		addAndMakeVisible(driveSlider);

		rangeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		rangeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
		rangeSlider.setRange(1, 250, 1);
		rangeSlider.setValue(10);
		rangeSlider.setValue(vt.getPropertyAsValue("Range", nullptr).getValue());
		rangeSlider.addListener(this);
		addAndMakeVisible(rangeSlider);

		wetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		wetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
		wetSlider.setRange(0, 1, 0.01f);
		wetSlider.setValue(100);
		wetSlider.setValue(vt.getPropertyAsValue("Wet", nullptr).getValue());
		wetSlider.addListener(this);
		addAndMakeVisible(wetSlider);
	}

	void resized() override {
		driveSlider.setBoundsRelative(0.01, 0.01, 0.15, 0.98);
		rangeSlider.setBoundsRelative(0.26, 0.01, 0.15, 0.98);
		wetSlider.setBoundsRelative(0.85, 0.01, 0.15, 0.98);
	}

	void sliderValueChanged(juce::Slider* slider) override {
		if (slider == &driveSlider) {
			vt.setProperty("Drive", slider->getValue(), nullptr);
			dynamic_cast<CWGDistortion*>(currentFx)->drive = slider->getValue();
		}
		if (slider == &rangeSlider) {
			vt.setProperty("Range", slider->getValue(), nullptr);
			dynamic_cast<CWGDistortion*>(currentFx)->range = slider->getValue();
		}
		if (slider == &wetSlider) {
			vt.setProperty("Wet", slider->getValue(), nullptr);
			currentFx->wet = slider->getValue();
		}
	}

private:
	juce::Slider driveSlider;
	juce::Slider rangeSlider;
	juce::Slider wetSlider;
};

class CWGFXViewerPanel : public juce::Component {
public:
	CWGFXViewerPanel() {
		setPaintingIsUnclipped(true);
		currentPanel = new CWGAbstractFxPanel();
		addAndMakeVisible(currentPanel);
	}

	void instantiate(CWGAudioProcessor* p) {
		processor = p;
	}

	void paint(juce::Graphics& g) override {
		g.setColour(juce::Colours::grey);
		g.drawRect(getLocalBounds());
	}

	void resized() override {
		currentPanel->setBoundsRelative(0, 0, 1, 1);
	}

	void setViewedFx(int id) {
		if (id != -1) {
			auto fx = processor->controller.FXs[id];
			auto vt = processor->VTS.state.getChildWithProperty("Uid", fx->uid.toString());
			switch (fx->type) {
			default: currentPanel = new CWGAbstractFxPanel(); break;
			case CWGFx::Type::Bitcrush: currentPanel = new CWGBitcrushViewer(fx, vt); break;
			case CWGFx::Type::Distortion: currentPanel = new CWGDistortionViewer(fx, vt); break;
			}
		}
		else {
			currentPanel = new CWGAbstractFxPanel();
		}
		addAndMakeVisible(currentPanel);
		resized();
	}

	CWGAbstractFxPanel* currentPanel;
	CWGAudioProcessor* processor;
};