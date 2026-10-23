#include "PluginProcessor.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <iostream>
int main(int argc,char** argv){
    juce::ScopedJuceInitialiser_GUI gui;
    if(argc<3){std::cerr<<"Usage: TatreeOffline input.wav output.wav [noise%=45] [de-ess%=30] [breath%=25]\n";return 1;}
    juce::AudioFormatManager formats;formats.registerBasicFormats();
    const auto input=juce::File::getCurrentWorkingDirectory().getChildFile(argv[1]);
    const auto output=juce::File::getCurrentWorkingDirectory().getChildFile(argv[2]);
    if(input==output || output.exists()){std::cerr<<"Output must be a new file, separate from input.\n";return 1;}
    std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(input));
    if(!reader || reader->numChannels<1 || reader->numChannels>2 || reader->lengthInSamples<1){std::cerr<<"Input must be readable mono or stereo audio.\n";return 1;}
    TatreeProcessor p;
    if(reader->numChannels==1){auto layout=p.getBusesLayout();layout.inputBuses.set(0,juce::AudioChannelSet::mono());layout.outputBuses.set(0,juce::AudioChannelSet::mono());p.setBusesLayout(layout);}
    const char* ids[]{"noise","ess","breath"};const float defaults[]{45,30,25};
    try{for(int i=0;i<3;++i){const float v=argc>i+3?std::stof(argv[i+3]):defaults[i];if(!std::isfinite(v)||v<0||v>100)throw std::runtime_error("range");auto* param=p.params.getParameter(ids[i]);param->setValueNotifyingHost(param->convertTo0to1(v));}}
    catch(...){std::cerr<<"Amounts must be numbers between 0 and 100.\n";return 1;}
    p.prepareToPlay(reader->sampleRate,512);
    juce::WavAudioFormat wav;
    auto stream=output.createOutputStream();if(!stream){std::cerr<<"Cannot write output.\n";return 1;}
    std::unique_ptr<juce::AudioFormatWriter>writer(wav.createWriterFor(stream.get(),reader->sampleRate,reader->numChannels,24,{},0));
    if(!writer){std::cerr<<"Cannot create WAV writer.\n";return 1;}stream.release();
    const int latency=p.getLatencySamples();juce::MidiBuffer midi;
    juce::int64 position=0;float peak=0;
    while(position<reader->lengthInSamples+latency){
        int n=int(std::min<juce::int64>(512,reader->lengthInSamples+latency-position));
        juce::AudioBuffer<float>b(int(reader->numChannels),n);b.clear();
        const int count=int(std::max<juce::int64>(0,std::min<juce::int64>(n,reader->lengthInSamples-position)));
        if(count>0&&!reader->read(&b,0,count,position,true,true)){std::cerr<<"Read failed.\n";return 1;}
        p.processBlock(b,midi);
        const int skip=int(std::clamp<juce::int64>(latency-position,0,n));
        for(int c=0;c<b.getNumChannels();++c)peak=std::max(peak,b.getMagnitude(c,skip,n-skip));
        if(n>skip&&!writer->writeFromAudioSampleBuffer(b,skip,n-skip)){std::cerr<<"Write failed.\n";return 1;}
        position+=n;
    }
    writer.reset();std::cout<<"Wrote "<<output.getFileName()<<"; latency trimmed; peak "<<juce::Decibels::gainToDecibels(peak,-100.0f)<<" dBFS\n";
    return 0;
}
