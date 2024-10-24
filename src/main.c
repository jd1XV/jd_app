#include "dep/jd_lib2/platforms/jd_windows.h"
#define JD_DEBUG
#define JD_IMPLEMENTATION
#include "dep/jd_lib2/jd_unity.h"

jd_AppMainFn {
    jd_DebugPrintSysInfo();
    jd_ErrorLogInit(jd_StrLit("error_logs/log.txt"), 128);
    
    jd_App* app = jd_AppCreate(&(jd_AppConfig){JD_AM_RELOADABLE, jd_StrLit("jd_app_test")});
    
    jd_PlatformWindowConfig w_config = {
        .app = app,
        .title = jd_StrLit("jd_app Test"),
        .id_str = jd_StrLit("MainWindowID"),
        .function_name = jd_StrLit("WindowFunc"),
        //.function_ptr = WindowFunc,
        .titlebar_style = jd_TitleBarStyle_Right
    };
    
    jd_PlatformWindow* main_window = jd_AppPlatformCreateWindow(&w_config);
    
    w_config.id_str = jd_StrLit("win2id");
    w_config.function_name = jd_StrLit("win2func");
    
    jd_PlatformWindow* win2 = jd_AppPlatformCreateWindow(&w_config);
    
    while (jd_AppIsRunning(app)) {
        jd_AppPlatformUpdate(app);
    }
    
    return 0;
}
