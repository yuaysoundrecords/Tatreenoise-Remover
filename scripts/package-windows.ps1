param([string]$BuildDirectory = 'build-windows-msvc')
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$Root = Split-Path $PSScriptRoot -Parent
$Build = if ([System.IO.Path]::IsPathRooted($BuildDirectory)) { [System.IO.Path]::GetFullPath($BuildDirectory) } else { [System.IO.Path]::GetFullPath((Join-Path $Root $BuildDirectory)) }
$Plugin = Join-Path $Build 'Tatree_artefacts/Release/VST3/Tatree Noise Remover 2.vst3'
$App = Join-Path $Build 'Tatree_artefacts/Release/Standalone/Tatree Noise Remover 2.exe'
foreach ($Required in @($Plugin,$App,(Join-Path $Build 'validation/audio-tests.txt'),(Join-Path $Build 'validation/native-ui-tests.txt'),(Join-Path $Build 'validation/pluginval.txt'))) {
    if (-not (Test-Path $Required)) { throw "Required build/test result missing: $Required" }
}
if (-not ((Get-Content (Join-Path $Build 'validation/pluginval.txt') -Raw) -match 'SUCCESS')) { throw 'Refusing to package a failed validator result.' }
$ResultFile = Join-Path $Build 'validation/result.json'
if (-not (Test-Path $ResultFile)) { throw 'No current validation result. Run build-windows.ps1 first.' }
$Result = Get-Content $ResultFile -Raw | ConvertFrom-Json
$PluginBinary = Join-Path $Plugin 'Contents/x86_64-win/Tatree Noise Remover 2.vst3'
if ($Result.status -ne 'passed' -or $Result.platform -ne 'Windows x64' -or
    $Result.plugin_sha256 -ne (Get-FileHash $PluginBinary -Algorithm SHA256).Hash.ToLowerInvariant() -or
    $Result.app_sha256 -ne (Get-FileHash $App -Algorithm SHA256).Hash.ToLowerInvariant()) {
    throw 'Validation result does not match these binaries. Refusing to package stale or failed output.'
}
$LicenseConfig = Join-Path $Build 'licensing-build.json'
if (-not (Test-Path $LicenseConfig)) { throw 'Licensing build metadata missing. Reconfigure and rebuild.' }
$License = Get-Content $LicenseConfig -Raw | ConvertFrom-Json
$Flavor = if ($License.activation_configured -eq $true) { '' } else { '-TRIAL-ONLY-STAGING-DO-NOT-SELL' }
$Stage = Join-Path $Build ('package/Tatree-' + $License.version + '-Windows-x64' + $Flavor + '-' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force (Join-Path $Stage 'VST3'),(Join-Path $Stage 'Standalone') | Out-Null
Copy-Item $Plugin (Join-Path $Stage 'VST3') -Recurse
Copy-Item $App (Join-Path $Stage 'Standalone')
Copy-Item (Join-Path $Root 'scripts/Install-VST3.cmd') $Stage
Copy-Item (Join-Path $Root 'docs/WINDOWS.md') (Join-Path $Stage 'READ-ME-WINDOWS.md')
Copy-Item (Join-Path $Root 'THIRD_PARTY.md') $Stage
Copy-Item $LicenseConfig $Stage
Copy-Item (Join-Path $Root 'docs/LICENSING.md') $Stage
Copy-Item (Join-Path $Root 'assets/OFL-*.txt') $Stage
# Keep the shipped readme's A/B audio and quality-report links self-contained.
Copy-Item (Join-Path $Root 'docs/DSP-QUALITY.md') $Stage
New-Item -ItemType Directory -Force (Join-Path $Stage 'audio'),(Join-Path $Stage 'evidence') | Out-Null
foreach ($Name in @('vocal-gain-before.wav','vocal-gain-after.wav','quality-speech-before.wav','quality-speech-after.wav','README.md')) {
    Copy-Item (Join-Path $Root ('docs/audio/' + $Name)) (Join-Path $Stage 'audio')
}
Copy-Item (Join-Path $Root 'tests/fixtures/WHISPER-LICENSE.txt'),(Join-Path $Root 'tests/fixtures/jfk.json') (Join-Path $Stage 'audio')
Copy-Item (Join-Path $Root 'docs/evidence/dsp-comparison.json'),(Join-Path $Root 'docs/evidence/dsp-sources.json') (Join-Path $Stage 'evidence')
$Evidence = Join-Path $Stage 'validation'
New-Item -ItemType Directory -Force (Join-Path $Evidence 'ui') | Out-Null
Get-ChildItem (Join-Path $Build 'validation') -File | Copy-Item -Destination $Evidence
Copy-Item (Join-Path $Build 'validation/ui/*.png') (Join-Path $Evidence 'ui')
$Entries = Get-ChildItem $Stage -Recurse -File | ForEach-Object {
    [ordered]@{path=$_.FullName.Substring($Stage.Length+1);sha256=(Get-FileHash $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()}
}
[ordered]@{product='Tatree Noise Remover 2';platform='Windows x64';status='test build';files=@($Entries)} | ConvertTo-Json -Depth 5 | Set-Content (Join-Path $Stage 'checksums.json') -Encoding UTF8
$Zip = $Stage + '.zip'
Compress-Archive -Path $Stage -DestinationPath $Zip
(Get-FileHash $Zip -Algorithm SHA256).Hash.ToLowerInvariant() | Set-Content ($Zip + '.sha256') -Encoding ASCII
Write-Host "Package: $Zip"
