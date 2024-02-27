/* date = February 19th 2024 8:39 pm */

#ifndef JD_MATH_H
#define JD_MATH_H

#ifndef JD_DEFS_H
#include "jd_defs.h"
#endif

u64 jd_Pow_u64(u64 b, u64 e);

#ifdef JD_IMPLEMENTATION

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

#endif // JD_IMPLEMENTATION

#endif //JD_MATH_H
