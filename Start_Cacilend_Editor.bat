@echo off
setlocal
set "ROOT=%~dp0"
set "UEEDITOR="
if defined UE_5_8_ROOT (
  if exist "%UE_5_8_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe" set "UEEDITOR=%UE_5_8_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe"
)
if not defined UEEDITOR for %%P in (
  "%ProgramFiles%\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
  "D:\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
  "C:\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
) do (
  if exist "%%~P" set "UEEDITOR=%%~P"
)
if not defined UEEDITOR (
  echo Unreal Engine 5.8 nije pronadjen.
  echo Instaliraj Unreal Engine 5.8 kroz Epic Games Launcher.
  echo Potrebni su Visual Studio 2022 C++ alati za prvi build projekta.
  pause
  exit /b 1
)
start "Cacilend UE5.8" "%UEEDITOR%" "%ROOT%CacilendUE.uproject" -log
endlocal
