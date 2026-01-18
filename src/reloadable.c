#define JD_APP_RELOADABLE
#include <platforms/jd_Windows.h>
#include <jd_app.h>
#include <jd_ui.h>
#include <jd_icon_font.h>

jd_AppWindowFunction(WindowFunc) {
    static b32 done = false;
    static jd_String string = {0};
    static jd_Arena* arena = 0;
    static jd_UIViewport* vp2 = 0;
    while (!done) {
        arena = jd_ArenaCreate(0, 0);
        vp2 = jd_UIInitForWindow(window);
        done = true;
    }
    
    jd_UIBegin(vp2);
    jd_UIFontPush(jd_StrLit("OS_BaseFontWindows"));
    
    jd_UIFontPop();
    jd_UIEnd();
    
    jd_WindowDrawFPS(window, jd_TextOrigin_BottomLeft, (jd_V2F){0.0f, jd_WindowGetDrawSize(window).y});
}

jd_AppWindowFunction(win2func) {
    static b32 init = false;
    static jd_UIViewport* vp = 0;
    static jd_Arena* perm_arena = 0;
    static jd_Arena* frame_arena = 0;
    static f32* frametimes = 0;
    static u32  frametime_sample_count = 16;
    static jd_String text_edit_string = {0};
    static u32 text_edit_string_size = 1024;
    
    static jd_String text_edit_string2 = {0};
    static u32 text_edit_string_size2 = 1024;
    
    if (!init) {
        vp = jd_UIInitForWindow(window);
        frame_arena = jd_ArenaCreate(0, 0);
        perm_arena = jd_ArenaCreate(0, 0);
        text_edit_string.mem = jd_ArenaAlloc(perm_arena, sizeof(c8) * text_edit_string_size);
        text_edit_string.count = 0;
        text_edit_string2.mem = jd_ArenaAlloc(perm_arena, sizeof(c8) * text_edit_string_size);
        text_edit_string.count = 0;
        frametimes = jd_ArenaAlloc(perm_arena, sizeof(f32) * frametime_sample_count);
        init = true;
    }
    
    jd_ArenaPopTo(frame_arena, 0);
    
    jd_UIBegin(vp);
    jd_UIFontPush(jd_StrLit("OS_BaseFontWindows"));
    jd_UIStyle style = jd_default_style_dark;
    jd_V4F global_bg_color = {.1, .3, .8, 1.0};
    style.bg_color = global_bg_color;
    
    jd_UIRegionBegin(jd_StrLit("##globalparent"), &style, (jd_UISize){0}, jd_UILayout_TopToBottom, 0.0f, false);
    {
        jd_AppDefaultTitlebar(window);
        
        jd_UISize size = {
            .rule = {jd_UISizeRule_Grow, jd_UISizeRule_FitChildren},
        };
        
        jd_UIRegionBegin(jd_StrLit("##menuregion"), &jd_default_style_dark, size, jd_UILayout_LeftToRight, 3.0f, true);
        {
            static b32 show_file_menu = false;
            jd_UISize menu_size = {
                .rule = {jd_UISizeRule_FitChildren, jd_UISizeRule_FitChildren} 
            };
            
            if (jd_UILabelButton(jd_StrLit("File")).last_active) {
                jd_UIRegionBeginAnchored(jd_StrLit("##filemenu"), &jd_default_style_dark, jd_UIGetLastBox(), (jd_V2F){0.0, 1.0}, (jd_V2F){0}, menu_size, jd_UILayout_TopToBottom, 0.0f, 0);
                {
                    
                    jd_UILabelButton(jd_StrLit("New###1"));
                    jd_UILabelButton(jd_StrLit("Open###2"));
                    jd_UILabelButton(jd_StrLit("Save###3"));
                    jd_UILabelButton(jd_StrLit("Save As###4"));
                }
                jd_UIRegionEnd();
            }
            
            
            if (jd_UILabelButton(jd_StrLit("Edit")).last_active) {
                jd_UIRegionBeginAnchored(jd_StrLit("##editmenu"), &jd_default_style_dark, jd_UIGetLastBox(), (jd_V2F){0.0, 1.0}, (jd_V2F){0}, menu_size, jd_UILayout_TopToBottom, 0.0f, 0);
                {
                    jd_UILabelButton(jd_StrLit("Assify"));
                    jd_UILabelButton(jd_StrLit("Dick Up"));
                    jd_UILabelButton(jd_StrLit("Enshittify"));
                    jd_UILabelButton(jd_StrLit("Crash the program"));
                    jd_UILabelButton(jd_StrLit("Free Transform"));
                    jd_UILabelButton(jd_StrLit("Shuffle"));
                    
                }
                jd_UIRegionEnd();
            }
            
            if (jd_UILabelButton(jd_StrLit("About")).last_active) {
                jd_UIRegionBeginAnchored(jd_StrLit("##optionsmenu"), &jd_default_style_dark, jd_UIGetLastBox(), (jd_V2F){0.0, 1.0}, (jd_V2F){0}, menu_size, jd_UILayout_TopToBottom, 0.0f, 0);
                {
                    
                }
                jd_UIRegionEnd();
            }
        }
        jd_UIRegionEnd();
        
        jd_UIRegionBegin(jd_StrLit("##middleregion"), &style, (jd_UISize){0}, jd_UILayout_LeftToRight, 0.0f, false);
        {
            jd_UISize sidebar_size = {
                .rule = {jd_UISizeRule_FitChildren, jd_UISizeRule_Grow}
            };
            
            f64 dpi_scale = jd_WindowGetDPIScale(window);
            
            jd_UIRegionBegin(jd_StrLit("##sidebar"), &jd_default_style_dark, sidebar_size, jd_UILayout_TopToBottom, 0.0f, false);
            {
                jd_V2F button_size = {50.0f * dpi_scale, 50.0f * dpi_scale};
                jd_UIFixedSizeButton(jd_StrLit(jd_FontIcon_Up), button_size, (jd_V2F){0.5, 0.5});
                jd_UIFixedSizeButton(jd_StrLit(jd_FontIcon_Down), button_size, (jd_V2F){0.5, 0.5});
                jd_UIFixedSizeButton(jd_StrLit(jd_FontIcon_Left), button_size, (jd_V2F){0.5, 0.5});
                jd_UIFixedSizeButton(jd_StrLit(jd_FontIcon_Right), button_size, (jd_V2F){0.5, 0.5});
            }
            jd_UIRegionEnd();
            
            jd_UIRegionBegin(jd_StrLit("##textedittest"), &jd_default_style_dark, (jd_UISize){jd_UISizeRule_Grow, jd_UISizeRule_Grow}, jd_UILayout_TopToBottom, 0.0f, false);
            {
                jd_UIInputTextBox(jd_StrLit("##texteditinputbox"), &text_edit_string, text_edit_string_size, 0, (jd_UISize){jd_UISizeRule_Grow, jd_UISizeRule_Grow});
                jd_UIInputTextBox(jd_StrLit("##texteditinputbox2"), &text_edit_string2, text_edit_string_size2, 0, (jd_UISize){jd_UISizeRule_Grow, jd_UISizeRule_Grow});
            }
            jd_UIRegionEnd();
        }
        jd_UIRegionEnd();
        
        jd_UIRegionBegin(jd_StrLit("##statspanel"), &jd_default_style_dark, size, jd_UILayout_LeftToRight, 3.0f, true);
        {
            static u64 index = 0;
            static u64 update_delay = 200;
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
                update_delay = framerate;
                
            }
            
            jd_String label = jd_StringPushF(frame_arena, jd_StrLit("FPS: %.0f###FPS_Readout"), framerate);
            
            jd_UILabel(label);
        }
        jd_UIRegionEnd();
    }
    jd_UIRegionEnd(); 
    jd_UIFontPop();
    jd_UIEnd();
    
    // TODO: Fix me! This should go before UIEnd, but the pointers aren't living long enough
    
    
}


#if 0

WindowFunc () {
    b32 init = false;
    jd_UIState* ui_state = 0;
    if (!init) {
        ui_state = jd_UIInitForWindow(window);
    }
    
    while (!ui_state->closed) {
        jd_UIPushActiveWindow(ui_state);
        
        jd_UIPopActiveWindow();
        
    }
}

#endif