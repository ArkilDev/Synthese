#include "GrainProcessor.h"

CWGGrainProcessor::CWGGrainProcessor(juce::AudioBuffer<float> file, int note, double pitch, juce::ADSR::Parameters param, double rate, float len) {
	gFileBuffer.makeCopyOf(file, true);
	gNote = note;
	gNotePitch = pitch;
	sampleRate = rate;
	adsr.setSampleRate(rate);
	adsr.setParameters(param);
	adsr.noteOn();
	length = len;
}

void CWGGrainProcessor::process(juce::AudioBuffer<float>& buffer) {
	float* filePointer = 0;
	float currentVal = 0;
	for (int i = 0; i < buffer.getNumSamples(); ++i) {
		if (gBufferPos <= gFileBuffer.getNumSamples() && gBufferPos - start < length * sampleRate /1000) {
			for (auto channel = 0; channel < buffer.getNumChannels(); ++channel) {
				filePointer = gFileBuffer.getWritePointer(channel);

				if (gBufferPos != std::trunc(gBufferPos)) {
					currentVal = (filePointer[(int)std::floor(gBufferPos)] + filePointer[(int)std::ceil(gBufferPos)]) / 2;
				}
				else {
					currentVal = filePointer[(int)gBufferPos];
				}

				buffer.setSample(channel, i, buffer.getSample(channel, i) + (currentVal * adsr.getNextSample()));
			}
			gBufferPos += gNotePitch;

		}
		else {
			gBufferPos = start;
		}
	}
}