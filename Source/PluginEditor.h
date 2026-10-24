#pragma once
#include "PluginProcessor.h"
#include "MotionState.h"
#include <juce_gui_extra/juce_gui_extra.h>

class TatreeLook final : public juce::LookAndFeel_V4 {
public:
    TatreeLook();
    juce::Font headline(float size) const;
    juce::Font body(float size) const;
    void drawRotarySlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider&) override;
    void drawButtonBackground(juce::Graphics&,juce::Button&,const juce::Colour&,bool,bool) override;
    void drawButtonText(juce::Graphics&,juce::TextButton&,bool,bool) override;
    const juce::Colour blue {0xff123d91}, ink {0xff102a4c}, cream {0xffeee9df}, red {0xffcf353e};
private:
    juce::Typeface::Ptr displayFace, bodyFace;
};
class TatreeEditor final : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit TatreeEditor(TatreeProcessor&);
    ~TatreeEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
    LawyerState currentMotionState() const { return motion.state; }
private:
    void timerCallback() override;
    void meter(juce::Graphics&,juce::Rectangle<float>,juce::String,bool,bool);
    void caption(juce::Graphics&,juce::String,juce::Rectangle<float>,float,juce::Colour,bool=false);
    void caption(juce::Graphics& g, const char* text, juce::Rectangle<float> r, float size, juce::Colour c, bool body=false) {
        caption(g, juce::String::fromUTF8(text), r, size, c, body);
    }
    TatreeProcessor& processor;
    TatreeLook look;
    juce::Image paper, lawyer, lawyerDeess, lawyerHot;
    std::array<juce::Slider,3> knobs;
    std::array<juce::Label,3> values;
    std::array<float,6> previousControls {{45,25,30,1,0,0}};
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>,3> knobAttachments;
    juce::TextButton filter {"FILTER"}, listen {"LISTEN"}, bypass {"BYPASS"}, motionButton {"MOTION FX"}, autoButton {"AUTO GAIN"};
    std::array<juce::Slider,2> gainSliders;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>,2> gainAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>,5> buttonAttachments;
    juce::TooltipWindow tooltips {this,650};
    MotionState motion;
    unsigned long long lastBlock = 0;
    float noAudioSeconds = 0, meterFade = 1;
    std::array<std::array<float,2>,2> heldPeaks {}, peakTimers {};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TatreeEditor)
};
