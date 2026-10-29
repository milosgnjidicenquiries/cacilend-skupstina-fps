@echo off
setlocal EnableExtensions
set "ROOT=%~dp0"
set "UE_ROOT="
for /f "usebackq delims=" %%E in (`powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%ROOT%Scripts\Find_UE_5_8.ps1" -Purpose Build`) do set "UE_ROOT=%%E"
if not defined UE_ROOT (
  echo Unreal Engine 5.8 Build.bat nije pronadjen.
  echo Instaliraj UE 5.8 ili proveri da li Epic Games Launcher prikazuje engine kao instaliran.
  echo Za rucnu putanju postavi UE_5_8_ROOT na folder koji sadrzi Engine.
  pause
  exit /b 1
)
if not exist "%ROOT%CacilendUE.uproject" (
  echo CacilendUE.uproject nije pronadjen u %ROOT%
  pause
  exit /b 1
)
echo [1/2] Kompajliram CacilendUEEditor Win64 Development preko UnrealBuildTool-a...
call "%UE_ROOT%\Engine\Build\BatchFiles\Build.bat" CacilendUEEditor Win64 Development -Project="%ROOT%CacilendUE.uproject" -WaitMutex -FromMsBuild
if errorlevel 1 (
  echo.
  echo UE5 C++ build nije uspeo. Sacuvaj ceo log iznad; nije napravljen validiran EXE.
  echo Proveri Visual Studio C++ workload, MSVC toolset, Windows SDK i tacnu UE verziju.
  pause
  exit /b 1
)
echo.
echo [2/2] C++ build je prosao. Pokrecem mapu, materijale i lokalni asset import u Unreal Editor-u...
call "%ROOT%Setup_Cacilend_Assets.bat"
endlocal
