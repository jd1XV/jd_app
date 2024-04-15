/* date = February 27th 2024 4:11 pm */

#ifndef JD_APP_H
#define JD_APP_H

#ifndef JD_UNITY_H
#include "jd_defs.h"
#include "jd_sysinfo.h"
#include "jd_string.h"
#include "jd_memory.h"
#include "jd_helpers.h"
#include "jd_renderer.h"
#include "jd_timer.h"
#include "jd_input.h"
#endif

#define JD_APP_MAX_WINDOWS 2048

typedef struct jd_PlatformWindow jd_PlatformWindow;
jd_V2F jd_PlatformWindowGetDrawSize(jd_PlatformWindow* window);
u32 jd_PlatformWindowGetDPI(jd_PlatformWindow* window);

typedef struct jd_App jd_App;
jd_App* jd_AppCreate(struct jd_AppConfig* config);

typedef void (*jd_AppWindowFunctionPtr)(struct jd_PlatformWindow* window);

typedef enum jd_AppMode {
    JD_AM_STATIC,
    JD_AM_RELOADABLE,
    JD_AM_COUNT
} jd_AppMode;

typedef struct jd_AppConfig {
    jd_AppMode mode;
    jd_String package_name;
} jd_AppConfig;

typedef enum  jd_PlatformWindowStyle {
    jd_WS_Light,
    jd_WS_Dark,
    jd_WS_Count
} jd_PlatformWindowStyle;

typedef struct jd_PlatformWindowConfig {
    jd_App* app;
    jd_String title;
    jd_String id_str;
    jd_AppWindowFunctionPtr function_ptr;
    jd_String function_name;
    jd_PlatformWindowStyle window_style;
} jd_PlatformWindowConfig;

void       jd_AppUpdatePlatformWindows(jd_App* app);
jd_PlatformWindow* jd_AppPlatformCreateWindow(jd_PlatformWindowConfig* config);
void       jd_AppPlatformCloseWindow(jd_PlatformWindow* window);
b32        jd_AppPlatformUpdate(jd_App* app);

#ifdef JD_WINDOWS

typedef void (*_jd_AppWindowFunction)(struct jd_PlatformWindow* window);
#define jd_AppWindowFunction(x) void x (struct jd_PlatformWindow* window)

typedef struct jd_PlatformWindow {
    jd_App* app;
    jd_Arena* arena;
    jd_V2F pos;
    jd_V2F size;
    jd_V2S32 pos_i;
    jd_V2S32 size_i;
    jd_DArray* input_events; // type: jd_InputEvent (jd_input.h)
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
} jd_PlatformWindow;

LRESULT CALLBACK jd_PlatformWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void _jd_Internal_WindowUpdateFunctionStub(jd_PlatformWindow* window) {}

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

#ifdef JD_IMPLEMENTATION
#include "win32_jd_app.c"
#endif

#endif

#endif //JD_APP_H
