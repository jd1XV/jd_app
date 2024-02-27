#include "dep/jd_lib2/jd_all.h"
#include <Windows.h>
#include "win32_app.h"

LRESULT CALLBACK jd_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void jd_AppUpdatePlatformWindows(jd_App* app) {
    jd_UserLockGet(app->lock);
    for (u64 i = 0; i < app->window_count; i++) {
        jd_Window* window = app->windows[i];
        
        MSG msg = {0};
        while (GetMessage(&msg, window->handle, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        // get the size
        RECT client_rect = {0};
        GetClientRect(window->handle, &client_rect);
        window->size.w = client_rect.right;
        window->size.h = client_rect.bottom;
        
        RECT window_rect = {0};
        GetWindowRect(window->handle, &window_rect);
        window->pos.x = window_rect.left;
        window->pos.y = window_rect.top;
        
        window->func(window);
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
                jd_MemMove(&app->windows[window_index], &app->windows[window_index] + 1, (app->window_count - (window_index + 1)) * sizeof(jd_Window*)); 
            }
            break;
        }
    }
    
    app->window_count--;
    
    jd_ArenaRelease(window->arena);
}

jd_Window* jd_AppPlatformCreateWindow(jd_App* app, jd_String id_str, jd_String title) {
    if (app->window_count >= JD_APP_MAX_WINDOWS) return 0;
    // Register the window class.
    jd_Arena* arena = jd_ArenaCreate(0, 0);
    jd_Window* window = jd_ArenaAlloc(arena, sizeof(*window));
    window->app = app;
    window->wndclass_str = jd_StringPush(arena, id_str);
    window->title = jd_StringPush(arena, title);
    window->arena = arena;
    
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
        return 0;
    }
    
    SetWindowLongPtrA(window->handle, 0, (LONG_PTR)window);
    
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
    
    window->pixel_format_descriptor = pfd;
    
    window->device_context = GetDC(window->handle);
    if (!window->device_context) {
        // TODO: err
    }
    
    i32 pf = ChoosePixelFormat(window->device_context, &window->pixel_format_descriptor);
    if (pf == 0) {
        // TODO: err
    }
    
    if (!(SetPixelFormat(window->device_context, pf, &window->pixel_format_descriptor))) {
        // TODO: err
    }
    
    window->ogl_context = wglCreateContext(window->device_context);
    wglMakeCurrent(window->device_context, window->ogl_context);
    gladLoadGL();
    
    ShowWindow(window->handle, SW_SHOW);
    
    wglMakeCurrent(window->device_context, window->ogl_context);
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

LRESULT CALLBACK jd_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY: {
            jd_Window* window = (jd_Window*)GetWindowLongPtrA(hwnd, 0);
            jd_App* app = window->app;
            jd_UserLockGet(app->lock);
            jd_AppPlatformCloseWindow(window);
            jd_UserLockRelease(app->lock);
            break;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
