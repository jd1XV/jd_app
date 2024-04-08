/* date = April 7th 2024 5:29 pm */

#ifndef JD_TIMER_H
#define JD_TIMER_H

#ifndef JD_UNITY_H
#include "jd_sysinfo.h"
#endif

typedef union jd_StopWatch {
    i64 start;
    f64 stop;
} jd_StopWatch; 

jd_ExportFn jd_ForceInline jd_StopWatch jd_StopWatchStart();
jd_ExportFn jd_ForceInline jd_StopWatch jd_StopWatchStop(jd_StopWatch watch);

#ifdef JD_IMPLEMENTATION
#ifdef JD_WINDOWS
#include "win32_jd_timer.c"
#endif
#endif

#endif //JD_TIMER_H
