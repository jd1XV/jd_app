#define JD_APP_STATIC
#include "dep/jd_lib2/platforms/jd_Windows.h"
#include "dep/jd_lib2/jd_app.h"
#include "dep/jd_lib2/jd_ui.h"

jd_AppWindowFunction(WindowFunc) {
    jd_WindowDrawFPS(window, jd_TextOrigin_TopLeft, (jd_V2F){0.0f, 40.0f});
}