#define JD_APP_RELOADABLE
#include "dep/jd_lib2/platforms/jd_Windows.h"
#include "dep/jd_lib2/jd_app.h"

jd_AppWindowFunction(WindowFunc) {
    jd_DrawRect(window->renderer, (jd_V2F){0.0f, 0.0f}, window->renderer->render_size, (jd_V4F){0.1f, 0.1f, .1f, 1.0f});
    //jd_DrawRect(window->renderer, {0.0f, window->renderer->render_size.y - 15.0f}, {window->renderer->render_size.x, 15.0f}, {0.15f, 0.15f, 0.15f, 1.0f});
    jd_DrawStringWithBG(window->renderer, window->renderer->default_font, jd_StrLit("Hot reloading finished"), (jd_V2F){0.0f, window->renderer->render_size.y}, (jd_V4F){1.0, 1.0, 1.0, 1.0f}, (jd_V4F){0.15, 0.15, 0.15, 1.0f}, 1280.0f);
}