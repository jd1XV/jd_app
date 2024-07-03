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
    
    jd_DataNode* employees = jd_DataBankAddRecord(db->root, jd_StrLit("employees"));
    jd_DataNode* projects  = jd_DataBankAddRecord(db->root, jd_StrLit("projects"));
    jd_DataNode* owners    = jd_DataBankAddRecord(db->root, jd_StrLit("owners"));
    jd_DataNode* documents = jd_DataBankAddRecord(db->root, jd_StrLit("documents"));
    
    for (u64 i = 0; i < KILOBYTES(32); i++) {
        jd_DataNode* employee = jd_DataBankAddRecord(employees, jd_StrLit("employee"));
        
        jd_Value name_value = jd_ValueCastString(jd_StrLit("Abe Simpson"));
        jd_DataPointAdd(employee, jd_StrLit("name"), name_value);
        
        jd_Value years_exp_value = jd_ValueCastU64(5);
        jd_DataPointAdd(employee, jd_StrLit("years_exp"), years_exp_value);
        
        jd_Value title_v = jd_ValueCastString(jd_StrLit("Mechanical Engineer"));
        jd_DataPointAdd(employee, jd_StrLit("title"), title_v);
        
        jd_Value years_exp_hh = jd_ValueCastU64(4);
        jd_DataPointAdd(employee, jd_StrLit("years_exp_hh"), years_exp_hh);
        
        jd_Value specialty = jd_ValueCastString(jd_StrLit("Mechanical Engineer"));
        jd_DataPointAdd(employee, jd_StrLit("speciality"), specialty);
    }
    
    for (u64 i = 0; i < KILOBYTES(32); i++) {
        jd_DataNode* project = jd_DataBankAddRecord(projects, jd_StrLit("employee"));
        
        jd_Value name_value = jd_ValueCastString(jd_StrLit("Abe Simpson"));
        jd_DataPointAdd(project, jd_StrLit("name"), name_value);
        
        jd_Value years_exp_value = jd_ValueCastU64(5);
        jd_DataPointAdd(project, jd_StrLit("years_exp"), years_exp_value);
        
        jd_Value title_v = jd_ValueCastString(jd_StrLit("Mechanical Engineer"));
        jd_DataPointAdd(project, jd_StrLit("title"), title_v);
        
        jd_Value years_exp_hh = jd_ValueCastU64(4);
        jd_DataPointAdd(project, jd_StrLit("years_exp_hh"), years_exp_hh);
        
        jd_Value specialty = jd_ValueCastString(jd_StrLit("Mechanical Engineer"));
        jd_DataPointAdd(project, jd_StrLit("speciality"), specialty);
    }
    
    jd_Timer t = {0};
    jd_DFile* out = 0;
    jd_FunctionTimer(out = jd_DataBankSerialize(db), t);
    
    jd_DString* report = jd_DStringCreate(2048);
    jd_DStringAppendF(report, jd_StrLit("Serialize time: %f | "), t.stop);
    jd_DStringAppendF(report, jd_StrLit("File Size: %llu \n"), out->view.size);
    
    jd_DebugPrint(jd_DStringGet(report));
    
    jd_DebugBreak();
    
    
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
