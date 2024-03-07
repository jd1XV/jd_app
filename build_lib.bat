@echo off
setlocal enabledelayedexpansion

set arg=%1%

set dir=%~dp0

set appname=jd_app_test

set subsys="windows"

set includes= ^
/I"C:\Code\jd_lib\src" ^
/I"C:\Code\jdat" ^
/I"C:\Code\jd_thread"

set compflags= ^
/EHsc /wd5105 /wd4005 /std:c17

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
cl /LD !compflags! !optiflags! app_functions.c /Fo..\dev\obj\ /link !linkflags! ../dev/obj/glad_wgl.obj ../dev/obj/glad.obj ../dev/obj/main.obj /OUT:../build/jd_app_pkg/!appname!.dll 
popd
