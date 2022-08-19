@echo off
rmdir /Q /S bin
mkdir bin
pushd bin

rem GS_Core
set gs_core=..\..\gs_core

rem Name
set name=%APP%

rem Include directories 
set inc=/I %gs_core%\third_party\include\ /I ..\source\ /I %gs_core%\source\ /I %gs_core%\source\reflection\

rem Source files
set src_main=%gs_core%\source\core\core_unity.c ..\source\unity.c 

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
    %gs_core%\third_party\libs\win\dbg\DetourTileCache-d.lib

set tp_libs=%gs_core%\third_party\libs\win\rel\Bullet3Collision.lib ^
    %gs_core%\third_party\libs\win\rel\Bullet3Common.lib ^
    %gs_core%\third_party\libs\win\rel\Bullet3Dynamics.lib ^
    %gs_core%\third_party\libs\win\rel\Bullet3Geometry.lib ^
    %gs_core%\third_party\libs\win\rel\BulletDynamics.lib ^
    %gs_core%\third_party\libs\win\rel\BulletCollision.lib ^
    %gs_core%\third_party\libs\win\rel\LinearMath.lib ^
    %gs_core%\third_party\libs\win\rel\enet.lib ^
    %gs_core%\third_party\libs\win\dbg\Recast.lib ^
    %gs_core%\third_party\libs\win\dbg\Detour.lib ^
    %gs_core%\third_party\libs\win\dbg\DetourCrowd.lib ^
    %gs_core%\third_party\libs\win\dbg\DetourTileCache.lib

rem Link options
set l_options=/EHsc /link /SUBSYSTEM:CONSOLE /NODEFAULTLIB:msvcrt.lib

rem Reflection Source
set src_refl=%gs_core%\source\reflection\reflection.c

rem Reflection Name
set refl_name=reflection

rem Reflection Dir
set refl_dir="../source"

rem Compile Reflection
cl /w /MTd /MP -Zi -D _WINSOCKAPI_ /DEBUG:FULL /Fe%refl_name%.exe ^
%src_refl% %inc% /EHsc /link /SUBSYSTEM:CONSOLE /NODEFAULTLIB:libcmtd.lib ^ /NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:libcmtd.lib ^
%os_libs%

rem Run Reflection
%refl_name%.exe %refl_dir%

rem Compile Release
rem cl /MP /FS /Ox /W0 /Fe%name%.exe %src_all% %inc% ^
rem /EHsc /link /SUBSYSTEM:CONSOLE /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:LIBCMT ^
rem %os_libs% %tp_libs%

rem Compile Cpp Lib Debug
cl /MTd /EHsc /c /w /MP -Zi /DEBUG:FULL ^
%gs_core%\source\core\core_ai.cpp %gs_core%\source\core\core_physics.cpp %inc%

rem Set cpp objects
set cpp_obj=core_ai.obj core_physics.obj

rem Compile Debug
cl /w /MTd /MP -Zi -D _WINSOCKAPI_ /DEBUG:FULL /Fe%name%.exe %src_all% %inc% ^
/EHsc /link %cpp_obj% /SUBSYSTEM:CONSOLE /NODEFAULTLIB:libcmtd.lib ^
/NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:libcmtd.lib ^
%os_libs% %tp_libs_dbg%

popd 
