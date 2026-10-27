param([string]$BuildDirectory = 'build-windows-msvc')
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
if ($env:CI -ne 'true' -or $env:RUNNER_ENVIRONMENT -ne 'github-hosted' -or $env:OS -ne 'Windows_NT') { throw 'Installer tests require a disposable GitHub-hosted Windows runner.' }
$Root = Split-Path $PSScriptRoot -Parent
$Build = [IO.Path]::GetFullPath((Join-Path $Root $BuildDirectory))
$VersionLine = Get-Content (Join-Path $Build 'CMakeCache.txt') | Where-Object { $_ -match '^CMAKE_PROJECT_VERSION:STATIC=' }
$Version = ($VersionLine -split '=',2)[1]
$Setup = Join-Path $Build "installer/Tatree-$Version-Windows-x64-Setup.exe"
$Zip = Join-Path $Build "installer/Tatree-$Version-Windows-Setup.zip"
$Extracted = Join-Path $Build 'validation/setup-download'
Expand-Archive -Path $Zip -DestinationPath $Extracted -Force
$Entries = @(Get-ChildItem $Extracted -Recurse -File)
if ($Entries.Count -ne 2 -or -not (Test-Path (Join-Path $Extracted 'INSTALL-TH-EN.txt'))) { throw 'Unexpected customer Setup archive contents.' }
$DownloadedSetup = Join-Path $Extracted (Split-Path $Setup -Leaf)
if ((Get-FileHash $DownloadedSetup).Hash -ne (Get-FileHash $Setup).Hash) { throw 'Customer Setup archive binary mismatch.' }
$Setup = $DownloadedSetup
Write-Host 'PASS customer ZIP contains only Setup and bilingual guide; installer bytes match'
$Target = Join-Path $env:CommonProgramW6432 'VST3/Tatree Noise Remover 2.vst3'
$AppDir = Join-Path $env:ProgramW6432 'Yuay Sound Records/Tatree Noise Remover 2'
$LogDir = Join-Path $Build 'validation'
function Install([string]$Phase) {
    $Log = Join-Path $LogDir "setup-$Phase.txt"
    $P = Start-Process -FilePath $Setup -ArgumentList @('/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART',"/LOG=`"$Log`"") -Wait -PassThru
    if ($P.ExitCode -ne 0) { throw "Setup $Phase failed: $($P.ExitCode)" }
    foreach ($Pair in @(
        @((Join-Path $Build 'Tatree_artefacts/Release/VST3/Tatree Noise Remover 2.vst3/Contents/x86_64-win/Tatree Noise Remover 2.vst3'),(Join-Path $Target 'Contents/x86_64-win/Tatree Noise Remover 2.vst3')),
        @((Join-Path $Build 'Tatree_artefacts/Release/Standalone/Tatree Noise Remover 2.exe'),(Join-Path $AppDir 'Tatree Noise Remover 2.exe')))) {
        if ((Get-FileHash $Pair[0]).Hash -ne (Get-FileHash $Pair[1]).Hash) { throw "Installed binary mismatch: $($Pair[1])" }
    }
    foreach ($Name in @('INSTALL-TH-EN.txt','Licenses/THIRD_PARTY.md','Licenses/OFL-Anton.txt','Licenses/OFL-NotoSansThai.txt')) {
        if (-not (Test-Path (Join-Path $AppDir $Name))) { throw "Missing installed file: $Name" }
    }
    Write-Host "PASS Windows $Phase install matches validated binaries and includes instructions/notices"
}
Install 'fresh'
$MarkerName = 'upgrade-fixture-' + [Guid]::NewGuid().ToString('N') + '.txt'
Set-Content (Join-Path $Target $MarkerName) 'Disposable installer test fixture'
Install 'upgrade'
if (Test-Path (Join-Path $Target $MarkerName)) { throw 'Old bundle content was not replaced.' }
$BackupRoot = Join-Path $env:ProgramData 'Yuay Sound Records/Tatree/Backups'
$Backup = @(Get-ChildItem $BackupRoot -Filter $MarkerName -Recurse -File)
if ($Backup.Count -ne 1) { throw 'Upgrade backup was not preserved outside scan folders.' }
$P = Start-Process -FilePath (Join-Path $AppDir 'unins000.exe') -ArgumentList @('/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART') -Wait -PassThru
if ($P.ExitCode -ne 0) { throw 'Windows uninstall failed.' }
if (Test-Path (Join-Path $Target 'Contents/x86_64-win/Tatree Noise Remover 2.vst3')) { throw 'Uninstall left the installed plug-in binary.' }
if (-not (Test-Path $Backup[0].FullName)) { throw 'Uninstall deleted the recovery backup.' }
Write-Host 'PASS Windows upgrade backup and uninstallation; backups preserved'
