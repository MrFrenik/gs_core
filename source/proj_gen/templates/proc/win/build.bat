@echo off
REM Build Script for %APP%
REM 
REM Usage:
REM   build.bat [options]
REM
REM Options:
REM   -dev              Build in dev mode (optimized with debug info) [default]
REM   -rel              Build in release mode (fully optimized)
REM   -dbg              Build in debug mode (no optimization)
REM   -refl             Build reflection.exe in gs_core
REM   -core             Build gs_core lib using provided optimization level
REM   -all              Build all: refl, core, and the app
REM   -run              Run the application after building
REM   -timeout N        Auto-close application after N seconds
REM   -help, --help, /? Show this help message
REM
REM Examples:
REM   build.bat                      Build app in dev mode (no run)
REM   build.bat -rel -run            Build app in release mode and run
REM   build.bat -refl                Build reflection.exe only
REM   build.bat -core -rel           Build gs_core in release mode
REM   build.bat -all -rel            Build everything in release mode (no run)
REM   build.bat -rel -run -timeout 5 Build app (release), run, auto-close after 5 seconds

REM Handle UNC paths immediately by pushing to the script directory
pushd "%~dp0" >nul 2>&1

setlocal enabledelayedexpansion

REM Check for help request
if /I "%1"=="-help" goto :show_help
if /I "%1"=="--help" goto :show_help
if /I "%1"=="/?" goto :show_help
if /I "%1"=="/h" goto :show_help

REM Parse arguments
set "PROJ_NAME=%APP%"
set "CONFIG=dev"
set "TIMEOUT_SECS="
set "BUILD_REFL=0"
set "BUILD_CORE=0"
set "BUILD_ALL=0"
set "BUILD_APP=1"
set "RUN_APP=0"
set "SANITIZE=0"

:parse_args
if "%1"=="" goto :args_done
if /I "%1"=="-rel" (
    set "CONFIG=rel"
    shift
    goto :parse_args
)
if /I "%1"=="-dbg" (
    set "CONFIG=dbg"
    shift
    goto :parse_args
)
if /I "%1"=="-dev" (
    set "CONFIG=dev"
    shift
    goto :parse_args
)
if /I "%1"=="-refl" (
    set "BUILD_REFL=1"
    set "BUILD_APP=0"
    shift
    goto :parse_args
)
if /I "%1"=="-core" (
    set "BUILD_CORE=1"
    set "BUILD_APP=0"
    shift
    goto :parse_args
)
if /I "%1"=="-all" (
    set "BUILD_ALL=1"
    set "BUILD_APP=1"
    shift
    goto :parse_args
)
if /I "%1"=="-run" (
    set "RUN_APP=1"
    shift
    goto :parse_args
)
if /I "%1"=="--sanitize" (
    set "SANITIZE=1"
    shift
    goto :parse_args
)
if /I "%1"=="-timeout" (
    set "TIMEOUT_SECS=%2"
    shift
    shift
    goto :parse_args
)
echo Warning: Unknown parameter "%1"
shift
goto :parse_args

:args_done

REM Get the directory where this script is located
set "ROOT_DIR=%~dp0"

REM gs_core path is substituted at generation time via %GS_CORE_DIR% token
set "GS_CORE_DIR=%GS_CORE_DIR%"

REM Find Visual Studio installation
set "VSVARS_PATH="
for %%V in (2019 2022 2017) do (
    for %%E in (Community Professional Enterprise) do (
        if exist "C:\Program Files (x86)\Microsoft Visual Studio\%%V\%%E\VC\Auxiliary\Build\vcvars64.bat" (
            set "VSVARS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\%%V\%%E\VC\Auxiliary\Build\vcvars64.bat"
            echo Found Visual Studio %%V %%E
            goto :found_vs
        )
    )
)

REM Check VS 2022 in Program Files (not x86)
for %%E in (Community Professional Enterprise) do (
    if exist "C:\Program Files\Microsoft Visual Studio\2022\%%E\VC\Auxiliary\Build\vcvars64.bat" (
        set "VSVARS_PATH=C:\Program Files\Microsoft Visual Studio\2022\%%E\VC\Auxiliary\Build\vcvars64.bat"
        echo Found Visual Studio 2022 %%E
        goto :found_vs
    )
)

echo ERROR: Visual Studio not found. Checked versions 2017, 2019, 2022.
exit /b 1

:found_vs

REM Initialize Visual Studio environment
call "%VSVARS_PATH%" >nul

REM Start timing
set "START_TIME=%TIME%"

REM Handle -all flag
if "%BUILD_ALL%"=="1" (
    set "BUILD_REFL=1"
    set "BUILD_CORE=1"
    set "BUILD_APP=1"
)

REM Build reflection if requested
if "%BUILD_REFL%"=="1" (
    echo ========================================
    echo Building reflection.exe...
    echo ========================================
    set "SAVED_DIR=%CD%"
    taskkill /F /IM reflection.exe >nul 2>&1
    pushd "%GS_CORE_DIR%" >nul 2>&1
    if !ERRORLEVEL! EQU 0 (
        call "proc\win\refl_cl.bat"
        set REFL_EXIT_CODE=!ERRORLEVEL!
        popd
    ) else (
        echo ERROR: Cannot pushd to gs_core at %GS_CORE_DIR%
        exit /b 1
    )
    cd /d "!SAVED_DIR!"
    timeout /t 1 /nobreak >nul 2>&1
    set "REFL_EXE=%GS_CORE_DIR%bin\reflection\reflection.exe"
    if not exist "!REFL_EXE!" (
        echo ERROR: Reflection build failed - executable not found at: !REFL_EXE!
        exit /b 1
    )
    if !REFL_EXIT_CODE! NEQ 0 (
        echo Warning: Reflection build script returned non-zero exit code: !REFL_EXIT_CODE!
        echo However, reflection.exe was created successfully.
    )
    echo Reflection build completed successfully!
)

REM Build gs_core if requested
if "%BUILD_CORE%"=="1" (
    echo ========================================
    echo Building gs_core library ^(%CONFIG% configuration^)...
    echo ========================================
    REM Map dev to rel for gs_core (it doesn't have a dev config)
    set "CORE_CONFIG=%CONFIG%"
    if "%CONFIG%"=="dev" set "CORE_CONFIG=rel"
    set "SAVED_DIR=%CD%"
    pushd "%GS_CORE_DIR%" >nul 2>&1
    if !ERRORLEVEL! EQU 0 (
        call "proc\win\cl.bat" !CORE_CONFIG! !SANITIZE!
        set CORE_EXIT_CODE=!ERRORLEVEL!
        popd
    ) else (
        echo ERROR: Cannot pushd to gs_core at %GS_CORE_DIR%
        exit /b 1
    )
    cd /d "!SAVED_DIR!"
    timeout /t 1 /nobreak >nul 2>&1
    if "%CONFIG%"=="dbg" (
        set "CORE_LIB=%GS_CORE_DIR%bin\core\gs_core_d.lib"
    ) else (
        set "CORE_LIB=%GS_CORE_DIR%bin\core\gs_core.lib"
    )
    if not exist "!CORE_LIB!" (
        echo ERROR: gs_core build failed - library not found at: !CORE_LIB!
        exit /b 1
    )
    if !CORE_EXIT_CODE! NEQ 0 (
        echo Warning: gs_core build script returned non-zero exit code: !CORE_EXIT_CODE!
        echo However, gs_core library was created successfully.
    )
    echo gs_core build completed successfully!
)

REM Build application if requested
if "%BUILD_APP%"=="1" (
    echo =========%PROJ_NAME%=========================
    echo Building %PROJ_NAME% application ^(%CONFIG% configuration^)...
    echo ========================================
    
    set "BUILD_SCRIPT=%ROOT_DIR%\proc\win\cl.bat"
    if not exist "!BUILD_SCRIPT!" (
        echo ERROR: Build script not found at: !BUILD_SCRIPT!
        exit /b 1
    )

    echo Starting compilation...
    REM Ensure we're in the root directory before calling the build script
    cd /d "%ROOT_DIR%"
    call "!BUILD_SCRIPT!" %CONFIG% !SANITIZE!
    set BUILD_EXIT_CODE=!ERRORLEVEL!

    REM Check if executable exists (this is the real test of build success)
    set "EXE_PATH=%ROOT_DIR%bin\%PROJ_NAME%\%PROJ_NAME%.exe"
    if not exist "!EXE_PATH!" (
        echo Build failed - executable not found at: !EXE_PATH!
        echo Build script returned exit code: !BUILD_EXIT_CODE!
        exit /b 1
    )

    if !BUILD_EXIT_CODE! NEQ 0 (
        echo Warning: Build script returned non-zero exit code: !BUILD_EXIT_CODE!
        echo However, executable was created successfully.
    )

    echo Build completed successfully!

    REM If sanitization is enabled, ensure ASan runtime DLL is next to the executable
    if "%SANITIZE%"=="1" (
        echo Sanitized build detected - copying AddressSanitizer runtime DLL...
        set "ASAN_DLL=clang_rt.asan_dynamic-x86_64.dll"
        set "ASAN_DBG_DLL=clang_rt.asan_dbg_dynamic-x86_64.dll"

        REM Probe common VS 2022 locations for the ASan runtime
        set "ASAN_DIR="
        for /D %%E in ("C:\Program Files\Microsoft Visual Studio\2022\*") do (
            for /D %%T in ("%%~fE\VC\Tools\MSVC\*") do (
                if exist "%%~fT\bin\Hostx64\x64\%ASAN_DLL%" (
                    set "ASAN_DIR=%%~fT\bin\Hostx64\x64"
                )
            )
        )

        if defined ASAN_DIR (
            echo   Found ASan runtime at: !ASAN_DIR!
            REM Copy only the essential ASan runtime DLLs and PDBs
            copy /Y "!ASAN_DIR!\%ASAN_DLL%" "%ROOT_DIR%bin\%PROJ_NAME%\" >nul 2>&1
            copy /Y "!ASAN_DIR!\%ASAN_DBG_DLL%" "%ROOT_DIR%bin\%PROJ_NAME%\" >nul 2>&1
            if exist "!ASAN_DIR!\clang_rt.asan_dynamic-x86_64.pdb" copy /Y "!ASAN_DIR!\clang_rt.asan_dynamic-x86_64.pdb" "%ROOT_DIR%bin\%PROJ_NAME%\" >nul 2>&1
            if exist "!ASAN_DIR!\clang_rt.asan_dbg_dynamic-x86_64.pdb" copy /Y "!ASAN_DIR!\clang_rt.asan_dbg_dynamic-x86_64.pdb" "%ROOT_DIR%bin\%PROJ_NAME%\" >nul 2>&1
            if not exist "%ROOT_DIR%bin\%PROJ_NAME%\%ASAN_DLL%" (
                echo   Warning: Failed to copy ASan DLL next to executable.
            ) else (
                echo   ASan runtime copied to application directory.
            )
        ) else (
            echo   Warning: Could not locate %ASAN_DLL% automatically.
            echo   Please copy it manually next to %PROJ_NAME%.exe if the runtime complains.
        )
    )

    REM Run application if requested
    if "!RUN_APP!"=="1" (
        echo Launching application...
        echo Executable: !EXE_PATH!
        if not "!TIMEOUT_SECS!"=="" (
            echo Timeout: !TIMEOUT_SECS! seconds
        )
        echo ----------------------------------------

        REM Run the executable from bin\%PROJ_NAME% directory
        pushd "bin\%PROJ_NAME%"

        if not "!TIMEOUT_SECS!"=="" (
            REM Run with timeout - use start with /wait and taskkill
            start "" /B %PROJ_NAME%.exe
            set "APP_PID="
            for /f "tokens=2" %%a in ('tasklist /FI "IMAGENAME eq %PROJ_NAME%.exe" /FO LIST ^| find "PID:"') do set "APP_PID=%%a"
            
            if defined APP_PID (
                echo Application started with PID: !APP_PID!
                timeout /t !TIMEOUT_SECS! /nobreak >nul
                echo Timeout reached - terminating application...
                taskkill /PID !APP_PID! /F >nul 2>&1
                set RUN_EXIT_CODE=0
            ) else (
                echo Warning: Could not determine application PID
                %PROJ_NAME%.exe
                set RUN_EXIT_CODE=!ERRORLEVEL!
            )
        ) else (
            REM Run normally without timeout
            %PROJ_NAME%.exe
            set RUN_EXIT_CODE=!ERRORLEVEL!
        )

        popd

        echo ----------------------------------------
        echo Application exited with code: !RUN_EXIT_CODE!
    )
)

REM Final popd to restore original directory
popd

REM Calculate elapsed time
set "END_TIME=%TIME%"

REM Convert times to centiseconds for calculation
for /f "tokens=1-4 delims=:., " %%a in ("%START_TIME%") do (
    set /a "start_cs=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)
for /f "tokens=1-4 delims=:., " %%a in ("%END_TIME%") do (
    set /a "end_cs=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)

set /a "elapsed_cs=end_cs-start_cs"
if !elapsed_cs! lss 0 set /a "elapsed_cs=8640000+elapsed_cs"

set /a "elapsed_secs=elapsed_cs/100"
set /a "elapsed_mins=elapsed_secs/60"
set /a "elapsed_secs_remainder=elapsed_secs%%60"
set /a "elapsed_ms=(elapsed_cs%%100)*10"

echo ========================================
echo Build complete!
if !elapsed_mins! gtr 0 (
    echo Total build time: !elapsed_mins!m !elapsed_secs_remainder!.!elapsed_ms!s
) else (
    echo Total build time: !elapsed_secs_remainder!.!elapsed_ms!s
)
echo ========================================

exit /b 0

:show_help
echo.
echo Build Script for %APP%
echo.
echo Usage:
echo   build.bat [options]
echo.
echo Options:
echo   -dev              Build in dev mode (optimized with debug info) [default]
echo   -rel              Build in release mode (fully optimized)
echo   -dbg              Build in debug mode (no optimization)
echo   -refl             Build reflection.exe in gs_core
echo   -core             Build gs_core lib using provided optimization level
echo   -all              Build all: refl, core, and the app
echo   -run              Run the application after building
echo   -timeout N        Auto-close application after N seconds
echo   --sanitize        Enable AddressSanitizer instrumentation (if supported)
echo   -help, --help, /? Show this help message
echo.
echo Examples:
echo   build.bat                      Build app in dev mode (no run)
echo   build.bat -rel -run            Build app in release mode and run
echo   build.bat -refl                Build reflection.exe only
echo   build.bat -core -rel           Build gs_core in release mode
echo   build.bat -all -rel            Build everything in release mode (no run)
echo   build.bat -rel -run -timeout 5 Build app (release), run, auto-close after 5 seconds
echo.
exit /b 0
