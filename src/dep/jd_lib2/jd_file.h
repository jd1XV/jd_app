/* date = February 22nd 2024 11:32 pm */

#ifndef JD_FILE_H
#define JD_FILE_H

#ifndef JD_DEFS_H
#include "jd_defs.h"
#endif 

#ifndef JD_STRING_H
#include "jd_string.h"
#endif

#ifndef JD_MEMORY_H
#include "jd_memory.h"
#endif

#ifndef JD_ERROR_H
#include "jd_error.h"
#endif

typedef jd_View jd_File;

#ifdef JD_IMPLEMENTATION

#ifdef JD_WINDOWS
#include "win32_jd_file.c"
#endif

#endif // JD_IMPLEMENTATION

#endif //JD_FILE_H
