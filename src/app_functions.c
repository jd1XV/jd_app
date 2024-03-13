#define JD_APP_RELOADABLE
#include "dep/jd_lib2/platforms/jd_Windows.h"
#include "dep/jd_lib2/jd_app.h"

jd_AppWindowFunction(WindowFunc) {
    static u32 done = 0;
    //jd_DrawRect(window->renderer, (jd_V2F){0.0f, 0.0f}, window->renderer->render_size, (jd_V4F){0.1f, 0.1f, .1f, 1.0f});
    //jd_DrawRect(window->renderer, {0.0f, window->renderer->render_size.y - 15.0f}, {window->renderer->render_size.x, 15.0f}, {0.15f, 0.15f, 0.15f, 1.0f});
    //jd_DrawStringWithBG(window->renderer, window->renderer->default_font, jd_StrLit("Hot reloading finished for real for real"), (jd_V2F){0.0f, window->renderer->render_size.y}, (jd_V4F){1.0, 1.0, 0.7, 1.0f}, (jd_V4F){0.25, 0.53, 0.34, 1.0f}, 1280.0f);
    
    while (!done) {
        jd_DString* d_string = jd_DStringCreate(MEGABYTES(2));
        for (u8 i =0 ; i< 8; i++) {
            jd_DStringAppend(d_string, jd_StrLit("continuations["));
            jd_DStringAppendU8(d_string, i, 10);
            jd_DStringAppend(d_string, jd_StrLit("] = continuation_tables[chunk_bytes["));
            jd_DStringAppendU8(d_string, i, 10);
            jd_DStringAppend(d_string, jd_StrLit("]];\n"));
        }
        
        jd_File file = {
            .mem = d_string->mem,
            .size = d_string->count
        };
        jd_DiskWriteFileToPath(file, jd_StrLit("generated/continuation.txt"));
        done = 1;
    }
    
}

