/* date = April 15th 2024 1:23 am */

#ifndef JD_UI_H
#define JD_UI_H

#ifndef JD_UNITY_H
#include "jd_app.h"
#endif

typedef struct jd_UITag {
    u32 hash;
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

typedef struct jd_UIBox {
    jd_UISize size;
    jd_Node(jd_UIBox);
} jd_UIBox;

typedef struct jd_UIWindow {
    jd_UISize size;
    jd_Node(jd_UIWindow);
} jd_UIWindow;

#endif //JD_UI_H
