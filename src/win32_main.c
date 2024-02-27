#if 0
#define JD_LIB_IMPLEMENTATION
#define JD_LIB_WIN32
#include <jd_lib.h>
#endif

#include "dep/jd_lib2/platforms/jd_windows.h"
#define JD_IMPLEMENTATION
#include "dep/jd_lib2/jd_all.h"
#include "win32_app.h"

jd_AppWindowFunction(WindowFunc) {
    
}

i32 APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstPrev, PSTR cmdline, i32 cmdshow) {
    
    // Run the message loop.
    jd_Arena* arena = jd_ArenaCreate(0, 0);
    jd_App app = {
        .lock = jd_UserLockCreate(arena, 16),
        .arena = jd_ArenaCreate(0, 0),
        .frame_arena = jd_ArenaCreate(GIGABYTES(1), 0),
        .instance = hInstance
    };
    
    jd_Window* main_window = jd_AppPlatformCreateWindow(&app, jd_StrLit("MainWindowID"), jd_StrLit("Hello World!"));
    while (app.window_count > 0) {
        jd_AppPlatformUpdate(&app);
    }
    
    //wglDeleteContext(main_window_context);
    
    return 0;
}
