#ifndef JD_RENDERER_H
#include "jd_renderer.h"
#endif

#include "../glad/glad_wgl.h"

static const jd_String app_manifest = jd_StrConst("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" \
                                                  "<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\" xmlns:asmv3=\"urn:schemas-microsoft-com:asm.v3\">" \
                                                  "<asmv3:application>" \
                                                  "<asmv3:windowsSettings>" \
                                                  "<dpiAware xmlns=\"http://schemas.microsoft.com/SMI/2005/WindowsSettings\">true</dpiAware>" \
                                                  "<dpiAwareness xmlns=\"http://schemas.microsoft.com/SMI/2016/WindowsSettings\">PerMonitorV2</dpiAwareness>" \
                                                  "</asmv3:windowsSettings>" \
                                                  "</asmv3:application>" \
                                                  "</assembly>");

#define JD_APP_MAX_PACKAGE_NAME_LENGTH KILOBYTES(1)

LRESULT CALLBACK jd_PlatformWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef enum jd_Cursor {
    jd_Cursor_Default,
    jd_Cursor_Resize_H,
    jd_Cursor_Resize_V,
    jd_Cursor_Resize_Diagonal_TL_BR,
    jd_Cursor_Resize_Diagonal_TR_BL,
    jd_Cursor_Hand,
    jd_Cursor_Text,
    jd_Cursor_Loading,
    jd_Cursor_Count
} jd_Cursor;

typedef struct jd_App {
    jd_Arena* arena;
    jd_UserLock* lock;
    jd_String package_name;
    
    HINSTANCE instance;
    
    struct jd_PlatformWindow* windows[JD_APP_MAX_WINDOWS];
    u64 window_count;
    
    jd_AppMode mode;
    HMODULE reloadable_dll;
    jd_Cursor cursor;
    jd_String lib_file_name;
    jd_String lib_copied_file_name;
    u64 reloadable_dll_file_time;
} jd_App;

void jd_AppSetCursor(jd_Cursor cursor) {
    switch (cursor) {
        default:
        case jd_Cursor_Count: {
            return;
        }
        
        case jd_Cursor_Default: {
            HCURSOR win_cursor = LoadCursorA(NULL, IDC_ARROW);
            SetCursor(win_cursor);
            break;
        }
        
        case jd_Cursor_Resize_H: {
            HCURSOR win_cursor = LoadCursorA(NULL, IDC_SIZEWE);
            SetCursor(win_cursor);
            break;
        }
        
        case jd_Cursor_Resize_V: {
            HCURSOR win_cursor = LoadCursorA(NULL, IDC_SIZENS);
            SetCursor(win_cursor);
            break;
        }
        
        case jd_Cursor_Resize_Diagonal_TL_BR: {
            HCURSOR win_cursor = LoadCursorA(NULL, IDC_SIZENWSE);
            SetCursor(win_cursor);
            break;
        }
        
        case jd_Cursor_Resize_Diagonal_TR_BL: {
            HCURSOR win_cursor = LoadCursorA(NULL, IDC_SIZENESW);
            SetCursor(win_cursor);
            break;
        }
        
        case jd_Cursor_Hand: {
            HCURSOR win_cursor = LoadCursorA(NULL, IDC_HAND);
            SetCursor(win_cursor);
            break;
        }
        
        case jd_Cursor_Text: {
            HCURSOR win_cursor = LoadCursorA(NULL, IDC_IBEAM);
            SetCursor(win_cursor);
            break;
        }
        
        case jd_Cursor_Loading: {
            HCURSOR win_cursor = LoadCursorA(NULL, IDC_WAIT);
            SetCursor(win_cursor);
            break;
        }
    }
}

void jd_AppFreeLib(jd_App* app) {
    FreeLibrary(app->reloadable_dll);
}

void jd_AppLoadLib(jd_App* app) {
    jd_DString* package_name_str = jd_DStringCreate(JD_APP_MAX_PACKAGE_NAME_LENGTH);
    if (app->package_name.count + sizeof("jd_app_pkg/_lib.dll.active") > JD_APP_MAX_PACKAGE_NAME_LENGTH) {
        jd_LogError("App name is too long!", jd_Error_OutOfMemory, jd_Error_Fatal);
    }
    
    jd_DStringAppend(package_name_str, jd_StrLit("jd_app_pkg/"));
    jd_DStringAppend(package_name_str, app->package_name);
    jd_DStringAppend(package_name_str, jd_StrLit("_lib"));
    jd_DStringAppend(package_name_str, jd_StrLit(".dll"));
    
    if (app->lib_file_name.count == 0) 
        app->lib_file_name = jd_StringPush(app->arena, jd_DStringGet(package_name_str));
    
    if (!jd_DiskPathExists(app->lib_file_name)) {
        jd_LogError("Could not find specified .dll!", jd_Error_FileNotFound, jd_Error_Fatal);
    }
    
    jd_DStringAppend(package_name_str, jd_StrLit(".active"));
    jd_DiskPathCopy(jd_DStringGet(package_name_str), app->lib_file_name, false);
    
    if (app->lib_copied_file_name.count == 0) 
        app->lib_copied_file_name = jd_StringPush(app->arena, jd_DStringGet(package_name_str));
    
    app->reloadable_dll = LoadLibraryExA(package_name_str->mem, NULL, 0);
    
    for (u64 i = 0; i < app->window_count; i++) {
        jd_PlatformWindow* window = app->windows[i];
        window->func = (_jd_AppWindowFunction)GetProcAddress(app->reloadable_dll, window->function_name.mem);
    }
    
    jd_DStringRelease(package_name_str);
}

void jd_AppUpdatePlatformWindows(jd_App* app) {
    jd_UserLockGet(app->lock);
    
    static u64 reload_delay = 0;
    
    u64 last_reload_time = app->reloadable_dll_file_time;
    u64 current_reload_time = jd_DiskGetFileLastMod(app->lib_file_name);
    
    if (last_reload_time < current_reload_time) {
        if (reload_delay && reload_delay % 32 == 0) {
            jd_AppFreeLib(app);
            jd_AppLoadLib(app);
            app->reloadable_dll_file_time = current_reload_time;
            reload_delay = 0;
        } else {
            reload_delay++;
        }
        
    }
    
    for (u64 i = 0; i < app->window_count; i++) {
        jd_PlatformWindow* window = app->windows[i];
        MSG msg = {0};
        while (PeekMessage(&msg, window->handle, 0, 0, PM_REMOVE) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } 
        
        if (window->closed) {
            jd_AppPlatformCloseWindow(window);
        }
    }
    
    for (u64 i = 0; i < app->window_count; i++) {
        jd_PlatformWindow* window = app->windows[i];
        
        // get the size
        RECT client_rect = {0};
        GetClientRect(window->handle, &client_rect);
        window->size.w = client_rect.right;
        window->size.h = client_rect.bottom;
        window->size_i.w = (i32)client_rect.right;
        window->size_i.h = (i32)client_rect.bottom;
        
        RECT window_rect = {0};
        GetWindowRect(window->handle, &window_rect);
        window->pos.x = window_rect.left;
        window->pos.y = window_rect.top;
        window->pos_i.x = (i32)window_rect.left;
        window->pos_i.y = (i32)window_rect.top;
        
        jd_RendererSetRenderSize(window->renderer, window->size);
        jd_RendererSetDPIScale(window->renderer, window->dpi_scaling);
        window->func(window);
        jd_RendererDraw(window->renderer);
        jd_ArenaPopTo(window->renderer->frame_arena, 0);
        SwapBuffers(window->device_context);
    }
    
    jd_UserLockRelease(app->lock);
}

void jd_AppPlatformCloseWindow(jd_PlatformWindow* window) {
    jd_App* app = window->app;
    u64 window_index = 0;
    for (window_index; window_index < app->window_count; window_index++) {
        if (app->windows[window_index] == window) {
            jd_ZeroMemory(&app->windows[window_index], sizeof(jd_PlatformWindow*));
            if (window_index != app->window_count - 1) {
                jd_MemMove(&app->windows[window_index], &app->windows[window_index] + 1, (app->window_count - (window_index + 1)) * sizeof(jd_PlatformWindow*)); }
            break;
        }
    }
    
    app->window_count--;
    
    jd_ArenaRelease(window->arena);
}

jd_PlatformWindow* jd_AppPlatformCreateWindow(jd_PlatformWindowConfig* config) {
    if (!config) {
        jd_LogError("Window initialized without jd_PlatformWindowConfig*", jd_Error_APIMisuse, jd_Error_Fatal);
        return 0;
    }
    
    if (!config->app) {
        jd_LogError("Window initialized without jd_App*", jd_Error_APIMisuse, jd_Error_Fatal);
        return 0;
    }
    
    if (config->app->window_count >= JD_APP_MAX_WINDOWS) {
        jd_LogError("Window count exceeds JD_APP_MAX_WINDOWS", jd_Error_APIMisuse, jd_Error_Critical);
        return 0;
    }
    
    // Register the window class.
    jd_Arena* arena = jd_ArenaCreate(0, 0);
    jd_PlatformWindow* window = jd_ArenaAlloc(arena, sizeof(*window));
    window->app = config->app;
    window->wndclass_str = jd_StringPush(arena, config->id_str);
    window->title = jd_StringPush(arena, config->title);
    window->arena = arena;
    window->input_events = jd_DArrayCreate(MEGABYTES(256) / sizeof(jd_InputEvent), sizeof(jd_InputEvent));
    
    switch (config->app->mode) {
        default: break;
        
        case JD_AM_STATIC: {
            if (config->function_ptr) {
                jd_LogError("App mode set to JD_AM_STATIC, but no Jd_AppWindowFunctionPtr supplied in jd_PlatformWindowConfig", jd_Error_APIMisuse, jd_Error_Fatal);
                return 0;
            }
            
            window->func = config->function_ptr;
        } break;
        
        case JD_AM_RELOADABLE: {
            if (config->function_name.count == 0) {
                jd_LogError("App mode set to JD_AM_RELOADABLE, but no function name supplied in jd_PlatformWindowConfig", jd_Error_APIMisuse, jd_Error_Fatal);
                return 0;
            }
            
            window->function_name = jd_StringPush(arena, config->function_name);
            window->func = (_jd_AppWindowFunction)GetProcAddress(config->app->reloadable_dll, config->function_name.mem);
            if (!window->func) {
                jd_LogError("Could not find specified function in .dll!", jd_Error_FileNotFound, jd_Error_Fatal);
            }
            
        } break;
    }
    
    window->dpi_scaling = 1.0f; // TODO: Handle HiDPI
    
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = jd_PlatformWindowProc;
    wc.hInstance     = config->app->instance;
    wc.lpszClassName = window->wndclass_str.mem;
    wc.cbWndExtra    = sizeof(jd_PlatformWindow*);
    
    RegisterClassEx(&wc);
    
    // Create the window->
    
    window->handle = CreateWindowExA(
                                     CS_OWNDC,                              // Optional window styles.
                                     window->wndclass_str.mem,               // Window class
                                     window->title.mem,                      // Window textc
                                     WS_OVERLAPPEDWINDOW,                   // Window style
                                     
                                     // Size and position
                                     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                     
                                     NULL,       // Parent window    
                                     NULL,       // Menu
                                     config->app->instance,  // Instance handle
                                     NULL        // Additional application data
                                     );
    
    if (window->handle == NULL) {
        // err
    }
    
    
    if (config->window_style == jd_WS_Dark) {
        BOOL USE_DARK_MODE = true;
        BOOL SET_IMMERSIVE_DARK_MODE_SUCCESS = SUCCEEDED(DwmSetWindowAttribute(window->handle, 20,
                                                                               &USE_DARK_MODE, sizeof(USE_DARK_MODE)));
    }
    
    
    window->device_context = GetDC(window->handle);
    
    if (!window->device_context) {
        // TODO: err
    }
    
    
    SetWindowLongPtrA(window->handle, 0, (LONG_PTR)window);
    
    
    WNDCLASSEX dummy_wc = {0};
    dummy_wc.cbSize = sizeof(WNDCLASSEX);
    dummy_wc.lpfnWndProc   = DefWindowProc;
    dummy_wc.hInstance     = config->app->instance;
    dummy_wc.lpszClassName = "dummy_wndclass";
    dummy_wc.cbWndExtra    = sizeof(jd_PlatformWindow*);
    RegisterClassEx(&dummy_wc);
    
    
    HWND dummy_win = CreateWindowExA(
                                     CS_OWNDC,                              // Optional window styles.
                                     "dummy_wndclass",                      // Window class
                                     window->title.mem,                      // Window textc
                                     WS_OVERLAPPEDWINDOW,                   // Window style
                                     
                                     // Size and position
                                     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                     
                                     NULL,       // Parent window    
                                     NULL,       // Menu
                                     config->app->instance,  // Instance handle
                                     NULL        // Additional application data
                                     );
    
    if (dummy_win == NULL) {
        // err
    }
    
    HDC dummy_hdc = GetDC(dummy_win);
    if (!dummy_hdc) {
        // err
    }
    
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
        32,                   // Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                   // Number of bits for the depthbuffer
        8,                    // Number of bits for the stencilbuffer
        0,                    // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    
    i32 pf = ChoosePixelFormat(dummy_hdc, &pfd);
    if (pf == 0) {
        // TODO: err
    }
    
    if (!(SetPixelFormat(dummy_hdc, pf, &window->pixel_format_descriptor))) {
        // TODO: err
    }
    
    HGLRC fake_context = wglCreateContext(dummy_hdc);
    wglMakeCurrent(dummy_hdc, fake_context);
    
    gladLoadGL();
    gladLoadWGL(dummy_hdc);
    
    const i32 pixel_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        0
    };
    
    const i32 ctx_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        0
    };
    
    u32 num_formats = 0;
    
    wglChoosePixelFormatARB(window->device_context, pixel_attribs, 0, 1, &pf, &num_formats);
    SetPixelFormat(window->device_context, pf, &window->pixel_format_descriptor);
    window->ogl_context = wglCreateContextAttribsARB(window->device_context, 0, ctx_attribs);
    
    wglMakeCurrent(dummy_hdc, 0);
    wglDeleteContext(fake_context);
    
    wglMakeCurrent(window->device_context, window->ogl_context);
    wglSwapIntervalEXT(0);
    ShowWindow(window->handle, SW_SHOW);
    
    window->renderer = jd_RendererCreate(window);
    DestroyWindow(dummy_win);
    jd_UserLockGet(config->app->lock);
    config->app->windows[config->app->window_count] = window;
    config->app->window_count++;
    jd_UserLockRelease(config->app->lock);
    
    return window;
}

b32 jd_AppPlatformUpdate(jd_App* app) {
    jd_AppUpdatePlatformWindows(app);
    return true;
}

b32 jd_AppIsRunning(jd_App* app) {
    return (app->window_count > 0);
}

jd_ForceInline void _jd_GetMods(jd_InputEvent* e) {
    if (GetKeyState(VK_LCONTROL)) {
        e->mods |= jd_KeyMod_Ctrl;
        e->mods |= jd_KeyMod_LCtrl;
    }
    
    if (GetKeyState(VK_RCONTROL)) {
        e->mods |= jd_KeyMod_Ctrl;
        e->mods |= jd_KeyMod_RCtrl;
    }
    
    if (GetKeyState(VK_LMENU)) {
        e->mods |= jd_KeyMod_Alt;
        e->mods |= jd_KeyMod_LAlt;
    }
    
    if (GetKeyState(VK_RMENU)) {
        e->mods |= jd_KeyMod_Alt;
        e->mods |= jd_KeyMod_RAlt;
    }
    
    if (GetKeyState(VK_LSHIFT)) {
        e->mods |= jd_KeyMod_Shift;
        e->mods |= jd_KeyMod_LShift;
    }
    
    if (GetKeyState(VK_RSHIFT)) {
        e->mods |= jd_KeyMod_Shift;
        e->mods |= jd_KeyMod_RShift;
    }
    
}

LRESULT CALLBACK jd_PlatformWindowProc(HWND window_handle, UINT msg, WPARAM w_param, LPARAM l_param) {
    static u32 count = 0;
    jd_InputEvent e = {0};
    jd_PlatformWindow* window = (jd_PlatformWindow*)GetWindowLongPtrA(window_handle, 0);
    switch (msg) {
        case WM_DESTROY: {
            window->closed = true;
            break;
        }
        
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            e.release_event = true;
        }
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN: {
            e.key = jd_MB_Left; 
            switch (msg) {
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP: {
                    e.key = jd_MB_Middle;
                    break;
                }
                
                case WM_RBUTTONDOWN:
                case WM_LBUTTONDOWN: {
                    e.key = jd_MB_Right;
                    break;
                }
            }
            
            _jd_GetMods(&e);
            
            POINT p = {0};
            GetCursorPos(&p);
            ScreenToClient(window_handle, &p);
            
            e.mouse_pos.x = (f32)p.x;
            e.mouse_pos.y = (f32)p.y;
            jd_DArrayPushBack(window->input_events, &e);
            break;
        }
        
        case WM_KEYDOWN:
        case WM_KEYUP: {
            
            if (w_param  < jd_Key_Tab) break;
            if (w_param >= jd_Key_Shift && w_param <= jd_Key_Menu) break;
            if (w_param >= jd_Key_LShift && w_param <= jd_Key_RMenu) break;
            
            e.release_event = (l_param & (1 << 31));
            
            if (w_param < jd_Key_Count) {
                e.key = w_param;
            }
            
            _jd_GetMods(&e);
            jd_DArrayPushBack(window->input_events, &e);
            
            break;
        }
        
        case WM_MOUSEMOVE: {
            
            break;
        }
        
        case WM_MOUSEWHEEL: {
            
            break;
        }
        
        case WM_CHAR: {
            
        }
        
    }
    
    return DefWindowProc(window_handle, msg, w_param, l_param);
}

jd_App* jd_AppCreate(jd_AppConfig* config) {
    jd_Arena* arena = jd_ArenaCreate(0, 0);
    jd_App* app = jd_ArenaAlloc(arena, sizeof(*app));
    app->lock = jd_UserLockCreate(arena, 16);
    app->arena = arena;
    app->instance = GetModuleHandle(NULL); 
    app->mode = config->mode;
    app->package_name = jd_StringPush(arena, config->package_name);
    
    if (app->mode == JD_AM_RELOADABLE) {
        if (app->package_name.count + sizeof("jd_app_pkg/.dll.active") > JD_APP_MAX_PACKAGE_NAME_LENGTH) {
            jd_LogError("App name is too long!", jd_Error_OutOfMemory, jd_Error_Fatal);
        }
        
        jd_AppLoadLib(app);
    }
    return app;
}

jd_V2F jd_PlatformWindowGetDrawSize(jd_PlatformWindow* window) {
    return window->size;
}

u32 jd_PlatformWindowGetDPI(jd_PlatformWindow* window) {
    u32 dpi = GetDpiForWindow(window->handle);
    return dpi;
}