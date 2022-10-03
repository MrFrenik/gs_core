@echo off
rmdir /Q /S bin\reflection
mkdir bin\reflection
pushd bin\reflection

rem Name
set name=reflection

rem Include directories 
set inc=/I ..\..\third_party\include\ /I ..\..\source\reflection\

rem Source files
set src_main=..\..\source\reflection\reflection.c

rem All source together
set src_all=%src_main%

rem OS Libraries
set os_libs=opengl32.lib kernel32.lib user32.lib ^
shell32.lib vcruntime.lib msvcrt.lib gdi32.lib Winmm.lib Advapi32.lib 

rem Link options
set l_options=/EHsc /link /SUBSYSTEM:CONSOLE /NODEFAULTLIB:msvcrt.lib 

rem Compile
cl /w /MTd /MP -Zi -D _WINSOCKAPI_ /DEBUG:FULL /Fe%name%.exe ^
%src_all% %inc% /EHsc /link /SUBSYSTEM:CONSOLE /NODEFAULTLIB:libcmtd.lib ^
/NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:libcmtd.lib ^
%os_libs% 

popd 
