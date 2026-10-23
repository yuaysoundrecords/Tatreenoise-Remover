#pragma once
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>

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
    void reset() {
        input.fill(0); output.fill(0); noise.fill(0); gains.fill(1); power.fill(0);
        cursor = count = 0; ready = false; reductionDb = 0;
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
        // Progressive control taper keeps ordinary voice settings conservative,
        // while retaining the full maximum attenuation for difficult noise.
        amount *= amount;
        fftData.fill(0);
        for (int i = 0; i < size; ++i) fftData[i] = input[(cursor + i) % size] * window[i];
        fft.performRealOnlyForwardTransform(fftData.data(), true);
        for (int k = 0; k < bins; ++k)
            power[k] = fftData[2*k]*fftData[2*k] + fftData[2*k+1]*fftData[2*k+1];
        float before = 1e-15f, after = 1e-15f;
        const float floor = juce::Decibels::decibelsToGain(-26.0f * amount);
        for (int k = 0; k < bins; ++k) {
            const float p = power[k];
            const float neighbours = (power[std::max(0,k-2)] + power[std::max(0,k-1)]
                                     + power[std::min(bins-1,k+1)] + power[std::min(bins-1,k+2)]) * .25f;
            // Protect narrow harmonic peaks from being learned as broadband noise.
            const float candidate = std::min(p, neighbours * 2.0f + 1e-15f);
            if (!ready) noise[k] = candidate * .7f;
            const float rate = candidate < noise[k] ? .075f : .0015f;
            noise[k] += rate * (candidate - noise[k]);
            float target = std::sqrt(std::max(floor * floor, 1.0f - amount * 2.8f * noise[k] / (p + 1e-15f)));
            if (amount < 1e-6f) target = 1;
            const float rateGain = target < gains[k] ? .55f : .18f;
            gains[k] += rateGain * (target - gains[k]);
        }
        for (int k = 0; k < bins; ++k) {
            const float g = (gains[std::max(0,k-1)] + 2*gains[k] + gains[std::min(bins-1,k+1)]) * .25f;
            before += power[k]; after += power[k]*g*g;
            fftData[2*k] *= g; fftData[2*k+1] *= g;
        }
        reductionDb = before > 1e-10f ? std::clamp(10.0f * std::log10(before / after), 0.0f, 40.0f) : 0;
        ready = true;
        fft.performRealOnlyInverseTransform(fftData.data());
        for (int i = 0; i < size; ++i) output[(cursor + i) % size] += fftData[i] * window[i] * .5f;
    }
    juce::dsp::FFT fft {10};
    std::array<float,size> input{}, output{}, window{};
    std::array<float,size*2> fftData{};
    std::array<float,bins> noise{}, gains{}, power{};
    int cursor = 0, count = 0;
    bool ready = false;
};
