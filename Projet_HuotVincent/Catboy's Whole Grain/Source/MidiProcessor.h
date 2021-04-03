#pragma once
#include <JuceHeader.h>

class CWGMidiProcessor {
public:
	CWGMidiProcessor();

	void setMidiMessage(juce::MidiBuffer& msg);
	float* process();
	int getNote();
	bool isNotePressed();
	bool isNoteReleased();
	float getPitch(juce::MidiMessage msg);
	float getVelocity();

private:
	juce::MidiBuffer midiMessages;
	juce::MidiBuffer::Iterator* mIt;
	juce::MidiMessage currentMessage;
	int currentMidiNote = 0;
	int samplePos = 0;
};