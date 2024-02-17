#include <jd_lib.h>
#include <Windows.h>


typedef struct jd_App {
    jd_ArenaArray* _windows; // type: jd_window
    jd_Arena* arena;
    jd_Arena* frame_arena;
} jd_App;

typedef struct jd_Window {
    jd_Arena* arena;
    jd_V2F pos;
    jd_V2F size;
    jd_V2S32 pos_i;
    jd_V2S32 size_i;
    
    HWND handle;
    WNDCLASSA wndclass;
    jd_StrA wndclass_str;
    PIXELFORMATDESCRIPTOR pixel_format_descriptor;
    
    HGLRC ogl_context;
    HDC device_context;
    
    jd_Renderer* renderer;
    jd_StrA title;
} jd_Window;

LRESULT CALLBACK jd_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
        
        case WM_SIZE: {
            
        } return 0;
        
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


jd_Window* jd_AppPlatformCreateWindow(jd_App* app, jd_StrA id_str, jd_StrA title) {
    jd_Window window = {0};
    
    // Register the window class.
    jd_Arena* arena = jd_ArenaCreate(0, 0);
    window.wndclass_str = jd_StrDup(arena, id_str);
    window.title = jd_StrDup(arena, title);
    
    WNDCLASSA wc = {0};
    
    wc.lpfnWndProc   = jd_WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = window.wndclass_str.val;
    
    RegisterClass(&wc);
    
    // Create the window.
    
    window.handle = CreateWindowExA(
                                    CS_OWNDC,                              // Optional window styles.
                                    window.wndclass_str.val,               // Window class
                                    window.title.val,                      // Window text
                                    WS_OVERLAPPEDWINDOW,                   // Window style
                                    
                                    // Size and position
                                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                    
                                    NULL,       // Parent window    
                                    NULL,       // Menu
                                    hInstance,  // Instance handle
                                    NULL        // Additional application data
                                    );
    
    if (window.handle == NULL)
    {
        return 0;
    }
    
    window.pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
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
    
    window.device_context = GetDC(window.handle);
    if (!window.device_context) {
        // TODO: err
    }
    
    s32 pf = ChoosePixelFormat(window.device_context, &window.pfd);
    if (pf == 0) {
        // TODO: err
    }
    
    if (!(SetPixelFormat(window.device_context, pf, &window.pfd))) {
        // TODO: err
    }
    
    window.ogl_context = wglCreateContext(main_window_device_context);
    wglMakeCurrent(window.device_context, window.ogl_context);
    gladLoadGL();
    
    ShowWindow(window.handle, cmdshow);
    
}