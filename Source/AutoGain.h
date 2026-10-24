#pragma once
#include <algorithm>
#include <cmath>

// Stereo-linked pre-chain levelling, not a limiter. Gate holds the gain during
// silence; peak headroom caps the requested boost. No audio-thread allocation.
class InputAutoGain {
public:
    void reset(double sampleRate) {
        rmsFollow = std::exp(-1.0f / float(sampleRate * .3));
        peakRelease = std::exp(-1.0f / float(sampleRate * .5));
        down = std::exp(-1.0f / float(sampleRate * .02));
        up = std::exp(-1.0f / float(sampleRate * 2));
        off = std::exp(-1.0f / float(sampleRate * .05));
        power = peak = gainDb = 0;
    }
    float process(float meanPower, float samplePeak, bool enabled) {
        power = rmsFollow * power + (1-rmsFollow) * meanPower;
        peak = std::max(samplePeak, peak * peakRelease);
        float target = gainDb;
        if (!enabled) target = 0;
        else if (power > 1e-5f && samplePeak > .0031623f) { // -50 dBFS RMS gate: never chase silence.
            const float rmsDb = 10 * std::log10(std::max(power, 1e-20f));
            const float peakDb = 20 * std::log10(std::max(peak, 1e-10f));
            target = std::clamp(std::min(-18-rmsDb, -3-peakDb), -24.0f, 12.0f);
        }
        const float a = !enabled ? off : target < gainDb ? down : up;
        gainDb = target + a * (gainDb-target);
        return std::pow(10.0f, gainDb/20);
    }
    float decibels() const { return gainDb; }
private:
    float power=0, peak=0, gainDb=0, rmsFollow=0, peakRelease=0, down=0, up=0, off=0;
};
