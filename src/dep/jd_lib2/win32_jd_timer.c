i64 jd_GetTicks() {
    LARGE_INTEGER perf_ct = {0};
    QueryPerformanceCounter(&perf_ct);
    return perf_ct.QuadPart;
}

jd_StopWatch jd_StopWatchStart() {
    jd_StopWatch watch = {0};
    watch.start = jd_GetTicks();
    return watch;
}

jd_StopWatch jd_StopWatchStop(jd_StopWatch watch) {
    i64 diff = jd_GetTicks() - watch.start;
    watch.stop = ((f64)diff / (jd_SysInfoGetPerformanceFrequency() / 1000.0f));
    return watch;
}