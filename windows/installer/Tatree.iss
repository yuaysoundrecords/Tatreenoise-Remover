#ifndef BuildRoot
  #error BuildRoot is required
#endif
#ifndef SourceRoot
  #error SourceRoot is required
#endif
#ifndef ProductVersion
  #error ProductVersion is required
#endif
[Setup]
AppId={{17B5CC53-94F1-4E3C-B11D-E868A5FB2592}
AppName=Tatree Noise Remover 2
AppVersion={#ProductVersion}
AppPublisher=Yuay Sound Records
AppPublisherURL=https://ko-fi.com/yuaylooper
DefaultDirName={autopf}\Yuay Sound Records\Tatree Noise Remover 2
DefaultGroupName=Yuay Sound Records
DisableProgramGroupPage=yes
PrivilegesRequired=admin
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputDir={#BuildRoot}\installer
OutputBaseFilename=Tatree-{#ProductVersion}-Windows-x64-Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
SetupLogging=yes
CloseApplications=no
RestartApplications=no
UninstallDisplayIcon={app}\Tatree Noise Remover 2.exe
InfoBeforeFile={#SourceRoot}\docs\WINDOWS-INSTALL-TH-EN.txt
[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
[Files]
Source: "{#BuildRoot}\Tatree_artefacts\Release\VST3\Tatree Noise Remover 2.vst3\*"; DestDir: "{commoncf64}\VST3\Tatree Noise Remover 2.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#BuildRoot}\Tatree_artefacts\Release\Standalone\Tatree Noise Remover 2.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceRoot}\docs\WINDOWS-INSTALL-TH-EN.txt"; DestDir: "{app}"; DestName: "INSTALL-TH-EN.txt"; Flags: ignoreversion
Source: "{#SourceRoot}\THIRD_PARTY.md"; DestDir: "{app}\Licenses"; Flags: ignoreversion
Source: "{#SourceRoot}\assets\OFL-*.txt"; DestDir: "{app}\Licenses"; Flags: ignoreversion
[Icons]
Name: "{group}\Tatree Noise Remover 2"; Filename: "{app}\Tatree Noise Remover 2.exe"
Name: "{group}\Tatree installation guide"; Filename: "{app}\INSTALL-TH-EN.txt"
[Code]
function PrepareToInstall(var NeedsRestart: Boolean): String;
var
  Target, BackupBase, Backup, UserCopy: String;
  Counter: Integer;
begin
  Result := '';
  UserCopy := ExpandConstant('{localappdata}\Programs\Common\VST3\Tatree Noise Remover 2.vst3');
  if DirExists(UserCopy) then begin
    Result := 'พบ Tatree ที่ติดตั้งแบบรายผู้ใช้ กรุณาย้ายเฉพาะชุดนี้ไปสำรองนอกโฟลเดอร์สแกนก่อน / Move this existing per-user Tatree bundle outside plug-in scan folders before installing: ' + UserCopy;
    exit;
  end;
  Target := ExpandConstant('{commoncf64}\VST3\Tatree Noise Remover 2.vst3');
  if DirExists(Target) then begin
    BackupBase := ExpandConstant('{commonappdata}\Yuay Sound Records\Tatree\Backups');
    Backup := BackupBase + '\' + GetDateTimeString('yyyymmdd-hhnnss', '-', ':');
    Counter := 0;
    while DirExists(Backup) do begin
      Counter := Counter + 1;
      Backup := BackupBase + '\' + GetDateTimeString('yyyymmdd-hhnnss', '-', ':') + '-' + IntToStr(Counter);
    end;
    if not ForceDirectories(Backup) then begin
      Result := 'สร้างโฟลเดอร์สำรองไม่ได้ / Cannot create the backup folder: ' + Backup;
      exit;
    end;
    if not RenameFile(Target, Backup + '\Tatree Noise Remover 2.vst3') then
      Result := 'สำรอง Tatree เดิมไม่ได้ กรุณาปิด DAW และลองใหม่ / Cannot back up Tatree. Close your DAW and retry. Existing files were not replaced.';
  end;
end;
