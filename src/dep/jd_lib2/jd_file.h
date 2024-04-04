/* date = February 22nd 2024 11:32 pm */

#ifndef JD_FILE_H
#define JD_FILE_H

#ifndef JD_UNITY_H
#include "jd_defs.h"
#include "jd_string.h"
#include "jd_memory.h"
#include "jd_error.h"
#endif

typedef jd_View jd_File;

#ifdef JD_IMPLEMENTATION

#ifdef JD_WINDOWS
#include "win32_jd_file.c"
#endif

#endif // JD_IMPLEMENTATION

#endif //JD_FILE_H
