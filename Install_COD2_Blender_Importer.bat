@echo off
setlocal EnableExtensions
set "ROOT=%~dp0"
set "TOOLS=%ROOT%Tools\third_party"
set "ADDON=%TOOLS%\cod-asset-importer"

where git >nul 2>nul
if errorlevel 1 (
  echo Git nije pronadjen. Instaliraj Git for Windows, pa ponovo pokreni skriptu.
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
  echo Rust/Cargo nije pronadjen.
  echo COD2 Asset Importer koristi Rust deo. Instaliraj Rust sa https://rustup.rs pa pokreni ovu skriptu ponovo.
  pause
  exit /b 1
)

if not exist "%TOOLS%" mkdir "%TOOLS%"
if not exist "%ADDON%\.git" (
  echo Preuzimam open-source Blender dodatak u Tools\third_party...
  git clone https://github.com/mauserzjeh/cod-asset-importer "%ADDON%"
  if errorlevel 1 (
    echo Git clone nije uspeo. Proveri internet i GitHub pristup.
    pause
    exit /b 1
  )
) else (
  echo Postojeca lokalna kopija dodatka je sacuvana; necu je prepisivati.
)

pushd "%ADDON%"
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
  echo Build Blender dodatka nije uspeo. Proveri Python/Rust verzije i log iznad.
  pause
  exit /b 1
)
popd

echo.
echo Dodatak je lokalno izgradjen.
echo 1. Otvori Blender.
echo 2. Edit ^> Preferences ^> Add-ons ^> Install from Disk.
echo 3. Izaberi ZIP iz foldera:
echo    %ADDON%\release
echo 4. Omoguci dodatak, pa File ^> Import ^> CoD Asset Importer ^> Import model.
echo 5. Izaberi XModel iz Build\LocalCOD2Extraction\xmodel (ili XAnim za animacije).
echo.
echo COD2 sadrzaj ostaje lokalan; nemoj ga uploadovati u javni GitHub repo.
pause
endlocal
