@echo off

rem Compile Reflection
call proc\win\refl_cl.bat

rem Compile Core
call proc\win\cl.bat

rmdir /Q /S bin\proj_gen
mkdir bin\proj_gen
pushd bin\proj_gen

rem ProjName
set proj_name=%1

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

rem Link options
set l_options=/EHsc /link /SUBSYSTEM:CONSOLE /NODEFAULTLIB:msvcrt.lib 

rem Compile Project Gen
cl /w /MTd /MP -Zi -D _WINSOCKAPI_ /DEBUG:FULL /Fe%name%.exe ^
%src_all% %inc% /EHsc /link /SUBSYSTEM:CONSOLE /NODEFAULTLIB:libcmtd.lib ^ /NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:libcmtd.lib ^
%os_libs%

rem Run Proj Gen
%name%.exe %proj_name% 

popd 





