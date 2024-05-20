#define JD_APP_RELOADABLE
#include "dep/jd_lib2/platforms/jd_Windows.h"
#include "dep/jd_lib2/jd_app.h"
#include "dep/jd_lib2/jd_ui.h"

jd_AppWindowFunction(WindowFunc) {
    static f64 frame_time = 0.0f;
    static jd_StopWatch watch = {0};
    
    static jd_DString* fps_string = 0;
    
    static jd_String utf8 = {0};
    static b32 done = 0;
    
    while (!done) {
        fps_string = jd_DStringCreate(256);
        done = true;
    }
    
    watch = jd_StopWatchStop(watch);
    frame_time = watch.stop;
    watch = jd_StopWatchStart();
    
    f64 fps_f = 1000.0f / frame_time;
    u32 fps_i = (u32)fps_f;
    
    jd_String string = {0};
    string.mem = fps_string->mem;
    string.count = fps_string->count;
}