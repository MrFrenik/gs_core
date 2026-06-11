@echo off

setlocal EnableExtensions DisableDelayedExpansion

:: ================================================================
:: proj_gen.bat - Project Generator Wrapper (FINAL FIXED VERSION)
:: ================================================================

set "PROJ_NAME=%~1"
if not defined PROJ_NAME (
    echo [ERROR] Missing project name argument.
    exit /b 1
)

set "USER_DIR=%~2"
set "BUILD_ARCH=%~3"
if not defined BUILD_ARCH set "BUILD_ARCH=x64"

:: Save our true working directory BEFORE any pushd changes it
for %%A in (.) do set "_WORKSPACE_ROOT=%%~fA"

:: Validate architecture
set "_valid_arch="
if /i "%BUILD_ARCH%"=="x64" set "_valid_arch=Y"
if /i "%BUILD_ARCH%"=="x86" set "_valid_arch=Y"
if /i "%BUILD_ARCH%"=="arm64" set "_valid_arch=Y"
if not defined _valid_arch (
    echo [ERROR] Invalid arch '%BUILD_ARCH%'. Must be x64, x86, or arm64.
    exit /b 1
)

:: --- Output directory ---
set "PROJECT_DIR="
if defined USER_DIR (
    if not exist "%USER_DIR%" (
        mkdir "%USER_DIR%" >nul 2>&1
        if not exist "%USER_DIR%" (
            echo [ERROR] Cannot create directory: %USER_DIR%
            exit /b 1
        )
        echo [OK] Created directory: %USER_DIR%
    )
    for %%A in ("%USER_DIR%") do set "PROJECT_DIR=%%~fA"
) else (
    for %%A in (".") do set "PROJECT_DIR=%%~fA"
)

:: --- Find vcvarsall.bat ---
set "_vcvarspath="
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "_vcvarspath=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
)
if not defined _vcvarspath if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" (
    set "_vcvarspath=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat"
)
if not defined _vcvarspath if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
    set "_vcvarspath=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"
)
if not defined _vcvarspath if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "_vcvarspath=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
)
if not defined _vcvarspath if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" (
    set "_vcvarspath=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat"
)
if not defined _vcvarspath if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
    set "_vcvarspath=C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"
)

if not defined _vcvarspath (
    echo [ERROR] Could not locate vcvarsall.bat.
    exit /b 1
)

echo [OK] Found vcvarsall.bat: %_vcvarspath%

:: --- Init MSVC environment ---
call "%_vcvarspath%" %BUILD_ARCH% >nul 2>&1
if errorlevel 1 exit /b 1

echo [OK] MSVC environment ready [%BUILD_ARCH%].

:: --- Compile dependencies (sequential) ---

echo [proj_gen] Compiling reflection...
pushd "%_WORKSPACE_ROOT%" >nul 2>&1
call proc\win\refl_cl.bat
if errorlevel 1 goto :compilation_failed

echo [proj_gen] Compiling gs_core (dbg)...
call proc\win\cl.bat dbg
if errorlevel 1 goto :compilation_failed

echo [proj_gen] Compiling proj_gen.exe...
call proc\win\pgen_cl.bat
if errorlevel 1 goto :compilation_failed

:: --- Run project generator ---

echo [proj_gen] Generating project '%PROJ_NAME%' in '%PROJECT_DIR%'
bin\proj_gen\proj_gen.exe -gcs "%CD%" --dir "%PROJECT_DIR%" "%PROJ_NAME%"
if errorlevel 1 exit /b 1

echo [OK] Project generation complete.
exit /b 0

:compilation_failed
echo [ERROR] Compilation failed. See output above.
exit /b 1
