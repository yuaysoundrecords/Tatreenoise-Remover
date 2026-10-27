#!/bin/bash
set -euo pipefail
[[ "$(uname -s)" == Darwin && "${CI:-}" == true && "${TATREE_TEST_SYSTEM_INSTALL:-}" == 1 && "${RUNNER_ENVIRONMENT:-}" == github-hosted ]] || {
  echo 'System installation tests require explicit opt-in on a disposable GitHub-hosted Mac runner.' >&2; exit 1;
}
pkg="$1"; stage="$2"; evidence="$3"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
mkdir -p "$evidence"
work=$(mktemp -d "${TMPDIR:-/tmp}/tatree-installer-tests.XXXXXX")
trap 'rm -rf "$work"' EXIT

# Product releases must never silently fall back to unsigned packaging.
if bash "$ROOT/scripts/package-macos-installer.sh" --stage "$stage" --output "$work/should-not-exist" --version 2.1.1 > "$work/release-guard.txt" 2>&1; then
  echo 'FAIL release without Developer ID was accepted' >&2; exit 1
fi
grep -q 'No unsigned fallback' "$work/release-guard.txt"
echo 'PASS release requires signing and notarization credentials'

# Exercise the actual preflight, with a harmless process using a host-like name.
mkdir -p "$work/Logic Pro.app/Contents/MacOS"
cp /bin/sleep "$work/Logic Pro.app/Contents/MacOS/Logic Pro"
"$work/Logic Pro.app/Contents/MacOS/Logic Pro" 30 &
fake_pid=$!
sleep .3
host_blocked=false
if ! bash "$ROOT/macos/installer/preinstall" dummy 0 / > "$work/open-host.txt" 2>&1; then host_blocked=true; fi
kill "$fake_pid" 2>/dev/null || true
wait "$fake_pid" 2>/dev/null || true
[[ "$host_blocked" == true ]]
grep -q 'Close your DAW' "$work/open-host.txt"
echo 'PASS preflight refuses installation while a DAW is running without killing it'

pkgutil --expand-full "$pkg" "$work/expanded"
for relative in 'Library/Audio/Plug-Ins/Components/Tatree Noise Remover 2.component/Contents/MacOS/Tatree Noise Remover 2' 'Library/Audio/Plug-Ins/VST3/Tatree Noise Remover 2.vst3/Contents/MacOS/Tatree Noise Remover 2' 'Applications/Tatree Noise Remover 2.app/Contents/MacOS/Tatree Noise Remover 2'; do
  test -f "$work/expanded/TatreePayload.pkg/Payload/$relative"
done
echo 'PASS installer contains AU, VST3 and Standalone at fixed system paths'
for notice in THIRD_PARTY.md OFL-Anton.txt OFL-NotoSansThai.txt; do
  test -f "$work/expanded/TatreePayload.pkg/Payload/Library/Application Support/Yuay Sound Records/Tatree/Licenses/$notice"
done
echo 'PASS installer includes third-party and embedded-font license notices'

# The prior build validation installs its own AU fixture in the runner home.
# Move only that exact fixture outside the scan path before installation.
user_au="$HOME/Library/Audio/Plug-Ins/Components/Tatree Noise Remover 2.component"
if [[ -d "$user_au" ]]; then
  cmp "$user_au/Contents/MacOS/Tatree Noise Remover 2" "$stage/AU/Tatree Noise Remover 2.component/Contents/MacOS/Tatree Noise Remover 2"
  mv "$user_au" "$work/previous-ci-au.component"
fi

# Verify a fresh native installation before exercising upgrade/backup.
global_au='/Library/Audio/Plug-Ins/Components/Tatree Noise Remover 2.component'
validate_installed_au() {
  local phase="$1"
  local INFO="$global_au/Contents/Info.plist"
  local TYPE SUBTYPE MAKER
  TYPE=$(/usr/libexec/PlistBuddy -c 'Print :AudioComponents:0:type' "$INFO")
  SUBTYPE=$(/usr/libexec/PlistBuddy -c 'Print :AudioComponents:0:subtype' "$INFO")
  MAKER=$(/usr/libexec/PlistBuddy -c 'Print :AudioComponents:0:manufacturer' "$INFO")
killall AudioComponentRegistrar 2>/dev/null || true
for attempt in 1 2 3 4 5; do
  auval -a > "$evidence/installer-$phase-au-list.txt" 2>&1
  if grep -q "$SUBTYPE.*$MAKER" "$evidence/installer-$phase-au-list.txt"; then break; fi
  sleep 2
done
python3 - "$TYPE" "$SUBTYPE" "$MAKER" "$evidence/installer-$phase-auval.txt" <<'PY'
import subprocess,sys
try:
    result=subprocess.run(['auval','-v',*sys.argv[1:4]],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=45)
    output=result.stdout.decode(errors='replace'); code=result.returncode
except subprocess.TimeoutExpired as e:
    output=(e.stdout or b'').decode(errors='replace')+'\nAU validation timed out after 45 seconds.'; code=124
with open(sys.argv[4],'w') as f: f.write(output)
if code:
    # Registration failures need the registrar reason, not only auval's -50.
    try:
        logs=subprocess.run(['/usr/bin/log','show','--info','--debug','--last','3m','--style','compact','--predicate','process == "AudioComponentRegistrar"'],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=10).stdout.decode(errors='replace')
        relevant=[line for line in logs.splitlines() if any(word in line.lower() for word in ['tatree','yuay','ttr2','error','fail','denied','invalid'])]
        if relevant: output+='\nRegistrar diagnostics:\n'+'\n'.join(relevant[-12:])
    except subprocess.TimeoutExpired: output+='\nRegistrar diagnostics timed out.'
    message=output[-3500:].replace('%','%25').replace('\r','%0D').replace('\n','%0A')
    print('::error title=Installed system AU validation failed::'+message)
    print(output)
raise SystemExit(code)
PY
}

sudo /usr/sbin/installer -pkg "$pkg" -target / > "$evidence/installer-fresh-install.txt" 2>&1 || { cat "$evidence/installer-fresh-install.txt"; exit 1; }
validate_installed_au fresh
echo 'PASS fresh system installation passes AU validation before upgrade fixture'

# Prepare a fully sealed prior copy outside scan paths; never expose a
# half-modified bundle to the registrar while constructing the test fixture.
old_au="$work/old-valid.component"
ditto "$stage/AU/Tatree Noise Remover 2.component" "$old_au"
marker="tatree-ci-backup-$$.txt"
mkdir -p "$old_au/Contents/Resources"
echo 'Installer backup fixture' > "$old_au/Contents/Resources/$marker"
codesign --force --sign - "$old_au"
codesign --verify --deep --strict "$old_au"
sudo chown -R root:wheel "$old_au"
sudo mv "$global_au" "$work/freshly-installed.component"
sudo chown -R "$(id -u):$(id -g)" "$work/freshly-installed.component"
sudo mv "$old_au" "$global_au"
sudo /usr/sbin/installer -pkg "$pkg" -target / > "$evidence/installer-install.txt" 2>&1 || { cat "$evidence/installer-install.txt"; exit 1; }
test ! -e "$global_au/Contents/Resources/$marker"
sudo find '/Library/Application Support/Yuay Sound Records/Tatree/Backups' -name "$marker" -print > "$evidence/installer-backup.txt"
test -s "$evidence/installer-backup.txt"
echo 'PASS native Installer replaces old copy and preserves backup'

sources=('AU/Tatree Noise Remover 2.component' 'VST3/Tatree Noise Remover 2.vst3' 'Standalone/Tatree Noise Remover 2.app')
destinations=('/Library/Audio/Plug-Ins/Components/Tatree Noise Remover 2.component' '/Library/Audio/Plug-Ins/VST3/Tatree Noise Remover 2.vst3' '/Applications/Tatree Noise Remover 2.app')
for i in 0 1 2; do
  cmp "$stage/${sources[$i]}/Contents/MacOS/Tatree Noise Remover 2" "${destinations[$i]}/Contents/MacOS/Tatree Noise Remover 2"
  codesign --verify --deep --strict "${destinations[$i]}"
  lipo "${destinations[$i]}/Contents/MacOS/Tatree Noise Remover 2" -verify_arch arm64 x86_64
done
echo 'PASS installed binaries match tested stage and remain signed Universal bundles'
python3 - "${destinations[@]}" <<'PY'
import plistlib,sys
ids=[]
for bundle in sys.argv[1:]:
    with open(bundle+'/Contents/Info.plist','rb') as f: ids.append(plistlib.load(f)['CFBundleIdentifier'])
assert ids==['net.yuaysoundrecords.tatree2','net.yuaysoundrecords.tatree2.vst3','net.yuaysoundrecords.tatree2.standalone'],ids
print('PASS distinct AU/VST3/App bundle IDs; original AU identity retained:',', '.join(ids))
PY

INFO="$global_au/Contents/Info.plist"
TYPE=$(/usr/libexec/PlistBuddy -c 'Print :AudioComponents:0:type' "$INFO")
SUBTYPE=$(/usr/libexec/PlistBuddy -c 'Print :AudioComponents:0:subtype' "$INFO")
MAKER=$(/usr/libexec/PlistBuddy -c 'Print :AudioComponents:0:manufacturer' "$INFO")
NAME=$(/usr/libexec/PlistBuddy -c 'Print :AudioComponents:0:name' "$INFO")
[[ "$NAME" == 'Yuay Sound Records:'* ]]
VENDOR="$stage/VST3/Tatree Noise Remover 2.vst3/Contents/Resources/moduleinfo.json"
python3 - "$VENDOR" <<'PY'
import re,sys
# Steinberg's pinned moduleinfo writer emits JSON5 (with trailing commas).
# Read the factory Vendor field without treating the entire file as strict JSON.
with open(sys.argv[1]) as f: text=f.read()
factory=re.search(r'"Factory Info"\s*:\s*\{\s*"Vendor"\s*:\s*"([^"]*)"',text)
assert factory and factory.group(1)=='Yuay Sound Records', 'Incorrect or missing VST3 factory vendor'
PY
echo 'PASS AU manufacturer name and VST3 vendor are Yuay Sound Records'
validate_installed_au upgrade
echo 'PASS installed system AU passes auval as normal user'
echo 'NOTE: unsigned CI installation is not proof of Gatekeeper approval on a quarantined customer download.'
