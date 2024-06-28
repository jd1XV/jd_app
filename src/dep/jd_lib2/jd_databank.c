jd_DataBank* jd_DataBankCreate(jd_DataBankConfig* config) {
    if (!config) {
        jd_LogError("No config provided to DataBank initilization", jd_Error_APIMisuse, jd_Error_Fatal);
        return 0;
    }
    
    jd_Arena* arena = jd_ArenaCreate(config->total_memory_cap, 0);
    jd_DataBank* db = jd_ArenaAlloc(arena, sizeof(*db));
    db->arena = arena;
    db->lock = jd_RWLockCreate(arena);
    db->disabled_types = config->disabled_types;
    db->primary_key_hash_table_slot_count = (config->primary_key_hash_table_slot_count > 0) ? config->primary_key_hash_table_slot_count : KILOBYTES(128);
    db->primary_key_hash_table = jd_ArenaAlloc(arena, sizeof(jd_DataNode) * db->primary_key_hash_table_slot_count);
    db->root = jd_ArenaAlloc(arena, sizeof(*db->root));
    db->root->bank = db;
    return db;
}

u64 jd_DataBankGetPrimaryKey(jd_DataBank* bank) {
    jd_RWLockGet(bank->lock, jd_RWLock_Write);
    u64 key = bank->primary_key_index++;
    jd_RWLockRelease(bank->lock, jd_RWLock_Write);
    return key;
}

u64 jd_DataBankGetHashTableSlot(jd_DataBank* bank, u64 primary_key) {
    static const u32 seed = 963489887;
    u32 hash = jd_HashU64toU32(primary_key, seed);
    return (hash & bank->primary_key_hash_table_slot_count - 1);
}

jd_DataNode* jd_DataBankAddRecord(jd_DataNode* parent, jd_String key) {
    if (!parent) {
        jd_LogError("No and/or null parent specified. To create a top level record, specify the DataBank root as parent. DataBank root can be accessed (thread-safe-ly) by using jd_DataBankGetRoot(my_data_bank).", jd_Error_APIMisuse, jd_Error_Fatal);
    }
    
    jd_DataBank* db = parent->bank;
    
    if (!db) {
        jd_LogError("Specified parent does not belong to a DataBank.", jd_Error_APIMisuse, jd_Error_Fatal);
    }
    
    u64 pk = jd_DataBankGetPrimaryKey(db);
    
    jd_RWLockGet(db->lock, jd_RWLock_Write);
    
    u32 slot = jd_DataBankGetHashTableSlot(db, pk);
    jd_DataNode* n = &db->primary_key_hash_table[slot];
    if (n->value.type != jd_DataType_None) {
        while (n->next_with_same_hash != 0) {
            n = n->next_with_same_hash;
        }
        
        n->next_with_same_hash = jd_ArenaAlloc(db->arena, sizeof(*n->next_with_same_hash));
    }
    
    n->lock = jd_RWLockCreate(db->arena);
    n->key = jd_StringPush(db->arena, key);
    n->value.U64 = pk;
    n->value.type = jd_DataType_Record;
    n->bank = db;
    jd_TreeLinkLastChild(parent, n);
    
    jd_RWLockRelease(db->lock, jd_RWLock_Write);
    
    return n;
}

jd_DataNode* jd_DataPointAdd(jd_DataNode* record, jd_String key, jd_Value value) {
    if (value.type == jd_DataType_None) {
        jd_LogError("No data type specified.", jd_Error_APIMisuse, jd_Error_Fatal);
    }
    
    else if (value.type == jd_DataType_Record) {
        jd_LogError("Value specified is a record. To add a sub-record, use jd_DataBankAddRecord", jd_Error_APIMisuse, jd_Error_Fatal);
    }
    
    if (!record) {
        jd_LogError("Null record specified. To create a top level record, specify the DataBank root as parent. DataBank root can be accessed (thread-safe-ly) by using jd_DataBankGetRoot(my_data_bank).", jd_Error_APIMisuse, jd_Error_Fatal);
    }
    
    jd_DataBank* bank = record->bank;
    
    if (!bank) {
        jd_LogError("Specified record does not belong to a DataBank", jd_Error_APIMisuse, jd_Error_Fatal);
    }
    
    jd_RWLockGet(record->lock, jd_RWLock_Write);
    
    if (record->value.type != jd_DataType_Record) {
        jd_LogError("Node passed to 'record' is not of type jd_DataType_Record.", jd_Error_APIMisuse, jd_Error_Fatal);
    }
    
    jd_DataNode* n = jd_ArenaAlloc(bank->arena, sizeof(*n));
    n->key = jd_StringPush(bank->arena, key);
    n->value.type = value.type;
    
    switch (value.type) {
        case jd_DataType_String: {
            n->value.string = jd_StringPush(bank->arena, value.string);
            break;
        }
        
        case jd_DataType_Bin: {
            n->value.bin = jd_StringPush(bank->arena, value.bin);
            break;
        }
        
        case jd_DataType_u64:
        case jd_DataType_u32:
        case jd_DataType_b32:
        case jd_DataType_c8:
        case jd_DataType_i64:
        case jd_DataType_i32:
        case jd_DataType_f32: 
        case jd_DataType_f64: {
            n->value = value;
            break;
        }
        
    }
    
    jd_TreeLinkLastChild(record, n);
    
    jd_RWLockRelease(record->lock, jd_RWLock_Write);
    
    return n;
}

jd_Value jd_ValueCastString(jd_String string) {
    jd_Value v = {0};
    v.type = jd_DataType_String;
    v.string = string;
    return v;
}

jd_Value jd_ValueCastBin(jd_View view) {
    jd_Value v = {0};
    v.type = jd_DataType_Bin;
    v.bin = view;
    return v;
}

jd_Value jd_ValueCastU64(u64 val) {
    jd_Value v = {0};
    v.type = jd_DataType_u64;
    v.U64 = val;
    return v;
}

jd_Value jd_ValueCastU32(u32 val) {
    jd_Value v = {0};
    v.type = jd_DataType_u32;
    v.U32 = val;
    return v;
}

jd_Value jd_ValueCastB32(b32 val) {
    jd_Value v = {0};
    v.type = jd_DataType_b32;
    v.B32 = val;
    return v;
}

jd_Value jd_ValueCastC8(c8 val) {
    jd_Value v = {0};
    v.type = jd_DataType_c8;
    v.C8 = val;
    return v;
}

jd_Value jd_ValueCastI64(i64 val) {
    jd_Value v = {0};
    v.type = jd_DataType_i64;
    v.I64 = val;
    return v;
}

jd_Value jd_ValueCastI32(i32 val) {
    jd_Value v = {0};
    v.type = jd_DataType_i32;
    v.I32 = val;
    return v;
}

jd_Value jd_ValueCastF32(f32 val) {
    jd_Value v = {0};
    v.type = jd_DataType_f32;
    v.F32 = val;
    return v;
}

jd_Value jd_ValueCastF64(f64 val) {
    jd_Value v = {0};
    v.type = jd_DataType_f64;
    v.F64 = val;
    return v;
}

jd_String jd_ValueAsString(jd_Value v) {
    return v.string;
}

jd_View jd_ValueAsBin(jd_Value v) {
    return v.bin;
}

u64 jd_ValueAsU64(jd_Value v) {
    return v.U64;
}

u32 jd_ValueAsU32(jd_Value v) {
    return v.U32;
}

b32 jd_ValueAsB32(jd_Value v) {
    return v.B32;
}

c8 jd_ValueAsC8(jd_Value v) {
    return v.C8;
}

i64 jd_ValueAsI64(jd_Value v) {
    return v.I64;
}

i32 jd_ValueAsI32(jd_Value v) {
    return v.I32;
}

f32 jd_ValueAsF32(jd_Value v) {
    return v.F32;
}

f64 jd_ValueAsF64(jd_Value v) {
    return v.F64;
}

