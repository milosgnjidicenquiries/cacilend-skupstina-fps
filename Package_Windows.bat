@echo off
setlocal
set "ROOT=%~dp0"
set "UAT="
for %%P in (
  "%ProgramFiles%\Epic Games\UE_5.8\Engine\Build\BatchFiles\RunUAT.bat"
  "D:\Epic Games\UE_5.8\Engine\Build\BatchFiles\RunUAT.bat"
  "C:\Epic Games\UE_5.8\Engine\Build\BatchFiles\RunUAT.bat"
) do (
  if exist "%%~P" set "UAT=%%~P"
)
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
