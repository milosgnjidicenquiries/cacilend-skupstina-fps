@echo off
setlocal EnableExtensions
set "ROOT=%~dp0"
set "EXTRACT=%ROOT%Build\LocalCOD2Extraction"
set "TOOLS=%ROOT%Tools\third_party\xmodelconverter"
set "ZIP=%TOOLS%\xmodelconverter.v24.1.zip"
set "UNPACK=%TOOLS%\unpacked"
set "CONVERTER=%TOOLS%\xmodelconverter.exe"

if "%~1"=="" (
  echo Upotreba: prevuci jedan .xanim_bin fajl iz Build\LocalCOD2Extraction\xanim na ovaj BAT.
  echo Prvi put ce preuzeti open-source COD2 XModel/XAnim Converter.
  pause
  exit /b 2
)
set "INPUT=%~f1"
if not exist "%INPUT%" (
  echo Ulazna datoteka nije pronadjena: %INPUT%
  pause
  exit /b 2
)
for %%F in ("%INPUT%") do set "EXT=%%~xF"
if /i not "%EXT%"==".xanim_bin" (
  echo Ova pomocna skripta konvertuje samo kompajlirane COD2 .xanim_bin animacije.
  echo Za XModel mesh-eve koristi COD Asset Importer Blender dodatak.
  pause
  exit /b 2
)
if not exist "%EXTRACT%\xmodel" (
  echo Ne postoji Build\LocalCOD2Extraction\xmodel.
  echo Prvo pokreni Extract_COD2_Assets.bat uz putanju do svoje lokalne COD2 instalacije.
  pause
  exit /b 2
)

if not exist "%CONVERTER%" (
  if not exist "%TOOLS%" mkdir "%TOOLS%"
  if not exist "%ZIP%" (
    echo Preuzimam open-source xmodelconverter v24.1...
    powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; Invoke-WebRequest -UseBasicParsing -Uri 'https://github.com/riicchhaarrd/xmodelconverter/releases/download/v24.1/xmodelconverter.v24.1.zip' -OutFile '%ZIP%'; if ((Get-Item '%ZIP%').Length -lt 100000) { throw 'Preuzeti ZIP je neocekivano mali.' }"
    if errorlevel 1 (
      echo Preuzimanje nije uspelo. Proveri internet.
      pause
      exit /b 1
    )
  )
  if not exist "%UNPACK%" mkdir "%UNPACK%"
  powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; Expand-Archive -LiteralPath '%ZIP%' -DestinationPath '%UNPACK%' -Force; $exe=Get-ChildItem -LiteralPath '%UNPACK%' -Recurse -Filter 'xmodelconverter.exe' | Select-Object -First 1; if (!$exe) { throw 'xmodelconverter.exe nije pronadjen u release ZIP-u.' }; Copy-Item -LiteralPath $exe.FullName -Destination '%CONVERTER%' -Force"
  if errorlevel 1 (
    echo Raspakivanje converter-a nije uspelo.
    pause
    exit /b 1
  )
)

echo Konvertujem:
echo %INPUT%
echo.
echo Radni folder ce biti Build\LocalCOD2Extraction da bi alat mogao da nadje xmodel/xmodelsurfs zavisnosti.
pushd "%EXTRACT%"
"%CONVERTER%" "%INPUT%"
set "RESULT=%ERRORLEVEL%"
popd
if not "%RESULT%"=="0" (
  echo Konverzija nije uspela. Pogledaj poruke converter-a i proveri da li su pripadajuci xanim/xmodel folderi kompletni.
  pause
  exit /b %RESULT%
)
echo.
echo Konverzija je zavrsena. Potrazi rezultat u Build\LocalCOD2Extraction\exported.
echo XANIM_EXPORT mora biti uvezen kroz Blender COD animation add-on, pa izvezen kao FBX u:
echo SourceArt\Import\Animations\AN_COD2_Idle.fbx ili AN_COD2_Walk.fbx
echo Zadrzi isti skeleton kao SK_COD2_Soldier; inace ce FBX import zahtevati mapiranje skeleton-a.
echo Ne uploaduj originalne COD2 fajlove u javni repo.
pause
endlocal
