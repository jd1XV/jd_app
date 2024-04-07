

#include "dep/glad/glad_wgl.h"
#include "dep/glad/glad.h"

#include <ft2build.h>
#include FT_FREETYPE_H

jd_String vs_string = jd_StrConst("#version 430\n"
                                  "layout (location = 0) in vec3 vert_xyz;\n"
                                  "layout (location = 1) in vec3 vert_uvw;\n"
                                  "layout (location = 2) in vec4 vert_col;\n"
                                  "out vec3 frag_uvw;\n"
                                  "out vec4 frag_col;\n"
                                  "uniform vec2 screen_conv;\n"
                                  "void main(){\n"
                                  "gl_Position.x = (vert_xyz.x * screen_conv.x) - 1;\n"
                                  "gl_Position.y = 1 - (vert_xyz.y * screen_conv.y);\n"
                                  "gl_Position.z = 0;\n"
                                  "gl_Position.w = 1;\n"
                                  "frag_uvw = vert_uvw;\n"
                                  "frag_col = vert_col;\n"
                                  "}\n"
                                  );

jd_String fs_string = jd_StrConst("#version 430\n"
                                  "in vec3 frag_uvw; \n"
                                  "in vec4 frag_col; \n"
                                  "out vec4 out_col; \n"
                                  "uniform sampler2DArray tex;\n"
                                  "void main() {\n"
                                  "float alpha = texture(tex, frag_uvw).r;\n"
                                  "out_col = vec4(frag_col.rgb, frag_col.a * alpha);\n"
                                  "}\n"
                                  );

void jd_ShaderCreate(jd_Renderer* renderer) {
    u32 id = 0;
    id = glCreateProgram();
    c8 info_log[512];
    
    u32 vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_string.mem, NULL);
    glCompileShader(vs);
    
    i32 success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vs, 512, NULL, info_log);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED: %s\n", info_log);
        glDeleteShader(vs);
        glDeleteProgram(id);
        return;
    }
    
    u32 fs = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(fs, 1, &fs_string.mem, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        glGetShaderInfoLog(fs, 512, NULL, info_log);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED: %s\n", info_log);
        glDeleteShader(vs);
        glDeleteShader(fs);
        glDeleteProgram(id);
        return;
    }
    
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    renderer->objects.shader = id;
}

u32 jd_RendererGetGLTextureIDByPage(jd_Renderer* renderer, u32 id) {
    
}

#define jd_Render_Font_Texture_Height 128
#define jd_Render_Font_Texture_Width 64
#define jd_Render_Font_Texture_Depth 2048 // This should cover the vast majority of modern devices, but the standard *does* only gaurantee 256.

#define jd_Default_Face_Point_Size 12

typedef struct _jd_FT_Instance {
    FT_Library library;
    b32 init;
} _jd_FT_Instance;

static _jd_FT_Instance _jd_ft_global_instance = {0};

#define _jd_GlyphHashTableLimit KILOBYTES(16)

u32 jd_GlyphHashGetIndexForCodepoint(u32 codepoint) {
    if (codepoint <= 128) 
        return codepoint;
    
    u32 hash = jd_HashU32toU32(codepoint, 5700877729);
    return hash & _jd_GlyphHashTableLimit - 1;
}

jd_Glyph* jd_TypefaceGetGlyph(jd_Typeface* face, u32 codepoint) {
    if (codepoint > face->range.end) {
        return 0;
    }
    
    u32 hash = jd_GlyphHashGetIndexForCodepoint(codepoint);
    jd_Glyph* glyph = &face->glyphs[hash];
    while (glyph && glyph->codepoint != codepoint) {
        glyph = glyph->next_with_same_hash;
    }
    
    return glyph;
}

jd_Glyph* jd_TypefaceInsertGlyph(jd_Typeface* face, u32 codepoint) {
    if (codepoint > face->range.end) {
        return 0;
    }
    u32 hash = jd_GlyphHashGetIndexForCodepoint(codepoint);
    jd_Glyph* glyph = &face->glyphs[hash];
    
    if (glyph->codepoint == 0) {
        glyph->codepoint = codepoint;
        return glyph;
    }
    
    while (glyph->next_with_same_hash != 0)
        glyph = glyph->next_with_same_hash;
    
    glyph->next_with_same_hash = jd_ArenaAlloc(face->arena, sizeof(jd_Glyph));
    glyph = glyph->next_with_same_hash;
    glyph->codepoint = codepoint;
    return glyph;
}

jd_Typeface* jd_TypefaceLoadFromMemory(jd_Renderer* renderer, jd_String id_str, jd_File file, jd_TypefaceUnicodeRange* range, i32 base_point_size) {
    if (file.size == 0) {
        jd_LogError("File is zero-sized! Did it load correctly?", jd_Error_MissingResource, jd_Error_Common);
        return 0;
    }
    
    if (!range)
        range = &jd_unicode_range_basic_latin;
    
    if (base_point_size == 0)
        base_point_size = jd_Default_Face_Point_Size;
    
    jd_UserLockGet(renderer->font_lock);
    
    if (!_jd_ft_global_instance.init) {
        i32 error = FT_Init_FreeType(&_jd_ft_global_instance.library);
        if (error) {
            jd_LogError("Could not initialize FreeType library.", jd_Error_OutOfMemory, jd_Error_Critical);
            jd_UserLockRelease(renderer->font_lock);
            return 0;
        }
        
        _jd_ft_global_instance.init = true;
    }
    
    FT_Face ft_face = {0};
    i32 error = FT_New_Memory_Face(_jd_ft_global_instance.library, file.mem, file.size, 0, &ft_face);
    if (error) {
        jd_LogError("Could not load font file with FreeType.", jd_Error_BadInput, jd_Error_Critical);
        jd_UserLockRelease(renderer->font_lock);
        return 0;
    }
    
    if (ft_face->num_glyphs == 0) {
        jd_LogError("Font does not contain any glyphs!", jd_Error_BadInput, jd_Error_Critical);
        jd_UserLockRelease(renderer->font_lock);
        return 0;
    }
    
    jd_Typeface* face = jd_DArrayPushBack(renderer->fonts, 0);
    
    if (!face) {
        jd_LogError("Could not allocate memory for the typeface.", jd_Error_OutOfMemory, jd_Error_Common);
        jd_UserLockRelease(renderer->font_lock);
        return 0;
    }
    
    u32 dpi = jd_WindowGetDPI(renderer->window);
    error = FT_Set_Char_Size(ft_face,    /* handle to face object         */
                             0,       /* char_width in 1/64 of points  */
                             base_point_size * 64,   /* char_height in 1/64 of points */
                             dpi,     /* horizontal device resolution  */
                             dpi);    /* vertical device resolution    */
    
    jd_UserLockRelease(renderer->font_lock);
    
    face->ascent = ft_face->size->metrics.ascender / 64;
    face->descent = ft_face->size->metrics.descender / 64 ;
    face->line_adv = face->ascent - face->descent;
    face->arena = jd_ArenaCreate(0, 0);
    face->range = *range;
    
    jd_Assert(renderer->texture_pass_count < jd_Renderer_Max_Texture_Passes);
    renderer->texture_passes[renderer->texture_pass_count].vertices = jd_DArrayCreate(MEGABYTES(4) / sizeof(jd_GLVertex), sizeof(jd_GLVertex));
    
    // allocate the first page of the texture
    // setup textures
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &renderer->texture_passes[renderer->texture_pass_count].gl_index);
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, renderer->texture_passes[renderer->texture_pass_count].gl_index);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, jd_Render_Font_Texture_Width, jd_Render_Font_Texture_Height, jd_Render_Font_Texture_Depth, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 3);
    
    renderer->texture_pass_count++;
    
    face->texture_width = jd_Render_Font_Texture_Width;
    face->texture_height = jd_Render_Font_Texture_Height;
    face->glyph_count = 0;
    
    u32 glyph_texture_index = 0;
    
    // white space for drawing plain colored rects
    {
        face->white_bitmap = jd_ArenaAlloc(renderer->arena, sizeof(u32) * face->texture_width * face->texture_height);
        jd_MemSet(face->white_bitmap, 0xFF, sizeof(u32) * face->texture_width * face->texture_height);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, jd_Render_Font_Texture_Width, jd_Render_Font_Texture_Height, 1, GL_RED, GL_UNSIGNED_BYTE, face->white_bitmap);
        glyph_texture_index++;
    }
    
    face->glyphs = jd_ArenaAlloc(face->arena, sizeof(jd_Glyph) * _jd_GlyphHashTableLimit);
    
    for (u32 i = 0; i < range->end; i++) {
        u32 glyph_index = FT_Get_Char_Index(ft_face, i);
        if (glyph_index && (FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_DEFAULT) == 0)) {
            if (FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL) != 0) {
                jd_LogError("Glyph was present but couldn't be rasterized!", jd_Error_BadInput, jd_Error_Common);
                continue;
            }
            
            jd_Assert(ft_face->glyph->bitmap.width <= jd_Render_Font_Texture_Width);
            jd_Assert(ft_face->glyph->bitmap.rows <= jd_Render_Font_Texture_Height);
            
            jd_Glyph* glyph = jd_TypefaceInsertGlyph(face, i);
            
            glyph->size.w = ft_face->glyph->bitmap.width;
            glyph->size.h = ft_face->glyph->bitmap.rows;
            glyph->offset.x = ft_face->glyph->bitmap_left;
            glyph->offset.y = -ft_face->glyph->bitmap_top;
            glyph->h_advance = ft_face->glyph->metrics.horiAdvance / 64;
            
            u8* bitmap = ft_face->glyph->bitmap.buffer;
            if (bitmap) {
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, glyph_texture_index, glyph->size.w, glyph->size.h, 1, GL_RED, GL_UNSIGNED_BYTE, face->white_bitmap);
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, glyph_texture_index, glyph->size.w, glyph->size.h, 1, GL_RED, GL_UNSIGNED_BYTE, bitmap);
                
                face->glyph_count++;
                
                glyph->texture_page = renderer->texture_pass_count - 1;
                glyph->texture_index = glyph_texture_index;
                
                glyph_texture_index++;
                
                if ((glyph_texture_index > 0) && (glyph_texture_index % jd_Render_Font_Texture_Depth) == 0) {
                    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
                    //glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
                    
                    jd_Assert(renderer->texture_pass_count < jd_Renderer_Max_Texture_Passes);
                    renderer->texture_passes[renderer->texture_pass_count].vertices = jd_DArrayCreate(MEGABYTES(4) / sizeof(jd_GLVertex), sizeof(jd_GLVertex));
                    
                    // allocate the first page of the texture
                    // setup textures
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                    glGenTextures(1, &renderer->texture_passes[renderer->texture_pass_count].gl_index);
                    //glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D_ARRAY, renderer->texture_passes[renderer->texture_pass_count].gl_index);
                    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                    
                    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, jd_Render_Font_Texture_Width, jd_Render_Font_Texture_Height, jd_Render_Font_Texture_Depth, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
                    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 3);
                    glyph_texture_index = 0;
                    
                    
                    renderer->texture_pass_count++;
                }
            }
            
        }
    }
    
    FT_Done_Face(ft_face);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    
    return face;
}

b32 jd_Internal_DrawGlyph(jd_Renderer* renderer, jd_Typeface* face, jd_V2F window_pos, jd_V4F col, jd_Glyph* g) {
    f32 dpi_scaling = renderer->dpi_scaling;
    
    f32 x = window_pos.x;
    f32 y = window_pos.y;
    f32 z = 0.0f;
    
    jd_V3F tx = { g->size.x * (1.0f / (f32)jd_Render_Font_Texture_Width), g->size.y * (1.0f / (f32)jd_Render_Font_Texture_Height), g->texture_index };
    
    jd_Rectangle rect = {0};
    rect.min.x = x + (g->offset.x);
    rect.min.y = y + (g->offset.y);
    rect.max.x = rect.min.x + (g->size.x);
    rect.max.y = rect.min.y + (g->size.y);
    
    if (rect.max.x < 0.0f || rect.max.y < 0.0f) {
        return true;
    }
    
    if (rect.min.x > renderer->render_size.x) {
        return true;
    }
    
    if (rect.min.y > renderer->render_size.y) {
        return false;
    }
    
    jd_GLVertex top_right = {
        .pos = { rect.max.x, rect.max.y, z },
        .tx = tx,
        .col = col
    };
    
    jd_GLVertex bottom_right = {
        .pos = { rect.max.x, rect.min.y, z },
        .tx = {tx.u, 0.0f, tx.w},
        .col = col
    };
    
    jd_GLVertex bottom_left = {
        .pos = { rect.min.x, rect.min.y, z },
        .tx = {0.0f, 0.0f, tx.w},
        .col = col
    };
    
    jd_GLVertex top_left = {
        .pos = { rect.min.x, rect.max.y, z },
        .tx = {0.0f, tx.v, tx.w},
        .col = col
    };
    
    jd_DArray* vertices = renderer->texture_passes[g->texture_page].vertices;
    jd_DArrayPushBack(vertices, &top_right);
    jd_DArrayPushBack(vertices, &bottom_right);
    jd_DArrayPushBack(vertices, &bottom_left);
    jd_DArrayPushBack(vertices, &bottom_left);
    jd_DArrayPushBack(vertices, &top_left);
    jd_DArrayPushBack(vertices, &top_right);
    
    return true;
}

jd_V2F jd_CalcStringBoxMax(jd_Renderer* renderer, jd_Typeface* face, jd_String str, f32 wrap_width) {
    jd_V2F max = {0.0f, face->line_adv};
    jd_V2F pos = {0.0f, 0.0f};
    
    jd_UTFDecodedString utf32_string = jd_UnicodeDecodeUTF8String(renderer->frame_arena, jd_UnicodeTF_UTF32, str, false);
    
    for (u64 i = 0; i < utf32_string.count; i++) {
        jd_Glyph* glyph = jd_TypefaceGetGlyph(face, utf32_string.utf32[i]);
        
        if (!glyph || glyph->codepoint == 0) {
            glyph = jd_TypefaceGetGlyph(face, '?');
        }
        
        f32 adv = glyph->h_advance;
        
        if ((pos.x + adv > wrap_width)) {
            max.x = wrap_width;
            pos.x = 0;
            pos.y += face->line_adv;
            max.y = jd_Max(max.y, pos.y);
        }
        
        else {
            pos.x += adv;
            max.x = jd_Max(max.x, pos.x);
        }
    }
    
    max.x /= renderer->dpi_scaling;
    max.y /= renderer->dpi_scaling;
    
    return max;
}

void jd_DrawString(jd_Renderer* renderer, jd_Typeface* face, jd_String str, jd_V2F window_pos, jd_V4F color, f32 wrap_width) {
    jd_V2F pos = window_pos;
    pos.x *= renderer->dpi_scaling;
    pos.y *= renderer->dpi_scaling;
    pos.y += face->descent;
    
    jd_V2F starting_pos = pos;
    
    jd_UTFDecodedString utf32_string = jd_UnicodeDecodeUTF8String(renderer->frame_arena, jd_UnicodeTF_UTF32, str, false);
    
    jd_V4F supplied_color = color;
    jd_V4F glyph_color = supplied_color;
    
    for (u64 i = 0; i < utf32_string.count; i++) {
        glyph_color = supplied_color;
        if (utf32_string.utf32[i] == 0x0a) {
            pos.x = starting_pos.x;
            pos.y += face->line_adv;
            continue;
        }
        
        if (utf32_string.utf32[i] == 0x0d) {
            continue;
        }
        
        jd_Glyph* glyph = jd_TypefaceGetGlyph(face, utf32_string.utf32[i]);
        if (!glyph || glyph->codepoint == 0) {
            glyph = jd_TypefaceGetGlyph(face, '?');
            glyph_color = (jd_V4F){1.0, 0.0, 0.0, 1.0};
        }
        
        f32 adv = glyph->h_advance;
        
        if ((pos.x - starting_pos.x) + adv > wrap_width) {
            pos.x = starting_pos.x;
            pos.y += face->line_adv;
        }
        
        if (!(jd_Internal_DrawGlyph(renderer, face, pos, glyph_color, glyph))) {
            break;
        }
        
        pos.x += adv;
    }
}

void jd_DrawStringWithBG(jd_Renderer* renderer, jd_Typeface* face, jd_String str, jd_V2F window_pos, jd_V4F text_color, jd_V4F bg_color, f32 wrap_width) {
    jd_V2F max = jd_CalcStringBoxMax(renderer, face, str, wrap_width);
    jd_V2F box_pos = {window_pos.x, window_pos.y - max.y};
    jd_DrawRect(renderer, box_pos, max, bg_color);
    jd_DrawString(renderer, face, str, window_pos, text_color, wrap_width);
}

void jd_DrawRect(jd_Renderer* renderer, jd_V2F window_pos, jd_V2F size, jd_V4F col) {
    f32 x = window_pos.x * renderer->dpi_scaling;
    f32 y = window_pos.y * renderer->dpi_scaling;
    f32 z = 0.0f;
    
    jd_Rectangle rect;
    rect.min.x = x;
    rect.min.y = y;
    rect.max.x = x + (size.x * renderer->dpi_scaling);
    rect.max.y = y + (size.y * renderer->dpi_scaling);
    
    jd_GLVertex top_right = {
        .pos = { rect.max.x, rect.max.y, z },
        .tx = {1.0f, 1.0f, 0.0f},
        .col = col
    };
    
    jd_GLVertex bottom_right = {
        .pos = { rect.max.x, rect.min.y, z },
        .tx = {1.0f, 0.0f, 0.0f},
        .col = col
    };
    
    jd_GLVertex bottom_left = {
        .pos = { rect.min.x, rect.min.y, z },
        .tx = {0.0f, 0.0f, 0.0f},
        .col = col
    };
    
    jd_GLVertex top_left = {
        .pos = { rect.min.x, rect.max.y, z },
        .tx = {0.0f, 1.0f, 0.0f},
        .col = col
    };
    
    jd_DArray* vertices = renderer->texture_passes[0].vertices;
    
    jd_DArrayPushBack(vertices, &top_right);
    jd_DArrayPushBack(vertices, &bottom_right);
    jd_DArrayPushBack(vertices, &bottom_left);
    jd_DArrayPushBack(vertices, &bottom_left);
    jd_DArrayPushBack(vertices, &top_left);
    jd_DArrayPushBack(vertices, &top_right);
}

jd_Renderer* jd_RendererCreate(struct jd_Window* window) {
    u32 max_array_tex_layers = 256;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &max_array_tex_layers);
    jd_Arena* arena = jd_ArenaCreate(0, 0);
    jd_Renderer* renderer = jd_ArenaAlloc(arena, sizeof(*renderer));
    renderer->arena = arena;
    renderer->frame_arena = jd_ArenaCreate(0, 0);
    renderer->max_texture_layers = max_array_tex_layers;
    renderer->dpi_scaling = 1.0f;
    renderer->font_lock = jd_UserLockCreate(renderer->arena, 32);
    
    renderer->fonts = jd_DArrayCreate(256, sizeof(jd_Typeface));
    renderer->window = window;
    
    jd_RenderObjects* objects = &renderer->objects;
    jd_ShaderCreate(renderer);
    
    jd_File libmono = jd_DiskFileReadFromPath(renderer->frame_arena, jd_StrLit("C:\\Windows\\Fonts\\consola.ttf"));
    renderer->default_face = jd_TypefaceLoadFromMemory(renderer, jd_StrLit("libmono"), libmono, &jd_unicode_range_bmp, 10);
    
    glGenVertexArrays(1, &objects->vao);
    glGenBuffers(1, &objects->vbo);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(objects->vao);
    glBindBuffer(GL_ARRAY_BUFFER, objects->vbo);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(jd_GLVertex), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(jd_GLVertex), (void*)(sizeof(jd_V3F)));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(jd_GLVertex), (void*)(sizeof(jd_V3F) * 2));
    
    //glBindBuffer(GL_ARRAY_BUFFER, objects->vbo);
    glBufferData(GL_ARRAY_BUFFER, MEGABYTES(16), NULL, GL_DYNAMIC_DRAW);
    //glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    return renderer;
}

void jd_RendererSetDPIScale(jd_Renderer* renderer, f32 scale) {
    b32 font_refresh = false;
    if (scale != renderer->dpi_scaling) font_refresh = true;
#if 0
    renderer->dpi_scaling = scale;
    if (font_refresh) jd_RefreshFonts(renderer);
#endif
}

void jd_RendererSetRenderSize(jd_Renderer* renderer, jd_V2F render_size) {
    renderer->render_size = render_size;
    glViewport(0, 0, (i32)render_size.w, (i32)render_size.h);
    glClearColor(0.33f, 0.33f, 0.33f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void jd_RendererDraw(jd_Renderer* renderer) {
    jd_V2F size = jd_WindowGetDrawSize(renderer->window);
    
    //glActiveTexture(GL_TEXTURE0);
    for (u64 i = 0; i < renderer->texture_pass_count; i++) {
        jd_DArray* vertices = renderer->texture_passes[i].vertices;
        
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices->view.count * sizeof(jd_GLVertex), vertices->view.mem);
        glBindTexture(GL_TEXTURE_2D_ARRAY, renderer->texture_passes[i].gl_index);
        glUseProgram(renderer->objects.shader);
        i32 tex_loc = glGetUniformLocation(renderer->objects.shader, "tex");
        i32 screen_conv_location = glGetUniformLocation(renderer->objects.shader, "screen_conv");
        glUniform1i(tex_loc, 0);
        glUniform2f(screen_conv_location, 2.0f / size.w, 2.0f / size.h);
        
        glBindVertexArray(renderer->objects.vao);
        glBindBuffer(GL_ARRAY_BUFFER, renderer->objects.vbo);
        glDrawArrays(GL_TRIANGLES, 0, vertices->view.count);
        
        jd_DArrayClear(vertices);
    }
    
    //jd_ArenaPopTo(renderer->frame_arena, 0);
}
