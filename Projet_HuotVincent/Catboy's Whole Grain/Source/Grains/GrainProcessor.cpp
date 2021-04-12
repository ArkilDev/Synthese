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

		//If position not at the end of the sample nor the grain length
		if (gBufferPos <= gFileBuffer.getNumSamples() && gBufferPos - start < length * sampleRate / 1000) {

			//Linear interpolation
			for (auto channel = 0; channel < buffer.getNumChannels(); ++channel) {
				filePointer = gFileBuffer.getWritePointer(channel);

				if (gBufferPos != std::trunc(gBufferPos)) {
					currentVal = (filePointer[(int)std::floor(gBufferPos)] + filePointer[(int)std::ceil(gBufferPos)]) / 2;
				}
				else {
					currentVal = filePointer[(int)gBufferPos];
				}

				//Set value in output buffer
				buffer.setSample(channel, i, buffer.getSample(channel, i) + (currentVal * adsr.getNextSample() * grainVol));
			}
			gBufferPos += gNotePitch;

		}
		else {
			gBufferPos = start;
		}
	}

	//Pan handling
	if (grainPan > 0) {
		buffer.applyGain(0, 0, buffer.getNumSamples(), 1 - grainPan);
	}
	else {
		buffer.applyGain(1, 0, buffer.getNumSamples(), 1 - (grainPan * -1));
	}
}