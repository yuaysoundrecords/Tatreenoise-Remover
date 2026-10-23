#include "PluginEditor.h"
#include <iostream>

// Native integration/recording harness. Every processing state is driven by DSP.
class RenderApp final : public juce::JUCEApplication, private juce::Timer {
public:
    const juce::String getApplicationName() override { return "Tatree render"; }
    const juce::String getApplicationVersion() override { return "2.0"; }
    void initialise(const juce::String& args) override {
        folder=juce::File(args.unquoted());folder.createDirectory();
        folder.getChildFile("frames").createDirectory();
        processor=std::make_unique<TatreeProcessor>();
        editor.reset(static_cast<TatreeEditor*>(processor->createEditor()));
        editor->addToDesktop(0);editor->setVisible(true);
        startScenario(0);startTimerHz(30);
    }
    void shutdown() override { stopTimer();editor.reset();processor.reset(); }
private:
    void fail(const char* why) { std::cerr<<why<<'\n';setApplicationReturnValue(1);stopTimer();quit(); }
    void set(const char* id,float v) {
        auto* p=processor->params.getParameter(id);p->setValueNotifyingHost(p->convertTo0to1(v));
    }
    bool capture(const juce::String& name) {
        auto image=editor->createComponentSnapshot(editor->getLocalBounds());
        juce::FileOutputStream out(folder.getChildFile(name+".png"));
        if(!out.openedOk() || !juce::PNGImageFormat().writeImageToStream(image,out)) { fail("Capture write failed");return false; }
        return true;
    }
    bool controlsWork() {
        for(int c=0;c<editor->getNumChildComponents();++c) {
            auto* child=editor->getChildComponent(c);
            if(auto* knob=dynamic_cast<juce::Slider*>(child))knob->setValue(61,juce::sendNotificationSync);
            if(auto* button=dynamic_cast<juce::TextButton*>(child)) {
                const char* id=button->getButtonText()=="FILTER"?"filter":button->getButtonText()=="LISTEN"?"listen":button->getButtonText()=="BYPASS"?"bypass":"motion";
                bool old=button->getToggleState();button->setToggleState(!old,juce::sendNotificationSync);
                if((processor->params.getRawParameterValue(id)->load()>.5f)==old) { fail("Button binding failed");return false; }
                button->setToggleState(old,juce::sendNotificationSync);
            }
        }
        if(std::abs(processor->params.getRawParameterValue("noise")->load()-61)>.1f) { fail("Knob binding failed");return false; }
        for(int c=0;c<editor->getNumChildComponents();++c) {
            auto* value=dynamic_cast<juce::Label*>(editor->getChildComponent(c));
            if(value && value->getName()=="value-noise") {
                value->showEditor();
                auto* text=value->getCurrentTextEditor();
                if(!text){fail("Numeric editor did not open");return false;}
                text->setText("72.5%",false);value->hideEditor(false);
                if(std::abs(processor->params.getRawParameterValue("noise")->load()-72.5f)>.11f) { fail("Exact numeric edit failed");return false; }
                value->setText("invalid",juce::sendNotificationSync);
                if(std::abs(processor->params.getRawParameterValue("noise")->load()-72.5f)>.11f) { fail("Invalid numeric input changed audio parameter");return false; }
                value->setText("500%",juce::sendNotificationSync);
                if(processor->params.getRawParameterValue("noise")->load()!=100) { fail("Numeric edit clamp failed");return false; }
            }
        }
        std::cout<<"Native knobs, four buttons, exact numeric edits and invalid-input handling passed\n";
        return true;
    }
    void startScenario(int next) {
        scenario=next;ticks=0;phase=0;
        set("noise",scenario==1?90:scenario>=6?0:45);
        set("ess",scenario==1 || scenario>=6?0:scenario==2?90:30);
        set("breath",scenario==6?100:0);
        set("bypass",scenario==4?1:0);set("motion",scenario==5?0:1);
        set("listen",scenario==7?1:0);set("filter",scenario==0 || scenario==8?1:0);
        processor->prepareToPlay(48000,1600);
    }
    void timerCallback() override {
        if(stopped) {
            if(++ticks<55)return;
            if(editor->currentMotionState()!=LawyerState::idle) { fail("Motion stayed active after audio stopped");return; }
            if(!capture("stopped"))return;
            std::cout<<"Stopped transport decays meters and returns motion to idle\n";
            stopTimer();quit();return;
        }
        juce::AudioBuffer<float> b(2,1600);juce::MidiBuffer midi;
        for(int i=0;i<1600;++i) {
            float x=0;
            if(scenario==1)x=(random.nextFloat()-.5f)*.07f;
            else if(scenario==6)x=(random.nextFloat()-.5f)*.008f;
            else if(scenario!=0) {
                const double frequency=scenario==2?8500:scenario>=8?50:400;
                const float amplitude=scenario==3 || scenario==5?1.12f:.2f;
                x=amplitude*float(std::sin(phase*juce::MathConstants<double>::twoPi*frequency/48000));
            }
            ++phase;b.setSample(0,i,x);b.setSample(1,i,x*.95f);
        }
        processor->processBlock(b,midi);++ticks;
        if(ticks>=4 && ticks%2==0 && !capture("frames/frame-"+juce::String(frameIndex++).paddedLeft('0',3)))return;
        const int captureAt=scenario>=8?12:35;
        if(ticks<captureAt)return;
        static const char* names[]{"idle","noise","de-esser","input-hot","bypass","reduced-motion","breath","listen","filter-on","filter-off"};
        static const LawyerState expected[]{LawyerState::idle,LawyerState::noise,LawyerState::deEss,LawyerState::hot,LawyerState::bypass,LawyerState::hot,LawyerState::breath,LawyerState::listen,LawyerState::idle,LawyerState::idle};
        if(!capture(names[scenario]))return;
        std::cout<<"Captured "<<names[scenario]<<"; state="<<int(editor->currentMotionState())<<'\n';
        if(editor->currentMotionState()!=expected[scenario]) { fail("Unexpected DSP-driven motion state");return; }
        if(scenario==0 && !controlsWork())return;
        if(scenario==3) {
            for(auto size : {juce::Point<int>{864,608},juce::Point<int>{1620,1140}}) {
                editor->setSize(size.x,size.y);if(!capture("responsive-"+juce::String(size.x)))return;
            }
            editor->setSize(1080,760);
        }
        if(scenario==9) {
            juce::AudioBuffer<float> hot(2,1600);hot.clear();hot.setSample(0,1599,1.1f);
            processor->processBlock(hot,midi);stopped=true;ticks=0;return;
        }
        startScenario(scenario+1);
    }
    std::unique_ptr<TatreeProcessor> processor;
    std::unique_ptr<TatreeEditor> editor;
    juce::File folder;
    juce::Random random {42};
    int ticks=0,scenario=0,frameIndex=0;
    double phase=0;
    bool stopped=false;
};
START_JUCE_APPLICATION(RenderApp)
