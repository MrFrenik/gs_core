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
set src_main=%root%\source\unity.c

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
    %gs_core%\third_party\libs\win\dbg\Recast.lib ^
    %gs_core%\third_party\libs\win\dbg\Detour.lib ^
    %gs_core%\third_party\libs\win\dbg\DetourCrowd.lib ^
    %gs_core%\third_party\libs\win\dbg\DetourTileCache.lib

rem Link options
set l_options=/EHsc /link /SUBSYSTEM:CONSOLE /NODEFAULTLIB:msvcrt.lib 

rem Reflection Dirs
set in_dir="%root%/source" 
set out_dir="%root%/source/generated" 
set id_offset="14"

rem Run Reflection
%gs_core%\bin\reflection\reflection.exe %in_dir% %out_dir% %proj_name% %id_offset%

rem Get Date time for pdb gen 
set _my_datetime=%date%_%time%
set _my_datetime=%_my_datetime: =_%
set _my_datetime=%_my_datetime::=%
set _my_datetime=%_my_datetime:/=_%
set _my_datetime=%_my_datetime:.=_%
For /f "tokens=2-4 delims=/ " %%a in ('date /t') do (set my_date=%%c-%%a-%%b)
For /f "tokens=1-2 delims=/:" %%a in ("%TIME%") do (set my_time=%%a%%b) 

set pdb_name=%proj_name%_%_my_datetime%

rem Compile Debug DLL
cl /w /MTd /MP -Z7 -D _WINSOCKAPI_ -D GS_API_DLL_EXPORT ^
-D GS_DEBUG /D_USRDLL /D_WINDLL /DEBUG:FULL %src_all% %inc% ^
/EHsc /link /PDB:%pdb_name%.pdb /DLL /SUBSYSTEM:CONSOLE /NODEFAULTLIB:libcmtd.lib ^
/NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:libcmtd.lib ^
%os_libs% %tp_libs_dbg% /OUT:%proj_name%_d.dll 

popd 
