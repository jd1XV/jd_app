/* date = February 22nd 2024 8:05 pm */

#ifndef JD__WINDOWS_H
#define JD__WINDOWS_H

#include <Windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <shobjidl_core.h>
#include <intrin.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <dwmapi.h>
#include <vsstyle.h>

#define JD_WINDOWS

#define jd_ProcessExit(x) ExitProcess(x)

#ifdef JD_CONSOLE
#define jd_AppMainFn i32 main()
#else
#define jd_AppMainFn i32 APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstPrev, PSTR cmdline, i32 cmdshow)
#endif

#ifdef JD_APP_RELOADABLE
#define jd_AppWindowFunction(x) __declspec(dllexport) void __cdecl x (struct jd_PlatformWindow* window)
#else
#define jd_AppWindowFunction(x) void x (struct jd_PlatformWindow* window)
#endif

#endif //JD__WINDOWS_H
