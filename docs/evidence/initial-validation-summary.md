# Validation — Tatree 2 development test build

Validated on the Linux x86_64 cloud worker on 2026-10-23. This is a native C++/JUCE rebuild, not a modified v1 binary.

## Passed

- Release build: VST3, Standalone, `TatreeTests`, `TatreeRender`, `TatreeOffline`.
- CTest: **1/1 suite**, with **24 explicit passing audio/motion checks**. See `docs/evidence/audio-tests.txt` and `ctest.txt`.
- STFT identity reconstruction and 1024-sample latency: maximum error below 1e-5 at zero processing.
- Latency-aligned plugin and host bypass, state restoration, silent input, hot-event latch, 44.1/48/96 kHz, irregular and one-sample blocks, finite output for nonfinite input.
- Stationary synthetic white noise at maximum noise amount: **−8.342 dB RMS** relative to input in the measured settled interval. This does not establish speech quality or improvement over v1.
- De-esser-only test: 400 Hz changes approximately **0 dB**, 8.5 kHz approximately **−3.256 dB** at maximum amount. Tests use steady tones, not human sibilants.
- pluginval **1.0.4, strictness 5, seed 42**: VST3 scan/load, editor creation while processing, state, automation, bus/layout operations, audio processing at three sample rates × five block sizes. Process exited **0**, result **SUCCESS**. No GUI tests were disabled. See `docs/evidence/pluginval.txt`.
- Native UI integration: all three knob bindings and four button bindings; detector-driven idle/noise/de-ess/hot/bypass/reduced-motion screenshots; state assertions for all fixtures; transport-stop meter/motion decay to idle; 864×608, 1080×760, 1620×1140 layouts. See `native-ui-tests.txt` and screenshots.
- Offline file smoke checks: 48 kHz stereo, 384000 input/output frames, 24-bit output; 44.1 kHz mono, 5003 input/output frames (partial last block). Input files remain untouched.
- Mac build script: Bash syntax check passed. CMake/Windows UTF-8 source flag and GitHub workflow are supplied.

## Not run / limitations

- **macOS AU/VST3/Standalone and Windows binaries have not been built or run here.** The cloud machine is Linux. `scripts/build-macos.command` and the GitHub build workflow provide those next steps; the workflow has not been published or executed by this task.
- Logic, Ableton, REAPER and other real DAWs on the user's Mac have not been tested.
- No physical audio interface/microphone or real-time monitoring was tested on this cloud machine.
- Steinberg's separate VST3 validator was **skipped by pluginval** because that executable was not supplied. pluginval's own VST3 tests did execute and pass. AU validation is not applicable to this Linux VST3 run.
- No developer signing/notarization or public release was performed.
- The included audio A/B is an explicitly **synthetic** test fixture, not a speech-quality demonstration. Real recordings still need listening tests, especially quiet consonants and music transients at high noise/breath amounts.

## Review artifacts

Open `TEST-ME.html` after extracting the source/test kit to view an actual native-UI recording and play the synthetic audio A/B without compiling. It is a documentation page, not a browser version of the audio plugin. Use the Mac build instructions for interactive native controls and your own recordings.

The original tracked v1 ZIP and JPG were not modified. Source changes are local to the workspace; nothing was pushed or published to GitHub.
