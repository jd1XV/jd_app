jd_Node* _jd_SLLPush(jd_Node* node, void* data, u64 size) {
    if (!node || !node->arena) return 0;
    
    if (node->data) {
        // not the first item in the list
        node->next = jd_ArenaAlloc(node->arena, size);
        node = node->next;
    } else {
        node->data = jd_ArenaAlloc(node->arena, size);
    }
    
    jd_MemCpy(node->data, data, size);
    node->size = size;
    
    return node;
}

jd_Node* _jd_DLLPush(jd_Node* node, void* data, u64 size) {
    
}

jd_Node* _jd_DLLInsertNext(jd_Node* node, void* data, u64 size) {
    
}

jd_Node* _jd_DLLInsertLast(jd_Node* node, void* data, u64 size) {
    
}

jd_Node* _jd_TreePushChild(jd_Node* node, void* data, u64 size) {
    
}

jd_Node* _jd_TreePushSibling(jd_Node* node, void* data, u64 size) {
    
}

jd_Node* _jd_TreeInsertNext(jd_Node* node, void* data, u64 size) {
    
}

jd_Node* _jd_TreeInsertLast(jd_Node* node, void* data, u64 size) {
    
}

