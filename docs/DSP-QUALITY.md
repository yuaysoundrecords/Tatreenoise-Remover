# DSP quality review — 2.0.2

## What was actually inspected

- [xiph/speexdsp](https://github.com/xiph/speexdsp/blob/a2133f5904ddb616b6578920bd2199f5accb484d/libspeexdsp/preprocess.c): pinned source, SHA-256 in `evidence/dsp-sources.json`.
- [xiph/rnnoise](https://github.com/xiph/rnnoise/blob/130913db902535a19eb477a9f4cdd5c922d63cf2/README): pinned source, SHA-256 in `evidence/dsp-sources.json`.
- [airwindows/airwindows](https://github.com/airwindows/airwindows/blob/f06250c08225d4040af3addab2026e99779ef42e/plugins/WinVST/DeEss/DeEssProc.cpp): pinned source, SHA-256 in `evidence/dsp-sources.json`.
- [webrtc-mirror/webrtc](https://github.com/webrtc-mirror/webrtc/blob/c940dba16ab898003c8a93472c046c6178ce96e7/common_audio/vad/vad_core.c): pinned source, SHA-256 in `evidence/dsp-sources.json`.

SpeexDSP provides noise/speech estimation, gain floors and statistical suppression; Airwindows DeEss illustrates dedicated sibilance detection, split-band attenuation and sample-rate scaling; WebRTC VAD uses band evidence and temporal hangover. RNNoise is a trained RNN speech-enhancement system. The local C++ minimum-tracking estimator is informed by the SpeexDSP algorithm; its upstream BSD notice/credits are retained in `THIRD_PARTY.md`. No SpeexDSP library, RNNoise model, Airwindows or WebRTC source is linked/bundled. Only SpeexDSP was executed as a comparative processor. No claim is made of audio A/B parity with Airwindows, RNNoise or a commercial de-breath product.

## Changes driven by the review

- Noise: replaced continuously rising background learning with smoothed two-window minimum tracking, conservative startup, speech-presence gating of upward noise adaptation, and separate suppression weights for speech/background bins. Rates scale with sample rate. Existing 1024-sample latency, float processing and bounded/frequency-smoothed masks remain.
- De-esser: replaced the exponential one-pole approximation with a bilinear/TPT complementary split at 4.5 kHz (clamped to 40% of sample rate where necessary). Matched 5 ms RMS detectors avoid the peak-biased ratio that missed sibilance mixed with vowels. Gain attack/release remains separate. Detector/gain is stereo linked; only the high band is reduced.
- De-breath: requires sustained low-level unvoiced evidence (65 ms threshold, full confidence at 100 ms), with decay through gaps. It preserves the 40 ms consonant-like test event and releases faster for a following voiced word. This is a conservative heuristic, **not a trained breath/phoneme classifier**.

## Controlled recorded-speech comparison (Linux)

One fixed JFK speech sample, scaled by 0.5 to prevent clipping; deterministic seed 749; white, pink, 60/120 Hz hum and modulated broadband noise; SNR 0/10 dB. Each input has 0.5 s noise-only preroll and a 1 s tail. Both processors receive identical 16-bit WAV samples. No AGC, de-essing, breath processing or low-cut filter. Tatree removes its declared 1024-sample delay; SpeexDSP 1.2.1-3 removes its measured 160-sample delay. Speex uses 10 ms frames and noise suppression -12 dB, VAD/AGC/dereverb off. Knob positions are not equivalent suppression settings.

Values below are output SNR in dB; higher is better against this specific clean reference. Full SI-SDR, correlation gain, binary hashes and environment versions are in [raw results](evidence/dsp-comparison.json). Correlation gain is not an intelligibility or listening score.

| Noise | Input SNR | 2.0.1 at 45% | 2.0.2 at 45% | SpeexDSP -12 dB |
| --- | ---: | ---: | ---: | ---: |
| white | 0 | 1.70 | 3.99 | 8.64 |
| white | 10 | 10.88 | 13.27 | 16.66 |
| pink | 0 | 1.38 | 2.48 | 3.23 |
| pink | 10 | 10.67 | 12.15 | 13.09 |
| hum | 0 | 0.89 | 10.99 | 11.41 |
| hum | 10 | 10.08 | 18.95 | 20.39 |
| modulated | 0 | 1.56 | 0.82 | 0.39 |
| modulated | 10 | 10.70 | 10.75 | 10.48 |

Clean-speech correlation gain at the default 45% improved from 0.881 to 0.960; clean-reference error SNR improved from 17.25 to 23.01 dB. The new processor improves SNR in 7 of 8 noisy cases at 45%. In the difficult modulated-noise/0 dB case, SNR is lower than 2.0.1 (0.82 versus 1.56 dB), while retaining more voice level (gain 0.946 versus 0.879). SpeexDSP remains ahead on several stationary-noise cases, particularly white noise. This is an improvement over our preceding implementation, not proof of general superiority or parity.

[Noisy speech sample](audio/quality-speech-before.wav) / [2.0.2 at Noise 45%](audio/quality-speech-after.wav): the white/10 dB case above. Source licence/provenance: `tests/fixtures/`.

## Validation and limits

Core tests additionally cover quiet 6 kHz sibilance at 16/44.1/48/96 kHz, simultaneous 400 Hz vowel body and 8.5 kHz sibilance, short unvoiced bursts, following-word recovery, silence, bypass, gains and automation/state safety. The mandatory speech suite includes clean-voice retention and white/pink/hum/modulated fixtures. Native Windows results are recorded in `VALIDATION.md` when complete.

The comparative benchmark ran on Linux and uses one English speaker. Thai speech, singing, actual breath recordings, keyboard/cafe/traffic noises, blind perceptual listening, CPU comparisons and commercial product parity remain unverified. Independent per-channel spectral noise masks can still alter stereo ambience; de-esser/breath/Auto Gain detectors are linked. Use actual recordings and Listen before aggressive settings.

Reproduce (external SpeexDSP reference library is development-only, not shipped):

```sh
python tests/quality_benchmark.py --processor build/TatreeOffline_artefacts/Release/TatreeOffline --speex-library /path/to/libspeexdsp.so.1 --output build/fresh-quality-comparison
```

Optionally add `--baseline /path/to/2.0.1/TatreeOffline` compiled with the same `--filter-off` CLI option to reproduce the before/after rows. Existing output directories are rejected to preserve evidence.
