param([string]$BuildDirectory = 'build-windows-msvc')
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$Root = Split-Path $PSScriptRoot -Parent
$Build = if ([IO.Path]::IsPathRooted($BuildDirectory)) { [IO.Path]::GetFullPath($BuildDirectory) } else { [IO.Path]::GetFullPath((Join-Path $Root $BuildDirectory)) }
$Result = Get-Content (Join-Path $Build 'validation/result.json') -Raw | ConvertFrom-Json
$Plugin = Join-Path $Build 'Tatree_artefacts/Release/VST3/Tatree Noise Remover 2.vst3/Contents/x86_64-win/Tatree Noise Remover 2.vst3'
$App = Join-Path $Build 'Tatree_artefacts/Release/Standalone/Tatree Noise Remover 2.exe'
if ($Result.status -ne 'passed' -or $Result.platform -ne 'Windows x64' -or
    $Result.plugin_sha256 -ne (Get-FileHash $Plugin -Algorithm SHA256).Hash.ToLowerInvariant() -or
    $Result.app_sha256 -ne (Get-FileHash $App -Algorithm SHA256).Hash.ToLowerInvariant()) {
    throw 'Refusing to build Setup from unvalidated or changed binaries.'
}
$VersionLine = Get-Content (Join-Path $Build 'CMakeCache.txt') | Where-Object { $_ -match '^CMAKE_PROJECT_VERSION:STATIC=' }
$Version = ($VersionLine -split '=',2)[1]
if ($Version -notmatch '^\d+\.\d+\.\d+$') { throw 'Invalid product version.' }
$Compiler = $env:TATREE_ISCC
if (-not $Compiler) { $Compiler = Join-Path ${env:ProgramFiles(x86)} 'Inno Setup 6/ISCC.exe' }
if (-not (Test-Path $Compiler)) { throw 'Install Inno Setup 6, or set TATREE_ISCC to ISCC.exe.' }
$Output = Join-Path $Build 'installer'
New-Item -ItemType Directory -Force $Output | Out-Null
& $Compiler "/DBuildRoot=$Build" "/DSourceRoot=$Root" "/DProductVersion=$Version" (Join-Path $Root 'windows/installer/Tatree.iss')
if ($LASTEXITCODE -ne 0) { throw 'Windows Setup compilation failed.' }
$Setup = Join-Path $Output "Tatree-$Version-Windows-x64-Setup.exe"
if (-not (Test-Path $Setup)) { throw 'Windows Setup executable was not produced.' }
$Stage = Join-Path $Output "Tatree-$Version-Windows-Setup"
if (Test-Path $Stage) {
    $ResolvedStage = [IO.Path]::GetFullPath($Stage)
    if ((Split-Path $ResolvedStage -Parent) -ne [IO.Path]::GetFullPath($Output)) { throw 'Unsafe staging path.' }
    Remove-Item -LiteralPath $ResolvedStage -Recurse -Force
}
New-Item -ItemType Directory -Force $Stage | Out-Null
Copy-Item $Setup $Stage
Copy-Item (Join-Path $Root 'docs/WINDOWS-INSTALL-TH-EN.txt') (Join-Path $Stage 'INSTALL-TH-EN.txt')
$Zip = $Stage + '.zip'
Compress-Archive -Path (Join-Path $Stage '*') -DestinationPath $Zip -Force
(Get-FileHash $Zip -Algorithm SHA256).Hash.ToLowerInvariant() | Set-Content ($Zip + '.sha256') -Encoding ASCII
Write-Host "Windows Setup package: $Zip"
