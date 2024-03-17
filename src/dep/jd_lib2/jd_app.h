/* date = February 27th 2024 4:11 pm */

#ifndef JD_APP_H
#define JD_APP_H

#ifndef JD_DEFS_H
#include "jd_defs.h"
#endif

#ifndef JD_SYSINFO_H
#include "jd_sysinfo.h"
#endif

#ifndef JD_STRING_H
#include "jd_string.h"
#endif

#ifndef JD_MEMORY_H
#include "jd_memory.h"
#endif

#ifndef JD_HELPERS_H
#include "jd_helpers.h"
#endif

#define JD_APP_MAX_WINDOWS 2048

typedef struct jd_Window jd_Window;
jd_V2F jd_WindowGetDrawSize(jd_Window* window);

typedef struct jd_App jd_App;
jd_App* jd_AppCreate(struct jd_AppConfig* config);

typedef void (*jd_AppWindowFunctionPtr)(struct jd_Window* window);

typedef enum jd_AppMode {
    JD_AM_STATIC,
    JD_AM_RELOADABLE,
    JD_AM_COUNT
} jd_AppMode;

typedef struct jd_AppConfig {
    jd_AppMode mode;
    jd_String package_name;
} jd_AppConfig;

typedef enum  jd_WindowStyle {
    jd_WS_Light,
    jd_WS_Dark,
    jd_WS_Count
} jd_WindowStyle;

typedef struct jd_WindowConfig {
    jd_App* app;
    jd_String title;
    jd_String id_str;
    jd_AppWindowFunctionPtr function_ptr;
    jd_String function_name;
    jd_WindowStyle window_style;
} jd_WindowConfig;

void       jd_AppUpdatePlatformWindows(jd_App* app);
jd_Window* jd_AppPlatformCreateWindow(jd_WindowConfig* config);
void       jd_AppPlatformCloseWindow(jd_Window* window);
b32        jd_AppPlatformUpdate(jd_App* app);

#ifdef JD_WINDOWS

#ifndef JD_RENDERER_H
#include "jd_renderer.h"
#endif

typedef void (*_jd_AppWindowFunction)(struct jd_Window* window);
#define jd_AppWindowFunction(x) void x (struct jd_Window* window)

typedef struct jd_Window {
    jd_App* app;
    jd_Arena* arena;
    jd_V2F pos;
    jd_V2F size;
    jd_V2S32 pos_i;
    jd_V2S32 size_i;
    
    _jd_AppWindowFunction func;
    jd_String function_name;
    
    HWND handle;
    WNDCLASSA wndclass;
    jd_String wndclass_str;
    PIXELFORMATDESCRIPTOR pixel_format_descriptor;
    
    HGLRC ogl_context;
    HDC device_context;
    jd_Renderer* renderer;
    
    f32 dpi_scaling;
    
    jd_String title;
    b8 closed;
} jd_Window;

LRESULT CALLBACK jd_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void _jd_Internal_WindowUpdateFunctionStub(jd_Window* window) {}

#define jd_AppMainFn i32 APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstPrev, PSTR cmdline, i32 cmdshow)

#ifdef JD_APP_RELOADABLE
#define jd_AppWindowFunction(x) __declspec(dllexport) void __cdecl x (struct jd_Window* window)
#else
#define jd_AppWindowFunction(x) void x (struct jd_Window* window)
#endif

#ifdef JD_IMPLEMENTATION
#include "win32_jd_app.c"
#endif

#endif

#endif //JD_APP_H
