typedef struct jd_Internal_UIState {
    jd_Arena* arena;
    
    jd_View   box_array;
    u64       box_array_size;
    
    jd_UIViewport viewports[256];
    u64 viewport_count;
    u64 active_viewport;
    
    jd_DArray* seeds;       // u32
    jd_DArray* style_stack; // jd_UIBoxStyle
} jd_Internal_UIState;

static jd_Internal_UIState _jd_internal_ui_state = {0};

#define jd_UIViewports_Max      256
#define jd_UIBox_HashTable_Size KILOBYTES(4)

jd_ForceInline jd_UIViewport* jd_UIViewportGetCurrent() {
    return &_jd_internal_ui_state.viewports[_jd_internal_ui_state.active_viewport];
}

jd_ForceInline jd_String _jd_UIStringGetDisplayPart(jd_String str) {
    return jd_StringGetPrefix(str, jd_StrLit("##"));
}

jd_ForceInline jd_String _jd_UIStringGetHashPart(jd_String str) {
    return jd_StringGetPostfix(str, jd_StrLit("###"));
}

jd_UITag jd_UITagFromString(jd_String str) {
    if (!str.count) jd_LogError("UI Error: Cannot create a tag from an empty string", jd_Error_APIMisuse, jd_Error_Fatal);
    u32* seed = jd_DArrayGetBack(_jd_internal_ui_state.seeds);
    if (!seed) jd_LogError("UI Error: Seed stack is empty!", jd_Error_MissingResource, jd_Error_Fatal);
    u32 hash = jd_HashStrToU32(_jd_UIStringGetHashPart(str), *seed);
    
    jd_UITag t = {
        .key = hash,
        .seed = *seed,
        .id_str = str
    };
    
    return t;
}

jd_ExportFn void jd_UIStylePush(jd_UIStyle* style) {
    jd_DArrayPushBack(_jd_internal_ui_state.style_stack, style);
}

jd_ExportFn void jd_UIStylePop() {
    jd_DArrayPopBack(_jd_internal_ui_state.style_stack); 
}

jd_ExportFn jd_UIResult jd_UIBox(jd_UIBoxConfig* config) {
    jd_UIViewport* vp = jd_UIViewportGetCurrent();
    
    jd_UIBoxRec* parent = config->parent;
    if (!parent) {
        parent = vp->root;
    }
    
    jd_UIStyle* style = config->style;
    if (!style) {
        style = jd_DArrayGetBack(_jd_internal_ui_state.style_stack);
    }
    
    jd_UITag tag = jd_UITagFromString(config->string);
    
}

jd_ExportFn jd_UIViewport* jd_UIGetViewport(jd_PlatformWindow* window) {
    if (!_jd_internal_ui_state.arena) { // not yet initialized
        _jd_internal_ui_state.arena = jd_ArenaCreate(GIGABYTES(2), KILOBYTES(2));
        _jd_internal_ui_state.box_array_size = jd_UIBox_HashTable_Size;
        _jd_internal_ui_state.box_array = jd_ArenaAllocView(_jd_internal_ui_state.arena, sizeof(jd_UIBoxRec) * jd_UIBox_HashTable_Size);
        _jd_internal_ui_state.seeds = jd_DArrayCreate(MEGABYTES(4), KILOBYTES(2));
        _jd_internal_ui_state.style_stack = jd_DArrayCreate(MEGABYTES(4), KILOBYTES(2));
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
        vp = &_jd_internal_ui_state.viewports[_jd_internal_ui_state.viewport_count];
        
        if (!vp) { // Sanity check
            jd_LogError("Could not allocate a viewport!", jd_Error_OutOfMemory, jd_Error_Fatal);
        } 
        
        vp->window = window;
    }
    
    return vp;
}

/*

Lifecycle: 

 - UI is built by user code
- 


*/