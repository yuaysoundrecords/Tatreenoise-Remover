# Current validation — 2.1.2

Native Windows and Mac builds passed on [run 40507885353](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40507885353), source `8a7deb1c85ba8cfb038a5cb0bdc8e320cf6c408f`.

- Windows: packaging guards, audio/speech tests, native UI/control/caption/resize checks and pluginval passed.
- Mac: Universal AU/VST3/Standalone, audio/speech and native UI checks, signatures/architectures, fresh and upgrade installer tests, vendor metadata and installed AU validation passed.
- The bilingual helper was run against installed Mac bundles with cancellation input. It discovered/verified the targets and cancelled without modifying them. Its approval/quarantine-removal path on browser-downloaded current bundles has not been tested by this environment.
- Local checks: shell syntax, UTF-8 archive filenames, executable permissions and ZIP integrity passed. Customer platform packages exclude development logs and internal reports.
- DSP/processor code remains unchanged from the previous release. The UI change is the illustrated optional Ko-fi badge; the remaining changes are packaging/instructions.
- GitHub reported successful evidence/package uploads. This environment could not download the final evidence ZIPs because their Azure storage hostname is denied by network policy. Final screenshots and platform archive contents have not been visually/re-extraction checked locally here. Artifact metadata and outer digests are recorded in `docs/evidence/current/platform-run.json`.
- Mac binaries are ad-hoc signed, not Developer ID signed or notarized. A runner installation/cancellation test is not a customer Gatekeeper or full DAW compatibility test.
- Older GitHub platform binary artifacts have been removed; test evidence is separate. Superseded local distribution files and old screenshots were removed from active folders after verified recovery archiving.
- Gumroad/KVR files and screenshots have not been updated by this environment; no authenticated editing tool is available.
