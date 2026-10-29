@echo off
setlocal EnableExtensions
set "ROOT=%~dp0"
set "TOOLS=%ROOT%Tools\third_party"
set "ADDON=%TOOLS%\cod-asset-importer"
set "ADDON_SRC=%TOOLS%\cod-asset-importer-source"
set "RELEASE_DIR=%ADDON%\release"
set "RELEASE_ZIP=%RELEASE_DIR%\cod_asset_importer_v3.5.0.zip"
set "RELEASE_URL=https://github.com/mauserzjeh/cod-asset-importer/releases/download/v3.5/cod_asset_importer_v3.5.0.zip"

if not exist "%RELEASE_DIR%" mkdir "%RELEASE_DIR%"

echo [1/2] Pokusavam da preuzmem gotov COD Asset Importer v3.5 (COD2 model/map podrška)...
if exist "%RELEASE_ZIP%" (
  for %%F in ("%RELEASE_ZIP%") do if %%~zF GTR 100000 goto :USE_RELEASE
)
powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; try { Invoke-WebRequest -UseBasicParsing -Uri '%RELEASE_URL%' -OutFile '%RELEASE_ZIP%'; if ((Get-Item '%RELEASE_ZIP%').Length -lt 100000) { throw 'Preuzeti ZIP je neocekivano mali.' }; Write-Host 'Release ZIP je preuzet.'; exit 0 } catch { Write-Error $_; exit 1 }"
if not errorlevel 1 goto :USE_RELEASE

echo.
echo Gotov release nije preuzet; prelazim na rezervnu instalaciju iz izvornog koda.
where git >nul 2>nul
if errorlevel 1 (
  echo Git nije pronadjen. Instaliraj Git for Windows ili proveri internet.
  pause
  exit /b 1
)
where python >nul 2>nul
if errorlevel 1 (
  echo Python nije pronadjen. Instaliraj Python 3 i dodaj ga u PATH.
  pause
  exit /b 1
)
where cargo >nul 2>nul
if errorlevel 1 (
  echo Rust/Cargo nije pronadjen. Instaliraj Rust sa https://rustup.rs pa pokreni ponovo.
  pause
  exit /b 1
)
if not exist "%TOOLS%" mkdir "%TOOLS%"
if not exist "%ADDON_SRC%\.git" (
  git clone https://github.com/mauserzjeh/cod-asset-importer "%ADDON_SRC%"
  if errorlevel 1 (
    echo Git clone nije uspeo. Proveri internet i GitHub pristup.
    pause
    exit /b 1
  )
) else (
  echo Postojeca lokalna kopija dodatka je sacuvana; necu je prepisivati.
)
pushd "%ADDON_SRC%"
if exist requirements.txt (
  python -m pip install -r requirements.txt
  if errorlevel 1 (
    popd
    echo Instalacija Python zavisnosti nije uspela.
    pause
    exit /b 1
  )
)
python setup.py build_rust --inplace --release --create-release-package
if errorlevel 1 (
  popd
  echo Build dodatka nije uspeo. Proveri Python/Rust verzije i log iznad.
  pause
  exit /b 1
)
popd
echo.
set "RELEASE_DIR=%ADDON_SRC%\release"
echo Dodatak je lokalno izgradjen u %RELEASE_DIR%
goto :INSTALL_HELP

:USE_RELEASE
echo Gotov release ZIP je spreman:
echo %RELEASE_ZIP%

:INSTALL_HELP
echo.
echo Sledeci koraci:
echo 1. Otvori Blender 4.3 ili kompatibilnu verziju.
echo 2. Edit ^> Preferences ^> Add-ons ^> Install from Disk.
echo 3. Izaberi ZIP iz foldera:
echo    %RELEASE_DIR%
echo 4. Ukljuci dodatak.
echo 5. File ^> Import ^> CoD Asset Importer ^> Import model.
echo 6. Za lokalni COD2 sadrzaj koristi Build\LocalCOD2Extraction\xmodel.
echo 7. Za uvoz mape izaberi Import map i odgovarajuci D3DBSP u folderu maps.
echo.
echo Izvezene FBX fajlove i teksture postavi u SourceArt\Import prema Docs\ASSET_IMPORT_PIPELINE_SR.md.
echo Ne commit-uj originalne COD2 arhive niti izvezene vlasnicke assete u javni GitHub repo.
pause
endlocal
