#define JD_APP_RELOADABLE
#include "dep/jd_lib2/platforms/jd_Windows.h"
#include "dep/jd_lib2/jd_app.h"
#include "dep/jd_lib2/jd_ui.h"

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
    
#if 0    
    jd_DrawStringWithBG(jd_StrLit("OS_BaseFontWindows"), string, (jd_V2F){0.0f, 40.0f}, jd_TextOrigin_TopLeft, (jd_V4F){0.8f, 0.5f, 0.7f, 1.0f}, (jd_V4F){0.15f, 0.15f, 0.15f, 1.0f}, jd_PlatformWindowGetDrawSize(window).x);
#endif
    
    static jd_DString* label = 0;
    if (!label) {
        label = jd_DStringCreate(4096);
        jd_DStringAppend(label, jd_StrLit("Button "));
    }
    
    jd_UIFontPush(jd_StrLit("OS_BaseFontWindows"));
    jd_UIStylePush(&jd_default_style_dark);
    
    jd_UIBoxConfig config = {0};
    config.clickable = true;
    config.label = jd_DStringGet(label);
    config.string_id = jd_StrLit("##dynamiclabelbutton");
    config.rect = (jd_RectF32){50.0f, 50.0f, 200.f, 45.0f};
    config.use_padding = true;
    config.cursor = jd_Cursor_Hand;
    
    if (jd_UIBox(&config).l_clicked) {
        jd_DStringAppend(label, jd_StrLit("Clicked! "));
    }
    
    jd_UIPopFont(jd_StrLit("OS_BaseFontWindows"));
    jd_UIStylePop();
    
    jd_WindowDrawFPS(window, jd_TextOrigin_BottomLeft, (jd_V2F){0.0f, jd_PlatformWindowGetDrawSize(window).y});
}