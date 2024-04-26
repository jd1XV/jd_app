jd_DArray* jd_DArrayCreate(u64 max, u64 stride) {
    if (stride <= 0) 
        return 0; // err
    u64 a_cap = max * stride;
    jd_Arena* arena = jd_ArenaCreate(jd_Max(max * stride, KILOBYTES(64)), jd_Max(stride * 4, KILOBYTES(4)));
    if (!arena) // err 
        return 0;
    
    jd_DArray* arr = jd_ArenaAlloc(arena, sizeof(jd_DArray));
    arr->view.mem = arena->mem + arena->pos;
    arr->arena = arena;
    arr->stride = stride;
    return arr;
}

jd_ForceInline void* jd_DArrayGetIndex(jd_DArray* d_array, u64 index) {
    return (void*)((u8*)(d_array->view.mem + (d_array->stride * index)));
}

jd_ForceInline void* jd_DArrayGetBack(jd_DArray* d_array) {
    return jd_DArrayGetIndex(d_array, d_array->view.count - 1);
}

jd_ForceInline void* jd_DArrayPushBack(jd_DArray* d_array, void* data) { 
    void* ptr = jd_ArenaAlloc(d_array->arena, d_array->stride);
    if (!ptr) return 0;
    
    if (data != 0) {
        jd_MemCpy(ptr, data, d_array->stride);
    }
    d_array->view.count++;
    return ptr;
}

jd_ForceInline void* jd_DArrayPushAtIndex(jd_DArray* d_array, u64 index, void* data) { 
    u64 space_in_front = (d_array->arena->pos - (d_array->stride * index));
    u8* space = jd_ArenaAlloc(d_array->arena, d_array->stride);
    u8* insert = jd_DArrayGetIndex(d_array, index);
    u8* next = insert + d_array->stride;
    jd_MemMove(next, insert, space_in_front);
    if (data) jd_MemCpy(insert, data, d_array->stride);
    d_array->view.count++;
    return insert;
}

jd_ForceInline b32 jd_DArrayPopIndex(jd_DArray* d_array, u64 index) { 
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

jd_ForceInline b32 jd_DArrayPopBack(jd_DArray* d_array) { 
    jd_DArrayPopIndex(d_array, d_array->view.count - 1);
    d_array->view.count--;
    return true;
}

jd_ForceInline b32 jd_DArrayPopFront(jd_DArray* d_array) { 
    jd_DArrayPopIndex(d_array, 0);
    return true;
}

jd_ForceInline b32 jd_DArrayClear(jd_DArray* d_array) { 
    jd_DArrayClearToIndex(d_array, 0);
    return true;
}

jd_ForceInline b32 jd_DArrayClearNoDecommit(jd_DArray* d_array) {
    jd_DArrayClearToIndexNoDecommit(d_array, 0);
    return true;
}

jd_ForceInline b32 jd_DArrayClearToIndex(jd_DArray* d_array, u64 index) { 
    jd_ArenaPopTo(d_array->arena, sizeof(jd_DArray) + (d_array->stride * index));
    d_array->view.count = index;
    return true;
}

jd_ForceInline b32 jd_DArrayClearToIndexNoDecommit(jd_DArray* d_array, u64 index) {
    d_array->view.count = index;
    d_array->arena->pos = sizeof(jd_Arena) + sizeof(jd_DArray);
    return true;
}

void jd_DArrayRelease(jd_DArray* d_array) { 
    jd_ArenaRelease(d_array->arena);
}
