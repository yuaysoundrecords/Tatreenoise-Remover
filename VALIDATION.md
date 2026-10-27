# Current source — 2.1.2

The illustrated Ko-fi badge and optional bilingual AU/VST3 approval helper are implemented. Shell syntax, UTF-8 ZIP entries, executable permissions and archive integrity were checked locally. Current platform CI results are not yet confirmed; GitHub authentication began returning HTTP 401 during this update. The helper has not been tested here with browser-quarantined current bundles on a customer Mac. Historical test evidence below applies only to its recorded versions.

## Historical validation — 2.1.1 free / Mac setup

Windows and Mac native validation **passed** on [run 40474891230](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40474891230), source `c1d4e81d4f0bdc8b2b91cf886616b156b47fea5a`.

- Native Mac `.pkg` tests passed fresh installation and upgrade/backup, open-host refusal, installed binary matching, signatures/architectures, font/component notices, distinct bundle identities and AU validation as the normal user after both installs.
- AU manufacturer and VST3 factory vendor checked as **Yuay Sound Records**. The original AU component tuple and AU bundle ID are preserved; App/VST3 bundle identifiers are separate to avoid registration collisions.
- Audio/speech suites, native UI/caption/resize tests, Windows packaging guards and Windows pluginval passed. No audio-processing source changed from the accepted free version.
- [Mac Setup — UNSIGNED TEST](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40474891230/artifacts/12721595164). This is **not Developer ID signed or Apple-notarized**. CLI installation on a fresh CI machine does not prove Gatekeeper approval after a browser download. No claim that every customer DAW/Gatekeeper hang is resolved.
- Complete artifact IDs, sizes and outer ZIP digests: `docs/evidence/current/platform-run.json`. Final platform archives have not been downloaded and reverified in this cloud; validated binary binding is checked in CI.

## Prior 2.1.0 free validation

Native Windows and Mac validation **passed** on [run 40320381346](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40320381346), source commit `b3ab9d57b4c0fffb4e853e345d8a3ce8fdfd4b6a`.

- [Windows x64 free package](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40320381346/artifacts/12648308649): MSVC build, audio/speech tests, native UI with optional Ko-fi tip, pluginval strictness 5, package guards and hash-bound packaging passed.
- [Mac Universal free package](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40320381346/artifacts/12648702859): AU/VST3/Standalone, both architectures, audio/speech tests, native UI, ad-hoc signatures and `auval` passed. Not Developer ID notarized.
- Local CTest 2/2 and all native audio/UI/caption/resize scenarios passed. Current screenshots/logs are in `docs/evidence/current/`.
- No trial timer, licence-key verification, activation UI, payment gating or startup network requests remain. The only product UI change from accepted 2.0.4 is the optional top-right **TIP / KO-FI** button, opening `https://ko-fi.com/yuaylooper` only on a user click. Automated tests do not open the external browser; the donation profile URL has not been independently checked.
- Processor, DSP, Motion, speech-caption logic and audio tests match accepted commit `9fe5511` byte-for-byte. Hash evidence: `docs/evidence/current/source-scope.json`.
- Run IDs, artifact sizes and outer ZIP digests: `docs/evidence/current/platform-run.json`. This cloud has not downloaded and rechecked the final platform ZIPs; package binary binding is verified inside CI.
- Gumroad price/settings/files have **not** been changed by this environment; the free listing handoff is in `docs/GUMROAD.md`.

## Historical 2.0.4 validation

Latest changes: no startup/idle speech banner; new Breath control caption; compact source tree; one-click Universal Mac build helper. Native Windows and Mac CI validation **passed** on [run 40083443048](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40083443048), source commit `f1bd09ef65dbb980a62f8d9b57d6fd6ff2f5f798`.

- [Windows x64 tested package](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40083443048/artifacts/12556048528): MSVC build, 64 core + 9 recorded-speech checks, native UI/startup/De-breath quotes, pluginval strictness 5, package guards and hash-bound packaging all passed.
- [Mac Universal test build](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40083443048/artifacts/12556491264): AU/VST3/Standalone build, both architectures verified by lipo, CTest 2/2, native UI, ad-hoc signature verification, microphone usage metadata and `auval` all passed. Native tests run on the hosted Mac architecture; this is not a notarized Developer ID release.

- Local CTest 2/2 suites passed (64 core assertions + 9 additional recorded-speech checks; speech suite repeats the core tests).
- Local native UI passes startup silence, all five control captions, Auto Gain ON/OFF, quote expiry, actual DSP states and resizing. Captures/logs: `docs/evidence/current/` (Linux).
- Windows packaging guards pass: missing metadata rejected, paths with spaces supported, changed binary hashes rejected.
- The prior 2.0.3 Windows native build/audio/UI/pluginval passed on run 40074931266. Large DSP test fixtures remain heap-allocated to avoid Windows startup stack exhaustion.
- Noise benchmark and full unfavorable cases: [DSP-QUALITY.md](docs/DSP-QUALITY.md), raw data/source pins in `docs/evidence/dsp-*.json`. Noise/Ess/Breath/Auto Gain DSP is unchanged from the validated 2.0.3 revision; the latest changes affect captions/build portability/packaging.

Gumroad upload was not performed: no authenticated product-file editing tool is available. Downloading the final Windows artifact back into this cloud was denied at its Azure storage hostname; no post-download local binary verification is claimed. CI itself binds validated binary hashes before packaging. Outer artifact digests/IDs are recorded in `docs/evidence/current/platform-run.json`. Broader DAW/device coverage, real Thai voice/singing coverage, Developer ID/Authenticode and Mac notarization remain unverified. Mac test bundles use ad-hoc signatures only.

Obsolete binaries, historical screenshots/videos and generated builds are excluded from the active source tree. Originals/old evidence were verified into a recovery archive outside the project before removal; Git history is retained.
