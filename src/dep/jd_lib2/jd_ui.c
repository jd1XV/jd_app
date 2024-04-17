jd_ExportFn jd_UIResult jd_UIBox(jd_UIBoxConfig* cfg) {
    
}

jd_UIBoxRec* _jd_UIAllocRootBox(jd_UIState* state) {
    jd_UIBoxRec* root = state->roots_back;
    jd_UIBoxRec* rec = jd_ArenaAlloc(state->box_arena, sizeof(*rec));
    jd_DLinkNext(root, rec);
    state->roots_back = rec;
    return rec;
}

void jd_UIBoxSetTop(jd_UIBoxRec* box) {
    jd_UIBoxRec* b = box;
    jd_UIBoxRec* prev = b->prev;
    jd_UIBoxRec* next = b->next;
    
    if (prev) {
        prev->next = next;
    }
    
    if (next) {
        next->prev = prev;
    }
    
    jd_ForDLLBackward(b, b->last != 0);
    if (b->parent)
        jd_TreeLinkFirstChild(box->parent, box);
    else
        jd_DLinkPrev(b, box);
} 
}

jd_UIBoxConfig jd_UIBoxConfigInit(jd_UIBoxRec* parent, jd_UIState* state) {
    jd_UIBoxConfig cfg = {
        .parent = (parent == 0) ? _jd_UIAllocRootBox(state),
        .style  = state->default_box_style,
    };
    
    return cfg;
}

jd_ExportFn jd_UIState* jd_UIGetState(jd_String state_id, jd_PlatformWindow* window) {
    jd_UIState* state = window->ui_states;
    jd_ForSLL(state, state != 0) {
        if (jd_StrMatch(state->str_id, state_id)) {
            return state;
        }
    }
    
    return state;
}

