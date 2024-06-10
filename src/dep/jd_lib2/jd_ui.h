/* date = April 15th 2024 1:23 am */

#ifndef JD_UI_H
#define JD_UI_H

#ifndef JD_UNITY_H
#include "jd_app.h"
#endif

typedef struct jd_UITag {
    u32 key;
    u32 seed;
} jd_UITag;

typedef struct jd_UIRect {
    jd_V2F size;
    jd_V2F pos;
} jd_UIRect;

typedef struct jd_UIStyle {
    jd_String     id;
    jd_V2F        offset;
    jd_V2F        label_anchor;
    jd_V4F        color_bg;
    jd_V4F        color_border;
    jd_V4F        color_label;
    jd_V2F        padding;
    f32           corner_radius;
    f32           border;
} jd_UIStyle;

typedef struct jd_UIBoxRec {
    jd_UITag  tag;
    jd_UIRect rect;
    jd_V2F    pos; // platform window space
    jd_V2F    label_anchor;
    jd_String label;
    
    struct jd_UIBoxRec* next_with_same_hash;
    
    u64 frame_last_touched;
    
    jd_Node(jd_UIBoxRec);
} jd_UIBoxRec;

typedef struct jd_UIViewport {
    jd_V2F size;
    jd_UIBoxRec* root;
    jd_UIBoxRec* popup_root;
    jd_UIBoxRec* menu_root;
    jd_UIBoxRec* titlebar_root;
    
    jd_UIBoxRec* root_new;
    jd_UIBoxRec* popup_root_new;
    jd_UIBoxRec* menu_root_new;
    jd_UIBoxRec* titlebar_root_new;
    
    jd_UIBoxRec* hot;
    jd_UIBoxRec* active;
    jd_UIBoxRec* last_active;
    jd_PlatformWindow* window;
    
    b32 roots_init;
    
    jd_InputEventSlice new_inputs;
    jd_InputEventSlice old_inputs;
} jd_UIViewport;

typedef struct jd_UIResult {
    jd_UIBoxRec* box;
    
    b8 l_clicked;
    b8 r_clicked;
    b8 m_clicked;
    
    b8 control_clicked;
    b8 alt_clicked;
    b8 shift_clicked;
    
    b8 text_input;
    
    b8 drag_drop_recieved;
    b8 drag_drop_sent;
    
    b8 hovered;
} jd_UIResult;

typedef struct jd_UIBoxConfig {
    jd_UIBoxRec*  parent;
    jd_UIStyle*   style;
    jd_String     string;
    jd_V4F        bg_color;
    jd_UIRect     rect;
    
    b8            act_on_click;
    b8            static_color;
    b8            disabled;
    b8            label_selectable;
    jd_Cursor     cursor;
    
    // TODO: Texture information? I feel like there's no reason to not have it this low level with this design,
    // and it makes textured rectangles a first class citizen. Thereotically, with the right application of these
    // flags, we can have skinning by default.
} jd_UIBoxConfig;

jd_ExportFn jd_UIResult jd_UIBox(jd_UIBoxConfig* cfg);
jd_ExportFn jd_UIViewport* jd_UIBeginViewport(jd_PlatformWindow* window);
jd_ExportFn jd_ForceInline void jd_UISeedPushPtr(void* ptr);
jd_ExportFn jd_ForceInline void jd_UISeedPop();

#ifdef JD_IMPLEMENTATION
#include "jd_ui.c"
#endif

#endif //JD_UI_H
