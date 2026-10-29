@echo off
setlocal
set "ROOT=%~dp0"
where python >nul 2>nul
if errorlevel 1 (
  echo Python nije pronadjen. Instaliraj Python 3.10+ ili ga ukljuci u PATH.
  pause
  exit /b 1
)
if "%~1"=="" (
  python "%ROOT%Scripts\extract_cod2_assets.py"
) else (
  python "%ROOT%Scripts\extract_cod2_assets.py" --game-root "%~1"
)
if errorlevel 1 (
  echo Ekstrakcija nije uspela. Proveri da li je putanja do COD2 instalacije ispravna.
  pause
  exit /b 1
)
pause
endlocal
