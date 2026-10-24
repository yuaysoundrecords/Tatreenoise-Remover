#pragma once
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>

// Minimum-tracking estimation informed by SpeexDSP (Xiph/Jean-Marc Valin).
// BSD attribution and algorithm references: THIRD_PARTY.md, docs/DSP-QUALITY.md.
// Causal, 4x-overlapped square-root Hann analysis/synthesis; fixed latency N.
// All work buffers are allocated at construction, never on the audio thread.
class SpectralDenoiser {
public:
    static constexpr int size = 1024, hop = 256, bins = size / 2 + 1;
    SpectralDenoiser() {
        for (int i = 0; i < size; ++i)
            window[i] = std::sqrt(0.5f - 0.5f * std::cos(juce::MathConstants<float>::twoPi * i / size));
        reset();
    }
    void reset(double sampleRate = 48000) {
        input.fill(0); output.fill(0); noise.fill(0); gains.fill(1); power.fill(0);
        smoothPower.fill(0); localMinimum.fill(1e30f); previousMinimum.fill(1e30f);
        const float scale=float(48000/sampleRate);
        powerRate=1-std::pow(.8f,scale);noiseDown=1-std::pow(.92f,scale);
        noiseUp=1-std::pow(.94f,scale);noiseSpeech=1-std::pow(.9998f,scale);
        gainDown=1-std::pow(.45f,scale);gainUp=1-std::pow(.82f,scale);
        minimumPeriod=std::max(1,int(std::round(sampleRate*.25/hop)));
        cursor = count = frames = minimumCounter = 0; reductionDb = 0;
    }
    float process(float x, float amount) {
        auto y = output[cursor]; output[cursor] = 0; input[cursor] = x;
        cursor = (cursor + 1) % size;
        if (++count == hop) { count = 0; frame(amount); }
        return y;
    }
    float reductionDb = 0;
private:
    void frame(float amount) {
        // Apply stronger suppression to stationary background bins while
        // retaining a conservative oversubtraction weight during speech.
        amount=std::clamp(amount,0.0f,1.0f);
        fftData.fill(0);
        for (int i = 0; i < size; ++i) fftData[i] = input[(cursor + i) % size] * window[i];
        fft.performRealOnlyForwardTransform(fftData.data(), true);
        for (int k = 0; k < bins; ++k)
            power[k] = fftData[2*k]*fftData[2*k] + fftData[2*k+1]*fftData[2*k+1];
        float before = 1e-15f, after = 1e-15f;
        const float floor = juce::Decibels::decibelsToGain(-26.0f * amount);
        frames=std::min(frames+1,5);
        const bool rotateMinimum=++minimumCounter>=minimumPeriod;
        if(rotateMinimum)minimumCounter=0;
        for (int k = 0; k < bins; ++k) {
            const float p = power[k];
            const float neighbours=(power[std::max(0,k-1)]+2*p+power[std::min(bins-1,k+1)])*.25f;
            smoothPower[k]+=powerRate*(neighbours-smoothPower[k]);
            if(frames<=4) {
                // Full analysis window has not arrived yet. A conservative
                // startup estimate avoids treating the first word as room tone.
                noise[k]=neighbours*.15f;
                localMinimum[k]=previousMinimum[k]=smoothPower[k];
            }
            if(rotateMinimum) {
                previousMinimum[k]=localMinimum[k];localMinimum[k]=smoothPower[k];
            } else localMinimum[k]=std::min(localMinimum[k],smoothPower[k]);
            const float minimum=std::min(localMinimum[k],previousMinimum[k]);
            const bool speechLikely=smoothPower[k]>2.5f*(minimum+1e-15f);
            const float rate=p<noise[k]?noiseDown:speechLikely?noiseSpeech:noiseUp;
            noise[k]+=rate*(p-noise[k]);
            float target=std::sqrt(std::max(floor*floor,1-amount*2.8f*(speechLikely?.5f:1.0f)*noise[k]/(p+1e-15f)));
            if(amount<1e-6f)target=1;
            gains[k]+=(target<gains[k]?gainDown:gainUp)*(target-gains[k]);
        }
        for (int k = 0; k < bins; ++k) {
            const float g = (gains[std::max(0,k-1)] + 2*gains[k] + gains[std::min(bins-1,k+1)]) * .25f;
            before += power[k]; after += power[k]*g*g;
            fftData[2*k] *= g; fftData[2*k+1] *= g;
        }
        reductionDb = before > 1e-10f ? std::clamp(10.0f * std::log10(before / after), 0.0f, 40.0f) : 0;
        fft.performRealOnlyInverseTransform(fftData.data());
        for (int i = 0; i < size; ++i) output[(cursor + i) % size] += fftData[i] * window[i] * .5f;
    }
    juce::dsp::FFT fft {10};
    std::array<float,size> input{}, output{}, window{};
    std::array<float,size*2> fftData{};
    std::array<float,bins> noise{}, gains{}, power{};
    std::array<float,bins> smoothPower{},localMinimum{},previousMinimum{};
    float powerRate=0,noiseDown=0,noiseUp=0,noiseSpeech=0,gainDown=0,gainUp=0;
    int cursor=0,count=0,frames=0,minimumCounter=0,minimumPeriod=1;
};
