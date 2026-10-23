#include "PluginEditor.h"
#include <BinaryData.h>
#include <cstdlib>

TatreeLook::TatreeLook() {
    displayFace=juce::Typeface::createSystemTypefaceFor(BinaryData::AntonRegular_ttf,BinaryData::AntonRegular_ttfSize);
    bodyFace=juce::Typeface::createSystemTypefaceFor(BinaryData::NotoSansThai_ttf,BinaryData::NotoSansThai_ttfSize);
}
juce::Font TatreeLook::headline(float size) const { return juce::Font(juce::FontOptions(displayFace).withHeight(size)); }
juce::Font TatreeLook::body(float size) const { return juce::Font(juce::FontOptions(bodyFace).withHeight(size)); }
void TatreeLook::drawRotarySlider(juce::Graphics& g,int x,int y,int width,int height,float pos,float start,float end,juce::Slider& s) {
    const auto area=juce::Rectangle<float>(float(x),float(y),float(width),float(height)).reduced(16);
    const float radius=std::min(area.getWidth(),area.getHeight())*.5f, cx=area.getCentreX(), cy=area.getCentreY();
    juce::Path track,fill; track.addCentredArc(cx,cy,radius,radius,0,start,end,true);
    fill.addCentredArc(cx,cy,radius,radius,0,start,start+pos*(end-start),true);
    g.setColour(ink.withAlpha(.17f));g.strokePath(track,juce::PathStrokeType(8));
    g.setColour(s.isEnabled()?blue:ink.withAlpha(.3f));g.strokePath(fill,juce::PathStrokeType(8));
    const float activity=float(double(s.getProperties().getWithDefault("activity",0.0)));
    if(activity>.02f || s.isMouseOverOrDragging()){
        g.setColour(blue.withAlpha(s.isMouseOverOrDragging()?.35f:.12f+activity*.25f));
        g.drawEllipse(cx-radius-7,cy-radius-7,2*radius+14,2*radius+14,2);
    }
    for(int i=0;i<=20;++i){float a=start+(end-start)*i/20.0f;float r=radius+11;
        g.setColour(blue.withAlpha(.8f));g.drawLine(cx+std::sin(a)*r,cy-std::cos(a)*r,cx+std::sin(a)*(r+4),cy-std::cos(a)*(r+4),1.4f);}
    const float r=radius-13;
    juce::DropShadow(juce::Colours::black.withAlpha(.22f),8,{0,4}).drawForPath(g,[&]{juce::Path p;p.addEllipse(cx-r,cy-r,2*r,2*r);return p;}());
    g.setColour(juce::Colour(0xff232a30));g.fillEllipse(cx-r,cy-r,2*r,2*r);
    g.setColour(juce::Colour(0xff60666a));g.drawEllipse(cx-r+2,cy-r+2,2*r-4,2*r-4,1.4f);
    g.setColour(juce::Colour(0xff343c42));g.drawEllipse(cx-r+7,cy-r+7,2*r-14,2*r-14,1);
    float a=start+pos*(end-start);
    g.setColour(cream);g.drawLine(cx+std::sin(a)*r*.35f,cy-std::cos(a)*r*.35f,cx+std::sin(a)*r*.86f,cy-std::cos(a)*r*.86f,4);
    if(s.hasKeyboardFocus(false)){g.setColour(blue);g.drawEllipse(cx-radius-19,cy-radius-19,2*radius+38,2*radius+38,2);}
}
void TatreeLook::drawButtonBackground(juce::Graphics& g,juce::Button& b,const juce::Colour&,bool over,bool down){
    auto r=b.getLocalBounds().toFloat().reduced(2);
    auto active=b.getToggleState();
    auto color=active?blue:cream;
    if(b.getButtonText()=="BYPASS" && active)color=red;
    if(over)color=color.darker(.08f);
    if(down)r.translate(0,1);
    g.setColour(ink.withAlpha(.15f));g.fillRoundedRectangle(r.translated(0,2),3);
    g.setColour(color);g.fillRoundedRectangle(r,3);
    g.setColour(ink);g.drawRoundedRectangle(r,3,1.5f);
    if(b.hasKeyboardFocus(false)){g.setColour(blue);g.drawRect(b.getLocalBounds(),1);}
}
void TatreeLook::drawButtonText(juce::Graphics& g,juce::TextButton& b,bool,bool){
    g.setColour(b.getToggleState()?cream:ink);g.setFont(headline(float(b.getHeight())*(b.getButtonText()=="MOTION FX"?20.0f/26:32.0f/52)));
    g.drawText(b.getButtonText(),b.getLocalBounds(),juce::Justification::centred);
}
TatreeEditor::TatreeEditor(TatreeProcessor& p):AudioProcessorEditor(p),processor(p){
    setLookAndFeel(&look);
    paper=juce::ImageCache::getFromMemory(BinaryData::paper_png,BinaryData::paper_pngSize);
    lawyer=juce::ImageCache::getFromMemory(BinaryData::lawyercolour_png,BinaryData::lawyercolour_pngSize);
    lawyerDeess=juce::ImageCache::getFromMemory(BinaryData::lawyerdeess_png,BinaryData::lawyerdeess_pngSize);
    lawyerHot=juce::ImageCache::getFromMemory(BinaryData::lawyerhot_png,BinaryData::lawyerhot_pngSize);
    const char* ids[]{"noise","breath","ess"};const char* names[]{"Noise reduction","Breath reduction","De-esser"};
    for(int i=0;i<3;++i){auto& k=knobs[i];addAndMakeVisible(k);
        k.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);k.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);
        k.setRotaryParameters(juce::MathConstants<float>::pi*1.23f,juce::MathConstants<float>::pi*2.77f,true);
        k.setDoubleClickReturnValue(true,i==0?45:i==1?25:30);
        k.setName(names[i]);k.setTitle(names[i]);k.setWantsKeyboardFocus(true);
        k.setTooltip(juce::String(names[i])+". Drag or use arrow keys; double-click to reset.");
        auto& value=values[i];addAndMakeVisible(value);
        value.setName(juce::String("value-")+ids[i]);value.setJustificationType(juce::Justification::centred);
        value.setColour(juce::Label::textColourId,look.blue);value.setColour(juce::Label::outlineWhenEditingColourId,look.blue);
        value.setEditable(false,true,false);value.setTooltip("Double-click to type an exact percentage.");
        const auto refresh=[this,i]{values[i].setText(juce::String(knobs[i].getValue(),1)+"%",juce::dontSendNotification);};
        value.onTextChange=[this,i,refresh]{
            auto text=values[i].getText().trim().trimCharactersAtEnd("%").trim().replaceCharacter(',', '.');
            char* end=nullptr;const float n=std::strtof(text.toRawUTF8(),&end);
            if(end!=text.toRawUTF8() && end && *end=='\0' && std::isfinite(n))
                knobs[i].setValue(juce::jlimit(0.0f,100.0f,n),juce::sendNotificationSync);
            refresh();
        };
        value.onEditorHide=refresh;knobs[i].onValueChange=refresh;
        knobAttachments[i]=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.params,ids[i],k);
        refresh();
    }
    std::array<juce::TextButton*,4> buttons{&filter,&listen,&bypass,&motionButton};const char* bids[]{"filter","listen","bypass","motion"};
    for(int i=0;i<4;++i){auto& b=*buttons[i];b.setClickingTogglesState(true);addAndMakeVisible(b);
        buttonAttachments[i]=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.params,bids[i],b);}
    filter.setTooltip("75 Hz low-cut: remove rumble before noise reduction.");
    listen.setTooltip("Listen to the removed signal (input minus cleaned output).");
    bypass.setTooltip("Latency-matched dry signal. Smooth 15 ms transition.");
    motionButton.setTooltip("Turn off motion effects. Status text and audio remain active.");
    setResizable(true,true);setResizeLimits(864,608,1620,1140);
    if(auto* c=getConstrainer())c->setFixedAspectRatio(1080.0/760.0);
    setSize(1080,760);startTimerHz(30);
}
TatreeEditor::~TatreeEditor(){stopTimer();setLookAndFeel(nullptr);}
void TatreeEditor::resized(){
    auto scale=float(getWidth())/1080;
    knobs[0].setBounds(juce::Rectangle<int>(45,211,258,230)*scale);
    knobs[1].setBounds(juce::Rectangle<int>(310,282,160,165)*scale);
    knobs[2].setBounds(juce::Rectangle<int>(480,282,160,165)*scale);
    values[0].setBounds(juce::Rectangle<int>(72,438,206,43)*scale);
    values[1].setBounds(juce::Rectangle<int>(330,450,120,36)*scale);
    values[2].setBounds(juce::Rectangle<int>(500,450,120,36)*scale);
    for(int i=0;i<3;++i)values[i].setFont(look.headline((i==0?39.0f:31.0f)*scale));
    filter.setBounds(juce::Rectangle<int>(43,663,164,52)*scale);
    listen.setBounds(juce::Rectangle<int>(386,663,170,52)*scale);
    bypass.setBounds(juce::Rectangle<int>(635,663,170,52)*scale);
    motionButton.setBounds(juce::Rectangle<int>(900,687,128,26)*scale);
}
void TatreeEditor::timerCallback(){
    const auto block = processor.meters.blocks.load(std::memory_order_relaxed);
    noAudioSeconds = block != lastBlock ? 0 : noAudioSeconds + 1.0f/30;
    lastBlock = block;
    meterFade = noAudioSeconds > .2f ? meterFade * .8465f : 1.0f;
    const char* ids[]{"noise","breath","ess","filter","listen","bypass"};
    const LawyerControl controls[]{LawyerControl::noise,LawyerControl::breath,LawyerControl::deEss,LawyerControl::filter,LawyerControl::listen,LawyerControl::bypass};
    for(int i=0;i<6;++i){const float value=processor.params.getRawParameterValue(ids[i])->load();
        if(std::abs(value-previousControls[i])>.01f){motion.touch(controls[i]);previousControls[i]=value;}}
    for(int i=0;i<3;++i)if(!values[i].isBeingEdited())values[i].setText(juce::String(knobs[i].getValue(),1)+"%",juce::dontSendNotification);
    const float activity[]{processor.meters.noiseDb.load()/12,processor.meters.breathDb.load()/8,processor.meters.deEssDb.load()/8};
    for(int i=0;i<3;++i)knobs[i].getProperties().set("activity",double(std::clamp(activity[i]*meterFade,0.0f,1.0f)));
    for(int kind=0;kind<2;++kind)for(int ch=0;ch<2;++ch){
        float now=(kind==0?processor.meters.input[ch].load():processor.meters.output[ch].load())*meterFade;
        if(now>=heldPeaks[kind][ch]){heldPeaks[kind][ch]=now;peakTimers[kind][ch]=1.0f;}
        else if(peakTimers[kind][ch]>0)peakTimers[kind][ch]-=1.0f/30;
        else heldPeaks[kind][ch]*=.936f;
    }
    motion.reduced=processor.params.getRawParameterValue("motion")->load()<.5f;
    motion.update(processor.meters.noiseDb.load()*meterFade,processor.meters.deEssDb.load()*meterFade,processor.meters.hot.exchange(false),
        processor.meters.bypassed.load(),1.0f/30,processor.meters.breathDb.load()*meterFade,listen.getToggleState());
    repaint();
}
void TatreeEditor::caption(juce::Graphics& g,juce::String text,juce::Rectangle<float> r,float size,juce::Colour c,bool body){
    g.setColour(c);g.setFont(body?(size>=40?look.body(size*1.15f).boldened():look.body(size*1.15f)):look.headline(size));g.drawText(text,r,juce::Justification::centred,false);
}
void TatreeEditor::meter(juce::Graphics& g,juce::Rectangle<float> r,juce::String title,bool output,bool reduction){
    caption(g,title,{r.getX(),r.getY(),r.getWidth(),35},36,look.cream);
    auto track=juce::Rectangle<float>(r.getX()+27,r.getY()+60,r.getWidth()-35,18);
    for(int tick=0;tick<6;++tick){const float dbs[]{-60,-36,-24,-12,-6,0};const float gr[]{0,4,8,12,18,24};
        const float value=reduction?gr[tick]:dbs[tick];const float f=reduction?value/24:(value+60)/60;
        const float x=track.getX()+f*track.getWidth();
        caption(g,juce::String(int(value)),{x-14,r.getY()+38,28,22},18,look.cream,true);
    }
    for(int ch=0;ch<2;++ch){
        const float level=reduction?std::clamp(processor.meters.reductionDb.load()*meterFade/24,0.0f,1.0f)
            :std::clamp((juce::Decibels::gainToDecibels(output?processor.meters.output[ch].load()*meterFade:processor.meters.input[ch].load()*meterFade,-60.0f)+60)/60,0.0f,1.0f);
        caption(g,reduction?"":ch==0?"L":"R",{r.getX(),track.getY()+ch*26-2,20,20},15,look.cream,true);
        for(int i=0;i<36;++i){const float f=float(i)/36;
            auto color=reduction?juce::Colour(0xff4ac8f5):f>.95f?look.red:f>.8f?juce::Colour(0xffeabe45):juce::Colour(0xff72c9a0);
            g.setColour(float(i+1)/36<=level?color:juce::Colour(0xff28435d));
            g.fillRect(track.getX()+i*(track.getWidth()/36),track.getY()+ch*26,track.getWidth()/36-2,16.0f);
        }
        if(!reduction){
            const auto held=heldPeaks[output?1:0][ch];
            const float peak=std::clamp((juce::Decibels::gainToDecibels(held,-60.0f)+60)/60,0.0f,1.0f);
            if(peak>0){g.setColour(look.cream);float x=track.getX()+peak*(track.getWidth()-2);g.fillRect(x,track.getY()+ch*26,1.5f,16.0f);}
        }
    }
    caption(g,reduction ? juce::String(processor.meters.reductionDb.load()*meterFade,1)+" dB" : juce::String(juce::Decibels::gainToDecibels(std::max(heldPeaks[output?1:0][0],heldPeaks[output?1:0][1]),-60.0f),1)+" dBFS",{r.getX(),r.getY()+111,r.getWidth(),18},14,look.cream,true);
}
void TatreeEditor::paint(juce::Graphics& g){
    g.addTransform(juce::AffineTransform::scale(float(getWidth())/1080));
    g.setColour(look.cream);g.fillRect(0,0,1080,760);
    g.drawImage(paper,{0,0,1080,760});
    g.setColour(look.blue);g.setFont(look.headline(145));g.drawText("TATREE NOISE REMOVER",juce::Rectangle<int>{32,3,875,122},juce::Justification::centredLeft,false);
    caption(g,"AUDIO",{920,43,125,22},21,look.blue);
    caption(g,"DENOISER",{920,65,125,22},21,look.blue);
    caption(g,"Y U A Y   S O U N D   R E C O R D S",{244,127,566,24},16,look.blue,true);
    g.setColour(look.blue.withAlpha(.65f));g.drawLine(36,138,230,138,1);g.drawLine(815,138,1040,138,1);
    caption(g,"NOISE REDUCTION",{36,166,272,45},54,look.blue);
    caption(g,"BREATH",{308,252,164,31},28,look.blue);
    caption(g,"REDUCTION",{308,278,164,27},23,look.blue);
    caption(g,"DE-ESSER",{480,278,160,27},28,look.blue);
    caption(g,"0%",{49,424,38,23},16,look.blue,true);caption(g,"100%",{264,424,45,23},16,look.blue,true);
    for(int i=1;i<3;++i){float x=i==1?310.0f:480.0f;caption(g,"0%",{x,446,35,22},13,look.blue,true);caption(g,"100%",{x+125,446,40,22},13,look.blue,true);}
    g.setColour(look.blue.withAlpha(.3f));g.drawLine(299,252,299,474,1);g.drawLine(472,300,472,474,1);
    {
        juce::Graphics::ScopedSaveState save(g);g.reduceClipRegion(647,140,400,349);
        juce::Graphics::ScopedSaveState imageSave(g);
        auto transform=juce::AffineTransform::rotation(motion.rotation(),844,390).translated(0,motion.lift());
        g.addTransform(transform);g.setOpacity(motion.state==LawyerState::bypass?.65f:1.0f);
        const auto& pose = motion.state==LawyerState::hot ? lawyerHot : motion.state==LawyerState::deEss ? lawyerDeess : lawyer;
        g.drawImage(pose,{650,155,392,330},juce::RectanglePlacement::centred);
    }
    {
        juce::Graphics::ScopedSaveState save(g);g.addTransform(juce::AffineTransform::rotation(-.075f,828,183));
        g.setColour(look.blue);g.fillRect(653,150,388,71);
        caption(g,"เงียบไว้น้อง!",{665,149,363,71},58,look.cream,true);
    }
    auto state=motion.state;
    juce::String text=state==LawyerState::hot?"INPUT HOT!":state==LawyerState::deEss?"DE-ESSING":state==LawyerState::noise?"NOISE ACTIVE":state==LawyerState::breath?"BREATH CTRL":state==LawyerState::listen?"DELTA LISTEN":state==LawyerState::bypass?"BYPASSED":motion.touchHold>0 && motion.recentControl==LawyerControl::filter?(filter.getToggleState()?"LOW CUT 75Hz":"LOW CUT OFF"):"READY!";
    auto accent=state==LawyerState::hot?look.red:look.blue;
    {
        juce::Graphics::ScopedSaveState save(g);
        g.addTransform(juce::AffineTransform::rotation(.06f,976,286));
        g.setColour(look.cream.withAlpha(.94f));g.fillRect(923,254,116,55);
        caption(g,text,{925,256,112,48},23,accent,false);
        g.setColour(accent);g.drawLine(928,310,1034,310,2);
    }
    caption(g,state==LawyerState::hot?"ลด Input ลงหน่อยน้อง!":state==LawyerState::deEss?"เสียงซ่า… ขอค้าน!":state==LawyerState::noise?"เสียงรบกวน ออกจากศาล!":state==LawyerState::breath?"เบาลมหายใจหน่อยน้อง!":state==LawyerState::listen?"กำลังฟังเฉพาะเสียงที่เอาออก":state==LawyerState::bypass?"พักก่อน • ฟังเสียงต้นฉบับ":"อย่าดัง Noise ต่ำ เดี๋ยวโดนจับ",{650,471,392,22},16,look.blue,true);
    g.setColour(look.ink);g.fillRoundedRectangle(22,498,1036,143,5);
    meter(g,{39,507,320,125},"IN",false,false);
    meter(g,{383,507,310,125},"GAIN REDUCTION",false,true);
    meter(g,{716,507,320,125},"OUT",true,false);
    g.setColour(look.cream.withAlpha(.35f));g.drawLine(373,514,373,628,1);g.drawLine(704,514,704,628,1);
    caption(g,filter.getToggleState()?"75 Hz / ON":"75 Hz / OFF",{44,718,163,19},13,look.blue,true);
    caption(g,listen.getToggleState()?"REMOVED SIGNAL":"REMOVED SIGNAL / OFF",{363,718,214,19},12,look.blue,true);
    caption(g,bypass.getToggleState()?"DRY / ON":"DRY / OFF",{636,718,170,19},13,look.blue,true);
    caption(g,"YUAY SOUND RECORDS",{872,658,180,25},19,look.blue);
    g.setColour(look.blue.withAlpha(.3f));g.drawLine(269,665,269,736,1);g.drawLine(605,665,605,736,1);g.drawLine(850,665,850,736,1);
}
