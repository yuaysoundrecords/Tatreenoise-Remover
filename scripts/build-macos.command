#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/.."
if [[ "$(uname -s)" != Darwin ]]; then
  echo 'This script builds on macOS. See README for Linux/Windows.' >&2; exit 1
fi
if ! xcode-select -p >/dev/null 2>&1; then
  echo 'Install Apple Command Line Tools first: xcode-select --install' >&2; exit 1
fi
if ! command -v cmake >/dev/null 2>&1; then
  echo 'CMake is required. Install it from cmake.org or run: brew install cmake' >&2; exit 1
fi
cmake -S . -B build-macos -DCMAKE_BUILD_TYPE=Release "-DCMAKE_OSX_ARCHITECTURES=$(uname -m)"
cmake --build build-macos --config Release --parallel 3
ctest --test-dir build-macos -C Release --output-on-failure
printf '\nBuild and automated tests passed. Your files are in:\n%s/build-macos/Tatree_artefacts/Release\n' "$PWD"
printf 'Test the Standalone app first. See docs/TESTING.md before installing AU/VST3.\n'
open build-macos/Tatree_artefacts/Release
