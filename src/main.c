#include "dep/jd_lib2/platforms/jd_windows.h"
#define JD_DEBUG
#define JD_IMPLEMENTATION
#include "dep/jd_lib2/jd_unity.h"

jd_AppMainFn {
    jd_DebugPrintSysInfo();
    jd_ErrorLogInit(jd_StrLit("error_logs/log.txt"), 128);
    
    jd_DataBankConfig cfg = {0};
    jd_DataBank* db = jd_DataBankCreate(&cfg);
    
    jd_DataNodeOptions options = {0};
    
    options.display = jd_StrLit("Employees");
    jd_DataNode* employees = jd_DataBankAddRecord(db->root, jd_StrLit("employees"), &options);
    
    options.display = jd_StrLit("Projects");
    jd_DataBankAddRecord(db->root, jd_StrLit("projects"), &options);
    
    options.display = jd_StrLit("Owners");
    jd_DataBankAddRecord(db->root, jd_StrLit("owners"), &options);
    
    options.display = jd_StrLit("Documents");
    jd_DataBankAddRecord(db->root, jd_StrLit("documents"), &options);
    
    options.display = jd_StrLit("Employee");
    jd_DataNode* employee = jd_DataBankAddRecord(employees, jd_StrLit("employee"), &options);
    
    options.display = jd_StrLit("Name");
    jd_Value name_value = jd_ValueCastString(jd_StrLit("Abe Simpson"));
    jd_DataPointAdd(employee, jd_StrLit("name"), name_value, &options);
    
    options.display = jd_StrLit("Years of Experience");
    jd_Value years_exp_value = jd_ValueCastU64(5);
    jd_DataPointAdd(employee, jd_StrLit("years_exp"), years_exp_value, &options);
    
    
    jd_DFile* file = jd_DataBankSerialize(db);
    jd_DataBank* ds = jd_DataBankDeserialize(jd_DFileGet(file));
    
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
