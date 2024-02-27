#include <jd_lib.h>

typedef struct jd_SLLNode {
    void* data;
    struct jd_SLLNode* next;
} jd_SLLNode;

typedef struct jd_SLL {
    jd_Arena* arena;
    jd_SLLNode* first;
    jd_SLLNode* last;
} jd_SLL;

typedef struct jd_DLLNode {
    void* data;
    struct jd_DLLNode* next;
    struct jd_DLLNode* last;
} jd_DLLNode;

typedef struct jd_DLL {
    jd_Arena* arena;
    jd_DLLNode* first;
    jd_DLLNode* last;
} jd_DLL;

jd_SLL jd_SLLCreate(jd_Arena* arena) {
    jd_SLL sll = {
        .arena = arena
    };
    
    return sll;
}

jd_SLLNode* jd_SLLPush(jd_SLL* list, void* data) {
    if (!list) return 0;
    if (!list->first) {
        list->first = jd_ArenaAlloc(list->arena, sizeof(*list->first));
        list->first->data = data;
        list->last = list->first;
    } else {
        list->last->next = jd_ArenaAlloc(list->arena, sizeof(*list->last->next));
        jd_SLLNode* node = list->last->next;
        node->data = data;
        list->last = node;
    }
}

void jd_SLLPop(jd_SLL* list, jd_SLLNode* node) {
    if (!node || !list) return;
    
    jd_SLLNode* find = list->first;
    jd_SLLNode* last = NULL;
    
    while (find) {
        
        if (find == node) {
            if (last) last->next = find->next;
            if (find == list->first) list->first = find->next;
            if (find == list->last) list->last = last;
        }
        
        last = find;
        find = find->next;
    }
}

jd_DLLNode* jd_DLLPush(jd_DLL* list, void* data) {
    if (!list) return 0;
    if (!list->first) {
        list->first = jd_ArenaAlloc(list->arena, sizeof(*list->first));
        list->first->data = data;
        list->last = list->first;
    } else {
        list->last->next = jd_ArenaAlloc(list->arena, sizeof(*list->last->next));
        jd_DLLNode* node = list->last->next;
        node->last = list->last;
        node->data = data;
        list->last = node;
    }
}
