u64 jd_Pow_u64(u64 b, u64 e) {
    u64 x = 0;
    if (e > 0) {
        x = b;
    }
    
    for (u64 i = 1; i < e; i++) {
        x *= b;
    }
    return x;
}
