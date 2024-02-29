#ifndef JD_RENDERER_H
#include "jd_renderer.h"
#endif

#include "../glad/glad_wgl.h"

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
    jd_Renderer* renderer;
    
    f32 dpi_scaling;
    
    jd_String title;
    b8 closed;
} jd_Window;

LRESULT CALLBACK jd_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static void _jd_Internal_WindowUpdateFunctionStub(jd_Window* window) {}

void jd_AppUpdatePlatformWindows(jd_App* app) {
    jd_UserLockGet(app->lock);
    for (u64 i = 0; i < app->window_count; i++) {
        jd_Window* window = app->windows[i];
        MSG msg = {0};
        while (PeekMessage(&msg, window->handle, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } 
        
        if (window->closed) {
            jd_AppPlatformCloseWindow(window);
        }
    }
    
    for (u64 i = 0; i < app->window_count; i++) {
        jd_Window* window = app->windows[i];
        
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
        SwapBuffers(window->device_context);
    }
    
    jd_UserLockRelease(app->lock);
}

void jd_AppPlatformCloseWindow(jd_Window* window) {
    jd_App* app = window->app;
    u64 window_index = 0;
    for (window_index; window_index < app->window_count; window_index++) {
        if (app->windows[window_index] == window) {
            jd_ZeroMemory(&app->windows[window_index], sizeof(jd_Window*));
            if (window_index != app->window_count - 1) {
                jd_MemMove(&app->windows[window_index], &app->windows[window_index] + 1, (app->window_count - (window_index + 1)) * sizeof(jd_Window*)); }
            break;
        }
    }
    
    app->window_count--;
    
    jd_ArenaRelease(window->arena);
}

jd_Window* jd_AppPlatformCreateWindow(jd_App* app, jd_String id_str, jd_String title, jd_AppWindowFunctionPtr function) {
    if (app->window_count >= JD_APP_MAX_WINDOWS) return 0;
    // Register the window class.
    jd_Arena* arena = jd_ArenaCreate(0, 0);
    jd_Window* window = jd_ArenaAlloc(arena, sizeof(*window));
    window->app = app;
    window->wndclass_str = jd_StringPush(arena, id_str);
    window->title = jd_StringPush(arena, title);
    window->arena = arena;
    window->func = function;
    window->dpi_scaling = 1.0f; // TODO: Handle HiDPI
    
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = jd_WindowProc;
    wc.hInstance     = app->instance;
    wc.lpszClassName = window->wndclass_str.mem;
    wc.cbWndExtra    = sizeof(jd_Window*);
    
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
                                     app->instance,  // Instance handle
                                     NULL        // Additional application data
                                     );
    
    if (window->handle == NULL)
    {
        // err
    }
    
    window->device_context = GetDC(window->handle);
    
    if (!window->device_context) {
        // TODO: err
    }
    
    
    SetWindowLongPtrA(window->handle, 0, (LONG_PTR)window);
    
    
    WNDCLASSEX dummy_wc = {0};
    dummy_wc.cbSize = sizeof(WNDCLASSEX);
    dummy_wc.lpfnWndProc   = DefWindowProc;
    dummy_wc.hInstance     = app->instance;
    dummy_wc.lpszClassName = "dummy_wndclass";
    dummy_wc.cbWndExtra    = sizeof(jd_Window*);
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
                                     app->instance,  // Instance handle
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
    jd_UserLockGet(app->lock);
    app->windows[app->window_count] = window;
    app->window_count++;
    jd_UserLockRelease(app->lock);
    
    return window;
}

b32 jd_AppPlatformUpdate(jd_App* app) {
    jd_AppUpdatePlatformWindows(app);
    return true;
}

b32 jd_AppIsRunning(jd_App* app) {
    return (app->window_count > 0);
}

LRESULT CALLBACK jd_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY: {
            jd_Window* window = (jd_Window*)GetWindowLongPtrA(hwnd, 0);
            window->closed = true;
            break;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

jd_App* jd_AppCreate() {
    jd_Arena* arena = jd_ArenaCreate(0, 0);
    jd_App* app = jd_ArenaAlloc(arena, sizeof(*app));
    app->lock = jd_UserLockCreate(arena, 16);
    app->arena = arena;
    app->frame_arena = jd_ArenaCreate(GIGABYTES(1), 0);
    app->instance = GetModuleHandle(NULL);  
    return app;
}

jd_V2F jd_WindowGetDrawSize(jd_Window* window) {
    return window->size;
}