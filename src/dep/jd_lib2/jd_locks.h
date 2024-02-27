/* date = February 23rd 2024 11:27 pm */

#ifndef JD_LOCKS_H
#define JD_LOCKS_H

#ifndef JD_MEMORY_H
#include "jd_memory.h"
#endif

typedef struct jd_UserLock jd_UserLock;
jd_UserLock* jd_UserLockCreate(jd_Arena* arena, u64 spin_count);
void jd_UserLockGet(jd_UserLock* lock);
void jd_UserLockRelease(jd_UserLock* lock);
b32 jd_UserLockTryGet(jd_UserLock* lock);
void jd_UserLockDelete(jd_UserLock* lock);

#ifdef JD_IMPLEMENTATION

#ifdef JD_WINDOWS
#include "win32_jd_locks.c"
#endif

#endif

#endif //JD_LOCKS_H
