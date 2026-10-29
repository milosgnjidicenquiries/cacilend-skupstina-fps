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
if not exist "%ROOT%Build" mkdir "%ROOT%Build"
set "PACKAGE_LOG=%ROOT%Build\UE5-package.log"
echo Paketiram Windows Shipping build. Log: "%PACKAGE_LOG%"
call "%UAT%" BuildCookRun -project="%ROOT%CacilendUE.uproject" -noP4 -platform=Win64 -clientconfig=Shipping -build -cook -stage -pak -archive -prereqs -archivedirectory="%ROOT%Build\Windows" -utf8output > "%PACKAGE_LOG%" 2>&1
if errorlevel 1 (
  echo Pakovanje nije uspelo. Poslednjih 100 linija loga:
  powershell.exe -NoProfile -Command "Get-Content -LiteralPath '%PACKAGE_LOG%' -Tail 100"
  echo.
  echo Ceo log je sacuvan u Build\UE5-package.log.
  pause
  exit /b 1
)
echo Pakovanje je zavrseno. Zavrsne linije:
powershell.exe -NoProfile -Command "Get-Content -LiteralPath '%PACKAGE_LOG%' -Tail 20"
echo.
echo Proveri izlazni Windows build u folderu Build\Windows.
pause
endlocal
