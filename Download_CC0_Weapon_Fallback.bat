@echo off
setlocal EnableExtensions
set "ROOT=%~dp0"
set "OUT=%ROOT%SourceArt\Import\Weapons"
if not exist "%OUT%" mkdir "%OUT%"
echo Downloading optional CC0 placeholder weapon (not a COD2 original)...
powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; $url='https://raw.githubusercontent.com/petroulacl/fps-asset-kit/main/weapons/flat_guns_west/Flat%%20Guns%%20West/FBX/Rifle_Assault_West.Rig.fbx'; $out=Join-Path '%OUT%' 'SM_CC0_Rifle.fbx'; Invoke-WebRequest -Uri $url -OutFile $out; if ((Get-Item $out).Length -lt 10000) { throw 'Downloaded file is unexpectedly small'; }; Write-Host ('Saved '+$out)"
if errorlevel 1 (
  echo Download nije uspeo. Moze se uvesti rucno sa FPS Asset Kit repozitorijuma.
  pause
  exit /b 1
)
echo.
echo Fajl je u SourceArt\Import\Weapons\SM_CC0_Rifle.fbx
echo Pokreni Import_Cacilend_Assets.bat ili Setup_Cacilend_Assets.bat.
echo Ovaj CC0 model je privremeni fallback, nije originalni COD2 model.
pause
endlocal
