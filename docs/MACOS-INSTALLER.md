# Mac setup and Gatekeeper

Company shown to the host: **Yuay Sound Records**. AU manufacturer and VST3 vendor are checked in the native Mac installer tests. This is product metadata, not a claim that Apple has verified the publisher of an unsigned package.

## Install with Setup.pkg

1. Quit every DAW and the Tatree standalone app. Save projects first; the installer never closes hosts for you.
2. If Tatree was previously copied into `~/Library/Audio/Plug-Ins/Components` or `~/Library/Audio/Plug-Ins/VST3`, move those Tatree bundles to a backup folder **outside** the plug-in scan paths. Do not remove unrelated plug-ins. Setup stops if it detects a conflicting copy in the logged-in user's folders.
3. Open the Setup `.pkg` and follow macOS Installer. It requires an administrator and installs for all users:
   - AU: `/Library/Audio/Plug-Ins/Components/Tatree Noise Remover 2.component`
   - VST3: `/Library/Audio/Plug-Ins/VST3/Tatree Noise Remover 2.vst3`
   - Standalone: `/Applications/Tatree Noise Remover 2.app`
4. Existing system copies are backed up under `/Library/Application Support/Yuay Sound Records/Tatree/Backups/`. The installer verifies code signatures without executing the plug-in as root. Open the DAW as your regular user after installation, then rescan.

ปลั๊กอินยังใช้ฟรี ไม่จำกัดเวลา ไม่ต้องใส่คีย์ การติดตั้งไม่เปลี่ยนพารามิเตอร์หรือโปรเจกต์เก่า

## What this fixes, and what it cannot promise

The original ZIP contains ad-hoc signed binaries. A `.pkg` makes installation consistent and keeps the install/approval step outside the DAW, but it **does not itself notarize software or grant Gatekeeper exceptions to every plug-in**. A successful CI installation on a non-quarantined runner is not proof that a browser-downloaded package will be approved on a customer's Mac.

The owner currently has no Apple Developer membership. Therefore the available setup is explicitly named **UNSIGNED-TEST**. If macOS blocks it, review only that trusted package in **System Settings → Privacy & Security**. Do not disable Gatekeeper globally. Even after allowing that installer, macOS may still block a bundled binary; use a properly notarized release or build from source locally if that happens. No script removes quarantine or changes Gatekeeper policy.

If the DAW has already frozen, close it safely before installation. Changing the installer cannot diagnose every DAW hang; a new crash report/scan log is needed if a notarized build still hangs.

## Build the unsigned test setup

`BUILD-MAC.command` builds and validates the Universal bundles and creates both the ZIP and a native installer in `build-macos/installer/`. The installer is tested on a fresh Mac CI runner: payload paths, refusal with a running host, backup/upgrade, matching installed binaries, code signatures, architectures, AU validation and vendor metadata.

No permission change or automatic installation occurs on a developer's Mac when running the build script. System installation tests run only on a disposable `CI=true` Mac runner.

## Developer ID release (when membership/certificates are available)

The distribution needs two certificate identities in the local login keychain: **Developer ID Application** (signing the bundles) and **Developer ID Installer** (signing the `.pkg`), plus a `notarytool` keychain profile. Never paste certificate private keys or passwords into chat or commit them.

After building:

```sh
export TATREE_DEVELOPER_ID_APPLICATION='Developer ID Application: YOUR LEGAL NAME (TEAMID)'
export TATREE_DEVELOPER_ID_INSTALLER='Developer ID Installer: YOUR LEGAL NAME (TEAMID)'
export TATREE_NOTARY_PROFILE='tatree-notary'
bash scripts/package-macos-installer.sh \
  --stage build-macos/package/Tatree-2-macOS \
  --output build-macos/notarized-installer --version 2.1.1
```

Configure `tatree-notary` using Apple's `xcrun notarytool store-credentials` locally. The release script signs the payload with hardened runtime, signs the installer, submits to Apple, requires **Accepted**, staples and validates the ticket, then runs `pkgutil --check-signature` and `spctl --assess --type install`. It fails if credentials are missing; there is no silent unsigned fallback. If notarization fails, do not upload the intermediate package. Apple determines the verified publisher name from the certificate, so an individual account may show a legal personal name even though the plug-in vendor remains Yuay Sound Records.

Test the final notarized `.pkg` after an actual browser download on a separate Mac and open it in supported DAWs before describing the Gatekeeper issue as fixed. That final browser/Gatekeeper test and Apple notarization remain pending.
