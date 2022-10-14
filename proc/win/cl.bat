@echo off
rmdir /Q /S bin\core
mkdir bin\core
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
shell32.lib vcruntime.lib msvcrt.lib gdi32.lib Winmm.lib Advapi32.lib 

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
    %root%\third_party\libs\win\dbg\DetourTileCache-d.lib

set tp_libs=%root%\third_party\libs\win\rel\Bullet3Collision.lib ^
    %root%\third_party\libs\win\rel\Bullet3Common.lib ^
    %root%\third_party\libs\win\rel\Bullet3Dynamics.lib ^
    %root%\third_party\libs\win\rel\Bullet3Geometry.lib ^
    %root%\third_party\libs\win\rel\BulletDynamics.lib ^
    %root%\third_party\libs\win\rel\BulletCollision.lib ^
    %root%\third_party\libs\win\rel\LinearMath.lib ^
    %root%\third_party\libs\win\rel\enet.lib ^
    %root%\third_party\libs\win\dbg\Recast.lib ^
    %root%\third_party\libs\win\dbg\Detour.lib ^
    %root%\third_party\libs\win\dbg\DetourCrowd.lib ^
    %root%\third_party\libs\win\dbg\DetourTileCache.lib

rem Link options
set l_options=/EHsc /link /SUBSYSTEM:CONSOLE /NODEFAULTLIB:msvcrt.lib 

rem Reflection Dir
set refl_dir=%root%\source\core\

rem Out Dir
set out_dir=%root%\source\core\generated\

rem Run Reflection
%root%\bin\reflection\reflection.exe %refl_dir% %out_dir% %proj_name%

rem Compile Cpp objects (dbg)
cl /MTd /EHsc /c /w /MP -Z7 ^
%inc% %root%\source\core\gs_core_ai.cpp %root%\source\core\gs_core_physics.cpp ^
%root%\source\core\gs_core_util.cpp /link /NODEFAULTLIB:libcmtd.lib ^
/NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:libcmtd.lib

rem Set cpp objects
set cpp_obj=gs_core_ai.obj gs_core_physics.obj

rem Compile objects (dbg)
cl /MTd /EHsc /c /w /MP /Fd -Z7 -D _WINSOCKAPI_ -D GS_DEBUG ^
%src_all% %inc% /DEBUG ^
/link /NODEFAULTLIB:libcmtd.lib ^
/NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:libcmtd.lib

rem Compile static lib (dbg)
lib *obj /out:%proj_name%_d.lib

popd 
