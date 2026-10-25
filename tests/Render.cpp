#include "PluginEditor.h"
#include <iostream>

// Native integration/recording harness. Every processing state is driven by DSP.
class RenderApp final : public juce::JUCEApplication, private juce::Timer {
public:
    const juce::String getApplicationName() override { return "Tatree render"; }
    const juce::String getApplicationVersion() override { return "2.1.0"; }
    void initialise(const juce::String& args) override {
        auto arguments=juce::StringArray::fromTokens(args,true);arguments.removeEmptyStrings();
        recordFrames=arguments.contains("--frames");arguments.removeString("--frames");
        if(arguments.size()!=1){std::cerr<<"Usage: TatreeRender output-directory [--frames]\n";setApplicationReturnValue(1);quit();return;}
        folder=juce::File(arguments[0].unquoted());folder.createDirectory();
        folder.getChildFile("frames").createDirectory();
        processor=std::make_unique<TatreeProcessor>();
        editor.reset(static_cast<TatreeEditor*>(processor->createEditor()));
        editor->addToDesktop(0);editor->setVisible(true);
        if(editor->currentSpeechCue()!=SpeechCue::idle){fail("Editor must open without speech");return;}
        if(!capture("startup"))return;
        if(!licencePanelWorks())return;
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
            if(auto* knob=dynamic_cast<juce::Slider*>(child))knob->setValue(knob->getName()=="inputGain"?3:knob->getName()=="outputGain"?-3:61,juce::sendNotificationSync);
            if(auto* button=dynamic_cast<juce::TextButton*>(child)) {
                if(button->getName()=="license-unlock")continue;
                const char* id=button->getButtonText()=="FILTER"?"filter":button->getButtonText()=="LISTEN"?"listen":button->getButtonText()=="BYPASS"?"bypass":button->getButtonText()=="AUTO GAIN"?"autoGain":"motion";
                bool old=button->getToggleState();button->setToggleState(!old,juce::sendNotificationSync);
                if((processor->params.getRawParameterValue(id)->load()>.5f)==old) { fail("Button binding failed");return false; }
                button->setToggleState(old,juce::sendNotificationSync);
            }
        }
        if(std::abs(processor->params.getRawParameterValue("inputGain")->load()-3)>.1f || std::abs(processor->params.getRawParameterValue("outputGain")->load()+3)>.1f) {fail("Gain trim binding failed");return false;}
        set("inputGain",0);set("outputGain",0);
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
        std::cout<<"Native knobs, gain trims, five buttons, exact numeric edits and invalid-input handling passed\n";
        return true;
    }
    bool licencePanelWorks() {
        juce::TextButton* unlock=nullptr;
        for(int i=0;i<editor->getNumChildComponents();++i)
            if(auto* b=dynamic_cast<juce::TextButton*>(editor->getChildComponent(i)))
                if(b->getName()=="license-unlock")unlock=b;
        if(!unlock || !unlock->onClick){fail("License button missing");return false;}
        unlock->onClick();
        juce::Component* panel=nullptr;juce::TextEditor* key=nullptr;juce::TextButton* close=nullptr;
        for(int i=0;i<editor->getNumChildComponents();++i) {
            auto* candidate=editor->getChildComponent(i);
            for(int j=0;j<candidate->getNumChildComponents();++j) {
                auto* child=candidate->getChildComponent(j);
                if(child->getName()=="Gumroad license key") {panel=candidate;key=dynamic_cast<juce::TextEditor*>(child);}
                if(child->getName()=="license-close")close=dynamic_cast<juce::TextButton*>(child);
            }
        }
        if(!panel || !panel->isVisible() || !key || !close){fail("Activation panel did not open");return false;}
        key->setText("UI-TEST-NOT-A-REAL-KEY",false);
        if(!capture("license-panel"))return false;
        for(auto size:{juce::Point<int>{864,688},juce::Point<int>{1620,1290}}) {
            editor->setSize(size.x,size.y);if(!capture("license-panel-"+juce::String(size.x)))return false;
        }
        editor->setSize(1080,860);close->onClick();
        if(panel->isVisible() || key->getText().isNotEmpty()){fail("Closing activation did not clear key");return false;}
        if(editor->currentSpeechCue()!=SpeechCue::idle){fail("Licensing fabricated a speech cue");return false;}
        std::cout<<"License panel opens, resizes, closes and clears key without touching DSP\n";
        return true;
    }
    void startScenario(int next) {
        scenario=next;ticks=0;phase=0;
        set("autoGain",scenario==10?1:0);set("inputGain",0);set("outputGain",scenario==11?24:0);
        set("noise",scenario==1?90:scenario>=6?0:45);
        set("ess",scenario==1 || scenario>=6?0:scenario==2?90:30);
        set("breath",scenario==6?100:0);
        set("bypass",scenario==4?1:0);set("motion",scenario==5?0:1);
        set("listen",scenario==7?1:0);set("filter",scenario==0 || scenario==8?1:0);
        processor->prepareToPlay(48000,1600);
    }
    void quoteAction() {
        if(quoteIndex<4) {
            const char* names[]{"inputGain","De-esser","Noise reduction","Breath reduction"};
            for(int i=0;i<editor->getNumChildComponents();++i)
                if(auto* knob=dynamic_cast<juce::Slider*>(editor->getChildComponent(i)))
                    if(knob->getName()==names[quoteIndex]) {knob->setValue(knob->getValue()+1,juce::sendNotificationSync);return;}
        } else if(quoteIndex<6) {
            for(int i=0;i<editor->getNumChildComponents();++i)
                if(auto* button=dynamic_cast<juce::TextButton*>(editor->getChildComponent(i)))
                    if(button->getButtonText()=="AUTO GAIN") {button->triggerClick();return;}
        } else return;
        fail("Quote control not found");
    }
    void quoteTick() {
        ++ticks;
        if(quoteIndex==6) {
            if(ticks<100)return;
            if(editor->currentSpeechCue()!=SpeechCue::idle){fail("Quote did not expire after control release");return;}
            if(!capture("quote-expired"))return;
            std::cout<<"Control quotes expire without fabricating DSP activity\n";stopTimer();quit();return;
        }
        if(ticks<10)return;
        const SpeechCue expected[]{SpeechCue::peak,SpeechCue::deEss,SpeechCue::noise,SpeechCue::breath,SpeechCue::autoGain,SpeechCue::autoGain};
        const char* names[]{"quote-peak","quote-de-esser","quote-noise","quote-breath","quote-auto-on","quote-auto-off"};
        if(editor->currentSpeechCue()!=expected[quoteIndex]){fail("User control did not show its assigned quote");return;}
        if(quoteIndex>=4 && (processor->params.getRawParameterValue("autoGain")->load()>.5f)!=(quoteIndex==4)){fail("Auto Gain quote must work on both ON and OFF clicks");return;}
        if(editor->currentMotionState()!=LawyerState::idle){fail("Control quote fabricated DSP activity");return;}
        if(!capture(names[quoteIndex]))return;
        if(quoteIndex==0) {
            for(auto size:{juce::Point<int>{864,688},juce::Point<int>{1620,1290}}) {
                editor->setSize(size.x,size.y);if(!capture("quote-peak-"+juce::String(size.x)))return;
            }
            editor->setSize(1080,860);
        }
        std::cout<<"User interaction captured "<<names[quoteIndex]<<"\n";
        ++quoteIndex;ticks=0;quoteAction();
    }
    void timerCallback() override {
        if(quoteTesting){quoteTick();return;}
        if(stopped) {
            if(++ticks<55)return;
            if(editor->currentMotionState()!=LawyerState::idle) { fail("Motion stayed active after audio stopped");return; }
            if(!capture("stopped"))return;
            std::cout<<"Stopped transport decays meters and returns motion to idle\n";
            quoteTesting=true;quoteIndex=0;ticks=0;quoteAction();return;
        }
        juce::AudioBuffer<float> b(2,1600);juce::MidiBuffer midi;
        for(int i=0;i<1600;++i) {
            float x=0;
            if(scenario==1)x=(random.nextFloat()-.5f)*.07f;
            else if(scenario==6)x=(random.nextFloat()-.5f)*.008f;
            else if(scenario!=0) {
                const double frequency=scenario==2?8500:scenario==8 || scenario==9?50:400;
                const float amplitude=scenario==3 || scenario==5?1.12f:scenario==10?.07f:.2f;
                x=amplitude*float(std::sin(phase*juce::MathConstants<double>::twoPi*frequency/48000));
            }
            ++phase;b.setSample(0,i,x);b.setSample(1,i,x*.95f);
        }
        processor->processBlock(b,midi);++ticks;
        if(recordFrames && ticks>=4 && ticks%2==0 && !capture("frames/frame-"+juce::String(frameIndex++).paddedLeft('0',3)))return;
        const int captureAt=scenario==8 || scenario==9?12:35;
        if(ticks<captureAt)return;
        static const char* names[]{"idle","noise","de-esser","input-hot","bypass","reduced-motion","breath","listen","filter-on","filter-off","auto-gain","output-hot"};
        static const LawyerState expected[]{LawyerState::idle,LawyerState::noise,LawyerState::deEss,LawyerState::hot,LawyerState::bypass,LawyerState::hot,LawyerState::breath,LawyerState::listen,LawyerState::idle,LawyerState::idle,LawyerState::idle,LawyerState::hot};
        if(!capture(names[scenario]))return;
        std::cout<<"Captured "<<names[scenario]<<"; state="<<int(editor->currentMotionState())<<'\n';
        if(editor->currentMotionState()!=expected[scenario]) { fail("Unexpected DSP-driven motion state");return; }
        if(scenario==0 && !controlsWork())return;
        if(scenario==3) {
            for(auto size : {juce::Point<int>{864,688},juce::Point<int>{1620,1290}}) {
                editor->setSize(size.x,size.y);if(!capture("responsive-"+juce::String(size.x)))return;
            }
            editor->setSize(1080,860);
        }
        if(scenario==10 && processor->meters.autoGainDb.load()<.1f){fail("Auto gain UI fixture did not engage");return;}
        if(scenario==11) {
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
    bool stopped=false,recordFrames=false,quoteTesting=false;
    int quoteIndex=0;
};
START_JUCE_APPLICATION(RenderApp)
