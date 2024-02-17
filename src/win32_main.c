#define JD_LIB_IMPLEMENTATION
#define JD_LIB_WIN32
#include <jd_lib.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

s32 APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstPrev, PSTR cmdline, s32 cmdshow)
{
    
    // Run the message loop.
    
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    wglMakeCurrent(main_window_device_context, main_window_context);
    wglDeleteContext(main_window_context);
    
    return 0;
}
