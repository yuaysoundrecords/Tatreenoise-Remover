# Filesystem/packaging guards only. Fixtures below are NOT executable Windows builds.
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$Root = Split-Path $PSScriptRoot -Parent
$Fixture = Join-Path $Root ('build-package-tests/fixture with spaces-' + [Guid]::NewGuid().ToString('N'))
$Packager = Join-Path $Root 'scripts/package-windows.ps1'
function Expect-Rejection([string]$Message) {
    $Rejected = $false
    try { & $Packager -BuildDirectory $Fixture } catch { $Rejected = $true }
    if (-not $Rejected) { throw $Message }
}
try {
    $Plugin = Join-Path $Fixture 'Tatree_artefacts/Release/VST3/Tatree Noise Remover 2.vst3/Contents/x86_64-win/Tatree Noise Remover 2.vst3'
    $App = Join-Path $Fixture 'Tatree_artefacts/Release/Standalone/Tatree Noise Remover 2.exe'
    $Evidence = Join-Path $Fixture 'validation'
    New-Item -ItemType Directory -Force (Split-Path $Plugin),(Split-Path $App),(Join-Path $Evidence 'ui') | Out-Null
    Set-Content $Plugin 'DUMMY PLUGIN - PACKAGING TEST ONLY'
    Set-Content $App 'DUMMY APPLICATION - PACKAGING TEST ONLY'
    Set-Content (Join-Path $Evidence 'audio-tests.txt') 'fixture'
    Set-Content (Join-Path $Evidence 'native-ui-tests.txt') 'fixture'
    Set-Content (Join-Path $Evidence 'pluginval.txt') 'SUCCESS'
    [IO.File]::WriteAllBytes((Join-Path $Evidence 'ui/idle.png'),[Convert]::FromBase64String('iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mP8/x8AAwMCAO+jN1sAAAAASUVORK5CYII='))
    Expect-Rejection 'Missing validation metadata was accepted.'
    Write-Host 'PASS package refuses missing validation metadata'
    [ordered]@{status='passed';platform='Windows x64';toolchain='fixture-not-executable';plugin_sha256=(Get-FileHash $Plugin -Algorithm SHA256).Hash.ToLowerInvariant();app_sha256=(Get-FileHash $App -Algorithm SHA256).Hash.ToLowerInvariant()} | ConvertTo-Json | Set-Content (Join-Path $Evidence 'result.json')
    & $Packager -BuildDirectory $Fixture
    $Archives = @(Get-ChildItem (Join-Path $Fixture 'package/*.zip'))
    if ($Archives.Count -ne 1) { throw 'Absolute build path did not produce exactly one package.' }
    if (-not (Test-Path ($Archives[0].FullName + '.sha256'))) { throw 'Package checksum missing.' }
    Write-Host 'PASS absolute build path with spaces, complete archive and checksum'
    Add-Content $Plugin 'CHANGED AFTER VALIDATION'
    Expect-Rejection 'Changed plugin binary was packaged against stale validation.'
    Write-Host 'PASS package refuses stale binary hashes'
    if (@(Get-ChildItem (Join-Path $Fixture 'package/*.zip')).Count -ne 1) { throw 'Rejected validation still created another archive.' }
} finally {
    if (Test-Path $Fixture) { Remove-Item $Fixture -Recurse -Force }
}
