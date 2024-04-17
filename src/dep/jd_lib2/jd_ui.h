/* date = April 15th 2024 1:23 am */

#ifndef JD_UI_H
#define JD_UI_H

#ifndef JD_UNITY_H
#include "jd_app.h"
#endif

typedef struct jd_UITag {
    u32 hash;
    jd_String id_str;
} jd_UITag;

typedef enum jd_UISizeRule {
    jd_UI_SizedByLabel,
    jd_UI_SizedAbsolute,
    jd_UI_SizedByParent,
    jd_UI_SizedByChildren,
    jd_UI_SizedFillToRight,
    jd_UI_SizeRule_Count
} jd_UISizeRule;

typedef struct jd_UISize {
    jd_UISizeRule rule;
    jd_V2F v;
} jd_UISize;

typedef struct jd_UIStyle {
    jd_V2F padding;
    jd_V2F label_anchor;
} jd_UIStyle;

typedef struct jd_UIBoxStyle {
    jd_V2F        label_anchor;
    jd_V4F        color;
    jd_V4F        color_border;
    jd_V4F        label_color;
    jd_UISizeRule size_rule;
    jd_V2F        padding;
    f32           corner_radius;
    f32           border;
} jd_UIBoxStyle;

typedef struct jd_UIState {
    jd_Arena* box_arena;
    jd_View   box_array;
    u64       box_array_size;
    
    struct jd_UIBoxRec* roots_front;
    struct jd_UIBoxRec* roots_back;
    
    struct jd_UIBoxRec* hot;
    struct jd_UIBoxRec* active;
    u64          box_seed;
    
    jd_UIBoxStyle default_box_style;
    
    jd_String    str_id;
    
    jd_Node(jd_UIState);
} jd_UIState;

typedef struct jd_UIBoxRec {
    jd_UIState* state;
    jd_UITag  tag;
    jd_UISize size;
    jd_V2F    label_anchor;
    jd_String label;
    
    u64 frame_last_touched;
    jd_Node(jd_UIBoxRec);
} jd_UIBoxRec;

typedef struct jd_UIResult {
    jd_UIBoxRec* box;
    b8 l_clicked;
    b8 r_clicked;
    b8 m_clicked;
    b8 text_input;
    b8 drag_drop_recieved;
    b8 drag_drop_sent;
} jd_UIResult;

typedef struct jd_UIBoxConfig {
    jd_UIBoxRec*  parent;
    jd_UIBoxStyle style;
    jd_String     label;
    jd_String     exp_id;
    
    b8            disabled;
    b8            label_selectable;
    
    // TODO: Texture information? I feel like there's no reason to not have it this low level with this design,
    // and it makes textured rectangles a first class citizen. Thereotically, with the right application of these
    // flags, we can have skinning by default.
} jd_UIBoxConfig;

jd_ExportFn jd_UIResult jd_UIBox(jd_UIBoxConfig* cfg);
jd_ExportFn jd_UIState* jd_UIGetState(jd_String state_id, jd_PlatformWindow* window);


#ifdef JD_IMPLEMENTATION
#include "jd_ui.c"
#endif

#endif //JD_UI_H
