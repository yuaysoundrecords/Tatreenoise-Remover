# Current validation — 2.1.3

Windows job [122348352586](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40549297617/job/122348352586) passed. Mac job on [run 40552716233](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40552716233) passed. The Windows code and payload inputs are identical across the two source commits; the subsequent changes only corrected Mac ZIP packaging and selected a supported Sequoia CI runner.

- Windows: audio/speech, native UI and pluginval checks passed. The customer ZIP was extracted and its Setup bytes verified before testing fresh installation, upgrade/backup, installed binary hashes, required notices and uninstallation. Backups survived uninstall.
- Mac Sequoia: Universal AU/VST3/Standalone, audio/speech/UI, signatures/architectures, fresh and upgrade installer tests, vendor metadata, installed AU validation and helper cancellation passed. Customer ZIP root entries, payload hashes, UTF-8 helper filename and executable permissions passed.
- Local shell/PowerShell syntax and archive-format fixture checks passed. The fixture was labelled as non-native test data and was not distributed.
- No DSP, plug-in UI or activation changes were made for this packaging update. Customer archives exclude development logs, internal reports and personal notes.
- Mac binaries remain ad-hoc signed and not Apple-notarized; Windows Setup is not Authenticode-signed. CI installation and helper cancellation do not establish browser Gatekeeper approval, quarantine removal or full DAW compatibility on customer Macs.
- Platform artifact metadata and outer ZIP digests are in docs/evidence/current/platform-run.json. Final packages could not be downloaded back into this cloud due to denied Azure storage domains; native CI checked the actual customer ZIPs before upload.
- Only the current Windows/Mac Setup artifacts remain in the downloadable platform set. Recovery archives and test evidence are separate.
- Gumroad content/files are not updated by this environment. The Gumroad domains are denied by network policy and no authenticated editing tool is available. No claim of inspecting the live Prued FX layout is made.
