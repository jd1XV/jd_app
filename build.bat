@echo off
setlocal enabledelayedexpansion

set appname=jd_app_test

set jd_lib_path=../jd_lib
set freetype_path=C:\Code\freetype-2.13.2

set glad_path=!jd_lib_path!\include\dep\glad

set includes= ^
/I"!jd_lib_path!/include" ^
/I"!freetype_path!/include"

set arg=%1%

set dir=%~dp0

set srcfiles=src/main.c !glad_path!/glad_wgl.c !glad_path!/glad.c 

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
User32.lib gdi32.lib Opengl32.lib Ws2_32.lib UxTheme.lib Crypt32.lib Wldap32.lib Normaliz.lib Shlwapi.lib Ole32.lib advapi32.lib dwmapi.lib Comdlg32.lib Shell32.lib Shcore.lib "!freetype_path!\objs\freetype.lib"

if %arg%==-d (set linkflags = /DEBUG:FULL !linkflags!)

if not exist "build" mkdir "build"
if not exist "build/assets" mkdir "build/assets"
if not exist "build/generated" mkdir "build/generated"
if not exist "build/jd_app_pkg" mkdir "build/jd_app_pkg"
attrib +h "build/jd_app_pkg"
if not exist "dev" mkdir "dev"
if not exist "dev/obj" mkdir "dev/obj"
if not exist "dev/lib" mkdir "dev/lib"

cl !optiflags! !includes! !compflags! !srcfiles! /Febuild\!appname!.exe /Fodev\obj\  /link /INCREMENTAL:NO !linkflags! /SUBSYSTEM:!subsys!

pushd build\
start /b mt.exe -nologo -manifest "!appname!.exe.manifest" -outputresource:"!appname!.exe"
popd