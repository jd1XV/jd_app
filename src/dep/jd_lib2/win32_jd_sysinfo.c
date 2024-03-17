static jd_SysInfo _jd_internal_sysinfo = {0};

void jd_SysInfoFetch() {
    if (_jd_internal_sysinfo.fetched) {
        jd_LogError("System info already fetched!", jd_Error_APIMisuse, jd_Error_Common);
        return;
    }
    
    enum {
        EAX = 0,
        EBX = 1,
        ECX = 2,
        EDX = 3
    };
    
    i32 cpu_info[4] = {0};
    
    jd_CPUFlags flags = 0;
    
    __cpuid(cpu_info, 0x1);
    if (cpu_info[EDX] & (1<<25)) {
        flags |= jd_CPUFlags_SupportsSSE;
    }
    if (cpu_info[EDX] & (1<<26)) {
        flags |= jd_CPUFlags_SupportsSSE2;
    }
    if (cpu_info[ECX] & (1<<0)) {
        flags |= jd_CPUFlags_SupportsSSE3;
    }
    if (cpu_info[ECX] & (1<<19)) {
        flags |= jd_CPUFlags_SupportsSSE41;
    }
    if (cpu_info[ECX] & (1<<20)) {
        flags |= jd_CPUFlags_SupportsSSE42;
    }
    if (cpu_info[ECX] & (1<<28)) {
        flags |= jd_CPUFlags_SupportsAVX;
    }
    
    __cpuid(cpu_info, 0x7);
    if (cpu_info[EBX] & (1<<5)) {
        flags |= jd_CPUFlags_SupportsAVX2;
    }
    
    _jd_internal_sysinfo.cpu_flags = flags;
    _jd_internal_sysinfo.fetched = true;
}

jd_CPUFlags jd_SysInfoGetCPUFlags() {
    if (!_jd_internal_sysinfo.fetched) {
        jd_SysInfoFetch();
    }
    
    return _jd_internal_sysinfo.cpu_flags;
}

void jd_DebugPrintSysInfo() {
    jd_CPUFlags flags = jd_SysInfoGetCPUFlags();
    jd_DebugPrint(jd_StrLit("CPU Vector Extensions Supported:\n"));
    
    if (jd_CPUFlagIsSet(flags, jd_CPUFlags_SupportsSSE)) {
        jd_DebugPrint(jd_StrLit("-- SSE\n"));
    }
    
    if (jd_CPUFlagIsSet(flags, jd_CPUFlags_SupportsSSE2)) {
        jd_DebugPrint(jd_StrLit("-- SSE2\n"));
    }
    
    if (jd_CPUFlagIsSet(flags, jd_CPUFlags_SupportsSSE3)) {
        jd_DebugPrint(jd_StrLit("-- SSE3\n"));
    }
    
    if (jd_CPUFlagIsSet(flags, jd_CPUFlags_SupportsSSE41)) {
        jd_DebugPrint(jd_StrLit("-- SSE4.1\n"));
    }
    
    if (jd_CPUFlagIsSet(flags, jd_CPUFlags_SupportsSSE42)) {
        jd_DebugPrint(jd_StrLit("-- SSE4.2\n"));
    }
    
    if (jd_CPUFlagIsSet(flags, jd_CPUFlags_SupportsSSE4A)) {
        jd_DebugPrint(jd_StrLit("-- SSE4A\n"));
    }
    
    if (jd_CPUFlagIsSet(flags, jd_CPUFlags_SupportsAVX)) {
        jd_DebugPrint(jd_StrLit("-- AVX\n"));
    }
    
    if (jd_CPUFlagIsSet(flags, jd_CPUFlags_SupportsAVX2)) {
        jd_DebugPrint(jd_StrLit("-- AVX2\n"));
    }
}