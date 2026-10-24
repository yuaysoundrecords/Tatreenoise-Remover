# Tatree 2 architecture

A new native JUCE 8.0.8 C++17 effect. No runtime browser, remote API, account, model download or cloud audio processing.

- `Denoiser.h`: fixed-buffer 1024-point real FFT, 256-sample hop, square-root Hann WOLA. Adaptive per-bin noise estimate, progressive strength taper for voice preservation, protected harmonic peaks, bounded Wiener-style gain, temporal and frequency smoothing. The initial profile is estimated automatically; very quiet consonants can still be attenuated at aggressive settings.
- `PluginProcessor`: mono/stereo, input trim, optional stereo-linked pre-chain Auto Gain, 75 Hz low-cut, denoiser, stereo-linked high-band de-esser and gentle low-level unvoiced expander, output trim (high-band confidence protects quiet voiced fundamentals). All callback buffers and filters are prepared in advance. Parameters smooth over 25 ms; bypass/listen/filter transitions over 15 ms. Host reports fixed 1024-sample latency. Bypass and removed-signal audition use aligned dry audio.
- `Metering`: lock-free scalar atomics bridge audio/UI. IN/OUT are peak with 200 ms decay, UI peak markers hold for one second; GR is block energy attenuation against the aligned dry signal, clamped to 0–40 dB and displayed on a 0–24 dB scale. GR rows duplicate a stereo-linked aggregate rather than representing independent channel GR. Noise/de-ess detector readings separately drive motion. Raw input, conditioned input and output hot events are latched until consumed. IN is post input/Auto Gain; output is post output trim. Bypass uses a separate raw 1024-sample delay; delta uses the conditioned aligned reference.
- `MotionState`: timer-thread state machine at 30 Hz. Input hot (800 ms hold) > bypass > delta listen > de-esser > breath > noise > idle. Filter/control-touch feedback is separate from detector activity. Noise and de-ess strength use 45 ms attack and 220 ms release to avoid chatter. Motion preference disables transforms, preserving visual status. Three pre-rendered full-colour cutouts: raised finger, de-esser hush, and input-hot surprise. Rotation/lift add motion; no live generation. A callback counter decays meters and motion to idle when the host stops delivering audio.
- `PluginEditor`: resizable 1080×860 native component, 80–150%; cream paper/cobalt typography, original-identity colour photo collage, native keyboard-accessible knobs with exact editable percentages, readable stereo meters and peak warning. Compiled assets/fonts require no network at runtime.
- `TatreeOffline`: offline mono/stereo WAV processor. Streams fixed 512-sample blocks and trims declared latency.
- `TatreeTests`: signal reconstruction, denoising, band-specific attenuation, bypass alignment, state restore, peak latch, rate/block size safety, motion priority/hold/reduced-motion checks.
- `TatreeRender`: native UI integration harness and evidence capture with DSP-driven inputs.

## Input levelling

`AutoGain.h`: optional (default off) pre-chain RMS levelling to -18 dBFS, stereo-linked mean power. 300 ms RMS detector, 500 ms peak decay, 20 ms downward and 2 s upward gain smoothing; correction bounded -24/+12 dB. A -50 dBFS current-sample/mean-power gate holds gain on silence; -3 dBFS observed-peak headroom caps requested boost. Disable returns to unity in 50 ms. This is not a lookahead limiter. Input/output trims are independent -24/+24 dB parameters with 25 ms smoothing, persisted and automatable. No additional latency.

## Compatibility

New plugin code `Ttr2`, manufacturer `Yuay`, bundle `net.yuaysoundrecords.tatree2`. Does not impersonate old binary IDs. Formats: VST3 + Standalone everywhere supported by JUCE, AU on macOS. No promise of compatibility with v1 project states.
