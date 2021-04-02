#include "MidiProcessor.h"

CWGMidiProcessor::CWGMidiProcessor() {}

void CWGMidiProcessor::setMidiMessage(juce::MidiBuffer& msg) {
	midiMessages = msg;
	mIt = new juce::MidiBuffer::Iterator(msg);
}

float* CWGMidiProcessor::process() {
	mIt = new juce::MidiBuffer::Iterator(midiMessages);

	while (mIt->getNextEvent(currentMessage, samplePos)) {
		if (currentMessage.isNoteOn()) {

		}
		else if (currentMessage.isNoteOff()) {

		}
	}

	return info[0];
}

bool CWGMidiProcessor::isNotePressed() {
	mIt = new juce::MidiBuffer::Iterator(midiMessages);
	while (mIt->getNextEvent(currentMessage, samplePos)) {
		if (currentMessage.isNoteOn()) {
			return true;
		}
	}
	return false;
}

bool CWGMidiProcessor::isNoteReleased() {
	mIt = new juce::MidiBuffer::Iterator(midiMessages);
	while (mIt->getNextEvent(currentMessage, samplePos)) {
		if (currentMessage.isNoteOff()) {
			return true;
		}
	}
	return false;
}

int CWGMidiProcessor::getNote() {
	juce::MidiBuffer::Iterator tempIt(midiMessages);
	while (mIt->getNextEvent(currentMessage, samplePos)) {
		if (currentMessage.isNoteOn())
			return currentMessage.getNoteNumber();
	}
	return 0; //Always check isNoteOn() first since 0 is a valid MIDI input
}

float CWGMidiProcessor::getPitch(juce::MidiMessage msg) {
	if (isNotePressed()) {
		int octave = std::trunc(msg.getNoteNumber() / 12);
		int semitone = msg.getNoteNumber() % 12;
		float pitch;

		juce::jmap<float>(semitone, 0, 11, 1 * std::pow(2, octave), 1 * std::pow(2, octave + 1));

		return 1 * std::pow(2, octave) + semitone;
	}
}

float CWGMidiProcessor::getVelocity() {
	if (isNotePressed())
		return currentMessage.getFloatVelocity();
	return 0;
}