@echo off
setlocal enabledelayedexpansion

set arg=%1%

set dir=%~dp0

set appname=jd_app_test

set srcfiles=main.c dep\glad\glad_wgl.c dep\glad\glad.c

set subsys="windows"

set includes= ^
/I"C:\Code\jd_lib\src" ^
/I"C:\Code\jdat" ^
/I"C:\Code\jd_thread" ^
/I"C:\Code\freetype-2.13.2\include"

set compflags= ^
/EHsc -W1 -Wno-unused-variable -Wno-pointer-sign -Wno-incompatible-pointer-types-discards-qualifiers /std:c17

set debugflags= ^
/Z7 /Od /D__JD_DEBUG__

set releaseflags= ^
/O2 /DJD_DISABLE_FUNCTION_TIMER

if %arg%==-d (set optiflags=!debugflags!)
if %arg%==-r (set optiflags=!releaseflags!)

set linkflags= ^
User32.lib gdi32.lib Opengl32.lib Ws2_32.lib Crypt32.lib Wldap32.lib Normaliz.lib Shlwapi.lib Ole32.lib advapi32.lib dwmapi.lib  Comdlg32.lib Shell32.lib Shcore.lib

if %arg%==-d (set linkflags = /DEBUG:FULL !linkflags!)

pushd src\
icx !optiflags! !includes! !compflags! !srcfiles! /Fe..\build\!appname!.exe /Fo..\dev\obj\  /link /INCREMENTAL:NO !linkflags! /SUBSYSTEM:!subsys!
popd

pushd build\
start /b mt.exe -nologo -manifest "!appname!.exe.manifest" -outputresource:"!appname!.exe"
popd