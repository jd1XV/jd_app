#define JD_APP_RELOADABLE
#include <platforms/jd_Windows.h>
#include <jd_app.h>
#include <jd_ui.h>
#include <jd_icon_font.h>

static f32 dpi_scaling = 0.0f;
static jd_Arena* perm_arena = 0;
static jd_Arena* frame_arena = 0;

void StatsPanel(jd_Window* window) {
    static f32* frametimes = 0;
    static u32  frametime_sample_count = 16;
    
    if (!frametimes) {
        frametimes = jd_ArenaAlloc(perm_arena, sizeof(f32) * frametime_sample_count);
    }
    
    jd_UIRowGrowBegin(jd_StrLit("statspanel"), jd_V2F(5.0f * dpi_scaling, 5.0f * dpi_scaling), 5.0 * dpi_scaling, 0);
    {
        static u64 index = 0;
        static u64 update_delay = 2;
        frametimes[index % frametime_sample_count] = jd_WindowGetFrameTime(window);
        index++;
        
        static f32 framerate = 0.0f;
        
        if (index % update_delay == 0) {
            f32 accum = 0;
            for (u64 i = 0; i < frametime_sample_count; i++) {
                accum += frametimes[i];
            }
            
            f32 avg = accum / (f32)frametime_sample_count;
            framerate = 1000.0 / avg;
            update_delay = jd_Max(2, framerate);
            
        }
        
        jd_String label = jd_StringPushF(frame_arena, jd_StrLit("FPS: %.0f###FPS_Readout"), framerate);
        
        jd_UILabel(label);
        jd_UILabel(jd_StrLit("|###1"));
        
        jd_UIBoxRec* debug_box = jd_UIGetDebugBox();
        if (debug_box) {
            jd_UILabel(jd_StrLit("|###3"));
            jd_String formatted_string = jd_StringPushF(frame_arena, 
                                                        jd_StrLit("String ID: %s - RSize: X: %.2f Y: %.2f - FSize: X: %.2f Y: %.2f"),
                                                        debug_box->string_id.mem,
                                                        debug_box->requested_size.x,
                                                        debug_box->requested_size.y,
                                                        debug_box->rect.x1,
                                                        debug_box->rect.y1);
            
            jd_UILabel(formatted_string);
        }
        
    }
    jd_UIRegionEnd();
}

jd_AppWindowFunction(jd_app_test_window_main) {
    static b32 init = false;
    static jd_UIViewport* vp = 0;
    
    static jd_String text_edit_string = {0};
    static u32 text_edit_string_size = 1024;
    
    static jd_String text_edit_string2 = {0};
    static u32 text_edit_string_size2 = 1024;
    
    if (!init) {
        vp = jd_UIInitForWindow(window);
        perm_arena = jd_ArenaCreate(0, 0);
        frame_arena = jd_ArenaCreate(0, 0);
        init = true;
        text_edit_string = jd_DiskFileReadFromPath(perm_arena, jd_StrLit("assets/emoji_test.txt"), true);
    }
    
    jd_ArenaPopTo(frame_arena, 0);
    
    dpi_scaling = jd_WindowGetDPIScale(window);
    
    jd_UIBegin(vp);
    jd_UIFontPush(jd_OSBaseFont(), 12 * dpi_scaling);
    jd_UIWindowRegionBegin(jd_V2F(400.0f, 400.0f), jd_UILayout_TopToBottom, 0.0f);
    {
        jd_AppDefaultTitlebar(window);
        StatsPanel(window);
        
        jd_UIInputTextBoxMultiline(jd_StrLit("multiline_text_edit"), jd_UIMakeShape(jd_UIGrow, jd_UIGrow), &text_edit_string, jd_StrLit("This is where the emoji should go"), text_edit_string.count); 
    }
    jd_UIRegionEnd(); 
    jd_UIFontPop();
    jd_UIEnd();
    
}