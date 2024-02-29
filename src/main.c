#include "dep/jd_lib2/platforms/jd_windows.h"
#define JD_IMPLEMENTATION
#include "dep/jd_lib2/jd_all.h"

#include <stdio.h>

jd_AppWindowFunction(WindowFunc) {
    jd_DrawRect(window->renderer, (jd_V2F){0.0f, 0.0f}, window->renderer->render_size, (jd_V4F){0.1f, 0.1f, .1f, 1.0f});
    //jd_DrawRect(window->renderer, {0.0f, window->renderer->render_size.y - 15.0f}, {window->renderer->render_size.x, 15.0f}, {0.15f, 0.15f, 0.15f, 1.0f});
    jd_DrawStringWithBG(window->renderer, window->renderer->default_font, jd_StrLit("This is a test"), (jd_V2F){0.0f, window->renderer->render_size.y}, (jd_V4F){1.0, 1.0, 1.0, 1.0f}, (jd_V4F){0.15, 0.15, 0.15, 1.0f}, 1280.0f);
}

jd_AppMainFn {
    jd_App* app = jd_AppCreate();
    c8 buf[4096] = {0};
    _getcwd(buf, 4096);
    OutputDebugStringA(buf);
    
    
    jd_Window* main_window = jd_AppPlatformCreateWindow(app, jd_StrLit("MainWindowID"), jd_StrLit("Hello World!"), WindowFunc);
    while (jd_AppIsRunning(app)) {
        jd_AppPlatformUpdate(app);
    }
    
    return 0;
}
