void jd_ZeroMemory(void* dest, u64 size) {
    ZeroMemory(dest, size);
}

void jd_MemCpy(void* dest, const void* src, u64 size) {
    CopyMemory(dest, src, size);
}

void jd_MemMove(void* dest, const void* src, u64 size) {
    MoveMemory(dest, src, size);
}

inline u8* _jd_Internal_ArenaReserve(u64 reserve, u64 commit_block_size) {
    u8* ptr = 0;
    
    ptr = VirtualAlloc(0, reserve, MEM_RESERVE, PAGE_READWRITE);
    VirtualAlloc(ptr, commit_block_size, MEM_COMMIT, PAGE_READWRITE);
    return ptr;
}

inline void _jd_Internal_ArenaCommit(jd_Arena* arena, u64 size) {
    VirtualAlloc(arena->mem + arena->commit_pos, size, MEM_COMMIT, PAGE_READWRITE);
}

inline void _jd_Internal_ArenaDecommit(jd_Arena* arena, u64 pos, u64 size) {
    VirtualFree(arena->mem + pos, size, MEM_DECOMMIT);
}

jd_Arena* jd_ArenaCreate(u64 capacity, u64 commit_page_size) {
    if (capacity == 0) capacity = GIGABYTES(4);
    if (capacity < KILOBYTES(64)) capacity = KILOBYTES(64);
    if (commit_page_size == 0) commit_page_size = jd_Max(capacity / 8096, KILOBYTES(64));
    
    jd_Arena* arena = (jd_Arena*)_jd_Internal_ArenaReserve(capacity, commit_page_size);
    
    arena->cap = capacity;
    arena->pos = sizeof(jd_Arena);
    arena->_commit_page_size = commit_page_size;
    arena->commit_pos = commit_page_size; // we are assuming we are always going to be using memory pretty shortly after getting it. Plus it's 8MB and it's 2023.
    
    arena->mem = (u8*)arena;
    
    return arena;
}

void* jd_ArenaAlloc(jd_Arena* arena, u64 size) {
    void* result = 0;
    
    jd_Assert(arena->pos + size < arena->cap);
    
    result = arena->mem + arena->pos;
    arena->pos += size;
    
    if (arena->pos > arena->commit_pos) {
        u64 pos_aligned = jd_AlignUpPow2(arena->pos, arena->_commit_page_size);
        u64 next_commit_pos = jd_Min(pos_aligned, arena->cap);
        u64 commit_size = next_commit_pos - arena->commit_pos;
        
        _jd_Internal_ArenaCommit(arena, commit_size);
        arena->commit_pos = next_commit_pos;
    }
    
    return result;
}


void jd_ArenaPopTo(jd_Arena* arena, u64 pos) {
    arena->pos = sizeof(jd_Arena) + pos;
    u64 pos_aligned = jd_AlignUpPow2(arena->pos, arena->_commit_page_size);
    if (pos_aligned - arena->pos > 0)
        jd_ZeroMemory(arena->mem + arena->pos, pos_aligned - arena->pos);
    if (arena->commit_pos > pos_aligned && arena->commit_pos > arena->_commit_page_size) {
        u64 diff = arena->commit_pos - pos_aligned;
        _jd_Internal_ArenaDecommit(arena, pos_aligned, diff);
        arena->commit_pos = pos_aligned;
    }
}

void jd_ArenaRelease(jd_Arena* arena) {
    VirtualFree(arena, 0, MEM_RELEASE);
}

#define jd_BitScanMSB32(in, mask) _BitScanReverse(in, mask)
#define jd_BitScanLSB32(in, mask) _BitScanForward(in, mask)

#define jd_BitScanMSB64(in, mask) _BitScanReverse64(in, mask)
#define jd_BitScanLSB64(in, mask) _BitScanForward64(in, mask)
