# Tatree Noise Remover 2

Windows x64 is the first target for this new native C++17/JUCE development version. It keeps the selected Thai indie cream/cobalt UI, the colour lawyer character and playful Thai tone, with functional controls and audio-driven Motion FX. macOS follows after Windows validation.

**Status:** development in progress. Native Windows x64 build, audio/motion tests, UI integration and pluginval passed; an unsigned test ZIP is ready. See [validation evidence](VALIDATION.md). There is no claim of a completed official Windows release yet.

## Features

- Progressive noise reduction, voice-conscious low-level/breath control, de-esser and 75 Hz low-cut.
- Colour lawyer reactions for noise, de-essing, input hot, breath, removed-signal listening and bypass; filter/control-touch feedback and a motion-off preference.
- Stereo IN/OUT peak meters with peak hold; actual energy-based gain-reduction meter.
- Exact percentage entry, drag/keyboard knobs, delta listen, latency-matched bypass, host automation and session recall.
- No runtime network connection, account or model download. Mono/stereo, 44.1/48/96 kHz checks.

## Windows first

Install Visual Studio 2022 **Desktop development with C++**, Windows SDK, Git and CMake ≥3.22. In PowerShell:

```powershell
./scripts/build-windows.ps1
```

The script builds, tests, captures the native UI, verifies the pinned Windows pluginval download, validates the actual plugin, and packages only matching binaries that passed. [Windows instructions and testing checklist](docs/WINDOWS.md).

GitHub Actions successfully ran the Windows pipeline: [validated Windows build](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/39935476433). Its macOS job is optional and runs only after Windows success.

## Development checks

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 3
ctest --test-dir build -C Release --output-on-failure
```

JUCE 8.0.8 is pinned by commit; a local copy can be selected with `-DJUCE_PATH=/path/to/JUCE`. Debian/Ubuntu development dependencies include ALSA, X11/extensions, Freetype and Fontconfig. WebKit and curl are disabled.

- [Current UI/audio review page](TEST-ME.html) — recorded native UI and audio examples, not a browser audio plugin.
- [Architecture](docs/ARCHITECTURE.md)
- [Visual QA](design-qa.md)
- [Third-party components/licences](THIRD_PARTY.md)

## Original v1.02

The original Windows VST3 distribution, promotional image and its terms remain unchanged in this repository. Original download: https://benjilennon.gumroad.com/l/rsecg . Developer: Yuay Sound Records, Thailand.

Version 2 is a new implementation with new plugin IDs, not recovered v1 source or a patched v1 binary. Listening comparisons with the original and the user's recordings remain necessary.
