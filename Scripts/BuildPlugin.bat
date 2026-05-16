@echo off
setlocal enabledelayedexpansion

set SCRIPT_DIR=%~dp0
set PLUGIN_ROOT=%SCRIPT_DIR%..
set PLUGIN=%PLUGIN_ROOT%\TypeTween.uplugin
set OUTPUT_ROOT=%PLUGIN_ROOT%\Build
set LOG_FILE=%OUTPUT_ROOT%\build_log.txt

if not exist "%OUTPUT_ROOT%" mkdir "%OUTPUT_ROOT%"

echo Build started %date% %time% > "%LOG_FILE%"
echo. >> "%LOG_FILE%"

REM ============================================================
REM  Build prebuilt binary packages per engine version
REM ============================================================
for %%V in (5.5 5.6 5.7) do (
    set "ENGINE_DIR="
    for /f "tokens=2*" %%A in (
        'reg query "HKLM\SOFTWARE\EpicGames\Unreal Engine\%%V" /v InstalledDirectory 2^>nul'
    ) do set "ENGINE_DIR=%%B"

    if defined ENGINE_DIR (
        echo ================================
        echo Building for UE %%V...
        echo Engine: !ENGINE_DIR!
        echo ================================
        echo [UE %%V] Building with engine: !ENGINE_DIR! >> "%LOG_FILE%"
        echo ------------------------------------------------ >> "%LOG_FILE%"

        call "!ENGINE_DIR!\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin ^
            -Plugin="!PLUGIN!" ^
            -Package="!OUTPUT_ROOT!\TypeTween_v1.0_UE%%V_Binaries" ^
            -Rocket >> "%LOG_FILE%" 2>&1

        set ERR=!ERRORLEVEL!
        echo ------------------------------------------------ >> "%LOG_FILE%"

        if !ERR! NEQ 0 (
            echo [ERROR] UE %%V FAILED with code !ERR!
            echo [UE %%V] FAILED with code !ERR! >> "%LOG_FILE%"
        ) else (
            echo [OK] UE %%V build succeeded.
            echo [UE %%V] SUCCESS >> "%LOG_FILE%"

            echo Cleaning intermediate files for UE %%V...
            rd /s /q "!OUTPUT_ROOT!\TypeTween_v1.0_UE%%V_Binaries\Intermediate"
            echo [UE %%V] Intermediate cleaned >> "%LOG_FILE%"

            REM ------------------------------------------------
            REM  Zip the Binaries folder
            REM ------------------------------------------------
            echo Zipping Binaries for UE %%V...
            set "BIN_ZIP=!OUTPUT_ROOT!\TypeTween_v1.0_UE%%V_Binaries.zip"
            if exist "!BIN_ZIP!" del /f /q "!BIN_ZIP!"
            powershell -NoProfile -Command "Compress-Archive -Path '!OUTPUT_ROOT!\TypeTween_v1.0_UE%%V_Binaries' -DestinationPath '!BIN_ZIP!'"
            if !ERRORLEVEL! NEQ 0 (
                echo [WARN] Zip failed for Binaries UE %%V
                echo [UE %%V] Binaries zip FAILED >> "%LOG_FILE%"
            ) else (
                echo [OK] Binaries zip created: TypeTween_v1.0_UE%%V_Binaries.zip
                echo [UE %%V] Binaries zip created >> "%LOG_FILE%"
            )

            REM ------------------------------------------------
            REM  Create Source copy by duplicating the build
            REM  and stripping the Binaries folder
            REM ------------------------------------------------
            echo Creating Source copy for UE %%V...
            echo [UE %%V] Creating source copy >> "%LOG_FILE%"

            set SOURCE_OUT=!OUTPUT_ROOT!\TypeTween_v1.0_UE%%V_Source

            if exist "!SOURCE_OUT!" rd /s /q "!SOURCE_OUT!"
            xcopy /e /i /q "!OUTPUT_ROOT!\TypeTween_v1.0_UE%%V_Binaries" "!SOURCE_OUT!" >> "%LOG_FILE%"
            rd /s /q "!SOURCE_OUT!\Binaries"

            echo [OK] Source copy created at: !SOURCE_OUT!
            echo [UE %%V] Source copy created >> "%LOG_FILE%"

            REM ------------------------------------------------
            REM  Zip the Source folder
            REM ------------------------------------------------
            echo Zipping Source for UE %%V...
            set "SRC_ZIP=!OUTPUT_ROOT!\TypeTween_v1.0_UE%%V_Source.zip"
            if exist "!SRC_ZIP!" del /f /q "!SRC_ZIP!"
            powershell -NoProfile -Command "Compress-Archive -Path '!OUTPUT_ROOT!\TypeTween_v1.0_UE%%V_Source' -DestinationPath '!SRC_ZIP!'"
            if !ERRORLEVEL! NEQ 0 (
                echo [WARN] Zip failed for Source UE %%V
                echo [UE %%V] Source zip FAILED >> "%LOG_FILE%"
            ) else (
                echo [OK] Source zip created: TypeTween_v1.0_UE%%V_Source.zip
                echo [UE %%V] Source zip created >> "%LOG_FILE%"
            )
        )
        echo. >> "%LOG_FILE%"
        echo.
    ) else (
        echo [SKIPPED] UE %%V not found in registry.
        echo [UE %%V] SKIPPED - not installed >> "%LOG_FILE%"
        echo.
    )
)

echo ================================
echo Done! Output folder structure:
echo.
echo   Build\
echo   ├── TypeTween_v1.0_UE5.5_Binaries\       ^<-- GitHub Release, Option A
echo   ├── TypeTween_v1.0_UE5.5_Binaries.zip    ^<-- ready to upload
echo   ├── TypeTween_v1.0_UE5.5_Source\          ^<-- Fab submission folder
echo   ├── TypeTween_v1.0_UE5.5_Source.zip       ^<-- ready to upload
echo   ├── TypeTween_v1.0_UE5.6_Binaries\
echo   ├── TypeTween_v1.0_UE5.6_Binaries.zip
echo   ├── TypeTween_v1.0_UE5.6_Source\
echo   ├── TypeTween_v1.0_UE5.6_Source.zip
echo   ├── TypeTween_v1.0_UE5.7_Binaries\
echo   ├── TypeTween_v1.0_UE5.7_Binaries.zip
echo   ├── TypeTween_v1.0_UE5.7_Source\
echo   └── TypeTween_v1.0_UE5.7_Source.zip
echo.
echo Open build_log.txt for details:
echo %LOG_FILE%
echo ================================
pause
endlocal