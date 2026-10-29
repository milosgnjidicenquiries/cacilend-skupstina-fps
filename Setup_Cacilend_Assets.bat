@echo off
setlocal
set "ROOT=%~dp0"
set "UE_ROOT="
for /f "usebackq delims=" %%E in (`powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%ROOT%Scripts\Find_UE_5_8.ps1" -Purpose Editor`) do set "UE_ROOT=%%E"
if defined UE_ROOT set "UEEDITOR=%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe"
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
