# Tatree Noise Remover 2.1.0 — licensing staging

Native audio cleanup plugin by Yuay Sound Records. Cream/cobalt Thai indie UI, colour lawyer and audio-driven Motion FX. Mono/stereo VST3 + Standalone on Windows; Universal AU/VST3/Standalone source build on macOS 11+.

- Noise Reduction, split-band De-esser, duration-gated Breath Reduction, 75 Hz filter and removed-signal Listen.
- Input/Output gain and optional smooth vocal Auto Gain (-18 dBFS active RMS target, breath/silence HOLD).
- No speech banner on startup. Five user-control captions appear for 3 seconds, including De-breath and both Auto Gain ON/OFF clicks.
- Real IN/OUT/GR meters, automation/state recall, fixed 1024-sample latency and original-dry Bypass.

**2.1 status:** licensing implementation and platform validation are in progress. Activation needs the public Gumroad product ID and a real paid-key integration test. Do not sell an unconfigured staging build as unlockable. See [licensing setup](docs/LICENSING.md).

Previous **2.0.4** binaries (no trial or key system): [Windows 2.0.4 tested ZIP](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40083443048/artifacts/12556048528) · [Mac Universal test build](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40083443048/artifacts/12556491264)

Windows and Mac automated validation passed for **2.0.4**; those results do not validate the new licensing changes. Mac builds have ad-hoc test signatures, not Developer ID notarization. For a local Mac build use the source kit and BUILD-MAC.command below.

## Build and test

**Windows:** Visual Studio 2022 Desktop C++, Windows SDK, Git and CMake ≥3.22; run `./scripts/build-windows.ps1` in PowerShell. It builds, tests audio/UI, validates VST3 and packages only matching validated binaries. [Windows guide](docs/WINDOWS.md).

**Mac:** install Apple Command Line Tools and CMake once, then double-click **BUILD-MAC.command**. Default builds Universal Apple Silicon + Intel with AU/VST3/Standalone. [Mac guide](docs/MACOS.md).

[Current validation](VALIDATION.md) · [UI and captions](docs/UI.md) · [Measured DSP comparison and limits](docs/DSP-QUALITY.md) · [Component licences](THIRD_PARTY.md)

Audio processing remains local. Version 2.1 checks Gumroad keys over HTTPS outside the audio callback; a remembered key is verified once per new host process. All instances share a 30-minute trial and expiry fades to latency-matched dry. No audio or project data is uploaded. No model downloads. Test builds are unsigned on Windows / ad-hoc signed on Mac; the owner accepted 2.0.4 in their DAW; Developer ID/notarization and live 2.1 paid-key checks remain separate release steps. Numerical fixtures do not establish perceptual or commercial parity.

Original v1 distribution and old design evidence are preserved in Git history and a verified recovery archive outside the active project; they are not needed to build v2. This source uses new IDs (`Yuay` / `Ttr2`) and does not replace v1 sessions. Original product: https://benjilennon.gumroad.com/l/rsecg .
