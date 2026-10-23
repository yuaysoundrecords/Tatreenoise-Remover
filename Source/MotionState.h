#pragma once
#include <algorithm>
#include <cmath>

enum class LawyerState { idle, noise, deEss, hot, bypass, breath, listen };
enum class LawyerControl { none, noise, breath, deEss, filter, listen, bypass };

// UI-thread state machine. Detector readings describe processing, touch feedback
// describes controls; these are deliberately separate to avoid false activity.
struct MotionState {
    LawyerState state = LawyerState::idle;
    LawyerControl recentControl = LawyerControl::none;
    float noise = 0, deEss = 0, breath = 0, hotHold = 0, phase = 0, touchHold = 0;
    bool reduced = false;

    void touch(LawyerControl control) { recentControl = control; touchHold = .65f; }

    void update(float noiseDb, float deEssDb, bool inputHot, bool bypassed,
                float dt, float breathDb = 0, bool listening = false) {
        dt = std::clamp(dt, 0.0f, .1f);
        const auto follow = [dt](float old, float value) {
            return value + (old-value)*std::exp(-dt/(value>old ? .045f : .22f));
        };
        noise = follow(noise, std::clamp(noiseDb/12, 0.0f, 1.0f));
        deEss = follow(deEss, std::clamp(deEssDb/8, 0.0f, 1.0f));
        breath = follow(breath, std::clamp(breathDb/8, 0.0f, 1.0f));
        hotHold = inputHot ? .8f : std::max(0.0f,hotHold-dt);
        touchHold = std::max(0.0f,touchHold-dt);
        state = hotHold>0 ? LawyerState::hot : bypassed ? LawyerState::bypass
              : listening ? LawyerState::listen : deEss>.06f ? LawyerState::deEss
              : breath>.06f ? LawyerState::breath : noise>.06f ? LawyerState::noise : LawyerState::idle;
        phase = std::fmod(phase+dt,1000.0f);
    }
    float rotation() const {
        if(reduced || state==LawyerState::bypass) return 0;
        if(state==LawyerState::hot) return .025f*std::sin(phase*42);
        if(state==LawyerState::listen) return -.035f + .008f*std::sin(phase*5);
        if(state==LawyerState::deEss) return .018f*deEss*std::sin(phase*22);
        if(state==LawyerState::noise) return .025f*noise*std::sin(phase*12);
        if(state==LawyerState::breath) return .012f*breath*std::sin(phase*5);
        return recentControl==LawyerControl::filter ? .015f*touchHold*std::sin(phase*16) : 0;
    }
    float lift() const {
        if(reduced || state==LawyerState::bypass) return 0;
        if(state==LawyerState::hot) return -4*std::abs(std::sin(phase*22));
        if(state==LawyerState::noise) return -4*noise*std::sin(phase*12);
        if(state==LawyerState::deEss) return -2*deEss*std::sin(phase*22);
        if(state==LawyerState::breath) return -3*breath*std::sin(phase*5);
        return std::sin(phase*2)*1.2f;
    }
};
