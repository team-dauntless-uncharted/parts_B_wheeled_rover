@echo off
REM --- 設定 ---
set BOARD=SPRESENSE:spresense:spresense
set SKETCH=%~dp0\100kinsat_neo.ino
set LIBRARIES=%~dp0\libraries
set EXTRA_FLAGS=-Isrc
set CPP_FLAGS=-std=gnu++14
set BAUDRATE=115200

REM --- 引数 ---
if "%1"=="" set ACTION=compile
if not "%1"=="" set ACTION=%1

REM --- ポート自動検出 ---
for /f "tokens=1" %%i in ('arduino-cli board list ^| find "SPRESENSE"') do set PORT=%%i

if "%PORT%"=="" (
    if "%ACTION%"=="upload" (
        echo [ERROR] SPRESENSE board not found!
        exit /b 1
    )
)

REM --- サブコマンド ---
if "%ACTION%"=="compile" (
    echo Compiling sketch...
    arduino-cli compile --fqbn %BOARD% --libraries %LIBRARIES% --build-property build.extra_flags="%EXTRA_FLAGS%" --build-property compiler.cpp.extra_flags="%CPP_FLAGS%" %SKETCH%
    if errorlevel 1 exit /b 1
)

if "%ACTION%"=="upload" (
    echo Uploading sketch...
    arduino-cli upload --fqbn %BOARD% -p %PORT% %SKETCH%
    if errorlevel 1 exit /b 1
)

if "%ACTION%"=="monitor" (
    echo Starting serial monitor...
    arduino-cli monitor -p %PORT% -c baudrate=%BAUDRATE%
)
