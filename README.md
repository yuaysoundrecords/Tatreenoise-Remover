# Tatree Noise Remover 2.1.2 — free

Native audio cleanup plugin by Yuay Sound Records. Cream/cobalt Thai indie UI, colour lawyer and audio-driven Motion FX. Mono/stereo VST3 + Standalone on Windows; Universal AU/VST3/Standalone source build on macOS 11+.

- Noise Reduction, split-band De-esser, duration-gated Breath Reduction, 75 Hz filter and removed-signal Listen.
- Input/Output gain and optional smooth vocal Auto Gain (-18 dBFS active RMS target, breath/silence HOLD).
- No speech banner on startup. Five user-control captions appear for 3 seconds, including De-breath and both Auto Gain ON/OFF clicks.
- Real IN/OUT/GR meters, automation/state recall, fixed 1024-sample latency and original-dry Bypass.

Version **2.1.2** adds the illustrated Ko-fi badge and a bilingual optional Mac AU/VST3 approval helper. Current platform packages must be rebuilt and verified before publication. Older binary download links have been removed to prevent version confusion.

## Free to use

All features are free, with no trial timer, activation key, account or payment requirement. The optional **TIP / KO-FI** button at the top right opens [Yuaylooper on Ko-fi](https://ko-fi.com/yuaylooper) only when clicked. Tipping does not unlock or change any feature.

## Mac installer

The build creates an **UNSIGNED-TEST** native installer and a separate optional approval helper named **1.Mac กดอันนี้ก่อนครับ - Mac Start with This!.command**. Install first, close the DAW, then review the helper before confirming. It removes quarantine only from the exact listed Tatree AU/VST3 bundles; it does not notarize the software or disable Gatekeeper globally. Customer archives contain installation instructions and required notices, without internal reports. [Installer details](docs/MACOS-INSTALLER.md).

## Build and test

**Windows:** Visual Studio 2022 Desktop C++, Windows SDK, Git and CMake ≥3.22; run `./scripts/build-windows.ps1` in PowerShell. It builds, tests audio/UI, validates VST3 and packages only matching validated binaries. [Windows guide](docs/WINDOWS.md).

**Mac:** install Apple Command Line Tools and CMake once, then double-click **BUILD-MAC.command**. Default builds Universal Apple Silicon + Intel with AU/VST3/Standalone. [Mac guide](docs/MACOS.md).

[Current validation](VALIDATION.md) · [UI and captions](docs/UI.md) · [Measured DSP comparison and limits](docs/DSP-QUALITY.md) · [Component licences](THIRD_PARTY.md)

Audio processing works offline, with no activation checks or model downloads. Clicking TIP / KO-FI opens the external donation page in your browser; nothing opens automatically. Test builds are unsigned on Windows / ad-hoc signed on Mac; Developer ID/notarization remains separate. Numerical fixtures do not establish perceptual or commercial parity.

Original v1 distribution and old design evidence are preserved in Git history and a verified recovery archive outside the active project; they are not needed to build v2. This source uses new IDs (`Yuay` / `Ttr2`) and does not replace v1 sessions. Original product: https://benjilennon.gumroad.com/l/rsecg .
