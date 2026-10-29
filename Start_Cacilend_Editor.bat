@echo off
setlocal
set "ROOT=%~dp0"
set "UE_ROOT="
for /f "usebackq delims=" %%E in (`powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%ROOT%Scripts\Find_UE_5_8.ps1" -Purpose Editor`) do set "UE_ROOT=%%E"
if defined UE_ROOT set "UEEDITOR=%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe"
if not defined UEEDITOR (
  echo Unreal Engine 5.8 nije pronadjen.
  echo Instaliraj Unreal Engine 5.8 kroz Epic Games Launcher.
  echo Potrebni su Visual Studio 2022 C++ alati za prvi build projekta.
  pause
  exit /b 1
)
start "Cacilend UE5.8" "%UEEDITOR%" "%ROOT%CacilendUE.uproject" -log
endlocal
