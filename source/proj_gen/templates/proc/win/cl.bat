@echo off
rmdir /Q /S bin\%APP%
mkdir bin\%APP%
pushd bin\%APP%

rem GS_Core
set gs_core=..\..\..\gs_core

rem Name
set proj_name=%APP%

rem Root Dir
set root=..\..\

rem Include directories 
set inc=/I %gs_core%\third_party\include\ /I %root%\source\ /I %gs_core%\source\

rem Source files
set src_main=%root%\source\%proj_name%_unity.c

rem All source together
set src_all=%src_main%

rem OS Libraries
set os_libs=opengl32.lib kernel32.lib user32.lib ^
shell32.lib vcruntime.lib msvcrt.lib gdi32.lib Winmm.lib Advapi32.lib 

rem Third Party Libs
set tp_libs_dbg=%gs_core%\third_party\libs\win\dbg\Bullet3Collision_Debug.lib ^
    %gs_core%\third_party\libs\win\dbg\Bullet3Common_Debug.lib ^
    %gs_core%\third_party\libs\win\dbg\Bullet3Dynamics_Debug.lib ^
    %gs_core%\third_party\libs\win\dbg\Bullet3Geometry_Debug.lib ^
    %gs_core%\third_party\libs\win\dbg\BulletDynamics_Debug.lib ^
    %gs_core%\third_party\libs\win\dbg\BulletCollision_Debug.lib ^
    %gs_core%\third_party\libs\win\dbg\LinearMath_Debug.lib ^
    %gs_core%\third_party\libs\win\dbg\enet.lib ^
    %gs_core%\third_party\libs\win\dbg\Recast-d.lib ^
    %gs_core%\third_party\libs\win\dbg\Detour-d.lib ^
    %gs_core%\third_party\libs\win\dbg\DetourCrowd-d.lib ^
    %gs_core%\third_party\libs\win\dbg\DetourTileCache-d.lib ^
    %gs_core%\bin\core\gs_core_d.lib

set tp_libs=%gs_core%\third_party\libs\win\rel\Bullet3Collision.lib ^
    %gs_core%\third_party\libs\win\rel\Bullet3Common.lib ^
    %gs_core%\third_party\libs\win\rel\Bullet3Dynamics.lib ^
    %gs_core%\third_party\libs\win\rel\Bullet3Geometry.lib ^
    %gs_core%\third_party\libs\win\rel\BulletDynamics.lib ^
    %gs_core%\third_party\libs\win\rel\BulletCollision.lib ^
    %gs_core%\third_party\libs\win\rel\LinearMath.lib ^
    %gs_core%\third_party\libs\win\rel\enet.lib ^
    %gs_core%\bin\core\gs_core.lib
    rem %gs_core%\third_party\libs\win\dbg\Recast.lib ^
    rem %gs_core%\third_party\libs\win\dbg\Detour.lib ^
    rem %gs_core%\third_party\libs\win\dbg\DetourCrowd.lib ^
    rem %gs_core%\third_party\libs\win\dbg\DetourTileCache.lib ^

rem Link options
set l_options=/EHsc /link /SUBSYSTEM:CONSOLE /NODEFAULTLIB:msvcrt.lib 

rem Reflection Dirs
set in_dir="%root%/source" 
set out_dir="%root%/source/generated" 
set dep_dir="%gs_core%/source" 
set id_offset="200"

rem Run Reflection
%gs_core%\bin\reflection\reflection.exe %in_dir% %out_dir% %proj_name% %dep_dir% %id_offset%

if [%1]==[] goto :error
if [%1]==[dbg] goto :dbg 
if [%1]==[rel] goto :rel
goto :error

:rel
    rem Compile Release
    echo Compiling Release...
    cl /w /MT /MP /O2 -D _WINSOCKAPI_ -D GS_CORE_APP_STANDALONE ^
    /Fe%proj_name%.exe %src_all% %inc% /EHsc /link /SUBSYSTEM:CONSOLE ^
    /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcmt.lib ^
    %os_libs% %tp_libs%
    goto :end

:dbg
    rem Compile Debug
    echo Compiling Debug...
    cl /w /MTd /MP -Zi -D _WINSOCKAPI_ -D GS_DEBUG -D GS_CORE_APP_STANDALONE /DEBUG:FULL ^
    /Fe%proj_name%.exe %src_all% %inc% /EHsc /link /SUBSYSTEM:CONSOLE ^
    /NODEFAULTLIB:libcmtd.lib /NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:libcmtd.lib ^
    %os_libs% %tp_libs_dbg%
    goto :end

:error
    echo Configuration missing: 'rel' or 'dbg' 

:end
    popd 
