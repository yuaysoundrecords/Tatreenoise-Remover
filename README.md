# Tatree Noise Remover 2.0.4

Native audio cleanup plugin by Yuay Sound Records. Cream/cobalt Thai indie UI, colour lawyer and audio-driven Motion FX. Mono/stereo VST3 + Standalone on Windows; Universal AU/VST3/Standalone source build on macOS 11+.

- Noise Reduction, split-band De-esser, duration-gated Breath Reduction, 75 Hz filter and removed-signal Listen.
- Input/Output gain and optional smooth vocal Auto Gain (-18 dBFS active RMS target, breath/silence HOLD).
- No speech banner on startup. Five user-control captions appear for 3 seconds, including De-breath and both Auto Gain ON/OFF clicks.
- Real IN/OUT/GR meters, automation/state recall, fixed 1024-sample latency and original-dry Bypass.

## Build and test

**Windows:** Visual Studio 2022 Desktop C++, Windows SDK, Git and CMake ≥3.22; run `./scripts/build-windows.ps1` in PowerShell. It builds, tests audio/UI, validates VST3 and packages only matching validated binaries. [Windows guide](docs/WINDOWS.md).

**Mac:** install Apple Command Line Tools and CMake once, then double-click **BUILD-MAC.command**. Default builds Universal Apple Silicon + Intel with AU/VST3/Standalone. [Mac guide](docs/MACOS.md).

[Current validation](VALIDATION.md) · [UI and captions](docs/UI.md) · [Measured DSP comparison and limits](docs/DSP-QUALITY.md) · [Component licences](THIRD_PARTY.md)

No runtime network, browser or model download. Test builds are unsigned on Windows / ad-hoc signed on Mac; real DAW listening, Developer ID/notarization and commercial-release licensing remain separate checks. Numerical fixtures do not establish perceptual or commercial parity.

Original v1 distribution and old design evidence are preserved in Git history and a verified recovery archive outside the active project; they are not needed to build v2. This source uses new IDs (`Yuay` / `Ttr2`) and does not replace v1 sessions. Original product: https://benjilennon.gumroad.com/l/rsecg .
