@echo off
setlocal
set "source=%~dp0VST3\Tatree Noise Remover 2.vst3"
set "base=%LOCALAPPDATA%\Programs\Common\VST3"
set "destination=%base%\Tatree Noise Remover 2.vst3"
if not exist "%source%\Contents\x86_64-win\Tatree Noise Remover 2.vst3" (
  echo Plugin files not found. Extract the entire Windows package first.
  pause
  exit /b 1
)
if not exist "%base%" mkdir "%base%"
if exist "%destination%" (
  move "%destination%" "%destination%.backup-%RANDOM%" >nul
  if errorlevel 1 (
    echo Could not back up the previous Tatree 2 build. Close your DAW and retry.
    pause
    exit /b 1
  )
)
robocopy "%source%" "%destination%" /E /COPY:DAT /R:2 /W:1 >nul
if errorlevel 8 (
  echo Installation failed. The previous build is retained as a backup.
  pause
  exit /b 1
)
echo Installed for this Windows user:
echo %destination%
echo Open your DAW and rescan VST3 plugins. Add the folder above if needed.
echo Original Tatree v1 files were not changed.
pause
exit /b 0
