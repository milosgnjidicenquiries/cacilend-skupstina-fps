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
  python -c "import numpy; import PIL" >nul 2>nul
  if errorlevel 1 (
    python -m pip install --user numpy pillow
  )
  python -c "import numpy; import PIL" >nul 2>nul
  if errorlevel 1 (
    echo NumPy/Pillow nedostaje ili nije instaliran. Preskacem generisanje PBR tekstura; proveri Python/pip.
  ) else (
    python "%ROOT%Scripts\generate_textures.py"
    if errorlevel 1 echo UPOZORENJE: generisanje tekstura nije uspelo; editor setup ce ipak biti pokrenut.
  )
) else (
  echo Python nije pronadjen; preskacem automatsko generisanje PBR tekstura.
)
start "Cacilend Setup" "%UEEDITOR%" "%ROOT%CacilendUE.uproject" "-ExecutePythonScript=%ROOT%Content\Python\Setup_Cacilend.py" -log
endlocal
