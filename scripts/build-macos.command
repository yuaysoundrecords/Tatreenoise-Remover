#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/.."
if [[ "$(uname -s)" != Darwin ]]; then
  echo 'Build this source kit on a Mac. Windows: scripts/build-windows.ps1' >&2; exit 1
fi
if ! xcode-select -p >/dev/null 2>&1; then
  echo 'Run xcode-select --install once, complete installation, then double-click this file again.' >&2; exit 1
fi
CMAKE_BIN="$(command -v cmake || true)"
if [[ -z "$CMAKE_BIN" ]]; then
  for candidate in /opt/homebrew/bin/cmake /usr/local/bin/cmake /Applications/CMake.app/Contents/bin/cmake; do
    if [[ -x "$candidate" ]]; then CMAKE_BIN="$candidate"; break; fi
  done
fi
if [[ -z "$CMAKE_BIN" ]]; then
  echo 'Install CMake from cmake.org (or brew install cmake), then double-click this file again.' >&2; exit 1
fi
BUILD_DIR="${TATREE_MAC_BUILD_DIR:-build-macos}"
case "$BUILD_DIR" in /*) ;; *) BUILD_DIR="$PWD/$BUILD_DIR" ;; esac
ARCHS="${TATREE_MAC_ARCHS:-arm64;x86_64}"
"$CMAKE_BIN" -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release "-DCMAKE_OSX_ARCHITECTURES=$ARCHS" -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0
"$CMAKE_BIN" --build "$BUILD_DIR" --config Release --parallel "${TATREE_BUILD_JOBS:-3}"
"$(dirname "$CMAKE_BIN")/ctest" --test-dir "$BUILD_DIR" -C Release --output-on-failure
mkdir -p "$BUILD_DIR/validation"
if ! "$BUILD_DIR/TatreeRender_artefacts/Release/TatreeRender.app/Contents/MacOS/TatreeRender" "$BUILD_DIR/validation/ui" > "$BUILD_DIR/validation/native-ui.txt" 2>&1; then
  cat "$BUILD_DIR/validation/native-ui.txt"; exit 1
fi
for name in startup quote-peak quote-de-esser quote-noise quote-breath quote-auto-on quote-auto-off quote-expired; do
  test -f "$BUILD_DIR/validation/ui/$name.png" || { echo "Missing UI evidence: $name" >&2; exit 1; }
done
mkdir -p "$BUILD_DIR/package"
STAGE="$(mktemp -d "$BUILD_DIR/package/Tatree-2-macOS.XXXXXX")"
mkdir -p "$STAGE/VST3" "$STAGE/AU" "$STAGE/Standalone"
for format in VST3 AU Standalone; do
  ditto "$BUILD_DIR/Tatree_artefacts/Release/$format" "$STAGE/$format"
done
for bundle in "$STAGE/VST3/Tatree Noise Remover 2.vst3" "$STAGE/AU/Tatree Noise Remover 2.component" "$STAGE/Standalone/Tatree Noise Remover 2.app"; do
  codesign --force --deep --sign - "$bundle"
  codesign --verify --deep --strict "$bundle"
  if [[ "$ARCHS" == 'arm64;x86_64' ]]; then
    binary="$bundle/Contents/MacOS/Tatree Noise Remover 2"
    lipo "$binary" -verify_arch arm64 x86_64
  fi
done
plutil -extract NSMicrophoneUsageDescription raw -o - "$STAGE/Standalone/Tatree Noise Remover 2.app/Contents/Info.plist"
if [[ "${CI:-}" == true && "${TATREE_TEST_SYSTEM_INSTALL:-}" != 1 ]]; then
  mkdir -p "$HOME/Library/Audio/Plug-Ins/Components"
  ditto "$STAGE/AU/Tatree Noise Remover 2.component" "$HOME/Library/Audio/Plug-Ins/Components/Tatree Noise Remover 2.component"
  # A fresh hosted Mac may have started its registrar before this folder existed.
  # Refresh that runner-only cache, then wait briefly for discovery.
  killall AudioComponentRegistrar 2>/dev/null || true
  INFO="$STAGE/AU/Tatree Noise Remover 2.component/Contents/Info.plist"
  AU_TYPE=$(/usr/libexec/PlistBuddy -c 'Print :AudioComponents:0:type' "$INFO")
  AU_SUBTYPE=$(/usr/libexec/PlistBuddy -c 'Print :AudioComponents:0:subtype' "$INFO")
  AU_MAKER=$(/usr/libexec/PlistBuddy -c 'Print :AudioComponents:0:manufacturer' "$INFO")
  for attempt in 1 2 3 4 5; do
    auval -a > "$BUILD_DIR/validation/au-list.txt" 2>&1
    if grep -q "$AU_SUBTYPE.*$AU_MAKER" "$BUILD_DIR/validation/au-list.txt"; then break; fi
    sleep 2
  done
  echo "Validating registered AU: $AU_TYPE / $AU_SUBTYPE / $AU_MAKER"
  if ! auval -v "$AU_TYPE" "$AU_SUBTYPE" "$AU_MAKER" > "$BUILD_DIR/validation/auval.txt" 2>&1; then
    cat "$BUILD_DIR/validation/au-list.txt" "$BUILD_DIR/validation/auval.txt"; exit 1
  fi
fi
cp "scripts/1.Mac กดอันนี้ก่อนครับ - Mac Start with This!.command" "$STAGE/1.Mac กดอันนี้ก่อนครับ - Mac Start with This!.command"
chmod 755 "$STAGE/1.Mac กดอันนี้ก่อนครับ - Mac Start with This!.command"
cp docs/MACOS-APPROVAL-TH-EN.txt "$STAGE/INSTALL-TH-EN.txt"
cp THIRD_PARTY.md assets/OFL-*.txt "$STAGE/"
FLAVOR=universal
if [[ "$ARCHS" != 'arm64;x86_64' ]]; then FLAVOR="${ARCHS//;/_}"; fi
# Validation evidence is uploaded separately; keep customer downloads clean.
# Hash the complete customer package.
(cd "$STAGE" && find . -type f ! -name SHA256SUMS.txt -exec shasum -a 256 {} \; > SHA256SUMS.txt)
ditto -c -k --sequesterRsrc --keepParent "$STAGE" "$BUILD_DIR/package/Tatree-2-macOS-$FLAVOR.zip"
VERSION=$(sed -n 's/^CMAKE_PROJECT_VERSION:STATIC=//p' "$BUILD_DIR/CMakeCache.txt")
bash scripts/package-macos-installer.sh --stage "$STAGE" --output "$BUILD_DIR/installer" --version "$VERSION" --test-unsigned
cp "scripts/1.Mac กดอันนี้ก่อนครับ - Mac Start with This!.command" "$BUILD_DIR/installer/"
cp docs/MACOS-APPROVAL-TH-EN.txt "$BUILD_DIR/installer/INSTALL-TH-EN.txt"
if [[ "${CI:-}" == true && "${TATREE_TEST_SYSTEM_INSTALL:-}" == 1 ]]; then
  if ! bash tests/MacInstallerTests.sh "$BUILD_DIR/installer/Tatree-$VERSION-macOS-Setup-UNSIGNED-TEST.pkg" "$STAGE" "$BUILD_DIR/validation" > "$BUILD_DIR/validation/installer-tests.txt" 2>&1; then
    cat "$BUILD_DIR/validation/installer-tests.txt"; exit 1
  fi
fi
if [[ "${CI:-}" == true && "${TATREE_TEST_SYSTEM_INSTALL:-}" == 1 ]]; then
  printf '\n\n' | bash "scripts/1.Mac กดอันนี้ก่อนครับ - Mac Start with This!.command" > "$BUILD_DIR/validation/approval-cancel.txt"
  grep -q 'Cancelled. No files were changed.' "$BUILD_DIR/validation/approval-cancel.txt"
fi
CUSTOMER_STAGE="$(mktemp -d "$BUILD_DIR/installer/.customer-setup.XXXXXX")"
cp "$BUILD_DIR/installer/Tatree-$VERSION-macOS-Setup-UNSIGNED-TEST.pkg" "$CUSTOMER_STAGE/"
cp "scripts/1.Mac กดอันนี้ก่อนครับ - Mac Start with This!.command" "$CUSTOMER_STAGE/"
cp docs/MACOS-APPROVAL-TH-EN.txt "$CUSTOMER_STAGE/INSTALL-TH-EN.txt"
(cd "$CUSTOMER_STAGE" && shasum -a 256 * > SHA256SUMS.txt)
ditto -c -k --sequesterRsrc "$CUSTOMER_STAGE" "$BUILD_DIR/installer/Tatree-$VERSION-macOS-Setup.zip"
rm -rf "$CUSTOMER_STAGE"
printf '\nBuild, automated audio/UI tests and ad-hoc signature checks passed.\n%s/package\n' "$BUILD_DIR"
echo 'Local test build; Developer ID notarization and real DAW listening are separate release checks.'
if [[ "${CI:-}" != true ]]; then open "$BUILD_DIR/package"; fi
