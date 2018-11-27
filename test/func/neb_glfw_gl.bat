
@echo off

set EXE_NAME="neb_glfw_gl"

IF NOT EXIST bin mkdir bin
pushd bin

set VC_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
if [%VisualStudioVersion%] == [] echo WARNING: VC cold. && call %VC_PATH%"\vcvarsall.bat" x64 >NUL

set COMPILER_FLAGS=-nologo -Fe%EXE_NAME% -D_CRT_SECURE_NO_WARNINGS -EHa- -Gm- -GR- -fp:fast -Oi -WX -W4 -wd4996 -wd4100 -wd4189 -wd4127 -wd4201 -wd4101 -wd4505 -wd4204 -wd4221 -wd4152
rem set COMPILER_FLAGS=%COMPILER_FLAGS% -MTd -Od -Z7
set COMPILER_FLAGS=%COMPILER_FLAGS% -MT -O2 -Z7

cl %COMPILER_FLAGS% -I../ -I../../../include/ ../main.c ../GL/gl3w.c user32.lib shell32.lib gdi32.lib opengl32.lib ../GLFW/glfw3_vc2017_MT.lib -link -incremental:no -opt:ref

if [%ERRORLEVEL%] == [0] %EXE_NAME%.exe

popd
