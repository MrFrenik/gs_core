@echo off
rmdir /Q /S bin\%APP%
mkdir bin\%APP%
pushd bin\%APP%

rem === gs_core and project root (self-contained, relative paths) ===
set "root=%ROOT_DIR%"
set "gs_core=%GS_CORE_DIR%"

rem Name
set proj_name=%APP%

rem Root Dir
rem (resolved dynamically above)

rem Include directories 
set inc=/I %gs_core%\third_party\include\ /I %root%\source\app\ /I %gs_core%\source\

rem Source files
set src_main=%root%\source\%proj_name%_unity.c

rem All source together
set src_all=%src_main%

rem OS Libraries
set os_libs=opengl32.lib kernel32.lib user32.lib ^
shell32.lib vcruntime.lib msvcrt.lib gdi32.lib Winmm.lib Advapi32.lib ^
dbghelp.lib ws2_32.lib crypt32.lib wldap32.lib iphlpapi.lib secur32.lib 

rem Third Party Libs
set tp_libs_dbg=%gs_core%\third_party\libs\win\dbg\Bullet3Collision_Debug.lib ^
    %gs_core%\third_party\libs\win\dbg\Bullet3Common_Debug.lib ^
    %gs_core%\third_party\libs\win\dbg\Bullet3Dynamics_Debug.lib ^
    %gs_core%\third_party\libs\win\dbg\Bullet3Geometry_Debug.lib ^
    %gs_core%\third_party\libs\win\dbg\BulletDynamics_Debug.lib ^
    %gs_core%\third_party\libs\win\dbg\BulletCollision_Debug.lib ^
    %gs_core%\third_party\libs\win\dbg\LinearMath_Debug.lib ^
    %gs_core%\third_party\libs\win\dbg\enet.lib ^
    %gs_core%\third_party\libs\win\dbg\cjson-d.lib ^
    %gs_core%\third_party\libs\win\dbg\libcurl-d.lib ^
    %gs_core%\third_party\libs\win\dbg\pcre2-8-staticd.lib ^
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
    %gs_core%\third_party\libs\win\rel\cjson.lib ^
    %gs_core%\third_party\libs\win\rel\libcurl.lib ^
    %gs_core%\third_party\libs\win\rel\pcre2-8-static.lib ^
    %gs_core%\bin\core\gs_core.lib
    rem %gs_core%\third_party\libs\win\dbg\Recast.lib ^
    rem %gs_core%\third_party\libs\win\dbg\Detour.lib ^
    rem %gs_core%\third_party\libs\win\dbg\DetourCrowd.lib ^
    rem %gs_core%\third_party\libs\win\dbg\DetourTileCache.lib ^

rem Reflection Dirs
set in_dir="%root%/source" 
set out_dir="%root%/source/generated" 
set dep_dir="%gs_core%/source" 
set id_offset="1000"

rem Run Reflection
%gs_core%\bin\reflection\reflection.exe %in_dir% %out_dir% %proj_name% %dep_dir% %id_offset%

if [%1]==[] goto :error
if [%1]==[dbg] goto :dbg 
if [%1]==[rel] goto :rel
if [%1]==[dev] goto :dev
goto :error

:rel
    rem Compile Release
    echo Compiling Release...
    set "extra_c_flags=/Zi"
    set "extra_link_flags=/IGNORE:4217 /IGNORE:4099"
    if [%2]==[1] (
        echo   AddressSanitizer enabled for Release build
        set "extra_c_flags=/fsanitize=address /Zi"
        set "extra_link_flags=/FORCE:MULTIPLE /IGNORE:4217"
    )
    cl /w /MT /MP /O2 %extra_c_flags% /DUMKA_BUILD /openmp -D _WINSOCKAPI_ -D GS_CORE_APP_STANDALONE -D CURL_STATICLIB ^
    /Fe%proj_name%.exe %src_all% %inc% /EHsc /link /SUBSYSTEM:CONSOLE /DEBUG ^
    /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcmt.lib ^
    %extra_link_flags% ^
    %os_libs% %tp_libs%
    goto :end

:dbg
    rem Compile Debug
    echo Compiling Debug...
    set "extra_c_flags=/Zi"
    set "extra_link_flags=/IGNORE:4217 /IGNORE:4099"
    if [%2]==[1] (
        echo   AddressSanitizer enabled for Debug build
        set "extra_c_flags=/fsanitize=address /Zi"
        set "extra_link_flags=/FORCE:MULTIPLE /IGNORE:4217"
    )
    cl /w /MTd /MP %extra_c_flags% /Ob0 /Oy- -D _WINSOCKAPI_ -D GS_DEBUG -D GS_CORE_APP_STANDALONE -D CURL_STATICLIB /DEBUG:FULL ^
    /Fe%proj_name%.exe %src_all% %inc% /EHsc /link /SUBSYSTEM:CONSOLE ^
    /NODEFAULTLIB:libcmtd.lib /NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:library ^
    %extra_link_flags% ^
    %os_libs% %tp_libs_dbg%
    goto :end

:dev
    rem Compile Dev
    echo Compiling Dev...
    set "extra_c_flags=/Zi"
    set "extra_link_flags=/IGNORE:4217 /IGNORE:4099"
    if [%2]==[1] (
        echo   AddressSanitizer enabled for Dev build
        set "extra_c_flags=/fsanitize=address /Zi"
        set "extra_link_flags=/FORCE:MULTIPLE /IGNORE:4217"
    )
    cl /W1 /MT /MP /O1 %extra_c_flags% -D _WINSOCKAPI_ -D GS_CORE_APP_STANDALONE -D CURL_STATICLIB ^
    /Fe%proj_name%.exe %src_all% %inc% /EHsc /link /SUBSYSTEM:CONSOLE /DEBUG ^
    /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcmt.lib ^
    %extra_link_flags% ^
    %os_libs% %tp_libs%
    goto :end

:error
    echo Configuration missing: 'dbg', 'rel', or 'dev' 

:end
    popd 
