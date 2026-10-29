@echo off
setlocal
set "ROOT=%~dp0"
set "UE_ROOT="
for /f "usebackq delims=" %%E in (`powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%ROOT%Scripts\Find_UE_5_8.ps1" -Purpose Editor`) do set "UE_ROOT=%%E"
if defined UE_ROOT set "UEEDITOR=%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe"
if not defined UEEDITOR (
  echo Unreal Engine 5.8 nije pronadjen.
  echo Instaliraj UE 5.8 preko Epic Games Launchera.
  pause
  exit /b 1
)
if not exist "%ROOT%Content\Python\Import_Cacilend_Assets.py" (
  echo Import skripta nije pronadjena u Content\Python.
  pause
  exit /b 1
)
start "Cacilend UE5 Asset Import" "%UEEDITOR%" "%ROOT%CacilendUE.uproject" "-ExecutePythonScript=%ROOT%Content\Python\Import_Cacilend_Assets.py" -log
endlocal
