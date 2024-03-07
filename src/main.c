#include "dep/jd_lib2/platforms/jd_windows.h"
#define JD_DEBUG
#define JD_IMPLEMENTATION
#include "dep/jd_lib2/jd_all.h"

#include <stdio.h>

jd_AppMainFn {
    jd_ErrorLogInit(jd_StrLit("error_logs/log.txt"), 128);
    jd_App* app = jd_AppCreate(&(jd_AppConfig){JD_AM_RELOADABLE, jd_StrLit("jd_app_test")});
    
    jd_LogError("This is a test error!", jd_Error_AccessDenied, jd_Error_Critical);
    jd_LogError("This is a another error!", jd_Error_AccessDenied, jd_Error_Critical);
    jd_LogError("This is a yet one more error!", jd_Error_AccessDenied, jd_Error_Critical);
    
    jd_WindowConfig w_config = {
        .app = app,
        .title = jd_StrLit("Hello World!"),
        .id_str = jd_StrLit("MainWindowID"),
        .function_name = jd_StrLit("WindowFunc")
    };
    
    jd_Window* main_window = jd_AppPlatformCreateWindow(&w_config);
    while (jd_AppIsRunning(app)) {
        jd_AppPlatformUpdate(app);
    }
    
    return 0;
}
