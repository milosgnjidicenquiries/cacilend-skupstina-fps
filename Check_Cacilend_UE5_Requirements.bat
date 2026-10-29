@echo off
setlocal
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Scripts\Check_Cacilend_UE5_Requirements.ps1" -ProjectRoot "%~dp0"
set "RESULT=%ERRORLEVEL%"
echo.
echo Izvestaj: "%~dp0Build\UE5-preflight-report.txt"
if not "%RESULT%"=="0" echo Proveri FAIL/WARN stavke iznad i u izvestaju.
pause
exit /b %RESULT%
