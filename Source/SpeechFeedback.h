#pragma once
#include <algorithm>

enum class SpeechCue { idle, peak, deEss, noise, autoGain, breath };

// User/control movement drives the quote. DSP activity independently drives
// meters and MotionState, so a sentence never fabricates processing activity.
struct SpeechFeedback {
    SpeechCue cue=SpeechCue::idle;
    float remaining=0;
    void trigger(SpeechCue next) {cue=next;remaining=3;}
    void update(float dt) {remaining=std::max(0.0f,remaining-dt);if(remaining<=0)cue=SpeechCue::idle;}
};
