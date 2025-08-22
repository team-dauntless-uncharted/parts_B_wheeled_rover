@echo off
setlocal

:: ボード設定
set BOARD=SPRESENSE:spresense:spresense:Core=Main,Memory=1536
set SKETCH=100kinsat_neo.ino

:: シリアルポート自動検出（最初に一致するものを取得）
for /f "tokens=1" %%a in ('arduino-cli board list ^| findstr "SPRESENSE"') do set PORT=%%a

if "%PORT%"=="" (
    echo [ERROR] SPRESENSE board not found!
    exit /b 1
)

:: サブコマンド選択
if "%1"=="compile" goto compile
if "%1"=="upload"  goto upload
if "%1"=="monitor" goto monitor

echo Usage: build.bat [compile|upload|monitor]
exit /b 1

:compile
echo === Compiling ===
arduino-cli compile ^
    --fqbn %BOARD% ^
    --libraries libraries ^
    --build-property build.extra_flags="-Isrc" ^
    --build-property compiler.cpp.extra_flags="-std=gnu++14" ^
    %SKETCH%
exit /b %errorlevel%

:upload
echo === Uploading ===
arduino-cli upload ^
    --fqbn %BOARD% ^
    -p %PORT% ^
    %SKETCH%
exit /b %errorlevel%

:monitor
echo === Monitoring ===
arduino-cli monitor -p %PORT% -c baudrate=115200
exit /b %errorlevel%
