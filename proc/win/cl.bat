@echo off
REM More robust directory cleanup with retry
if exist bin\core (
    rmdir /Q /S bin\core 2>nul
    if exist bin\core (
        timeout /t 1 /nobreak >nul 2>&1
        rmdir /Q /S bin\core 2>nul
    )
)
if not exist bin\core mkdir bin\core
pushd bin\core

rem Name
set proj_name=gs_core

rem Root
set root=..\..

rem Include directories 
set inc=/I %root%\third_party\include\ /I %root%\source\

rem Source files
set src_main=%root%\source\core\gs_core_unity.c

rem All source together
set src_all=%src_main%

rem OS Libraries
set os_libs=opengl32.lib kernel32.lib user32.lib ^
shell32.lib vcruntime.lib msvcrt.lib gdi32.lib Winmm.lib Advapi32.lib dbghelp.lib 

rem Third Party Libs
set tp_libs_dbg=%root%\third_party\libs\win\dbg\Bullet3Collision_Debug.lib ^
    %root%\third_party\libs\win\dbg\Bullet3Common_Debug.lib ^
    %root%\third_party\libs\win\dbg\Bullet3Dynamics_Debug.lib ^
    %root%\third_party\libs\win\dbg\Bullet3Geometry_Debug.lib ^
    %root%\third_party\libs\win\dbg\BulletDynamics_Debug.lib ^
    %root%\third_party\libs\win\dbg\BulletCollision_Debug.lib ^
    %root%\third_party\libs\win\dbg\LinearMath_Debug.lib ^
    %root%\third_party\libs\win\dbg\enet.lib ^
    %root%\third_party\libs\win\dbg\Recast-d.lib ^
    %root%\third_party\libs\win\dbg\Detour-d.lib ^
    %root%\third_party\libs\win\dbg\DetourCrowd-d.lib ^
    %root%\third_party\libs\win\dbg\DetourTileCache-d.lib ^
    %root%\third_party\libs\win\dbg\libcurl-d.lib ^
    %root%\third_party\libs\win\dbg\cjson-d.lib ^
    %root%\third_party\libs\win\dbg\pcre2-8-staticd.lib

set tp_libs=%root%\third_party\libs\win\rel\Bullet3Collision.lib ^
    %root%\third_party\libs\win\rel\Bullet3Common.lib ^
    %root%\third_party\libs\win\rel\Bullet3Dynamics.lib ^
    %root%\third_party\libs\win\rel\Bullet3Geometry.lib ^
    %root%\third_party\libs\win\rel\BulletDynamics.lib ^
    %root%\third_party\libs\win\rel\BulletCollision.lib ^
    %root%\third_party\libs\win\rel\LinearMath.lib ^
    %root%\third_party\libs\win\rel\enet.lib ^
    %root%\third_party\libs\win\rel\Recast.lib ^
    %root%\third_party\libs\win\rel\Detour.lib ^
    %root%\third_party\libs\win\rel\DetourCrowd.lib ^
    %root%\third_party\libs\win\rel\DetourTileCache.lib ^
    %root%\third_party\libs\win\rel\libcurl.lib ^
    %root%\third_party\libs\win\rel\cjson.lib ^
    %root%\third_party\libs\win\rel\pcre2-8-static.lib

rem Link options
set l_options=/EHsc /link /SUBSYSTEM:CONSOLE /NODEFAULTLIB:msvcrt.lib 

rem Reflection Dir
set refl_dir=%root%\source\core\

rem Out Dir
set out_dir=%root%\source\core\generated\

set lua_dir=%root%\third_party\lua\
set lua_inc=/I %lua_dir%\
set lua_src=%lua_dir%\lua_unity.c

rem Run Reflection
%root%\bin\reflection\reflection.exe %refl_dir% %out_dir% %proj_name%

rem First argument: configuration (dbg/rel), optional second: sanitize flag
set cfg=%1
set sanitize=%2

if [%cfg%]==[] goto :error
if [%cfg%]==[dbg] goto :dbg 
if [%cfg%]==[rel] goto :rel
goto :error

:rel
    echo Compiling Release...
    if [%sanitize%]==[1] (
        echo   AddressSanitizer enabled for Release build
        set extra_c_flags=/fsanitize=address /Zi
    ) else (
        set extra_c_flags=
    )
    rem Compile Cpp objects (rel)
    cl /MT /EHsc /c /w /MP %extra_c_flags% -D CURL_STATICLIB ^
    %inc% %root%\source\core\gs_core_ai.cpp %root%\source\core\gs_core_physics.cpp ^
    %root%\source\core\gs_core_util.cpp /link /NODEFAULTLIB:libcmtd.lib ^
    /NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:libcmtd.lib

    rem Set cpp objects
    set cpp_obj=gs_core_ai.obj gs_core_physics.obj

    rem Compile objects (rel)
    cl /MT /EHsc /c /w /MP %extra_c_flags% /Fd -D _WINSOCKAPI_ -D CURL_STATICLIB ^
    %src_all% %lua_src% %inc% %lua_inc% ^
    /link /NODEFAULTLIB:libcmtd.lib ^
    /NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:libcmtd.lib %tp_libs% 

    rem Compile static lib (rel)
    lib *obj /out:%proj_name%.lib

    goto :end

:dbg

    echo Compiling Debug...
    if [%sanitize%]==[1] (
        echo   AddressSanitizer enabled for Debug build
        set extra_c_flags=/fsanitize=address /Zi
    ) else (
        set extra_c_flags=
    )
    rem Compile Cpp objects (dbg)
    cl /std:c++17 /MTd /EHsc /c /w /MP %extra_c_flags% -D CURL_STATICLIB ^
    %inc% %root%\source\core\gs_core_ai.cpp %root%\source\core\gs_core_physics.cpp ^
    %root%\source\core\gs_core_util.cpp /link /NODEFAULTLIB:libcmtd.lib ^
    /NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:libcmtd.lib 

    rem Compile objects (dbg)
    cl /MTd /EHsc /c /w /MP %extra_c_flags% /Fd -D _WINSOCKAPI_ -D GS_DEBUG -D CURL_STATICLIB ^
    %src_all% %lua_src% %inc% %lua_inc% /DEBUG ^
    /link /NODEFAULTLIB:libcmtd.lib ^
    /NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:libcmtd.lib 

    rem Compile static lib (dbg)
    lib *obj /out:%proj_name%_d.lib 

    goto :end

:error
    echo Configuration missing: 'rel' or 'dbg' 

:end
    popd 
