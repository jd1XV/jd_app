#ifndef JD_RENDERER_H
#define JD_RENDERER_H

#ifndef JD_UNITY_H
#include "jd_defs.h"
#include "jd_file.h"
#include "jd_disk.h"
#include "jd_string.h"
#include "jd_memory.h"
#include "jd_data_structures.h"
#include "jd_disk.h"
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
    
    u32 codepoint;
    
    u32 hash;
    
    struct jd_Glyph* next_with_same_hash;
    
    u32 texture_page;
    u32 texture_index;
} jd_Glyph;

typedef struct jd_TypefaceUnicodeRange {
    u32 begin;
    u32 end;
} jd_TypefaceUnicodeRange;

static jd_ReadOnly jd_TypefaceUnicodeRange jd_unicode_range_basic_latin = {0, 127};
static jd_ReadOnly jd_TypefaceUnicodeRange jd_unicode_range_bmp = {0, 0xFFFF};

typedef enum jd_TextOrigin {
    jd_TextOrigin_TopLeft,
    jd_TextOrigin_BottomLeft,
    jd_TextOrigin_Count
} jd_TextOrigin;

typedef struct jd_Typeface {
    jd_Arena* arena;
    jd_String id_str;
    f32 face_size;
    
    f32 ascent;
    f32 descent;
    f32 line_gap;
    f32 line_adv;
    
    jd_TypefaceUnicodeRange range;
    
    jd_Glyph* glyphs; // array of glyphs
    u64 glyph_count;
    
    u32 texture_width;
    u32 texture_height;
    u8* white_bitmap;
} jd_Typeface;

typedef struct jd_GLVertex {
    jd_V3F pos;
    jd_V3F tx;
    jd_V4F col;
} jd_GLVertex;

typedef struct jd_RenderObjects {
    u32 vao;
    u32 vbo;
    u32 shader;
    u32 fbo;
    u32 tcb;
    u32 rbo;
    u32 fbo_tex;
} jd_RenderObjects;

typedef struct jd_TexturePass {
    u32 gl_index;
    jd_DArray* vertices;
} jd_TexturePass;

#define jd_Renderer_Max_Texture_Passes 256

typedef struct jd_Renderer {
    jd_Arena* arena;
    jd_Arena* frame_arena;
    
    jd_UserLock* font_lock; // should never collide in single threaded code, and makes multithreaded code safe. who
    //                         wants to load a font on a ui thread anyway?
    jd_DArray* fonts; // type: jd_Font
    
    jd_RenderObjects objects;
    
    f32 dpi_scaling;
    jd_V2F render_size;
    
    u32 max_texture_layers;
    
    jd_TexturePass texture_passes[jd_Renderer_Max_Texture_Passes];
    u32 texture_pass_count;
    
    jd_Typeface* default_face;
    
    struct jd_Window* window;
    
    struct jd_Window* linked_window;
} jd_Renderer;

jd_Renderer* jd_RendererCreate();

jd_ExportFn void jd_DrawString(jd_Renderer* renderer, jd_Typeface* face, jd_String str, jd_V2F window_pos, jd_TextOrigin baseline, jd_V4F color, f32 wrap_width);
jd_ExportFn void jd_DrawStringWithBG(jd_Renderer* renderer, jd_Typeface* face, jd_String str, jd_V2F window_pos, jd_TextOrigin baseline, jd_V4F text_color, jd_V4F bg_color, f32 wrap_width);
jd_ExportFn void jd_DrawRect(jd_Renderer* renderer, jd_V2F window_pos, jd_V2F size, jd_V4F col);
jd_ExportFn void jd_RendererSetDPIScale(jd_Renderer* renderer, f32 scale);
jd_ExportFn void jd_RendererSetRenderSize(jd_Renderer* renderer, jd_V2F render_size);
jd_ExportFn void jd_RefreshFonts(jd_Renderer* renderer);
jd_ExportFn void jd_RendererDraw(jd_Renderer* renderer);

#ifdef JD_IMPLEMENTATION
#include "jd_renderer.c"
#endif

#endif //JD_RENDERER_H
