@echo off
REM ============================================================================
rem proj_gen Build Script
rem Compiles proj_gen.exe from source\proj_gen\proj_gen.c
rem ============================================================================

setlocal enabledelayedexpansion

REM More robust directory cleanup with retry
if exist bin\proj_gen (
    rmdir /Q /S bin\proj_gen 2>nul
    if exist bin\proj_gen (
        timeout /t 1 /nobreak >nul 2>&1
        rmdir /Q /S bin\proj_gen 2>nul
    )
)
if not exist bin\proj_gen mkdir bin\proj_gen
pushd bin\proj_gen

rem Name 
set name=proj_gen

rem Include directories 
set inc=/I ..\..\third_party\include\

rem Source files
set src_main=..\..\source\proj_gen\proj_gen.c

rem All source together
set src_all=%src_main%

rem OS Libraries
set os_libs=opengl32.lib kernel32.lib user32.lib ^
shell32.lib vcruntime.lib msvcrt.lib gdi32.lib Winmm.lib Advapi32.lib 

echo [pgen] Compiling proj_gen.exe...
cl /w /MTd /MP -Zi -D _WINSOCKAPI_ /DEBUG:FULL /Fe%name%.exe ^
%src_all% %inc% /EHsc /link /SUBSYSTEM:CONSOLE /NODEFAULTLIB:libcmtd.lib ^
/NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:libcmtd.lib ^
%os_libs%

if errorlevel 1 (
    echo [ERROR] Failed to compile proj_gen.exe.
    exit /b 1
)

echo [OK] proj_gen.exe compiled successfully.

popd
endlocal
