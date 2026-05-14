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
            -Package="!OUTPUT_ROOT!\CommandPalette_UE%%V" ^
            -Rocket >> "%LOG_FILE%" 2>&1

        set ERR=!ERRORLEVEL!
        echo ------------------------------------------------ >> "%LOG_FILE%"

        if !ERR! NEQ 0 (
            echo [ERROR] UE %%V FAILED with code !ERR!
            echo [UE %%V] FAILED with code !ERR! >> "%LOG_FILE%"
        ) else (
            echo [OK] UE %%V succeeded.
            echo [UE %%V] SUCCESS >> "%LOG_FILE%"

            echo Cleaning intermediate files for UE %%V...
            rd /s /q "!OUTPUT_ROOT!\CommandPalette_UE%%V\Intermediate"
            echo [UE %%V] Intermediate cleaned >> "%LOG_FILE%"
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
echo Done! Open build_log.txt for details:
echo %LOG_FILE%
echo ================================
pause
endlocal