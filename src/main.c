#include <platforms/jd_windows.h>
#define JD_DEBUG
#define JD_IMPLEMENTATION
#include <jd_unity.h>

jd_AppMainFn {
    jd_DebugPrintSysInfo();
    jd_ErrorLogInit(jd_StrLit("error_logs/log.txt"), 64);
    
    jd_App* app = jd_AppCreate(&(jd_AppConfig){JD_AM_RELOADABLE, jd_StrLit("jd_app_test")});
    
    jd_WindowConfig w_config = {
        .app = app,
        .title = jd_StrLit("jd_app Test"),
        .id_string = jd_StrLit("main_window"),
        .function_name = jd_StrLit("jd_app_test_window_main"),
        .titlebar_style = jd_TitleBarStyle_Right
    };
    
    jd_Window* main_window = jd_AppCreateWindow(&w_config);
    
    while (jd_AppIsRunning(app)) {
        jd_AppPlatformUpdate(app);
    }
    
    
    return 0;
}
