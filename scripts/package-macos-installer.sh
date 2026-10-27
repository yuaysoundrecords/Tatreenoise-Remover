#!/bin/bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
[[ "$(uname -s)" == Darwin ]] || { echo 'Build Mac installers on macOS.' >&2; exit 1; }
stage= output= version= test_unsigned=false
while [[ $# -gt 0 ]]; do
  case "$1" in
    --stage) stage="$2"; shift 2;;
    --output) output="$2"; shift 2;;
    --version) version="$2"; shift 2;;
    --test-unsigned) test_unsigned=true; shift;;
    *) echo "Unknown argument: $1" >&2; exit 1;;
  esac
done
[[ -d "$stage" && -n "$output" && "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || {
  echo 'Usage: package-macos-installer.sh --stage PATH --output PATH --version X.Y.Z [--test-unsigned]' >&2; exit 1;
}
app_identity="${TATREE_DEVELOPER_ID_APPLICATION:-}"
installer_identity="${TATREE_DEVELOPER_ID_INSTALLER:-}"
profile="${TATREE_NOTARY_PROFILE:-}"
if [[ "$test_unsigned" == true ]]; then
  [[ -z "$app_identity$installer_identity$profile" ]] || { echo 'Do not mix unsigned test mode with signing credentials.' >&2; exit 1; }
else
  [[ "$app_identity" == 'Developer ID Application: '* && "$installer_identity" == 'Developer ID Installer: '* && -n "$profile" ]] || {
    echo 'Release requires Developer ID Application, Developer ID Installer and a notarytool keychain profile. No unsigned fallback.' >&2; exit 1;
  }
fi
mkdir -p "$output"
output="$(cd "$output" && pwd)"
work=$(mktemp -d "${TMPDIR:-/tmp}/tatree-installer.XXXXXX")
trap 'rm -rf "$work"' EXIT
payload="$work/payload"
mkdir -p "$payload/Library/Audio/Plug-Ins/Components" "$payload/Library/Audio/Plug-Ins/VST3" "$payload/Applications" "$work/scripts"
notices="$payload/Library/Application Support/Yuay Sound Records/Tatree/Licenses"
mkdir -p "$notices"
cp "$ROOT/THIRD_PARTY.md" "$ROOT/assets/OFL-Anton.txt" "$ROOT/assets/OFL-NotoSansThai.txt" "$notices/"
sources=('AU/Tatree Noise Remover 2.component' 'VST3/Tatree Noise Remover 2.vst3' 'Standalone/Tatree Noise Remover 2.app')
destinations=('Library/Audio/Plug-Ins/Components/Tatree Noise Remover 2.component' 'Library/Audio/Plug-Ins/VST3/Tatree Noise Remover 2.vst3' 'Applications/Tatree Noise Remover 2.app')
for i in 0 1 2; do
  source="$stage/${sources[$i]}"
  [[ -d "$source" && ! -L "$source" ]] || { echo "Missing or invalid bundle: $source" >&2; exit 1; }
  binary="$source/Contents/MacOS/Tatree Noise Remover 2"
  [[ -f "$binary" ]] || { echo "Missing binary: $binary" >&2; exit 1; }
  codesign --verify --deep --strict "$source"
  bundle="$payload/${destinations[$i]}"
  ditto "$source" "$bundle"
  if [[ "$test_unsigned" != true ]]; then
    if [[ $i == 2 ]]; then
      codesign --force --options runtime --timestamp --sign "$app_identity" --entitlements "$ROOT/macos/standalone.entitlements.plist" "$bundle"
    else
      codesign --force --options runtime --timestamp --sign "$app_identity" "$bundle"
    fi
    codesign --verify --deep --strict "$bundle"
  fi
done
# Do not ship build-machine ACLs or private directory modes to system loaders.
# This changes only the temporary payload, never Gatekeeper/quarantine policy.
chmod -RN "$payload"
find "$payload" -type d -exec chmod 755 {} +
find "$payload" -type f -exec chmod 644 {} +
for relative in "${destinations[@]}"; do
  chmod 755 "$payload/$relative/Contents/MacOS/Tatree Noise Remover 2"
  codesign --verify --deep --strict "$payload/$relative"
done
cp "$ROOT/macos/installer/preinstall" "$ROOT/macos/installer/postinstall" "$work/scripts/"
chmod 755 "$work/scripts/"*
pkgbuild --analyze --root "$payload" "$work/components.plist"
python3 - "$work/components.plist" <<'PY'
import plistlib,sys
p=sys.argv[1]
with open(p,'rb') as f: components=plistlib.load(f)
for c in components:
    c['BundleIsRelocatable']=False
    c['BundleOverwriteAction']='upgrade'
    c['BundleIsVersionChecked']=False
with open(p,'wb') as f: plistlib.dump(components,f)
PY
pkgbuild --root "$payload" --component-plist "$work/components.plist" --scripts "$work/scripts" \
  --ownership recommended --install-location / --identifier net.yuaysoundrecords.tatree2.installer --version "$version" "$work/TatreePayload.pkg"
cat > "$work/Distribution.xml" <<EOF
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="2">
  <title>Tatree Noise Remover $version</title>
  <welcome file="welcome.html" mime-type="text/html"/>
  <conclusion file="conclusion.html" mime-type="text/html"/>
  <options customize="never" require-scripts="true" rootVolumeOnly="true" hostArchitectures="arm64,x86_64"/>
  <volume-check><allowed-os-versions><os-version min="11.0"/></allowed-os-versions></volume-check>
  <choices-outline><line choice="tatree"/></choices-outline>
  <choice id="tatree" visible="false"><pkg-ref id="net.yuaysoundrecords.tatree2.installer"/></choice>
  <pkg-ref id="net.yuaysoundrecords.tatree2.installer" version="$version" onConclusion="none">TatreePayload.pkg</pkg-ref>
</installer-gui-script>
EOF
if [[ "$test_unsigned" == true ]]; then
  filename="Tatree-$version-macOS-Setup-UNSIGNED-TEST.pkg"
  productbuild --distribution "$work/Distribution.xml" --resources "$ROOT/macos/installer" --package-path "$work" "$output/$filename"
  echo 'UNSIGNED TEST ONLY: not Apple-notarized. Installing this does not guarantee Gatekeeper approval of the plug-ins.' >&2
else
  filename="Tatree-$version-macOS-Setup.pkg"
  pending_pkg="$work/Tatree-pending-notarization.pkg"
  productbuild --sign "$installer_identity" --timestamp --distribution "$work/Distribution.xml" \
    --resources "$ROOT/macos/installer" --package-path "$work" "$pending_pkg"
  xcrun notarytool submit "$pending_pkg" --keychain-profile "$profile" --wait --timeout 25m --output-format json > "$output/notary-result.json"
  python3 - "$output/notary-result.json" <<'PY'
import json,sys
with open(sys.argv[1]) as f: r=json.load(f)
if r.get('status')!='Accepted': raise SystemExit('Apple notarization was not accepted. Do not publish this package.')
PY
  xcrun stapler staple "$pending_pkg"
  xcrun stapler validate "$pending_pkg"
  pkgutil --check-signature "$pending_pkg"
  spctl --assess --type install --verbose=2 "$pending_pkg"
  mv "$pending_pkg" "$output/$filename"
fi
shasum -a 256 "$output/$filename" > "$output/$filename.sha256"
echo "Installer: $output/$filename"
