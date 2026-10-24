#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout TatreeProcessor::layout() {
    juce::AudioProcessorValueTreeState::ParameterLayout p;
    for (auto item : {std::pair{"noise", "Noise reduction"}, {"breath", "Breath reduction"}, {"ess", "De-esser"}})
        p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{item.first,1}, item.second,
              juce::NormalisableRange<float>{0,100,.1f}, juce::String(item.first) == "noise" ? 45.0f : juce::String(item.first) == "ess" ? 30.0f : 25.0f));
    p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"filter",1},"Low-cut filter",true));
    p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"listen",1},"Listen to removed signal",false));
    p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"bypass",1},"Bypass",false));
    p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"motion",1},"Motion FX",true));
    for (auto item : {std::pair{"inputGain", "Input gain"}, {"outputGain", "Output gain"}})
        p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{item.first,1}, item.second,
              juce::NormalisableRange<float>{-24,24,.1f}, 0.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));
    p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"autoGain",1},"Input auto gain",false));
    return p;
}
TatreeProcessor::TatreeProcessor() : AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true)
    .withOutput("Output",juce::AudioChannelSet::stereo(),true)), params(*this,nullptr,"Tatree2",layout()) { setLatencySamples(SpectralDenoiser::size); }
bool TatreeProcessor::isBusesLayoutSupported(const BusesLayout& l) const {
    return l.getMainInputChannelSet() == l.getMainOutputChannelSet()
        && (l.getMainInputChannelSet() == juce::AudioChannelSet::mono() || l.getMainInputChannelSet() == juce::AudioChannelSet::stereo());
}
void TatreeProcessor::prepareToPlay(double sr, int) {
    rate = sr; setLatencySamples(SpectralDenoiser::size); dryCursor = 0;
    autoGain.reset(sr);
    low.fill(0); envHigh = envFull = 0; essGain = breathGain = 1;
    const float g=std::tan(juce::MathConstants<float>::pi*std::min(4500.0f,float(sr)*.4f)/float(sr));
    crossover=g/(1+g); // Bilinear/TPT complementary one-pole split, stable at every rate.
    breathEvidence=breathPower=0;
    breathPowerFollow=std::exp(-1.0f/float(.005*sr));
    breathVoiceRelease=std::exp(-1.0f/float(.008*sr));
    powerFollow=std::exp(-1.0f/float(.005*sr));
    attack = std::exp(-1.0f / float(.002 * sr)); release = std::exp(-1.0f / float(.09 * sr));
    breathAttack = std::exp(-1.0f / float(.025 * sr));
    breathRelease = std::exp(-1.0f / float(.14 * sr));
    for (int c=0; c<2; ++c) {
        denoisers[c].reset(sr); dryDelay[c].fill(0); bypassDelay[c].fill(0);
        highpass[c].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sr,75);
        highpass[c].reset(); // Allocate filter state here, not on the first audio callback.
        meters.input[c].store(0); meters.output[c].store(0);
    }
    meters.autoGainDb.store(0);meters.autoGainHolding.store(true);
    for (auto entry : {std::pair{&inputGain,"inputGain"}, {&outputGain,"outputGain"}}) {
        entry.first->reset(sr,.025); entry.first->setCurrentAndTargetValue(juce::Decibels::decibelsToGain(params.getRawParameterValue(entry.second)->load()));
    }
    meters.hot.store(false); meters.noiseDb.store(0); meters.deEssDb.store(0); meters.breathDb.store(0); meters.reductionDb.store(0); meters.bypassed.store(false);
    for (auto entry : {std::pair{&noiseAmount,"noise"}, {&breathAmount,"breath"}, {&essAmount,"ess"}}) {
        entry.first->reset(sr,.025); entry.first->setCurrentAndTargetValue(params.getRawParameterValue(entry.second)->load() * .01f);
    }
    for (auto entry : {std::pair{&bypassMix,"bypass"}, {&listenMix,"listen"}, {&filterMix,"filter"}}) {
        entry.first->reset(sr,.015); entry.first->setCurrentAndTargetValue(params.getRawParameterValue(entry.second)->load());
    }
}
void TatreeProcessor::processBlock(juce::AudioBuffer<float>& b, juce::MidiBuffer&) { process(b,false); }
void TatreeProcessor::processBlockBypassed(juce::AudioBuffer<float>& b, juce::MidiBuffer&) { process(b,true); }
void TatreeProcessor::process(juce::AudioBuffer<float>& b, bool hostBypass) {
    juce::ScopedNoDenormals guard;
    noiseAmount.setTargetValue(params.getRawParameterValue("noise")->load() * .01f);
    breathAmount.setTargetValue(params.getRawParameterValue("breath")->load() * .01f);
    essAmount.setTargetValue(params.getRawParameterValue("ess")->load() * .01f);
    bypassMix.setTargetValue(hostBypass ? 1 : params.getRawParameterValue("bypass")->load());
    listenMix.setTargetValue(params.getRawParameterValue("listen")->load());
    filterMix.setTargetValue(params.getRawParameterValue("filter")->load());
    inputGain.setTargetValue(juce::Decibels::decibelsToGain(params.getRawParameterValue("inputGain")->load()));
    outputGain.setTargetValue(juce::Decibels::decibelsToGain(params.getRawParameterValue("outputGain")->load()));
    const bool autoEnabled=params.getRawParameterValue("autoGain")->load()>.5f;
    const int channels = std::min(2, std::min(b.getNumChannels(),getTotalNumInputChannels()));
    for (int c=channels; c<b.getNumChannels(); ++c) b.clear(c,0,b.getNumSamples());
    float inputPeak[2]{}, outputPeak[2]{}, maxEss=0, maxBreath=0;
    bool clipped=false;
    double dryEnergy = 0, wetEnergy = 0;
    for (int s=0; s<b.getNumSamples(); ++s) {
        float dry[2]{}, rawDry[2]{}, wet[2]{}, high[2]{}, input[2]{};
        const float inGain=inputGain.getNextValue(), outGain=outputGain.getNextValue();
        float inputPower=0, inputMax=0;
        for(int c=0;c<channels;++c) {
            float x=b.getSample(c,s); if(!std::isfinite(x))x=0;
            clipped |= std::abs(x)>=.98f;
            rawDry[c]=bypassDelay[c][dryCursor]; bypassDelay[c][dryCursor]=x;
            input[c]=x*inGain; inputPower+=input[c]*input[c]; inputMax=std::max(inputMax,std::abs(input[c]));
        }
        const float ag=autoGain.process(inputPower/float(std::max(1,channels)),inputMax,autoEnabled);
        const float n=noiseAmount.getNextValue(), e=essAmount.getNextValue(), br=breathAmount.getNextValue();
        const float bypass=bypassMix.getNextValue(), listen=listenMix.getNextValue(), filt=filterMix.getNextValue();
        float fullPower=0, highPower=0;
        for (int c=0; c<channels; ++c) {
            const float x=input[c]*ag;
            inputPeak[c]=std::max(inputPeak[c],std::abs(x)); clipped |= std::abs(x)>=.98f;
            dry[c]=dryDelay[c][dryCursor]; dryDelay[c][dryCursor]=x;
            const float hp=highpass[c].processSample(x);
            wet[c]=denoisers[c].process(x + filt*(hp-x),n);
            const float v=(wet[c]-low[c])*crossover;
            const float lowBand=v+low[c];low[c]=lowBand+v;high[c]=wet[c]-lowBand;
            fullPower += wet[c]*wet[c]; highPower += high[c]*high[c];
        }
        dryCursor=(dryCursor+1)%SpectralDenoiser::size;
        fullPower/=float(std::max(1,channels)); highPower/=float(std::max(1,channels));
        auto envelope=[this](float previous,float value) { return powerFollow*previous+(1-powerFollow)*value; };
        envFull=envelope(envFull,fullPower); envHigh=envelope(envHigh,highPower);
        const float ratio=envHigh/(envFull+1e-12f);
        const float activity=juce::jlimit(0.0f,1.0f,(ratio-.22f)/.45f)*juce::jlimit(0.0f,1.0f,envHigh/.00008f);
        const float essDb=e*16*activity;
        const float targetEss = juce::Decibels::decibelsToGain(-essDb);
        essGain = targetEss + (essGain - targetEss) * (targetEss < essGain ? attack : release);
        const float level=juce::Decibels::gainToDecibels(std::sqrt(envFull),-100.0f);
        // Only attenuate low-level, noise-like high-band material. Quiet voiced
        // fundamentals should not be mistaken for breaths.
        const float unvoiced = juce::jlimit(0.0f,1.0f,(ratio-.18f)/.35f);
        breathPower=breathPowerFollow*breathPower+(1-breathPowerFollow)*fullPower;
        const bool breathLike=unvoiced>.25f && level<-34 && breathPower>1e-7f && breathPower>envFull*.05f;
        breathEvidence=std::clamp(breathEvidence+(breathLike?1.0f:-4.0f)/float(rate),0.0f,.14f);
        // Require a sustained low-level unvoiced event. Short consonants must
        // survive; no extra lookahead/latency is added to the audible signal.
        const float durationConfidence=std::clamp((breathEvidence-.065f)/.035f,0.0f,1.0f);
        const float breathDb=br*unvoiced*durationConfidence*std::min(12.0f,std::max(0.0f,(-34-level)*.65f));
        const float targetBreath = juce::Decibels::decibelsToGain(-breathDb);
        breathGain = targetBreath + (breathGain - targetBreath) * (targetBreath < breathGain ? breathAttack : unvoiced<.1f ? breathVoiceRelease : breathRelease);
        if (envFull>1e-10f) { maxEss=std::max(maxEss,essDb); maxBreath=std::max(maxBreath,breathDb); }
        for (int c=0;c<channels;++c) {
            const float processed=(wet[c]-high[c]*(1-essGain))*breathGain;
            dryEnergy += double(dry[c]) * dry[c]; wetEnergy += double(processed) * processed;
            const float audition=processed+listen*((dry[c]-processed)-processed);
            const float adjusted=audition*outGain;
            const float out=bypass>=1.0f?rawDry[c]:bypass<=0.0f?adjusted:adjusted+bypass*(rawDry[c]-adjusted);
            clipped |= std::abs(out)>=.98f;
            b.setSample(c,s,out); outputPeak[c]=std::max(outputPeak[c],std::abs(out));
        }
    }
    const float decay=std::exp(-float(b.getNumSamples())/float(rate*.20));
    float noiseDb=0;
    for (int c=0;c<2;++c) {
        meters.input[c].store(std::max(inputPeak[c],meters.input[c].load()*decay));
        meters.output[c].store(std::max(outputPeak[c],meters.output[c].load()*decay));
        if(c<channels) noiseDb=std::max(noiseDb,denoisers[c].reductionDb);
    }
    meters.autoGainDb.store(autoGain.decibels());meters.autoGainHolding.store(autoGain.isHolding());
    meters.noiseDb.store(noiseDb); meters.deEssDb.store(maxEss); meters.breathDb.store(maxBreath);
    const bool isBypassed = hostBypass || params.getRawParameterValue("bypass")->load() > .5f;
    meters.bypassed.store(isBypassed);
    meters.reductionDb.store(!isBypassed && dryEnergy > 1e-12 ? std::clamp(float(10 * std::log10((dryEnergy+1e-20)/(wetEnergy+1e-20))),0.0f,40.0f) : 0.0f);
    meters.blocks.fetch_add(1, std::memory_order_relaxed);
    if (clipped) meters.hot.store(true); // Editor consumes this latch; short peaks cannot be missed.
}
juce::AudioProcessorEditor* TatreeProcessor::createEditor() { return new TatreeEditor(*this); }
void TatreeProcessor::getStateInformation(juce::MemoryBlock& out) { auto xml=params.copyState().createXml(); copyXmlToBinary(*xml,out); }
void TatreeProcessor::setStateInformation(const void* data,int size) {
    auto xml=getXmlFromBinary(data,size); if(xml && xml->hasTagName(params.state.getType())) params.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new TatreeProcessor(); }
