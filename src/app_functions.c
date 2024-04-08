#define JD_APP_RELOADABLE
#include "dep/jd_lib2/platforms/jd_Windows.h"
#include "dep/jd_lib2/jd_app.h"

jd_AppWindowFunction(WindowFunc) {
    static f64 frame_time = 0.0f;
    static jd_StopWatch watch = {0};
    
    static jd_DString* fps_string = 0;
    
    static jd_String utf8 = {0};
    static b32 done = 0;
    
    while (!done) {
        utf8 = jd_DiskFileReadFromPath(window->arena, jd_StrLit("generated/utf8.txt"));
        fps_string = jd_DStringCreate(256);
        done = true;
    }
    
    watch = jd_StopWatchStop(watch);
    frame_time = watch.stop;
    watch = jd_StopWatchStart();
    
    f64 fps_f = 1000.0f / frame_time;
    u32 fps_i = (u32)fps_f;
    
    jd_DStringClear(fps_string);
    jd_DStringAppendU32(fps_string, fps_i, 10);
    
    jd_String string = {0};
    string.mem = fps_string->mem;
    string.count = fps_string->count;
    
    jd_DrawRect(window->renderer, (jd_V2F){0.0f, 0.0f}, window->renderer->render_size, (jd_V4F){0.02f, 0.02f, 0.02f, 1.0f});
    jd_DrawString(window->renderer, window->renderer->default_face, utf8, (jd_V2F){0.0f, 0.0f}, jd_TextOrigin_TopLeft, (jd_V4F){1.0, 1.0, 0.7, 1.0f}, window->renderer->render_size.x);
    jd_DrawStringWithBG(window->renderer, window->renderer->default_face, string, (jd_V2F){0.0f, 0.0f}, jd_TextOrigin_TopLeft, (jd_V4F){1.0, 1.0, 1.0, 1.0}, (jd_V4F){.8, 0.0, 0.0, 1.0}, 1280.0f);
}