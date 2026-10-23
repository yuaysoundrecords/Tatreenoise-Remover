# Current validation — Windows-first development

**Windows x64 is the first release target. It is not yet certified or packaged as a Windows-tested release.** macOS work is deferred until Windows passes. Source and scripts are prepared; native Windows validation is still required.

## Latest verified work (Linux, 2026-10-23)

- Native Release build passed: VST3, Standalone, test renderer and offline processor.
- CTest **2/2 suites** passed. There are **31 core audio/motion assertions**, plus three speech-fixture checks in the optional speech mode. The speech suite reruns the core assertions; do not count those duplicate assertions as extra independent tests.
- Synthetic maximum noise attenuation: about −8.34 dB RMS, preserving the existing maximum setting. The default knob taper is now more conservative.
- Real recorded speech regression (fixed JFK fixture + deterministic 10 dB Gaussian noise, denoiser only): default 45% improves SNR from **9.996 to 10.841 dB**, correlation gain **0.881**. The previous development taper measured about 10.087 dB / 0.780 on this same fixture. This comparison is against the earlier new implementation, **not v1**, and does not establish general perceptual superiority.
- Breath regression: quiet voiced 400 Hz fundamentals are preserved within 0.2 dB even at maximum Breath; quiet unvoiced broadband material is still reduced by more than 3 dB.
- De-esser band test, exact WOLA reconstruction, aligned bypass, state restore, nonfinite-input handling, mono/stereo layouts and 44.1/48/96 kHz processing remain tested.
- Native UI integration passed: actual numeric-editor open/commit, exact percentage binding, invalid-input preservation, clamping, knob/button bindings, detector-driven poses, Breath, Listen, Filter on/off, reduced motion, stopped-transport decay and 80–150% layout captures.
- Linux VST3 passed pluginval 1.0.4 strictness 5, seed 42, including editor/automation/buses. No GUI tests were disabled. Steinberg's separate validator is still unrun.
- PowerShell scripts passed the PowerShell parser. Packaging guards were exercised with clearly labelled non-executable fixtures: missing validation is rejected, absolute paths with spaces package correctly, changed binary hashes invalidate the recorded test result. Fixture archives were deleted and are not deliverable Windows builds.

Evidence: `docs/evidence/windows-first/`. Older evidence in `docs/evidence/` belongs to the earlier development iteration; see its saved summary for that historical scope.

## Windows pipeline now prepared

`scripts/build-windows.ps1` uses Visual Studio 2022 x64, builds with static MSVC runtime, executes both CTest suites and speech evidence, tests the native editor, verifies the downloaded Windows pluginval checksum, validates the actual VST3, binds the result to binary SHA-256 hashes and only then packages a Windows ZIP.

The package includes VST3, Standalone, a per-user installer, instructions, checksums and selected evidence. GitHub Actions runs Windows first; the optional macOS job depends on Windows success and is off by default.

These scripts/workflow have **not yet completed a native Windows build** in this session. Passing syntax/filesystem tests does not establish that MSVC, Windows graphics/audio, the installer or a DAW work.

## Current external blocker

- GitHub API access is blocked; `gh api` cannot currently reach `api.github.com` to operate/inspect a Windows workflow. Existing GH_TOKEN presence was checked without exposing its value; permission remains to be tested after connectivity works.
- The alternative SDK path is blocked at `download.visualstudio.microsoft.com` after Microsoft's allowed `aka.ms` redirect. The required custom network additions have been saved to the environment draft but not observed applied to the current instance.
- JUCE 8 explicitly rejects MinGW, so that attempted compiler was abandoned without patching the vendor restriction. A supported Clang/MSVC-compatible route requires the Microsoft SDK above.
- The Wine Windows runtime and official Windows pluginval can start, but that is not a native Windows machine. Original v1 loaded in Wine; its GUI validator timed out there. This is an emulation limitation/undiagnosed Wine interaction, **not evidence that v1 is defective**.

## Still unverified

Actual Windows MSVC build/installer, Windows 10/11 DAWs and devices, real user recordings, signing/public release, and every macOS build. No source changes have been pushed to GitHub and no workflow run is claimed.

The original v1 ZIP and JPG remain unchanged. The new code uses different plugin IDs and does not replace v1 DAW sessions.
