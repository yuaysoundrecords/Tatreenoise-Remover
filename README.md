# Tatree Noise Remover 2.1.1 — free

Native audio cleanup plugin by Yuay Sound Records. Cream/cobalt Thai indie UI, colour lawyer and audio-driven Motion FX. Mono/stereo VST3 + Standalone on Windows; Universal AU/VST3/Standalone source build on macOS 11+.

- Noise Reduction, split-band De-esser, duration-gated Breath Reduction, 75 Hz filter and removed-signal Listen.
- Input/Output gain and optional smooth vocal Auto Gain (-18 dBFS active RMS target, breath/silence HOLD).
- No speech banner on startup. Five user-control captions appear for 3 seconds, including De-breath and both Auto Gain ON/OFF clicks.
- Real IN/OUT/GR meters, automation/state recall, fixed 1024-sample latency and original-dry Bypass.

[Windows 2.1.0 free ZIP](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40320381346/artifacts/12648308649) · [Mac Universal test build](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40320381346/artifacts/12648702859)

Version **2.1.0 free** passed native Windows and Mac validation on [run 40320381346](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40320381346). Mac builds have ad-hoc test signatures, not Developer ID notarization. For a local Mac build use the source kit and BUILD-MAC.command below.

## Free to use

All features are free, with no trial timer, activation key, account or payment requirement. The optional **TIP / KO-FI** button at the top right opens [Yuaylooper on Ko-fi](https://ko-fi.com/yuaylooper) only when clicked. Tipping does not unlock or change any feature.

## Mac installer

Native `.pkg` setup is being added with host-close checks, backups, fixed AU/VST3/App paths and vendor verification. The owner has no Apple Developer membership, so the test installer is unsigned and **does not establish Gatekeeper approval**. [Installer and notarization details](docs/MACOS-INSTALLER.md).

## Build and test

**Windows:** Visual Studio 2022 Desktop C++, Windows SDK, Git and CMake ≥3.22; run `./scripts/build-windows.ps1` in PowerShell. It builds, tests audio/UI, validates VST3 and packages only matching validated binaries. [Windows guide](docs/WINDOWS.md).

**Mac:** install Apple Command Line Tools and CMake once, then double-click **BUILD-MAC.command**. Default builds Universal Apple Silicon + Intel with AU/VST3/Standalone. [Mac guide](docs/MACOS.md).

[Current validation](VALIDATION.md) · [UI and captions](docs/UI.md) · [Measured DSP comparison and limits](docs/DSP-QUALITY.md) · [Component licences](THIRD_PARTY.md)

Audio processing works offline, with no activation checks or model downloads. Clicking TIP / KO-FI opens the external donation page in your browser; nothing opens automatically. Test builds are unsigned on Windows / ad-hoc signed on Mac; the owner accepted 2.0.4 in their DAW; Developer ID/notarization remains separate. Numerical fixtures do not establish perceptual or commercial parity.

Original v1 distribution and old design evidence are preserved in Git history and a verified recovery archive outside the active project; they are not needed to build v2. This source uses new IDs (`Yuay` / `Ttr2`) and does not replace v1 sessions. Original product: https://benjilennon.gumroad.com/l/rsecg .
