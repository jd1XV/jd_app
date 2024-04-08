@echo off
setlocal enabledelayedexpansion

set arg=%1%

set dir=%~dp0

set appname=jd_app_test_lib

set subsys="windows"

set compflags= ^
/EHsc /wd5105 /wd4005 /std:c17

set debugflags= ^
/Z7 /Od /D__JD_DEBUG__

set releaseflags= ^
/O2 /DJD_DISABLE_FUNCTION_TIMER

if %arg%==-d (set optiflags=!debugflags!)
if %arg%==-r (set optiflags=!releaseflags!)

set linkflags= ^
User32.lib gdi32.lib Opengl32.lib Ws2_32.lib Crypt32.lib Wldap32.lib Normaliz.lib Shlwapi.lib Ole32.lib advapi32.lib dwmapi.lib  Comdlg32.lib Shell32.lib Shcore.lib ../dev/lib/freetype.lib ../build/jd_app_test.lib

if %arg%==-d (set linkflags = /DEBUG:FULL !linkflags!)

if not exist build mkdir build
if not exist build/assets mkdir build/assets
if not exist build/generated mkdir build/generated
if not exist dev mkdir dev
if not exist dev/obj mkdir dev/obj
if not exist dev/lib mkdir dev/lib

pushd src\
cl /LD !compflags! !optiflags! app_functions.c /Fo..\dev\obj\ /link !linkflags! /INCREMENTAL:NO /OUT:../build/jd_app_pkg/!appname!.dll 
popd
