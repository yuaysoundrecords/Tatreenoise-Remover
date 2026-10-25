# Current validation — 2.0.4

Latest changes: no startup/idle speech banner; new Breath control caption; compact source tree; one-click Universal Mac build helper. Native Windows and Mac CI validation **passed** on [run 40083443048](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40083443048), source commit `f1bd09ef65dbb980a62f8d9b57d6fd6ff2f5f798`.

- [Windows x64 tested package](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40083443048/artifacts/12556048528): MSVC build, 64 core + 9 recorded-speech checks, native UI/startup/De-breath quotes, pluginval strictness 5, package guards and hash-bound packaging all passed.
- [Mac Universal test build](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40083443048/artifacts/12556491264): AU/VST3/Standalone build, both architectures verified by lipo, CTest 2/2, native UI, ad-hoc signature verification, microphone usage metadata and `auval` all passed. Native tests run on the hosted Mac architecture; this is not a notarized Developer ID release.

- Local CTest 2/2 suites passed (64 core assertions + 9 additional recorded-speech checks; speech suite repeats the core tests).
- Local native UI passes startup silence, all five control captions, Auto Gain ON/OFF, quote expiry, actual DSP states and resizing. Captures/logs: `docs/evidence/current/` (Linux).
- Windows packaging guards pass: missing metadata rejected, paths with spaces supported, changed binary hashes rejected.
- The prior 2.0.3 Windows native build/audio/UI/pluginval passed on run 40074931266. Large DSP test fixtures remain heap-allocated to avoid Windows startup stack exhaustion.
- Noise benchmark and full unfavorable cases: [DSP-QUALITY.md](docs/DSP-QUALITY.md), raw data/source pins in `docs/evidence/dsp-*.json`. Noise/Ess/Breath/Auto Gain DSP is unchanged from the validated 2.0.3 revision; the latest changes affect captions/build portability/packaging.

Gumroad upload was not performed: no authenticated product-file editing tool is available. Downloading the final Windows artifact back into this cloud was denied at its Azure storage hostname; no post-download local binary verification is claimed. CI itself binds validated binary hashes before packaging. Outer artifact digests/IDs are recorded in `docs/evidence/current/platform-run.json`. On 2026-10-25 the owner reported successful testing in their DAW and accepted the result. DAW name, OS, architecture and package version were not specified. Broader DAW/device coverage, real Thai voice/singing coverage, Developer ID/Authenticode and Mac notarization remain unverified. Mac test bundles use ad-hoc signatures only.

Obsolete binaries, historical screenshots/videos and generated builds are excluded from the active source tree. Originals/old evidence were verified into a recovery archive outside the project before removal; Git history is retained.
