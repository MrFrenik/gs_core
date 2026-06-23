@echo off
REM Wrapper to build proj_gen with VS environment
setlocal enabledelayedexpansion

REM Find Visual Studio
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
for %%E in (Community Professional Enterprise) do (
    if exist "C:\Program Files\Microsoft Visual Studio\2022\%%E\VC\Auxiliary\Build\vcvars64.bat" (
        set "VSVARS_PATH=C:\Program Files\Microsoft Visual Studio\2022\%%E\VC\Auxiliary\Build\vcvars64.bat"
        echo Found Visual Studio 2022 %%E
        goto :found_vs
    )
)
echo ERROR: Visual Studio not found
exit /b 1

:found_vs
call "%VSVARS_PATH%" >nul 2>&1
echo Initializing VS environment...
echo Building proj_gen...
call proc\win\pgen_cl.bat
