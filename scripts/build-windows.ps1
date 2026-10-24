param([string]$BuildDirectory = 'build-windows-msvc', [int]$Jobs = 3)
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
if ($env:OS -ne 'Windows_NT') { throw 'Run this script on Windows 10/11 x64 with Visual Studio 2022 C++ tools and CMake.' }
$Root = Split-Path $PSScriptRoot -Parent
Set-Location $Root
$Build = if ([System.IO.Path]::IsPathRooted($BuildDirectory)) { [System.IO.Path]::GetFullPath($BuildDirectory) } else { [System.IO.Path]::GetFullPath((Join-Path $Root $BuildDirectory)) }
Remove-Item (Join-Path $Build 'validation/result.json') -ErrorAction SilentlyContinue
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) { throw 'Install CMake 3.22 or later first.' }
Write-Host 'Configuring Windows x64 / MSVC...'
& cmake -S $Root -B $Build -G 'Visual Studio 17 2022' -A x64
if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }
& cmake --build $Build --config Release --parallel $Jobs
if ($LASTEXITCODE -ne 0) { throw 'Windows build failed.' }
& ctest --test-dir $Build -C Release --output-on-failure
if ($LASTEXITCODE -ne 0) { throw 'Audio/motion tests failed; not packaging this build.' }

$Evidence = Join-Path $Build 'validation'
New-Item -ItemType Directory -Force $Evidence | Out-Null
function Invoke-TestExecutable([string]$Executable, [string[]]$Arguments, [string]$Name) {
    $Stdout = Join-Path $Evidence ($Name + '.txt')
    $Stderr = Join-Path $Evidence ($Name + '-stderr.txt')
    $Quoted = ($Arguments | ForEach-Object { '"' + $_.Replace('"','\"') + '"' }) -join ' '
    $Options = @{FilePath=$Executable; Wait=$true; PassThru=$true; RedirectStandardOutput=$Stdout; RedirectStandardError=$Stderr}
    if ($Arguments.Count -gt 0) { $Options.ArgumentList = $Quoted }
    $Process = Start-Process @Options
    Get-Content $Stdout | Write-Host
    if ($Process.ExitCode -ne 0) { Get-Content $Stderr | Write-Host; throw "$Name failed (exit $($Process.ExitCode))." }
}
$Tests = Join-Path $Build 'TatreeTests_artefacts/Release/TatreeTests.exe'
Invoke-TestExecutable $Tests @() 'audio-tests'
Invoke-TestExecutable $Tests @('--speech-check',(Join-Path $Root 'tests/fixtures/jfk.wav')) 'speech-tests'
$Render = Join-Path $Build 'TatreeRender_artefacts/Release/TatreeRender.exe'
$Captures = Join-Path $Evidence 'ui'
Invoke-TestExecutable $Render @($Captures) 'native-ui-tests'
foreach ($Name in @('idle','noise','de-esser','input-hot','bypass','reduced-motion','breath','listen','filter-on','filter-off','auto-gain','output-hot','stopped','quote-peak','quote-de-esser','quote-noise','quote-auto-on','quote-auto-off')) {
    if (-not (Test-Path (Join-Path $Captures ($Name + '.png')))) { throw "Missing UI capture: $Name" }
}

Write-Host 'Downloading the pinned official Windows plugin validator...'
$Meta = Get-Content (Join-Path $PSScriptRoot 'pluginval-windows.json') -Raw | ConvertFrom-Json
$Tools = Join-Path $Build 'validation-tools'
New-Item -ItemType Directory -Force $Tools | Out-Null
$Archive = Join-Path $Tools 'pluginval.zip'
if (-not (Test-Path $Archive)) { Invoke-WebRequest $Meta.url -OutFile $Archive }
if ((Get-FileHash $Archive -Algorithm SHA256).Hash.ToLowerInvariant() -ne $Meta.sha256) { throw 'pluginval checksum mismatch. No validation or packaging performed.' }
Expand-Archive -Path $Archive -DestinationPath $Tools -Force
$Plugin = Join-Path $Build 'Tatree_artefacts/Release/VST3/Tatree Noise Remover 2.vst3'
Invoke-TestExecutable (Join-Path $Tools 'pluginval.exe') @('--strictness-level','5','--random-seed','42','--output-dir',(Join-Path $Evidence 'pluginval'),'--validate',$Plugin) 'pluginval'
if (-not ((Get-Content (Join-Path $Evidence 'pluginval.txt') -Raw) -match 'SUCCESS')) { throw 'Validator did not report SUCCESS.' }

$PluginBinary = Join-Path $Plugin 'Contents/x86_64-win/Tatree Noise Remover 2.vst3'
$AppBinary = Join-Path $Build 'Tatree_artefacts/Release/Standalone/Tatree Noise Remover 2.exe'
[ordered]@{status='passed';platform='Windows x64';toolchain='MSVC';validated_at_utc=[DateTime]::UtcNow.ToString('o');plugin_sha256=(Get-FileHash $PluginBinary -Algorithm SHA256).Hash.ToLowerInvariant();app_sha256=(Get-FileHash $AppBinary -Algorithm SHA256).Hash.ToLowerInvariant()} | ConvertTo-Json | Set-Content (Join-Path $Evidence 'result.json') -Encoding UTF8
& (Join-Path $PSScriptRoot 'package-windows.ps1') -BuildDirectory $Build
Write-Host 'Windows x64 build, audio tests, native UI tests and pluginval passed.'
Write-Host 'The package is a test build. DAW listening and signing/notarization are separate checks.'
