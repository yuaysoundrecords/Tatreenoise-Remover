# Current validation — 2.0.4

Latest changes: no startup/idle speech banner; new Breath control caption; compact source tree; one-click Universal Mac build helper. Native Windows/Mac CI validation for this exact revision is pending until recorded below.

- Local CTest 2/2 suites passed (64 core assertions + 9 additional recorded-speech checks; speech suite repeats the core tests).
- Local native UI passes startup silence, all five control captions, Auto Gain ON/OFF, quote expiry, actual DSP states and resizing. Captures/logs: `docs/evidence/current/` (Linux).
- Windows packaging guards pass: missing metadata rejected, paths with spaces supported, changed binary hashes rejected.
- The prior 2.0.3 Windows native build/audio/UI/pluginval passed on run 40074931266. Large DSP test fixtures remain heap-allocated to avoid Windows startup stack exhaustion.
- Noise benchmark and full unfavorable cases: [DSP-QUALITY.md](docs/DSP-QUALITY.md), raw data/source pins in `docs/evidence/dsp-*.json`. Noise/Ess/Breath/Auto Gain DSP is unchanged from the validated 2.0.3 revision; the latest changes affect captions/build portability/packaging.

No post-download local binary verification or Gumroad upload is claimed until actually performed. Real Thai voice, singing, user DAWs/devices, Developer ID/Authenticode and Mac notarization remain unverified. Mac test bundles use ad-hoc signatures only.

Obsolete binaries, historical screenshots/videos and generated builds are excluded from the active source tree. Originals/old evidence were verified into a recovery archive outside the project before removal; Git history is retained.
