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
    btnLoad.onClick = [&]() {audioProcessor.loadFile(); };
    addAndMakeVisible(btnLoad);
    setSize(1000, 600);
}

CWGAudioProcessorEditor::~CWGAudioProcessorEditor()
{
}

//==============================================================================
void CWGAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);

    if (eUpdateWaveDisplay) {
        juce::Path p;
        eSampleVal.clear();

        //draw waveform
        auto wave = audioProcessor.getWaveform();
        auto ratio = wave.getNumSamples() / getWidth();
        auto buffer = wave.getReadPointer(0);

        //scale x axis
        for (int i = 0; i < wave.getNumSamples(); i += ratio) {
            eSampleVal.push_back(buffer[i]); //get point in sample at ratio interval to make the length fit the window
        }

        //scale y axis and draw path
        p.startNewSubPath(0, getHeight() / 2);
        for (int i = 0; i < eSampleVal.size(); ++i) {
            auto point = juce::jmap<float>(eSampleVal[i], -1.0f, 1.0f, getHeight(), 0); //Map amplitude from (-1, 1) to (maxHeight, minHeight)
            p.lineTo(i, point);
        }

        g.strokePath(p, juce::PathStrokeType(2));

        eUpdateWaveDisplay = false;
    }
}

void CWGAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    btnLoad.setBounds(0, 0, 64, 32);
}

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
            audioProcessor.loadFile(file);
        }
    }

    repaint();
}