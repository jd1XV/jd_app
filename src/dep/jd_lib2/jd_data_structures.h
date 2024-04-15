/* date = February 20th 2024 1:11 am */

#ifndef JD_DATA_STRUCTURES_H
#define JD_DATA_STRUCTURES_H

#ifndef JD_UNITY_H
#include "jd_defs.h"
#endif

// NOTE(JD): This may well prove to be evil macro magic.
#define jd_Node(type) \
struct { \
struct type* next; \
struct type* last; \
struct type* first_child; \
struct type* last_child; \
struct type* parent; \
} node \


typedef struct jd_DArray {
    jd_Arena* arena;
    u64       stride;
    jd_View   view;
} jd_DArray;

jd_DArray* jd_DArrayCreate(u64 max, u64 stride);
void*      jd_DArrayGetIndex(jd_DArray* arr, u64 index);
void*      jd_DArrayGetBack(jd_DArray* arr);
void*      jd_DArrayPushBack(jd_DArray* d_array, void* data);
void*      jd_DArrayPushAtIndex(jd_DArray* d_array, u64 index, void* data);
b32        jd_DArrayPopIndex(jd_DArray* d_array, u64 index);
b32        jd_DArrayPopBack(jd_DArray* d_array);
b32        jd_DArrayPopFront(jd_DArray* d_array);
b32        jd_DArrayClear(jd_DArray* d_array);
b32        jd_DArrayClearToIndex(jd_DArray* d_array, u64 index);
b32        jd_DArrayClearNoDecommit(jd_DArray* d_array);
b32        jd_DArrayClearToIndexNoDecommit(jd_DArray* d_array, u64 index);
void       jd_DArrayRelease(jd_DArray* d_array);

#ifdef JD_IMPLEMENTATION
#include "jd_data_structures.c"
#endif

#endif //JD_DATA_STRUCTURES_H
