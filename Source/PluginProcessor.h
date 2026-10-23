#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Denoiser.h"

class TatreeProcessor final : public juce::AudioProcessor {
public:
    TatreeProcessor();
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlockBypassed(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "Tatree Noise Remover 2"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return double(SpectralDenoiser::size) / rate; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorParameter* getBypassParameter() const override { return params.getParameter("bypass"); }
    juce::AudioProcessorValueTreeState params;
    struct Metering {
        std::array<std::atomic<float>,2> input {{0,0}}, output {{0,0}};
        std::atomic<float> noiseDb {0}, deEssDb {0}, breathDb {0}, reductionDb {0};
        std::atomic<bool> bypassed {false};
        std::atomic<unsigned long long> blocks {0};
        std::atomic<bool> hot {false};
    } meters;
private:
    static juce::AudioProcessorValueTreeState::ParameterLayout layout();
    void process(juce::AudioBuffer<float>&, bool);
    std::array<SpectralDenoiser,2> denoisers;
    std::array<std::array<float,SpectralDenoiser::size>,2> dryDelay {};
    std::array<juce::dsp::IIR::Filter<float>,2> highpass;
    std::array<float,2> low {};
    int dryCursor = 0;
    double rate = 48000;
    float crossover = 0, envHigh = 0, envFull = 0, essGain = 1, breathGain = 1;
    float attack = 0, release = 0, breathAttack = 0, breathRelease = 0;
    juce::SmoothedValue<float> noiseAmount, breathAmount, essAmount, bypassMix, listenMix, filterMix;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TatreeProcessor)
};
