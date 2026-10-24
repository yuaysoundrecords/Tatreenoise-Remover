#pragma once
#include <algorithm>
#include <cmath>

// Stereo-linked pre-chain vocal levelling. This controls average level; it is
// not a compressor/limiter. Silence and low-level breaths hold the correction.
class InputAutoGain {
public:
    void reset(double sampleRate) {
        rate=float(sampleRate);window=std::max(1,int(std::round(sampleRate*.02)));
        rmsFollow=std::exp(-.02f/.4f);peakRelease=std::exp(-.02f/.8f);
        down=1-std::exp(-1.0f/(rate*.35f));up=1-std::exp(-1.0f/(rate*1.5f));
        off=1-std::exp(-1.0f/(rate*.05f));
        power=peak=framePeak=gainDb=targetDb=0;framePower=0;count=quietFrames=0;holding=true;
    }
    float process(float meanPower,float samplePeak,bool enabled) {
        framePower+=meanPower;framePeak=std::max(framePeak,samplePeak);
        if(++count>=window) {
            const float frameMean=float(framePower/count);
            peak=std::max(framePeak,peak*peakRelease);
            // A relative gate prevents a breath/room tone from pulling the
            // reference level down after a loud phrase. A new sustained softer
            // section is accepted after three seconds; silence never learns.
            const bool aboveFloor=frameMean>1.5848932e-5f; // -48 dBFS RMS.
            const bool active=aboveFloor && (power<=0 || frameMean>power*.015848932f || quietFrames>=150);
            if(active) {
                quietFrames=0;holding=false;
                power=power<=0?frameMean:rmsFollow*power+(1-rmsFollow)*frameMean;
                const float rmsDb=10*std::log10(std::max(power,1e-20f));
                const float peakDb=20*std::log10(std::max(peak,1e-10f));
                targetDb=std::clamp(std::min(-18-rmsDb,-3-peakDb),-24.0f,12.0f);
                if(std::abs(targetDb-gainDb)<.15f)targetDb=gainDb;
            } else {
                quietFrames=std::min(quietFrames+1,150);holding=true;targetDb=gainDb;
            }
            framePower=0;framePeak=0;count=0;
        }
        if(!enabled)gainDb+=off*(0-gainDb);
        else {
            const float step=(targetDb-gainDb)*(targetDb<gainDb?down:up);
            // Explicit dB/second bounds prevent plosives from causing abrupt
            // gain rides. Stereo channels always receive the same gain.
            gainDb+=std::clamp(step,-12.0f/rate,3.0f/rate);
        }
        return std::pow(10.0f,gainDb/20);
    }
    float decibels() const {return gainDb;}
    bool isHolding() const {return holding;}
private:
    double framePower=0;
    float rate=48000,power=0,peak=0,framePeak=0,gainDb=0,targetDb=0;
    float rmsFollow=0,peakRelease=0,down=0,up=0,off=0;
    int window=960,count=0,quietFrames=0;
    bool holding=true;
};
