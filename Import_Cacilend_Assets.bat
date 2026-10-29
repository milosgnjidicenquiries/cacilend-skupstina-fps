@echo off
setlocal
set "ROOT=%~dp0"
set "UEEDITOR="
for %%P in (
  "%ProgramFiles%\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
  "D:\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
  "C:\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
) do (
  if exist "%%~P" set "UEEDITOR=%%~P"
)
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
