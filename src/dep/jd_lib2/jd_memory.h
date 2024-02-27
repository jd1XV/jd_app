/* date = February 20th 2024 1:05 am */

#ifndef JD_MEMORY_H
#define JD_MEMORY_H

#ifndef JD_DEFS_H
#include "jd_defs.h"
#endif 

typedef struct jd_Arena {
    u8* mem;
    u64 cap;
    u64 pos;
    u64 commit_pos;
    u64 _commit_page_size;
    
} jd_Arena;

jd_Arena* jd_ArenaCreate(u64 reserve, u64 commit_block_size);
void* jd_ArenaAlloc(jd_Arena* arena, u64 size);
void jd_ArenaPopTo(jd_Arena* arena, u64 pos);
void jd_ArenaPopBlock(jd_Arena* arena, void* start, u64 size);
void jd_ArenaRelease(jd_Arena* arena);

void jd_ZeroMemory(void* dest, u64 size);
void jd_MemCpy(void* dest, const void* src, u64 size);
void jd_MemMove(void* dest, const void* src, u64 count);

#ifdef JD_IMPLEMENTATION

#ifdef JD_WINDOWS
#include "win32_jd_memory.c"
#endif

#endif // JD_IMPLEMENTATION

#endif //JD_MEMORY_H
