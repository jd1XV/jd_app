/* date = February 20th 2024 1:11 am */

#ifndef JD_DATA_STRUCTURES_H
#define JD_DATA_STRUCTURES_H

#ifndef JD_UNITY_H
#include "jd_defs.h"
#endif

typedef struct jd_Node {
    struct jd_Arena* arena;
    struct jd_Node* next;
    struct jd_Node* last;
    struct jd_Node* first_child;
    struct jd_Node* last_child;
    struct jd_Node* parent;
    
    void* data;
    u64 size;
} jd_Node;

jd_Node* _jd_SLLPush(jd_Node* node, void* data, u64 size);
jd_Node* _jd_SLLInsertNext(jd_Node* node, void* data, u64 size);
jd_Node* _jd_DLLPush(jd_Node* node, void* data, u64 size);
jd_Node* _jd_DLLInsertNext(jd_Node* node, void* data, u64 size);
jd_Node* _jd_DLLInsertLast(jd_Node* node, void* data, u64 size);
jd_Node* _jd_TreePushChild(jd_Node* node, void* data, u64 size);
jd_Node* _jd_TreePushSibling(jd_Node* node, void* data, u64 size);
jd_Node* _jd_TreeInsertNext(jd_Node* node, void* data, u64 size);
jd_Node* _jd_TreeInsertLast(jd_Node* node, void* data, u64 size);

#define jd_SLLPush(node, data)         _jd_SLLPush(node, data, sizeof(*data))
#define jd_SLLInsertNext(node, data)   _jd_SLLInsertNext(node, data, sizeof(*data))
#define jd_DLLPush(node, data)         _jd_DLLPush(node, data, sizeof(*data))
#define jd_DLLInsertNext(node, data)   _jd_DLLInsertNext(node, data, sizeof(*data))
#define jd_DLLInsertLast(node, data)   _jd_DLLInsertLast(node, data, sizeof(*data))
#define jd_TreePushChild(node, data)   _jd_TreePushChild(node, data, sizeof(*data))
#define jd_TreePushSibling(node, data) _jd_TreePushSibling(node, data, sizeof(*data))
#define jd_TreeInsertNext(node, data)  _jd_TreeInsertNext(node, data, sizeof(*data))
#define jd_TreeInsertLast(node, data)  _jd_TreeInsertLast(node, data, sizeof(*data))

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
void       jd_DArrayRelease(jd_DArray* d_array);

#ifdef JD_IMPLEMENTATION
#include "jd_data_structures.c"
#endif

#endif //JD_DATA_STRUCTURES_H
