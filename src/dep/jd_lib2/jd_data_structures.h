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
struct type* prev; \
struct type* first_child; \
struct type* last_child; \
struct type* parent; \
} \

#define jd_ForSLL(i, cond) for (; cond; i = i->next)
#define jd_SLNext(x) x->next
#define jd_SLink(x, y) \
do { \
void* _next = x->next; \
x->next = y; \
x->next->next = _next; \
} while (0) \

#define jd_SLinkClear(x) \
do { \
x->next = 0; \
} while (0) \

#define jd_ForDLLForward(i, cond) jd_ForSLL(i, cond)
#define jd_ForDLLBackward(i, cond) for (; cond; i = i->prev)
#define jd_DLNext(x) jd_SLNext(x)
#define jd_DLPrev(x) x->prev

#define jd_DLinkNext(x, y) \
do { \
void* _next = x->next; \
x->next = y; \
x->next->next = _next; \
x->next->last = x; \
} while (0) \

#define jd_DLinkPrev(x, y) \
do { \
void* _prev = x->prev; \
x->prev = y; \
x->prev->prev = _prev; \
x->prev->next = x; \
} while (0) \

#define jd_DLinksClear(x) \
do { \
jd_SLinkClear(x); \
x->prev = 0; \
} while (0) \

#define jd_TreeLinkNext(x, y) \
do { \
jd_DLinkNext(x, y); \
y->parent = x->parent; \
if (x->parent->last_child == x) { \
x->parent->last_child = y; \
} \
} while (0) \

#define jd_TreeLinkPrev(x, y) \
do { \
jd_DLinkPrev(x, y); \
y->parent = x->parent; \
if (x->parent->first_child == x) { \
x->parent->first_child = y; \
} \
} while (0) \

#define jd_TreeLinkLastChild(p, c) \
do { \
void* _lc = p->last_child; \
c->parent = p; \
c->prev = _lc; \
c->next = 0; \
if (p->last_child)   { p->last_child->next = c; } \
if (!p->first_child) { p->first_child = c; } \
p->last_child = c; \
} while (0) \

#define jd_TreeLinkFirstChild(p, c) \
do { \
void* _fc = p->first_child; \
c->parent = p; \
c->next = _fc; \
c->prev = 0; \
if (p->first_child) p->first_child->next = c; \
if (!p->last_child) p->last_child = c; \
p->first_child = c; \
} while (0) \

#define jd_TreeLinksClear(x) \
do { \
jd_DLinksClear(x); \
x->parent = 0; \
x->last_child = 0; \
x->first_child = 0; \
} while (0) \

#define jd_TreeTraversePreorder(x) \
do { \
if (x->first_child) { \
x = x->first_child; \
break; \
} \
if (x->next) { \
x = x->next; \
break; \
} \
else { \
while (x != 0 && x->next == 0) { \
x = x->parent; \
} \
\
if (x != 0) \
x = x->next; \
} \
} while (0) \

typedef struct jd_DArray {
    jd_Arena* arena;
    u64       stride;
    jd_View   view;
    jd_RWLock* lock;
} jd_DArray;

jd_ExportFn jd_DArray* jd_DArrayCreate(u64 max, u64 stride);
jd_ExportFn jd_DArray* jd_DArrayCreateWithLock(u64 max, u64 stride);
jd_ExportFn void*      jd_DArrayGetIndex(jd_DArray* arr, u64 index);
jd_ExportFn void*      jd_DArrayGetBack(jd_DArray* arr);
jd_ExportFn void*      jd_DArrayPushBack(jd_DArray* d_array, void* data);
jd_ExportFn void*      jd_DArrayPushAtIndex(jd_DArray* d_array, u64 index, void* data);
jd_ExportFn b32        jd_DArrayPopIndex(jd_DArray* d_array, u64 index);
jd_ExportFn b32        jd_DArrayPopBack(jd_DArray* d_array);
jd_ExportFn b32        jd_DArrayPopFront(jd_DArray* d_array);
jd_ExportFn b32        jd_DArrayClear(jd_DArray* d_array);
jd_ExportFn b32        jd_DArrayClearToIndex(jd_DArray* d_array, u64 index);
jd_ExportFn b32        jd_DArrayClearNoDecommit(jd_DArray* d_array);
jd_ExportFn b32        jd_DArrayClearToIndexNoDecommit(jd_DArray* d_array, u64 index);
jd_ExportFn void       jd_DArrayRelease(jd_DArray* d_array);

#ifdef JD_IMPLEMENTATION
#include "jd_data_structures.c"
#endif

#endif //JD_DATA_STRUCTURES_H
