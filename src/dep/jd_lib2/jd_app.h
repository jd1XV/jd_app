/* date = February 27th 2024 4:11 pm */

#ifndef JD_APP_H
#define JD_APP_H

#ifndef JD_DEFS_H
#include "jd_defs.h"
#endif

#ifndef JD_STRING_H
#include "jd_string.h"
#endif

#ifndef JD_MEMORY_H
#include "jd_memory.h"
#endif

#define JD_APP_MAX_WINDOWS 2048

typedef struct jd_Window jd_Window;
jd_V2F jd_WindowGetDrawSize(jd_Window* window);

typedef struct jd_App jd_App;
jd_App* jd_AppCreate();

typedef void (*jd_AppWindowFunctionPtr)(struct jd_Window* window);
#define jd_AppWindowFunction(x) void x (struct jd_Window* window)

void       jd_AppUpdatePlatformWindows(jd_App* app);
jd_Window* jd_AppPlatformCreateWindow(jd_App* app, jd_String id_str, jd_String title, jd_AppWindowFunctionPtr function);
void       jd_AppPlatformCloseWindow(jd_Window* window);
b32        jd_AppPlatformUpdate(jd_App* app);

#ifdef JD_WINDOWS

#define jd_AppMainFn i32 APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstPrev, PSTR cmdline, i32 cmdshow)

#ifdef JD_IMPLEMENTATION
#include "win32_jd_app.c"
#endif

#endif

#endif //JD_APP_H
