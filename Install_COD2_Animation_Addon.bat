@echo off
setlocal EnableExtensions
set "ROOT=%~dp0"
set "TOOLS=%ROOT%Tools\third_party\pv_blender_cod"
set "ZIP=%TOOLS%\_pv_blender_cod.zip"
set "URL=https://github.com/prov3ntus/pv_blender_cod/releases/download/v0.9.0/_pv_blender_cod.zip"

if not exist "%TOOLS%" mkdir "%TOOLS%"
if not exist "%ZIP%" (
  echo Preuzimam Blender CoD XModel/XAnim add-on...
  powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; Invoke-WebRequest -UseBasicParsing -Uri '%URL%' -OutFile '%ZIP%'; if ((Get-Item '%ZIP%').Length -lt 100000) { throw 'Preuzeti ZIP je neocekivano mali.' }"
  if errorlevel 1 (
    echo Preuzimanje add-on ZIP-a nije uspelo. Proveri internet i pokusaj ponovo.
    pause
    exit /b 1
  )
) else (
  echo ZIP vec postoji; cuvam postojeci fajl.
)

echo.
echo Add-on je spreman:
echo %ZIP%
echo.
echo Instalacija u Blenderu:
echo 1. Otvori Blender 3.x ili 4.x. Ovaj release navodi podrsku za 3.x-4.x.
echo 2. Edit ^> Preferences ^> Add-ons ^> Install from Disk.
echo 3. Izaberi gornji _pv_blender_cod.zip i omoguci add-on "pv_blender_cod".
echo 4. Iskljuci druge Call of Duty Blender add-ons da se importer ne bi sukobio s njima.
echo 5. XAnim podrsku tretiraj kao eksperimentalnu; README prijavljuje da XAnim import ima poznate probleme.
echo 6. Za animacije prvo konvertuj lokalni .xanim_bin sa Convert_COD2_Animations.bat, zatim importuj .xanim_export preko add-on-a.
echo.
echo Izvor i release notes: https://github.com/prov3ntus/pv_blender_cod/releases/tag/v0.9.0
pause
endlocal
