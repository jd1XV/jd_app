typedef struct jd_Internal_UIState {
    jd_Arena* arena;
    
    jd_UIBoxRec* box_array;
    u64          box_array_size;
    jd_UIBoxRec* box_free_slot;
    
    jd_UIViewport viewports[256];
    u64 viewport_count;
    u64 active_viewport;
    
    jd_DArray* seeds;       // u32
    jd_DArray* style_stack; // jd_UIBoxStyle
} jd_Internal_UIState;

static jd_Internal_UIState _jd_internal_ui_state = {0};

#define jd_UIViewports_Max      256
#define jd_UIBox_HashTable_Size KILOBYTES(1)

jd_ForceInline jd_UIViewport* jd_UIViewportGetCurrent() {
    return &_jd_internal_ui_state.viewports[_jd_internal_ui_state.active_viewport];
}

jd_ForceInline jd_String _jd_UIStringGetDisplayPart(jd_String str) {
    return jd_StringGetPrefix(str, jd_StrLit("##"));
}

jd_ForceInline jd_String _jd_UIStringGetHashPart(jd_String str) {
    return jd_StringGetPostfix(str, jd_StrLit("###"));
}

jd_ExportFn jd_UITag jd_UITagFromString(jd_String str) {
    if (!str.count) jd_LogError("UI Error: Cannot create a tag from an empty string", jd_Error_APIMisuse, jd_Error_Fatal);
    u32* seed = jd_DArrayGetBack(_jd_internal_ui_state.seeds);
    if (!seed) jd_LogError("UI Error: Seed stack is empty!", jd_Error_MissingResource, jd_Error_Fatal);
    u32 hash = jd_HashStrToU32(str, *seed);
    
    jd_UITag t = {
        .key = hash,
        .seed = *seed
    };
    
    return t;
}

jd_ExportFn void jd_UIStylePush(jd_UIStyle* style) {
    jd_DArrayPushBack(_jd_internal_ui_state.style_stack, style);
}

jd_ExportFn void jd_UIStylePop() {
    jd_DArrayPopBack(_jd_internal_ui_state.style_stack); 
}

jd_ExportFn jd_UIBoxRec* jd_UIBoxGetByTag(jd_UITag tag) {
    jd_UIBoxRec* b = &(_jd_internal_ui_state.box_array[tag.key % _jd_internal_ui_state.box_array_size]);
    if (b->tag.key == 0 && b->tag.seed == 0) {
        b->tag.key = tag.key;
        b->tag.seed = tag.seed;
        return b;
    }
    
    while (b->next_with_same_hash != 0) {
        b = b->next_with_same_hash;
        if (b->tag.key == tag.key && b->tag.seed == tag.seed)
            break;
    }
    
    if (b->tag.key != tag.key || b->tag.seed != tag.seed) {
        if (_jd_internal_ui_state.box_free_slot) {
            b = _jd_internal_ui_state.box_free_slot;
            _jd_internal_ui_state.box_free_slot = b->next;
            jd_DLinksClear(b);
        } else {
            b->next_with_same_hash = jd_ArenaAlloc(_jd_internal_ui_state.arena, sizeof(jd_UIBoxRec));
            b = b->next_with_same_hash;
        }
        
        b->tag.key = tag.key;
        b->tag.seed = tag.seed;
    }
    
    return b;
}

jd_ForceInline b32 jd_UIRectContainsPoint(jd_UIRect r, jd_V2F p) {
    jd_V2F min = r.pos;
    jd_V2F max = {r.pos.x + r.size.x, r.pos.y + r.size.y};
    return ((p.x > min.x && p.x < max.x) && (p.y > min.y && p.y < max.y));
}

jd_UIBoxRec* jd_UIPickBoxForPos(jd_UIViewport* vp, jd_V2F pos) {
    jd_UIBoxRec* ret = 0;
    jd_UIBoxRec* b = vp->root;
    
    while (b != 0) {
        if (jd_UIRectContainsPoint(b->rect, pos)) {
            ret = b;
        } 
        jd_TreeTraversePreorder(b);
    }
    
    return ret;
}

void jd_UIPickActiveBox(jd_UIViewport* vp) {
    jd_InputSliceForEach(i, vp->old_inputs) {
        jd_InputEvent* e = jd_DArrayGetIndex(vp->old_inputs.array, i);
        if (!e) { // Sanity Check
            jd_LogError("Incorrect input slice range!", jd_Error_APIMisuse, jd_Error_Fatal);
        }
        
        switch (e->key) {
            case jd_MB_Left:
            case jd_MB_Right:
            case jd_MB_Middle: {
                if (e->release_event) {
                    vp->last_active = vp->active;
                    vp->active = 0;
                } else {
                    vp->active = jd_UIPickBoxForPos(vp, e->mouse_pos);
                }
                
            }
        }
    }
}

jd_ExportFn jd_UIResult jd_UIBox(jd_UIBoxConfig* config) {
    jd_UIViewport* vp = jd_UIViewportGetCurrent();
    
    jd_UIBoxRec* parent = config->parent;
    if (!parent) {
        parent = vp->root_new;
    }
    
    jd_UIStyle* style = config->style;
    if (!style) {
        style = jd_DArrayGetBack(_jd_internal_ui_state.style_stack);
    }
    
    jd_UITag tag = jd_UITagFromString(config->string);
    jd_UIBoxRec* b = jd_UIBoxGetByTag(tag);
    
    jd_UIResult result = {0};
    
    jd_V4F color         = {0.6, 0.8, 0.5, 1.0f};
    jd_V4F hovered_color = {0.3, 0.4, 0.25, 1.0f};
    jd_V4F active_color  = {0.15, 0.2, 0.125, 1.0f};
    
    // input handling
    
    if (b == vp->hot) {
        color = hovered_color;
    }
    
    if (b == vp->active) {
        jd_InputSliceForEach(i, vp->old_inputs) {
            jd_InputEvent* e = jd_DArrayGetIndex(vp->old_inputs.array, i);
            if (!e) { // Sanity Check
                jd_LogError("Incorrect input slice range!", jd_Error_APIMisuse, jd_Error_Fatal);
            }
            
            switch (e->key) {
                case jd_MB_Left:
                case jd_MB_Right:
                case jd_MB_Middle: {
                }
            }
        }
        
        color = active_color;
    }
    
    if (b == vp->last_active) {
        jd_InputSliceForEach(i, vp->old_inputs) {
            jd_InputEvent* e = jd_DArrayGetIndex(vp->old_inputs.array, i);
            if (!e) { // Sanity Check
                jd_LogError("Incorrect input slice range!", jd_Error_APIMisuse, jd_Error_Fatal);
            }
            
            switch (e->key) {
                case jd_MB_Left:
                case jd_MB_Right:
                case jd_MB_Middle: {
                    if (e->release_event && vp->hot == b) {
                        result.control_clicked = jd_InputHasMod(e, jd_KeyMod_Ctrl);
                        result.alt_clicked     = jd_InputHasMod(e, jd_KeyMod_Alt);
                        result.shift_clicked   = jd_InputHasMod(e, jd_KeyMod_Shift);
                        result.l_clicked       = (e->key == jd_MB_Left);
                        result.m_clicked       = (e->key == jd_MB_Middle);
                        result.r_clicked       = (e->key == jd_MB_Right);
                        break;
                    }
                }
            }
        }
    }
    
    jd_TreeLinkLastChild(parent, b);
    
    b->rect = config->rect;
    jd_DrawRect(vp->window->renderer, b->rect.pos, b->rect.size, color);
    
    return result;
}

jd_ExportFn jd_UIViewport* jd_UIBeginViewport(jd_PlatformWindow* window) {
    if (!_jd_internal_ui_state.arena) { // not yet initialized
        _jd_internal_ui_state.arena = jd_ArenaCreate(GIGABYTES(2), KILOBYTES(4));
        _jd_internal_ui_state.box_array_size = jd_UIBox_HashTable_Size;
        _jd_internal_ui_state.box_array = jd_ArenaAlloc(_jd_internal_ui_state.arena, sizeof(jd_UIBoxRec) * jd_UIBox_HashTable_Size);
        _jd_internal_ui_state.seeds = jd_DArrayCreate(2048, sizeof(u32));
        _jd_internal_ui_state.style_stack = jd_DArrayCreate(2048, sizeof(jd_UIStyle));
        
        jd_UIStyle style = {0};
        jd_DArrayPushBack(_jd_internal_ui_state.style_stack, &style);
        
        u32 seed = 63;
        
        jd_DArrayPushBack(_jd_internal_ui_state.seeds, &seed);
    }
    
    jd_UIViewport* vp = 0;
    
    for (u64 i = 0; i < _jd_internal_ui_state.viewport_count; i++) {
        if (_jd_internal_ui_state.viewports[i].window == window) {
            _jd_internal_ui_state.active_viewport = i;
            vp = &_jd_internal_ui_state.viewports[_jd_internal_ui_state.active_viewport];
            break;
        }
    }
    
    if (!vp) {
        jd_Assert(_jd_internal_ui_state.viewport_count + 1 < jd_UIViewports_Max);
        vp = &_jd_internal_ui_state.viewports[_jd_internal_ui_state.viewport_count++];
        
        if (!vp) { // Sanity check
            jd_LogError("Could not allocate a viewport!", jd_Error_OutOfMemory, jd_Error_Fatal);
        } 
        
        vp->window = window;
        vp->old_inputs.array = window->input_events;
        vp->new_inputs.array = window->input_events;
    }
    
    if (!vp->root) {
        vp->root = jd_ArenaAlloc(_jd_internal_ui_state.arena, sizeof(jd_UIBoxRec));
    }
    
    if (!vp->root_new) {
        vp->root_new = jd_ArenaAlloc(_jd_internal_ui_state.arena, sizeof(jd_UIBoxRec));
    }
    
    vp->root->rect.size = window->size;
    
    // Get new inputs
    vp->old_inputs = vp->new_inputs;
    vp->new_inputs.index = vp->old_inputs.range;
    vp->new_inputs.range = vp->old_inputs.array->view.count;
    
    vp->hot = 0;
    
    jd_V2F mouse_pos = jd_AppGetMousePos(vp->window); // Real time mouse information used for setting the hot item
    vp->hot          = jd_UIPickBoxForPos(vp, mouse_pos);
    
    jd_UIPickActiveBox(vp);
    
    jd_UIBoxRec* old_root = vp->root;
    vp->root = vp->root_new;
    vp->root_new = old_root;
    jd_TreeLinksClear(vp->root_new);
    
    return vp;
}