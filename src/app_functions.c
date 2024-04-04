#define JD_APP_RELOADABLE
#include "dep/jd_lib2/platforms/jd_Windows.h"
#include "dep/jd_lib2/jd_app.h"

jd_AppWindowFunction(WindowFunc) {
    static jd_String utf8 = {0};
    static b32 done = 0;
    while (!done) {
        utf8 = jd_DiskFileReadFromPath(window->arena, jd_StrLit("generated/utf8.txt"));
        done = true;
    }
    
    //utf8 = jd_StrLit("Will this work?");
    
    jd_DrawRect(window->renderer, (jd_V2F){0.0f, 0.0f}, window->renderer->render_size, (jd_V4F){0.02f, 0.02f, 0.02f, 1.0f});
    jd_DrawString(window->renderer, window->renderer->default_face, utf8, (jd_V2F){0.0f, 0.0f}, (jd_V4F){1.0, 1.0, 0.7, 1.0f}, window->renderer->render_size.x);
    jd_RendererDraw(window->renderer);
}