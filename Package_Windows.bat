@echo off
setlocal
set "ROOT=%~dp0"
set "UE_ROOT="
for /f "usebackq delims=" %%E in (`powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%ROOT%Scripts\Find_UE_5_8.ps1" -Purpose Package`) do set "UE_ROOT=%%E"
if defined UE_ROOT set "UAT=%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat"
if not defined UAT (
  echo Unreal Engine 5.8 nije pronadjen. Instaliraj ga kroz Epic Games Launcher.
  pause
  exit /b 1
)
call "%UAT%" BuildCookRun -project="%ROOT%CacilendUE.uproject" -noP4 -platform=Win64 -clientconfig=Shipping -build -cook -stage -pak -archive -prereqs -archivedirectory="%ROOT%Build\Windows" -utf8output
if errorlevel 1 (
  echo Build nije uspeo. Pogledaj UAT log i Unreal Editor log.
  pause
  exit /b 1
)
echo Gotovo. Izlazni Windows build je u folderu Build\Windows.
pause
endlocal
