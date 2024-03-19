#ifndef JD_RENDERER_H
#define JD_RENDERER_H

#ifndef JD_DEFS_H
#include "jd_defs.h"
#endif

#ifndef JD_FILE_H
#include "jd_file.h"
#endif

#ifndef JD_DISK_H
#include "jd_disk.h"
#endif

#ifndef JD_STRING_H
#include "jd_string.h"
#endif

#ifndef JD_MEMORY_H
#include "jd_memory.h"
#endif

#ifndef JD_DATA_STRUCTURES_H
#include "jd_data_structures.h"
#endif

#ifndef JD_DISK_H
#include "jd_disk.h"
#endif

#ifndef JD_APP_H
#include "jd_app.h"
#endif

typedef struct jd_Rectangle {
    jd_V2F min;
    jd_V2F max;
} jd_Rectangle;

typedef struct jd_Glyph {
    jd_V2F offset;
    jd_V2F size;
    f32 h_advance;
    
    u32 texture_index;
    u32 depth;
} jd_Glyph;

typedef struct jd_TypefaceUnicodeRange {
    u32 begin;
    u32 end;
} jd_TypefaceUnicodeRange;

static jd_ReadOnly jd_TypefaceUnicodeRange jd_unicode_range_end = {0, 0};
static jd_ReadOnly jd_TypefaceUnicodeRange jd_unicode_range_basic_latin = {0, 127};
static jd_ReadOnly jd_TypefaceUnicodeRange jd_unicode_range_bmp = {0, 0xFFFF};

typedef struct jd_Typeface {
    jd_String id_str;
    f32 face_size;
    
    f32 ascent;
    f32 descent;
    f32 line_gap;
    f32 line_adv;
    
    jd_Glyph** glyphs;  // array of glyph pointers, sized per the provided unicode range. bmp takes about 512kb to store directly.
    
    u32 gl_texture;
    u32 texture_width;
    u32 texture_height;
    u8* white_bitmap;
} jd_Typeface;

#if 0
typedef struct jd_Font {
    jd_File ttf_file;
    f32 face_size;
    f32 ascent;
    
    jd_Glyph glyphs[1024];
    f32 advances[1024];
    
    u32 texture;
    u32 texture_width;
    u32 texture_height;
    u8* white_bitmap;
} jd_Font;
#endif

typedef struct jd_GLVertex {
    jd_V3F pos;
    jd_V3F tx;
    jd_V4F col;
} jd_GLVertex;

typedef struct jd_TextureSlice {
    u32 index;
    u32 max_nodes;
} jd_TextureSlice;

typedef struct jd_RGBATextureGroup {
    jd_DArray* texture;
} jd_RGBATextureGroup;

typedef struct jd_RenderObjects {
    u32 vao;
    u32 vbo;
    u32 shader;
    u32 fbo;
    u32 tcb;
    u32 rbo;
    u32 fbo_tex;
    u32 rgba_tex;
} jd_RenderObjects;

#define JD_RENDERBOX_GROW_DIR_L (1 << 0)
#define JD_RENDERBOX_GROW_DIR_R (1 << 1)
#define JD_RENDERBOX_GROW_DIR_U (1 << 2)
#define JD_RENDERBOX_GROW_DIR_D (1 << 3)
#define JD_RENDERBOX_GROW_NONE  (1 << 4)

typedef struct jd_RenderBox {
    jd_Rectangle rect;
    jd_V2F top_left;
    jd_V2F min_size;
    jd_V2F max_size;
    jd_V2F text_cursor;
    u32 flags;
} jd_RenderBox;

typedef struct jd_Renderer {
    jd_Arena* arena;
    
    jd_UserLock* font_lock; // should never collide in single threaded code, and makes multithreaded code safe. who
    //                         wants to load a font on a ui thread anyway?
    jd_DArray* fonts; // type: jd_Font
    
    jd_DArray* vertices; // type: jd_GLVertex
    jd_RenderObjects objects;
    
    f32 dpi_scaling;
    jd_V2F render_size;
    
    u32 max_texture_layers;
    u32 active_texture;
    
    struct jd_Window* window;
    
    struct jd_Window* linked_window;
} jd_Renderer;

#if 0
jd_Font* jd_FontPushFromFile(jd_Renderer* renderer, jd_String ttf_path, i32 face_size_pixels);
#endif
jd_Renderer* jd_RendererCreate();
#if 0
void jd_DrawString(jd_Renderer* renderer, jd_Font* font, jd_String str, jd_V2F window_pos, jd_V4F color, f32 wrap_width);
void jd_DrawStringWithBG(jd_Renderer* renderer, jd_Font* font, jd_String str, jd_V2F window_pos, jd_V4F text_color, jd_V4F bg_color, f32 wrap_width);
#endif
void jd_DrawRect(jd_Renderer* renderer, jd_V2F window_pos, jd_V2F size, jd_V4F col);
void jd_RendererSetDPIScale(jd_Renderer* renderer, f32 scale);
void jd_RendererSetRenderSize(jd_Renderer* renderer, jd_V2F render_size);
void jd_RefreshFonts(jd_Renderer* renderer);
void jd_RendererDraw(jd_Renderer* renderer);

#ifdef JD_IMPLEMENTATION
#include "jd_renderer.c"
#endif

#endif //JD_RENDERER_H
