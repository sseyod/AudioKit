//
//  AKThreePoleLowpassFilterDSPKernel.hpp
//  AudioKit
//
//  Autogenerated by scripts by Aurelius Prochazka. Do not edit directly.
//  Copyright (c) 2015 Aurelius Prochazka. All rights reserved.
//

#ifndef AKThreePoleLowpassFilterDSPKernel_hpp
#define AKThreePoleLowpassFilterDSPKernel_hpp

#import "AKDSPKernel.hpp"
#import "AKParameterRamper.hpp"

extern "C" {
#include "base.h"
#include "lpf18.h"
}

enum {
    distortionAddress = 0,
    cutoffFrequencyAddress = 1,
    resonanceAddress = 2
};

class AKThreePoleLowpassFilterDSPKernel : public AKDSPKernel {
public:
    // MARK: Member Functions

    AKThreePoleLowpassFilterDSPKernel() {}

    void init(int channelCount, double inSampleRate) {
        channels = channelCount;

        sampleRate = float(inSampleRate);

        sp_create(&sp);
        sp_lpf18_create(&lpf18);
        sp_lpf18_init(sp, lpf18);
        lpf18->dist = 0.5;
        lpf18->cutoff = 1500;
        lpf18->res = 0.5;
    }

    void reset() {
    }

    void setParameter(AUParameterAddress address, AUValue value) {
        switch (address) {
            case distortionAddress:
                distortionRamper.set(clamp(value, 0.0f, 2.0f));
                break;

            case cutoffFrequencyAddress:
                cutoffFrequencyRamper.set(clamp(value, 12.0f, 20000.0f));
                break;

            case resonanceAddress:
                resonanceRamper.set(clamp(value, 0.0f, 2.0f));
                break;

        }
    }

    AUValue getParameter(AUParameterAddress address) {
        switch (address) {
            case distortionAddress:
                return distortionRamper.goal();

            case cutoffFrequencyAddress:
                return cutoffFrequencyRamper.goal();

            case resonanceAddress:
                return resonanceRamper.goal();

            default: return 0.0f;
        }
    }

    void startRamp(AUParameterAddress address, AUValue value, AUAudioFrameCount duration) override {
        switch (address) {
            case distortionAddress:
                distortionRamper.startRamp(clamp(value, 0.0f, 2.0f), duration);
                break;

            case cutoffFrequencyAddress:
                cutoffFrequencyRamper.startRamp(clamp(value, 12.0f, 20000.0f), duration);
                break;

            case resonanceAddress:
                resonanceRamper.startRamp(clamp(value, 0.0f, 2.0f), duration);
                break;

        }
    }

    void setBuffers(AudioBufferList* inBufferList, AudioBufferList* outBufferList) {
        inBufferListPtr = inBufferList;
        outBufferListPtr = outBufferList;
    }

    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) override {
        // For each sample.
        for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
            double distortion = double(distortionRamper.getStep());
            double cutoffFrequency = double(cutoffFrequencyRamper.getStep());
            double resonance = double(resonanceRamper.getStep());

            int frameOffset = int(frameIndex + bufferOffset);

            lpf18->dist = (float)distortion;
            lpf18->cutoff = (float)cutoffFrequency;
            lpf18->res = (float)resonance;

            for (int channel = 0; channel < channels; ++channel) {
                float* in  = (float*)inBufferListPtr->mBuffers[channel].mData  + frameOffset;
                float* out = (float*)outBufferListPtr->mBuffers[channel].mData + frameOffset;

                sp_lpf18_compute(sp, lpf18, in, out);
            }
        }
    }

    // MARK: Member Variables

private:

    int channels = 2;
    float sampleRate = 44100.0;

    AudioBufferList* inBufferListPtr = nullptr;
    AudioBufferList* outBufferListPtr = nullptr;

    sp_data *sp;
    sp_lpf18 *lpf18;

public:
    AKParameterRamper distortionRamper = 0.5;
    AKParameterRamper cutoffFrequencyRamper = 1500;
    AKParameterRamper resonanceRamper = 0.5;
};

#endif /* AKThreePoleLowpassFilterDSPKernel_hpp */
