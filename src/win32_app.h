/* date = February 27th 2024 0:32 am */

#ifndef WIN32_APP_H
#define WIN32_APP_H

#define JD_APP_MAX_WINDOWS 2048

typedef struct jd_App {
    jd_Arena* arena;
    jd_Arena* frame_arena;
    jd_UserLock* lock;
    
    HINSTANCE instance;
    
    struct jd_Window* windows[JD_APP_MAX_WINDOWS];
    u64 window_count;
} jd_App;

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
    
    HWND handle;
    WNDCLASSA wndclass;
    jd_String wndclass_str;
    PIXELFORMATDESCRIPTOR pixel_format_descriptor;
    
    HGLRC ogl_context;
    HDC device_context;
    
    //jd_Renderer* renderer;
    jd_String title;
    b8 closed;
} jd_Window;


void jd_AppUpdatePlatformWindows(jd_App* app);
jd_Window* jd_AppPlatformCreateWindow(jd_App* app, jd_String id_str, jd_String title);
b32 jd_AppPlatformUpdate(jd_App* app);

#endif //WIN32_APP_H
