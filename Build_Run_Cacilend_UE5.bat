@echo off
setlocal EnableExtensions
set "ROOT=%~dp0"
set "UE_ROOT=%UE_5_8_ROOT%"
if not defined UE_ROOT set "UE_ROOT=%ProgramFiles%\Epic Games\UE_5.8"
if not exist "%UE_ROOT%\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=D:\Epic Games\UE_5.8"
if not exist "%UE_ROOT%\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=C:\Epic Games\UE_5.8"
if not exist "%UE_ROOT%\Engine\Build\BatchFiles\Build.bat" (
  echo Unreal Engine 5.8 Build.bat nije pronadjen.
  echo Instaliraj UE 5.8 ili postavi environment variable UE_5_8_ROOT na UE_5.8 folder.
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
  echo Proveri Visual Studio 2022 C++ workload, Windows SDK i tacnu UE verziju.
  pause
  exit /b 1
)
echo.
echo [2/2] C++ build je prosao. Pokrecem mapu, materijale i lokalni asset import u Unreal Editor-u...
call "%ROOT%Setup_Cacilend_Assets.bat"
endlocal
