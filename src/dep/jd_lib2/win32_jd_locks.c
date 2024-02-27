typedef struct jd_UserLock {
    CRITICAL_SECTION lock;
} jd_UserLock;


jd_UserLock* jd_UserLockCreate(jd_Arena* arena, u64 spin_count) {
    jd_UserLock* lock = jd_ArenaAlloc(arena, sizeof(CRITICAL_SECTION));
    if (spin_count == 0)
        InitializeCriticalSection(&lock->lock);
    else
        InitializeCriticalSectionAndSpinCount(&lock->lock, spin_count);
    return lock;
}

void jd_UserLockGet(jd_UserLock* lock) {
    EnterCriticalSection(&lock->lock);
}

void jd_UserLockRelease(jd_UserLock* lock) {
    LeaveCriticalSection(&lock->lock);
}

b32 jd_UserLockTryGet(jd_UserLock* lock) {
    if (TryEnterCriticalSection(&lock->lock) == 0) return false;
    else return true;
}

void jd_UserLockDelete(jd_UserLock* lock) {
    DeleteCriticalSection(&lock->lock);
}
