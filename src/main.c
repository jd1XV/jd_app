#include "dep/jd_lib2/platforms/jd_windows.h"
#define JD_DEBUG
//#define JD_CONSOLE
#define JD_IMPLEMENTATION
#include "dep/jd_lib2/jd_unity.h"

#include <stdio.h>

jd_AppMainFn {
    jd_DebugPrintSysInfo();
    jd_ErrorLogInit(jd_StrLit("error_logs/log.txt"), 128);
    jd_App* app = jd_AppCreate(&(jd_AppConfig){JD_AM_RELOADABLE, jd_StrLit("jd_app_test")});
    
    jd_String utf8_string = jd_DiskFileReadFromPath(app->arena, jd_StrLit("generated/utf8.txt"));
    
    jd_UTFDecodedString dec_str = jd_UnicodeDecodeUTF8String(app->arena, jd_UnicodeTF_UTF32, utf8_string, false);
    
    jd_File out = {
        .mem = dec_str.utf8,
        .size = dec_str.count * 4
    };
    
    jd_DiskWriteFileToPath(out, jd_StrLit("generated/utf32.txt"));
    
    jd_String utf8_encoded = jd_UnicodeEncodeUTF32toUTF8(app->arena, dec_str, false);
    jd_DiskWriteFileToPath(utf8_encoded, jd_StrLit("generated/utf8_gen.txt"));
    
    jd_LogError("This is a test error!", jd_Error_AccessDenied, jd_Error_Critical);
    jd_LogError("This is a another error!", jd_Error_AccessDenied, jd_Error_Critical);
    jd_LogError("This is a yet one more error!", jd_Error_AccessDenied, jd_Error_Critical);
    
#if 0    
    u64 codepoints = 12000;
    b8* touched = jd_ArenaAlloc(app->arena, _jd_GlyphHashTableLimit);
    i64 collisions = 0;
    
    for (u64 i = 0; i < codepoints; i++) {
        u32 hash = jd_GlyphHashGetIndexForCodepoint(i);
        if (touched[hash] == true) {
            collisions++;
        } else {
            touched[hash] = true;
        }
    }
    
    i64 theoretical_min = (codepoints < _jd_GlyphHashTableLimit) ? 0 : codepoints - _jd_GlyphHashTableLimit;
    
    printf("Minimum collisions: %lli (%f pct. of codepoints).\n" \
           "Minimum theoretical collisions: %lli (%f pct. of codepoints).\n",
           collisions, ((f64)collisions / (f64)codepoints) * 100.0f, theoretical_min, ((f64)theoretical_min / (f64)codepoints) * 100.0f);
    
#endif
    
    jd_WindowConfig w_config = {
        .app = app,
        .title = jd_StrLit("Hello World!"),
        .id_str = jd_StrLit("MainWindowID"),
        .function_name = jd_StrLit("WindowFunc"),
        .window_style = jd_WS_Dark
    };
    
    jd_Window* main_window = jd_AppPlatformCreateWindow(&w_config);
    while (jd_AppIsRunning(app)) {
        jd_AppPlatformUpdate(app);
    }
    
    return 0;
}
