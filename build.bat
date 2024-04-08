@echo off
setlocal enabledelayedexpansion

set arg=%1%

set dir=%~dp0

set appname=jd_app_test

set srcfiles=main.c dep\glad\glad_wgl.c dep\glad\glad.c

set subsys="windows"

set includes= ^
/I"C:\Code\freetype-2.13.2\include"

set compflags= ^
/EHsc /wd5105 /wd4005 /std:c17

set debugflags= ^
/Z7 /Od /D__JD_DEBUG__

set releaseflags= ^
/O2 /DJD_DISABLE_FUNCTION_TIMER

if %arg%==-d (set optiflags=!debugflags!)
if %arg%==-r (set optiflags=!releaseflags!)

set linkflags= ^
User32.lib gdi32.lib Opengl32.lib Ws2_32.lib Crypt32.lib Wldap32.lib Normaliz.lib Shlwapi.lib Ole32.lib advapi32.lib dwmapi.lib  Comdlg32.lib Shell32.lib Shcore.lib ..\dev\lib\freetype.lib

if %arg%==-d (set linkflags = /DEBUG:FULL !linkflags!)

if not exist build mkdir build
if not exist build/assets mkdir build/assets
if not exist build/generated mkdir build/generated
if not exist build/jd_app_pkg mkdir build/jd_app_pkg
attrib +h build/jd_app_pkg
if not exist dev mkdir dev
if not exist dev/obj mkdir dev/obj
if not exist dev/lib mkdir dev/lib

pushd src\
cl !optiflags! !includes! !compflags! !srcfiles! /Fe..\build\!appname!.exe /Fo..\dev\obj\  /link /INCREMENTAL:NO !linkflags! /SUBSYSTEM:!subsys!
popd

pushd build\
start /b mt.exe -nologo -manifest "!appname!.exe.manifest" -outputresource:"!appname!.exe"
popd