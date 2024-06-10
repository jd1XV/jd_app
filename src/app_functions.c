#define JD_APP_RELOADABLE
#include "dep/jd_lib2/platforms/jd_Windows.h"

#include "dep/jd_lib2/jd_app.h"

jd_AppWindowFunction(WindowFunc) {
    static b32 done = false;
    static jd_String string = {0};
    static jd_Arena* arena = 0;
    while (!done) {
        arena = jd_ArenaCreate(0, 0);
        string = jd_DiskFileReadFromPath(arena, jd_StrLit("generated/utf8.txt"), true);
        done = true;
    }
    
    jd_DrawStringWithBG(jd_StrLit("OS_BaseFontWindows"), string, (jd_V2F){0.0f, 40.0f}, jd_TextOrigin_TopLeft, (jd_V4F){0.8f, 0.5f, 0.7f, 1.0f}, (jd_V4F){0.15f, 0.15f, 0.15f, 1.0f}, jd_PlatformWindowGetDrawSize(window).x);
    jd_WindowDrawFPS(window, jd_TextOrigin_BottomLeft, (jd_V2F){0.0f, jd_PlatformWindowGetDrawSize(window).y});
}

jd_AppWindowFunction(win2func) {
    static b32 done = false;
    static jd_String string = {0};
    static jd_Arena* arena = 0;
    while (!done) {
        arena = jd_ArenaCreate(0, 0);
        string = jd_DiskFileReadFromPath(arena, jd_StrLit("generated/utf8_random.txt"), true);
        done = true;
    }
    
    jd_DrawStringWithBG(jd_StrLit("OS_BaseFontWindows"), string, (jd_V2F){0.0f, 40.0f}, jd_TextOrigin_TopLeft, (jd_V4F){0.8f, 0.5f, 0.7f, 1.0f}, (jd_V4F){0.15f, 0.15f, 0.15f, 1.0f}, jd_PlatformWindowGetDrawSize(window).x);
    jd_WindowDrawFPS(window, jd_TextOrigin_BottomLeft, (jd_V2F){0.0f, jd_PlatformWindowGetDrawSize(window).y});
}