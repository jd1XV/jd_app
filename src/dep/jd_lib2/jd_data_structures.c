jd_Node* _jd_SLLPush(jd_Node* node, void* data, u64 size) {
    if (!node || !node->arena) return 0;
    
    jd_Arena* arena = node->arena;
    
    if (node->data) {
        // not the first item in the list
        node->next = jd_ArenaAlloc(arena, sizeof(jd_Node));
        node = node->next;
    } else {
        node->data = jd_ArenaAlloc(arena, size);
    }
    
    jd_MemCpy(node->data, data, size);
    node->size = size;
    node->arena = arena;
    
    return node;
}

jd_Node* _jd_SLLInsertNext(jd_Node* node, void* data, u64 size) {
    if (!node || node->arena) return 0;
    jd_Node* old_next = node->next;
    
    jd_Arena* arena = node->arena;
    
    node->next = jd_ArenaAlloc(node->arena, sizeof(jd_Node));
    node = node->next;
    
    jd_MemCpy(node->data, data, size);
    node->size = size;
    node->next = old_next;
    node->arena = arena;
    
    return node;
}

jd_Node* _jd_DLLPush(jd_Node* node, void* data, u64 size) {
    if (!node || !node->arena) return 0;
    jd_Node* new_node = _jd_SLLPush(node, data, size);
    new_node->last = node;
    return new_node;
}

jd_Node* _jd_DLLInsertNext(jd_Node* node, void* data, u64 size) {
    if (!node || !node->arena) return 0;
    jd_Node* old_next = node->next;
    jd_Node* new_node = _jd_SLLInsertNext(node, data, size);
    new_node->last = node;
    if (old_next) old_next->last = new_node;
}

jd_Node* _jd_DLLInsertLast(jd_Node* node, void* data, u64 size) {
    
}

jd_Node* _jd_TreePushChild(jd_Node* node, void* data, u64 size) {
    
}

jd_Node* _jd_TreePushSibling(jd_Node* node, void* data, u64 size) {
    
}

jd_Node* _jd_TreeInsertNext(jd_Node* node, void* data, u64 size) {
    
}

jd_Node* _jd_TreeInsertLast(jd_Node* node, void* data, u64 size) {
    
}


jd_DArray* jd_DArrayCreate(u64 max, u64 stride) {
    if (stride <= 0) 
        return 0; // err
    u64 a_cap = max * stride;
    jd_Arena* arena = jd_ArenaCreate(jd_Max(max * stride, KILOBYTES(64)), jd_Max(stride * 4, KILOBYTES(64)));
    if (!arena) // err 
        return 0;
    
    jd_DArray* arr = jd_ArenaAlloc(arena, sizeof(jd_DArray));
    arr->view.mem = arena->mem + arena->pos;
    arr->arena = arena;
    arr->stride = stride;
    return arr;
}

void* jd_DArrayGetIndex(jd_DArray* d_array, u64 index) {
    return (void*)((u8*)d_array->view.mem + (d_array->stride * index));
}

void* jd_DArrayGetBack(jd_DArray* d_array) {
    return jd_DArrayGetIndex(d_array, d_array->view.count - 1);
}

void* jd_DArrayPushBack(jd_DArray* d_array, void* data) { 
    void* ptr = jd_ArenaAlloc(d_array->arena, d_array->stride);
    if (data != 0) {
        jd_MemCpy(ptr, data, d_array->stride);
    }
    
    d_array->view.count++;
    return ptr;
}

void* jd_DArrayPushAtIndex(jd_DArray* d_array, u64 index, void* data) { 
    u64 space_in_front = (d_array->arena->pos - (d_array->stride * index));
    u8* space = jd_ArenaAlloc(d_array->arena, d_array->stride);
    u8* insert = jd_DArrayGetIndex(d_array, index);
    u8* next = insert + d_array->stride;
    jd_MemMove(next, insert, space_in_front);
    if (data) jd_MemCpy(insert, data, d_array->stride);
    d_array->view.count++;
    return insert;
}

b32 jd_DArrayPopIndex(jd_DArray* d_array, u64 index) { 
    if (d_array->view.count == 0) return false;
    if (index > d_array->view.count - 1) return false;
    
    u8* ptr = jd_DArrayGetIndex(d_array, index);
    if (index < d_array->view.count - 1) {
        jd_MemMove(ptr, ptr + d_array->stride, d_array->stride * (d_array->view.count - (index + 1)));
    }
    
    jd_DArrayClearToIndex(d_array, (d_array->view.count - 1));
    d_array->view.count--;
    return true;
}

b32 jd_DArrayPopBack(jd_DArray* d_array) { 
    jd_DArrayPopIndex(d_array, d_array->view.count - 1);
    d_array->view.count--;
    return true;
}

b32 jd_DArrayPopFront(jd_DArray* d_array) { 
    jd_DArrayPopIndex(d_array, 0);
    return true;
}

b32 jd_DArrayClear(jd_DArray* d_array) { 
    jd_DArrayClearToIndex(d_array, 0);
    return true;
}

b32 jd_DArrayClearToIndex(jd_DArray* d_array, u64 index) { 
    jd_ArenaPopTo(d_array->arena, sizeof(jd_DArray) + (d_array->stride * index));
    d_array->view.count = index;
    return true;
}

void jd_DArrayRelease(jd_DArray* d_array) { 
    jd_ArenaRelease(d_array->arena);
}