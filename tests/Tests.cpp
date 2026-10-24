#include "PluginProcessor.h"
#include "MotionState.h"
#include <iostream>
#include <juce_audio_formats/juce_audio_formats.h>
#include <random>
#include <stdexcept>
#include <vector>
#include <memory>

static void check(bool ok,const char* message){if(!ok)throw std::runtime_error(message);std::cout<<"PASS "<<message<<'\n';}
static void set(TatreeProcessor& p,const char* id,float v){auto* param=p.params.getParameter(id);param->setValueNotifyingHost(param->convertTo0to1(v));}
static std::vector<float> run(TatreeProcessor& p,const std::vector<float>& in,int block=127,bool hostBypass=false){
    std::vector<float> out(in.size());juce::MidiBuffer midi;
    for(size_t i=0;i<in.size();i+=size_t(block)){
        const int n=int(std::min(size_t(block),in.size()-i));juce::AudioBuffer<float>b(2,n);
        for(int c=0;c<2;++c)for(int j=0;j<n;++j)b.setSample(c,j,in[i+size_t(j)]);
        if(hostBypass)p.processBlockBypassed(b,midi);else p.processBlock(b,midi);
        for(int j=0;j<n;++j){out[i+size_t(j)]=b.getSample(0,j);if(!std::isfinite(out[i+size_t(j)]))throw std::runtime_error("nonfinite output");}
    }return out;
}
static double rms(const std::vector<float>& v,int begin){double sum=0;for(size_t i=size_t(begin);i<v.size();++i)sum+=v[i]*v[i];return std::sqrt(sum/double(v.size()-size_t(begin)));}
int main(int argc, char** argv){
 // Keep processor fixtures off main's stack. MSVC can reserve distinct slots
 // for all scoped fixtures, exceeding the Windows default 1 MiB stack before
 // the first assertion. Plugin instances themselves are heap-created by JUCE.
 juce::ScopedJuceInitialiser_GUI gui;
 try {
    std::mt19937 rng(42);std::normal_distribution<float> normal(0,.03f);
    std::vector<float> noise(48000*3);for(auto& x:noise)x=normal(rng);
    {
        SpectralDenoiser d;float error=0;
        for(size_t i=0;i<noise.size();++i){float y=d.process(noise[i],0);if(i>=1024)error=std::max(error,std::abs(y-noise[i-1024]));}
        check(error<1e-5f,"STFT unity reconstruction and exact 1024-sample latency");
    }
    {
        auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;check(p.getLatencySamples()==1024,"latency is available before host preparation");set(p,"noise",0);set(p,"breath",0);set(p,"ess",0);set(p,"filter",0);p.prepareToPlay(48000,512);
        auto out=run(p,noise);float error=0;for(size_t i=1024;i<out.size();++i)error=std::max(error,std::abs(out[i]-noise[i-1024]));
        check(error<1e-5f,"zero processing preserves signal through variable-size blocks");
        check(p.getLatencySamples()==1024,"host latency is declared");
        set(p,"noise",100);set(p,"breath",75);set(p,"ess",100);set(p,"bypass",1);p.prepareToPlay(48000,512);
        out=run(p,noise);error=0;for(size_t i=1024;i<out.size();++i)error=std::max(error,std::abs(out[i]-noise[i-1024]));
        check(error<1e-7f,"bypass is latency-matched dry with processing active");
        set(p,"bypass",0);p.prepareToPlay(48000,512);out=run(p,noise,31,true);error=0;
        for(size_t i=4096;i<out.size();++i)error=std::max(error,std::abs(out[i]-noise[i-1024]));
        check(error<1e-7f,"host bypass also preserves declared latency");
    }
    {
        auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;set(p,"noise",100);set(p,"breath",0);set(p,"ess",0);set(p,"filter",0);p.prepareToPlay(48000,512);
        auto out=run(p,noise);const double db=20*std::log10(rms(out,48000)/rms(noise,48000));std::cout<<"Noise change: "<<db<<" dB\n";
        check(db<-3,"stationary broadband noise is reduced by more than 3 dB");
        check(p.meters.noiseDb.load()>.5f,"noise meter reports actual spectral attenuation");
        p.prepareToPlay(48000,512);std::vector<float> zero(10000);out=run(p,zero);
        check(rms(out,2048)==0,"silence stays silent without NaN or denormal artifacts");
    }
    {
        auto tone=[](double hz){std::vector<float>x(96000);for(size_t i=0;i<x.size();++i)x[i]=.15f*float(std::sin(juce::MathConstants<double>::twoPi*hz*double(i)/48000));return x;};
        double lowChange=0,highChange=0;
        for(double hz:{400.0,8500.0}){
            auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;set(p,"noise",0);set(p,"breath",0);set(p,"ess",100);set(p,"filter",0);p.prepareToPlay(48000,512);
            auto x=tone(hz);auto out=run(p,x);double db=20*std::log10(rms(out,48000)/rms(x,48000));
            if(hz<1000)lowChange=db;else highChange=db;
        }
        std::cout<<"De-ess low/high: "<<lowChange<<" / "<<highChange<<" dB\n";
        check(std::abs(lowChange)<.4 && highChange<-3,"de-esser attenuates sibilant band while preserving low-frequency voice");
    }
    {
        std::vector<float> quietVoice(96000), quietNoise(96000);
        for(size_t i=0;i<quietVoice.size();++i){
            quietVoice[i]=.004f*float(std::sin(juce::MathConstants<double>::twoPi*400*double(i)/48000));
            quietNoise[i]=normal(rng)*.1f;
        }
        auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;set(p,"noise",0);set(p,"ess",0);set(p,"breath",100);set(p,"filter",0);p.prepareToPlay(48000,128);
        auto voiceOut=run(p,quietVoice);double voiceDb=20*std::log10(rms(voiceOut,48000)/rms(quietVoice,48000));
        check(std::abs(voiceDb)<.2,"breath control preserves quiet voiced fundamentals at maximum amount");
        p.prepareToPlay(48000,128);auto breathOut=run(p,quietNoise);
        double breathDb=20*std::log10(rms(breathOut,48000)/rms(quietNoise,48000));
        check(breathDb<-3,"breath control still reduces quiet unvoiced broadband material");
    }
    {
        auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;set(p,"noise",0);set(p,"ess",0);set(p,"breath",100);set(p,"filter",0);p.prepareToPlay(48000,127);
        std::vector<float> consonant(48000,0);
        for(int i=12000;i<13920;++i)consonant[size_t(i)]=normal(rng)*.15f; // 40 ms unvoiced consonant-like burst.
        auto out=run(p,consonant);double inEnergy=0,outEnergy=0;
        for(int i=12000;i<13920;++i){inEnergy+=consonant[size_t(i)]*consonant[size_t(i)];outEnergy+=out[size_t(i+1024)]*out[size_t(i+1024)];}
        const double change=10*std::log10(outEnergy/inEnergy);std::cout<<"Short unvoiced burst: "<<change<<" dB\n";
        check(std::abs(change)<.75,"maximum de-breath preserves short unvoiced consonant-like bursts");
        std::vector<float> phrase(96000,0);
        for(int i=0;i<19200;++i)phrase[size_t(i)]=normal(rng)*.15f;
        for(int i=19200;i<96000;++i)phrase[size_t(i)]=.04f*float(std::sin(juce::MathConstants<double>::twoPi*400*i/48000));
        p.prepareToPlay(48000,127);out=run(p,phrase);inEnergy=outEnergy=0;
        for(int i=19680;i<21600;++i){inEnergy+=phrase[size_t(i)]*phrase[size_t(i)];outEnergy+=out[size_t(i+1024)]*out[size_t(i+1024)];}
        check(10*std::log10(outEnergy/inEnergy)>-.75,"de-breath recovers for the voiced word following a breath");
    }
    for(double sr:{16000.0,44100.0,48000.0,96000.0}) {
        auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;set(p,"noise",0);set(p,"ess",100);set(p,"breath",0);set(p,"filter",0);p.prepareToPlay(sr,127);
        std::vector<float> sibilant(size_t(sr*2),0);
        for(size_t i=0;i<sibilant.size();++i)sibilant[i]=.01f*float(std::sin(juce::MathConstants<double>::twoPi*6000*double(i)/sr));
        auto out=run(p,sibilant);const double change=20*std::log10(rms(out,int(sr))/rms(sibilant,int(sr)));
        std::cout<<"Quiet 6 kHz de-ess at "<<sr<<": "<<change<<" dB\n";
        check(change<-2,"de-esser detects quiet sibilance at 16/44.1/48/96 kHz");
    }
    {
        auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;set(p,"noise",0);set(p,"ess",100);set(p,"breath",0);set(p,"filter",0);p.prepareToPlay(48000,127);
        std::vector<float> mixed(96000,0);
        for(size_t i=0;i<mixed.size();++i)mixed[i]=.1f*float(std::sin(juce::MathConstants<double>::twoPi*400*double(i)/48000))+.08f*float(std::sin(juce::MathConstants<double>::twoPi*8500*double(i)/48000));
        auto out=run(p,mixed);const auto bandAmplitude=[&](double hz){double re=0,im=0;for(int i=48000;i<96000;++i){double phase=juce::MathConstants<double>::twoPi*hz*(i-1024)/48000;re+=out[size_t(i)]*std::cos(phase);im+=out[size_t(i)]*std::sin(phase);}return 2*std::sqrt(re*re+im*im)/48000;};
        const double voiceDb=20*std::log10(bandAmplitude(400)/.1),essDb=20*std::log10(bandAmplitude(8500)/.08);
        std::cout<<"Simultaneous voice/sibilance: "<<voiceDb<<" / "<<essDb<<" dB\n";
        check(std::abs(voiceDb)<.2 && essDb<-2,"split-band de-esser reduces sibilance without ducking simultaneous vowel body");
    }
    {
        auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;set(p,"noise",72);set(p,"motion",0);set(p,"filter",0);juce::MemoryBlock state;p.getStateInformation(state);
        auto restoredStorage=std::make_unique<TatreeProcessor>();auto& restored=*restoredStorage;restored.setStateInformation(state.getData(),int(state.getSize()));
        check(restored.params.getRawParameterValue("noise")->load()==72 && restored.params.getRawParameterValue("motion")->load()==0,"host session restores controls and motion preference");
        p.prepareToPlay(44100,128);std::vector<float> hot(2500,1.01f);run(p,hot);
        check(p.meters.hot.exchange(false),"input-hot latch catches peaks independently of output processing");
        check(!p.meters.hot.exchange(false),"UI can consume peak latch without losing persistent signal detection");
    }
    {
        MotionState m;for(int i=0;i<30;++i)m.update(8,0,false,false,1.f/30);
        check(m.state==LawyerState::noise,"noise attenuation triggers lawyer motion");
        for(int i=0;i<10;++i)m.update(8,6,false,false,1.f/30);
        check(m.state==LawyerState::deEss,"de-esser takes priority over noise motion");
        m.update(8,6,true,false,1.f/30);check(m.state==LawyerState::hot,"input-hot has highest active priority");
        for(int i=0;i<15;++i)m.update(8,6,false,false,1.f/30);
        check(m.state==LawyerState::hot,"short input peaks stay visible for 800 ms");
        for(int i=0;i<60;++i)m.update(0,0,false,false,1.f/30);
        check(m.state==LawyerState::idle,"motion settles back to idle");
        m.reduced=true;m.update(12,8,true,false,.03f);check(m.rotation()==0 && m.lift()==0,"reduced motion disables transforms while retaining status");
        m.update(12,8,true,true,.03f);check(m.state==LawyerState::hot,"input-hot warning is retained even in bypass");
        MotionState bypassed;bypassed.update(12,8,false,true,.03f);check(bypassed.state==LawyerState::bypass,"bypass suppresses processing animation when input is safe");
    }
    {
        MotionState m;for(int i=0;i<30;++i)m.update(0,0,false,false,1.f/30,8);
        check(m.state==LawyerState::breath,"actual breath attenuation triggers its own motion state");
        m.update(8,8,false,false,1.f/30,8,true);check(m.state==LawyerState::listen,"delta listen has a distinct listening state");
        m.update(8,8,true,false,1.f/30,8,true);check(m.state==LawyerState::hot,"input-hot warning overrides delta listen");
        MotionState filter;filter.touch(LawyerControl::filter);filter.update(0,0,false,false,.03f);
        check(filter.state==LawyerState::idle && filter.touchHold>0,"filter feedback does not fabricate noise-detector activity");
        filter.reduced=true;check(filter.rotation()==0 && filter.lift()==0,"control-touch motion respects reduced-motion preference");
    }
    {
        auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;set(p,"noise",0);set(p,"breath",0);set(p,"ess",0);set(p,"filter",0);
        set(p,"inputGain",6);set(p,"outputGain",-3);p.prepareToPlay(48000,512);
        auto out=run(p,noise);const double change=20*std::log10(rms(out,48000)/rms(noise,48000));
        check(std::abs(change-3)<.02,"input and output trim apply in dB independently of denoising");
        set(p,"autoGain",1);set(p,"bypass",1);set(p,"outputGain",24);p.prepareToPlay(48000,512);
        out=run(p,noise);float error=0;for(size_t i=1024;i<out.size();++i)error=std::max(error,std::abs(out[i]-noise[i-1024]));
        check(error<1e-7f,"bypass skips input trim, auto gain and output trim with exact latency");
        set(p,"bypass",0);set(p,"listen",1);set(p,"autoGain",0);p.prepareToPlay(48000,512);
        auto removed=run(p,noise);check(rms(removed,48000)<1e-5,"delta listen excludes input and output gain from removed-signal reference");
        juce::MemoryBlock state;p.getStateInformation(state);auto restoredStorage=std::make_unique<TatreeProcessor>();auto& restored=*restoredStorage;restored.setStateInformation(state.getData(),int(state.getSize()));
        const float savedInput=p.params.getRawParameterValue("inputGain")->load();
        const float recalledInput=restored.params.getRawParameterValue("inputGain")->load();
        const float recalledOutput=restored.params.getRawParameterValue("outputGain")->load();
        std::cout<<"Gain recall: input "<<savedInput<<" -> "<<recalledInput<<", output "<<recalledOutput<<"\n";
        // Normalising a bipolar 0.1 dB range may round differently on ARM.
        // Check recall against the saved value and intended setting, far below
        // one control step, instead of demanding bit equality with literal 6.
        check(std::abs(recalledInput-savedInput)<1e-4f && std::abs(recalledInput-6)<1e-4f && std::abs(recalledOutput-24)<1e-4f && restored.params.getRawParameterValue("autoGain")->load()==0,"gain controls and auto preference survive session recall");
        set(p,"listen",0);set(p,"bypass",0);set(p,"autoGain",0);set(p,"inputGain",0);set(p,"outputGain",24);p.prepareToPlay(48000,512);
        run(p,std::vector<float>(4096,.2f));check(p.meters.hot.exchange(false),"output overload warns even when raw input is safe");
    }
    {
        auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;set(p,"noise",0);set(p,"breath",0);set(p,"ess",0);set(p,"filter",0);set(p,"autoGain",1);set(p,"inputGain",6);p.prepareToPlay(48000,512);
        juce::MidiBuffer midi;double energy=0;float stereoError=0;int count=0;
        for(int block=0;block<1200;++block) {
            juce::AudioBuffer<float> b(2,512);
            for(int i=0;i<512;++i){float x=.07f*std::sin(float((block*512+i)*juce::MathConstants<double>::twoPi*400/48000));b.setSample(0,i,x);b.setSample(1,i,x*.5f);}
            p.processBlock(b,midi);
            if(block>1000)for(int i=0;i<512;++i){float l=b.getSample(0,i),r=b.getSample(1,i);energy+=(l*l+r*r)*.5;stereoError=std::max(stereoError,std::abs(r-l*.5f));++count;}
        }
        check(std::abs(10*std::log10(energy/count)+18)<.4,"pre-chain auto gain compensates input trim and reaches stereo RMS target");
        check(stereoError<1e-6f,"stereo-linked auto gain preserves left-right balance");
    }
    for(double sr:{44100.0,48000.0,96000.0}) {
        InputAutoGain ag;ag.reset(sr);float correction=1;
        for(int i=0;i<int(sr*12);++i)correction=ag.process(.05f*.05f,.0707107f,true);
        check(std::abs(20*std::log10(.05f*correction)+18)<.25,"auto gain converges to target for steady quiet voice within boost bound");
        const float held=ag.decibels();for(int i=0;i<int(sr*4);++i)ag.process(0,0,true);
        check(ag.decibels()<=12 && std::isfinite(ag.decibels()) && ag.decibels()<held+1,"auto gain silence gate avoids noise runaway");
        ag.reset(sr);for(int i=0;i<int(sr*3);++i)correction=ag.process(.64f,1.13f,true);
        check(.8f*correction<.14f && ag.decibels()>=-24,"auto gain attenuates loud input with bounded correction");
        for(int i=0;i<int(sr);++i)correction=ag.process(0,0,false);
        check(std::abs(correction-1)<1e-5f,"disabling auto gain returns smoothly to unity");
    }
    for(double sr:{44100.0,48000.0,96000.0}) {
        InputAutoGain ag;ag.reset(sr);float previous=0,maxRise=0,maxFall=0;
        for(int i=0;i<int(sr*8);++i) {
            const float level=i<int(sr*3)?.05f:i<int(sr*5)?.3f:.05f;
            ag.process(level*level,level*1.414214f,true);
            const float current=ag.decibels(),step=(current-previous)*float(sr);
            maxRise=std::max(maxRise,step);maxFall=std::max(maxFall,-step);previous=current;
        }
        check(maxRise<3.2f && maxFall<12.3f,"vocal auto gain obeys smooth gain-rise/fall bounds through phrase level changes");
        const float held=ag.decibels();
        for(int i=0;i<int(sr);++i)ag.process(.000001f,.0014f,true);
        check(ag.isHolding() && std::abs(ag.decibels()-held)<.2f,"vocal auto gain holds low breaths instead of learning them as a quiet phrase");
    }
    {
        InputAutoGain ag;ag.reset(48000);double energy[3]{};int counts[3]{};
        const float levels[]{.04f,.15f,.05f};
        for(int i=0;i<48000*24;++i) {
            const int section=i/(48000*8);const float level=levels[section];
            const float gain=ag.process(level*level,level*1.414214f,true);
            if(i%(48000*8)>=48000*6){energy[section]+=double(level*gain)*(level*gain);++counts[section];}
        }
        double low=100,high=-100;
        for(int i=0;i<3;++i){const double db=10*std::log10(energy[i]/counts[i]);std::cout<<"Vocal staging phrase "<<i<<": "<<db<<" dBFS RMS\n";low=std::min(low,db);high=std::max(high,db);}
        check(high-low<.8 && low>-19 && high<-17,"vocal auto gain stages settled phrase levels around -18 dBFS without fast compression");
    }
    for(double sr:{44100.0,48000.0,96000.0}){
        auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;p.prepareToPlay(sr,64);std::vector<float> input(8192);input[12]=1;input[23]=std::numeric_limits<float>::quiet_NaN();
        auto out=run(p,input,1);check(std::isfinite(rms(out,0)),"44.1/48/96 kHz and one-sample blocks stay finite");
    }
    if(argc==3 && juce::String(argv[1])=="--speech-check"){
        juce::AudioFormatManager formats;formats.registerBasicFormats();
        auto file=juce::File::getCurrentWorkingDirectory().getChildFile(argv[2]);
        std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
        check(bool(reader),"real-speech fixture opens");
        const int n=int(reader->lengthInSamples);juce::AudioBuffer<float> speech(1,n);
        check(reader->read(&speech,0,n,0,true,false),"real-speech fixture decodes");
        std::vector<float> clean(size_t(n),0), noisy(size_t(n+1024),0);
        double energy=0;for(int i=0;i<n;++i){clean[size_t(i)]=speech.getSample(0,i);energy+=clean[size_t(i)]*clean[size_t(i)];}
        const float sigma=float(std::sqrt(energy/n/10));std::normal_distribution<float> noiseDist(0,sigma);std::mt19937 speechRng(76);
        for(int i=0;i<n;++i)noisy[size_t(i)]=clean[size_t(i)]+noiseDist(speechRng);
        for(float amount : {25.0f,45.0f,65.0f}){
            auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;set(p,"noise",amount);set(p,"ess",0);set(p,"breath",0);set(p,"filter",0);p.prepareToPlay(reader->sampleRate,512);
            auto processed=run(p,noisy,512);double before=0,after=0,dot=0;
            for(int i=0;i<n;++i){before+=std::pow(noisy[size_t(i)]-clean[size_t(i)],2);after+=std::pow(processed[size_t(i+1024)]-clean[size_t(i)],2);dot+=processed[size_t(i+1024)]*clean[size_t(i)];}
            std::cout<<"Real speech amount "<<amount<<": input SNR "<<10*std::log10(energy/before)<<" dB, output SNR "<<10*std::log10(energy/after)<<" dB, voice correlation gain "<<dot/energy<<"\n";
            if(amount==45)check(after<before*std::pow(10.0,-.3/10) && dot/energy>.85,"default noise amount improves noisy real-speech SNR and retains voice level");
        }
        {
            auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;set(p,"noise",45);set(p,"ess",0);set(p,"breath",0);set(p,"filter",0);p.prepareToPlay(reader->sampleRate,512);
            auto input=clean;input.resize(size_t(n+1024),0);auto y=run(p,input,127);double dot=0,error=0;
            for(int i=0;i<n;++i){dot+=y[size_t(i+1024)]*clean[size_t(i)];error+=std::pow(y[size_t(i+1024)]-clean[size_t(i)],2);}
            const double gain=dot/energy, errorSnr=10*std::log10(energy/error);
            std::cout<<"Clean speech gain "<<gain<<", error SNR "<<errorSnr<<" dB\n";
            check(gain>.94 && errorSnr>21,"default denoiser preserves clean recorded speech instead of learning it as noise");
        }
        for(int kind=0;kind<4;++kind) {
            const int pre=int(reader->sampleRate*.5),total=pre+n;
            std::vector<float> fixture(size_t(total+1024),0), disturbance(size_t(total),0);
            std::mt19937 gen(640+kind);std::normal_distribution<float> dist(0,1);float pink=0;double noiseEnergy=0;
            for(int i=0;i<total;++i){
                const double t=i/reader->sampleRate;float x=dist(gen);
                if(kind==1){pink=.97f*pink+.03f*x;x=pink;}
                if(kind==2)x=float(std::sin(juce::MathConstants<double>::twoPi*60*t)+.4*std::sin(juce::MathConstants<double>::twoPi*120*t))+.1f*x;
                if(kind==3)x*=std::sin(juce::MathConstants<double>::twoPi*3.1*t)>0?1.0f:.15f;
                disturbance[size_t(i)]=x;if(i>=pre)noiseEnergy+=x*x;
            }
            const float scale=float(std::sqrt(energy/(10*noiseEnergy)));
            for(int i=0;i<total;++i)fixture[size_t(i)]=disturbance[size_t(i)]*scale+(i>=pre?clean[size_t(i-pre)]:0);
            auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;set(p,"noise",45);set(p,"ess",0);set(p,"breath",0);set(p,"filter",0);p.prepareToPlay(reader->sampleRate,512);
            auto y=run(p,fixture,127);double error=0,dot=0;
            for(int i=0;i<n;++i){float out=y[size_t(pre+i+1024)];error+=std::pow(out-clean[size_t(i)],2);dot+=out*clean[size_t(i)];}
            const double snr=10*std::log10(energy/error),gain=dot/energy;
            std::cout<<"Speech/noise case "<<kind<<": SNR "<<snr<<" dB, voice gain "<<gain<<"\n";
            check(snr>10.25 && gain>.90,"default denoiser improves white/pink/hum/modulated speech fixtures while retaining voice");
        }
        {
            auto processorStorage=std::make_unique<TatreeProcessor>();auto& p=*processorStorage;set(p,"noise",0);set(p,"ess",0);set(p,"breath",0);set(p,"filter",0);set(p,"autoGain",1);p.prepareToPlay(reader->sampleRate,512);
            juce::MidiBuffer midi;float previous=0,maxRise=0,maxFall=0,peak=0;
            for(int offset=0;offset<n;offset+=512) {
                const int count=std::min(512,n-offset);juce::AudioBuffer<float> b(2,count);
                for(int c=0;c<2;++c)for(int k=0;k<count;++k)b.setSample(c,k,clean[size_t(offset+k)]);
                p.processBlock(b,midi);const float gain=p.meters.autoGainDb.load();
                const float slew=(gain-previous)*float(reader->sampleRate)/count;previous=gain;
                maxRise=std::max(maxRise,slew);maxFall=std::max(maxFall,-slew);peak=std::max(peak,b.getMagnitude(0,count));
            }
            std::cout<<"Recorded vocal auto gain: maximum rise/fall "<<maxRise<<" / "<<maxFall<<" dB/s, output peak "<<peak<<"\n";
            check(maxRise<3.2f && maxFall<12.3f && peak<.98f,"recorded vocal auto gain remains smooth with headroom on the reference fixture");
        }
    }
    std::cout<<"All audio and motion checks passed.\n";return 0;
 }catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}
}
