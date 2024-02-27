/* date = February 19th 2024 8:16 pm */

#ifndef JD_DEFS_H
#define JD_DEFS_H

typedef unsigned long long u64;
typedef unsigned int       u32;
typedef unsigned short     u16;
typedef unsigned char       u8;

typedef signed long long   i64;
typedef signed int         i32;
typedef signed short       i16;
typedef signed char         i8;

typedef i8                  b8;
typedef i16                b16;
typedef i32                b32;

typedef double             f64;
typedef float              f32;

typedef char                c8;

#define true  1
#define false 0

typedef struct jd_View {
    u8* mem;
    union {
        u64 size; // NOTE: Just for the sake of semantic comfort, files have a "size," and strings have a "count"
        u64 count; 
    };
} jd_View;

typedef struct jd_V2F {
    union {
        struct {
            f32 x;
            f32 y;
        };
        struct {
            f32 u;
            f32 v;
        };
        struct {
            f32 w;
            f32 h;
        }; 
    };
    
} jd_V2F;

typedef struct jd_V3F {
    union {
        struct {
            f32 x;
            f32 y;
            f32 z;
        };
        struct {
            f32 u;
            f32 v;
            f32 w;
        };
    };
    
} jd_V3F;

typedef struct jd_V4F {
    union {
        struct {
            f32 r;
            f32 b;
            f32 g;
            f32 a;
        };
        struct {
            f32 x0;
            f32 x1;
            f32 y0;
            f32 y1;
        };
        struct {
            f32 x;
            f32 y;
            f32 w;
            f32 h;
        };
    };
} jd_V4F;

typedef struct jd_V2U64 {
    
    u64 x;
    u64 y;
    
} jd_V2U64;

typedef struct jd_V2I32 {
    union {
        struct {
            i32 x;
            i32 y;
        };
        struct {
            i32 w;
            i32 h;
        };
    };
    
} jd_V2S32;

#pragma section("jd_readonlysec", read)
#define jd_ReadOnly __declspec(allocate("jd_readonlysec"))

#ifdef JD_DEBUG
#include <assert.h>
#define jd_Assert(x) assert(x)
#else
#define jd_Assert(x)
#endif

#endif //JD_DEFS_H
