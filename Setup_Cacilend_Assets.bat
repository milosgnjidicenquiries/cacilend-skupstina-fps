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
  pause
  exit /b 1
)
if not exist "%ROOT%Source\CacilendUE\CacilendUE.Build.cs" (
  echo Struktura projekta nedostaje. Ponovo preuzmi iz repozitorijuma.
  pause
  exit /b 1
)
where python >nul 2>nul
if not errorlevel 1 (
  python -m pip install --user numpy pillow
  python "%ROOT%Scripts\generate_textures.py"
) else (
  echo Python nije pronadjen; preskacem automatsko generisanje PBR tekstura.
)
start "Cacilend Setup" "%UEEDITOR%" "%ROOT%CacilendUE.uproject" "-ExecutePythonScript=%ROOT%Content\Python\Setup_Cacilend.py" -log
endlocal
