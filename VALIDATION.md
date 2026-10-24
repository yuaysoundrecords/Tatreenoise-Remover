# Current validation — Windows-first development

**Windows 2.0.3 x64 native build and automated validation passed. A Windows test ZIP is available.** This is an unsigned test build; DAW listening and installer testing on user machines remain required. macOS has not been built.

## 2.0.3 — interaction quotes and smooth vocal staging

- Four new user-provided captions use a shared fitted bold font and semantic two-line wrapping. Actual outline padding is 23 px horizontally / 12 px vertically. Quotes trigger from Input/Output, Ess, Noise, and Auto Gain ON **and OFF**, and expire about 3 s after the last change. Audio detectors continue driving independent meter/motion states.
- Vocal Auto Gain now uses 20 ms windows, 400 ms RMS integration, bounded -12/+3 dB/s correction, 350 ms downward / 1.5 s upward smoothing, a 0.15 dB deadband and absolute/relative breath/silence HOLD. Targets -18 dBFS active RMS with -24/+12 dB correction limits. It is not a lookahead limiter or LUFS normalizer.
- Local CTest 2/2 passed: **64 core assertions plus 9 speech checks** (the speech suite repeats the core tests). Includes smooth gain-slew bounds at 44.1/48/96 kHz, breath HOLD, settled vocal staging, and gain headroom/slew on actual recorded speech. The reference vocal showed max rise/fall about 3.00/12.00 dB/s and peak 0.862 on that fixture.
- Native UI integration passed all DSP scenes and actual user-control quote captures: Peak, Ess, Noise, Auto ON, Auto OFF, quote expiry and resize. See `docs/evidence/vocal-quotes-2.0.3/` (Linux captures).
- The Noise/Ess/Breath improvements from 2.0.2 remain. Both 2.0.2 native Windows runs 40057766709 and 40057912614 passed. Full external comparison, including unfavorable cases, is in `docs/DSP-QUALITY.md`; peer comparison was executed on Linux and does not establish commercial parity.
- Final 2.0.3 native Windows validation **passed** in 5m37s: MSVC build, both CTest suites (64 core + 9 speech checks), native GUI/control-quote tests, pluginval strictness 5 and package guards. [Tested Windows package](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40074931266/artifacts/12552657419). The package includes self-contained A/B speech examples, source notices, quality report and recorded validation.
- Windows test startup stack exhaustion was resolved by heap-allocating large processor fixtures; no assertions were disabled or relaxed. Tests also pass on the final Linux 2.0.3 build.
- GitHub reports outer package artifact SHA-256 `e4df4a851c128ebfb117894243d4390d1ce9cf5e6598829c7dba574fa1d48a8b`. Downloading the final run evidence back into this environment was denied at `productionresultssa2.blob.core.windows.net`; no independent post-download binary/ZIP verification is claimed for this run. The runner itself validates binary hashes before packaging. Native Windows screenshots/logs are the `Windows-validation-evidence` artifact (ID 12553135620).

## 2.0.1 gain and warning update — 2026-10-24

Adds independent ±24 dB Input/Output trims, optional pre-chain stereo-linked Auto Gain, and the requested Thai warning captions on the lawyer banner. Auto Gain defaults off; the original dry bypass still ignores all gains. RMS target is -18 dBFS, bounded correction -24/+12 dB, silence gate -50 dBFS and requested peak headroom -3 dBFS. This is not a limiter and cannot repair clipped recordings.

Native Windows MSVC build, CTest 2/2, **50 core assertions plus 3 speech checks**, native UI integration (12 driven scenes plus stopped transport), pluginval strictness 5 and packaging guards all passed. Local Linux checks also passed. These include gain/delta/bypass/recall/output-overload checks, Auto Gain at 44.1/48/96 kHz, and end-to-end stereo target/balance checks. Native Windows validation for this exact source:

https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40055383919

The Windows job passed in 5m23s. Tested ZIP artifact:
https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40055383919/artifacts/12545221698

GitHub reports artifact SHA-256 `234f1740b2d718e1de3cab24227d5bcd3e67fc52fa00253f492cea67bca4f7dc` (outer artifact archive). Download back into this environment is blocked at `productionresultssa9.blob.core.windows.net`; post-download ZIP/binary checks were therefore not repeated for 2.0.1. The runner itself checked and bound binary hashes before packaging. No installer/real DAW listening claim is made. Local UI images/logs: `docs/evidence/gain-update/`. Windows evidence is the `Windows-validation-evidence` artifact on the run page.

The results below describe the preceding Windows-tested version (2.0.0).

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

The native Windows job completed successfully: MSVC build, both CTest suites, audio/speech evidence, native UI integration, pluginval strictness 5, and packaging guards. The packaged VST3 and Standalone hashes were independently checked against the runner validation metadata after download.

## Native Windows execution

GitHub API connectivity and repository push permissions were verified on 2026-10-23. Source was pushed to the review branch `codex/windows-first-tatree2`; native Windows execution completed successfully:

https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/39935476433

The Windows job passed in approximately six minutes. All 10 UI states plus stopped transport were captured; macOS was intentionally skipped. The resulting test ZIP was downloaded and its archive integrity and validated binary hashes checked.

## Still unverified

Installer behavior on user machines, Windows 10/11 DAWs and devices, real user recordings, signing/public release, and every macOS build.

The original v1 ZIP and JPG remain unchanged. The new code uses different plugin IDs and does not replace v1 DAW sessions.
