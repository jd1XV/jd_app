#define STB_TRUETYPE_IMPLEMENTATION
#include "dep/stb_truetype.h"
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
    char info_log[512];
    
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

#define _JD_RENDER_FONT_TEXTURE_HEIGHT 256
#define _JD_RENDER_FONT_TEXTURE_WIDTH 128

jd_Typeface* jd_TypefaceLoadFromMemory(jd_Renderer* renderer, jd_File file, i32 base_point_size) {
    if (file.size == 0) {
        jd_LogError("File is zero-sized! Did it load correctly?", jd_Error_MissingResource, jd_Error_Common);
        return 0;
    }
    
    
}

#if 0
jd_Font* jd_FontPushFromFile(jd_Renderer* renderer, jd_String ttf_path, i32 face_size_pixels) {
    if (!jd_DiskPathExists(ttf_path)) return NULL;
    jd_Font* font = jd_DArrayPushBack(renderer->fonts, NULL);
    font->ttf_file = jd_DiskFileReadFromPath(renderer->arena, ttf_path);
    font->face_size = face_size_pixels;
    jd_File ttf_file = font->ttf_file;
    if (ttf_file.size == 0) {
        jd_DArrayPopIndex(renderer->fonts, renderer->fonts->view.count - 1);
    }
    else {
        // setup textures
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &font->texture);
        //glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, font->texture);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, _JD_RENDER_FONT_TEXTURE_WIDTH, _JD_RENDER_FONT_TEXTURE_HEIGHT, 1024, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 3);
        
        font->texture_width = _JD_RENDER_FONT_TEXTURE_WIDTH;
        font->texture_height = _JD_RENDER_FONT_TEXTURE_HEIGHT;
        
        // white space for drawing plain colored rects
        {
            font->white_bitmap = jd_ArenaAlloc(renderer->arena, sizeof(u32) * font->texture_width * font->texture_height);
            __m256i white = _mm256_set1_epi8(0xff);
            for (u64 i = 0; i < _JD_RENDER_FONT_TEXTURE_WIDTH * _JD_RENDER_FONT_TEXTURE_HEIGHT; i += 32) {
                
                _mm256_storeu_si256 ((__m256i*)(font->white_bitmap + i), white);
            }
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, _JD_RENDER_FONT_TEXTURE_WIDTH, _JD_RENDER_FONT_TEXTURE_HEIGHT, 1, GL_RED, GL_UNSIGNED_BYTE, font->white_bitmap);
        }
        
        stbtt_fontinfo stb_font;
        if (stbtt_InitFont(&stb_font, ttf_file.mem, stbtt_GetFontOffsetForIndex(ttf_file.mem, 0))) {
            
            f32 scale = stbtt_ScaleForPixelHeight(&stb_font, face_size_pixels);
            f32 em_scale = stbtt_ScaleForMappingEmToPixels(&stb_font, face_size_pixels);
            
            i32 ascent, descent, gap;
            stbtt_GetFontVMetrics(&stb_font, &ascent, &descent, &gap);
            
            font->ascent = ascent * em_scale;
            font->descent = descent * em_scale;
            font->line_gap = gap * em_scale;
            font->line_adv = (font->ascent - font->descent) + font->line_gap;
            
            jd_Glyph* glyph_ptr = font->glyphs + 32;
            f32* advance_ptr = font->advances + 32;
            
            for (u32 i = 32; i < 1024; i++, advance_ptr++, glyph_ptr++) {
                i32 w = 0;
                i32 h = 0;
                i32 xoff = 0;
                i32 yoff = 0;
                
                u8* bitmap = stbtt_GetCodepointBitmap(&stb_font, 0, scale, i, &w, &h, &xoff, &yoff);
                if (bitmap) {
                    jd_Assert(w <= _JD_RENDER_FONT_TEXTURE_WIDTH);
                    jd_Assert(h <= _JD_RENDER_FONT_TEXTURE_HEIGHT);
                    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, w, h, 1, GL_RED, GL_UNSIGNED_BYTE, bitmap);
                    stbtt_FreeBitmap(bitmap, 0);
                }
                
                glyph_ptr->offset.x = xoff;
                glyph_ptr->offset.y = yoff;
                glyph_ptr->size.w = w;
                glyph_ptr->size.h = h;
                
                i32 advance = 0;
                i32 lsb = 0;
                stbtt_GetCodepointHMetrics(&stb_font, i, &advance, &lsb);
                
                *advance_ptr = advance*em_scale;
            }
        }
        
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }
    return font;
}


void jd_RefreshFonts(jd_Renderer* renderer) {
    for (u64 i = 0; i < renderer->fonts->view.count; i++) {
        jd_Font* font = jd_DArrayGetIndex(renderer->fonts, i);
        glBindTexture(GL_TEXTURE_2D_ARRAY, font->texture);
        stbtt_fontinfo stb_font;
        if (stbtt_InitFont(&stb_font, font->ttf_file.mem, stbtt_GetFontOffsetForIndex(font->ttf_file.mem, 0))) {
            
            f32 scale = stbtt_ScaleForPixelHeight(&stb_font, font->face_size * renderer->dpi_scaling);
            f32 em_scale = stbtt_ScaleForMappingEmToPixels(&stb_font, font->face_size * renderer->dpi_scaling);
            
            i32 ascent, descent, gap;
            stbtt_GetFontVMetrics(&stb_font, &ascent, &descent, &gap);
            
            font->ascent = ascent * em_scale;
            font->descent = descent * em_scale;
            font->line_gap = gap * em_scale;
            font->line_adv = (font->ascent - font->descent) + font->line_gap;
            
            jd_Glyph* glyph_ptr = font->glyphs + 32;
            f32* advance_ptr = font->advances + 32;
            
            for (u32 i = 32; i < 1024; i++, advance_ptr++, glyph_ptr++) {
                i32 w = 0;
                i32 h = 0;
                i32 xoff = 0;
                i32 yoff = 0;
                
                u8* bitmap = stbtt_GetCodepointBitmap(&stb_font, 0, scale, i, &w, &h, &xoff, &yoff);
                if (bitmap) {
                    jd_Assert(w <= _JD_RENDER_FONT_TEXTURE_WIDTH);
                    jd_Assert(h <= _JD_RENDER_FONT_TEXTURE_HEIGHT);
                    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, w, h, 1, GL_RED, GL_UNSIGNED_BYTE, font->white_bitmap);
                    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, w, h, 1, GL_RED, GL_UNSIGNED_BYTE, bitmap);
                    stbtt_FreeBitmap(bitmap, 0);
                }
                
                glyph_ptr->offset.x = xoff;
                glyph_ptr->offset.y = yoff;
                glyph_ptr->size.w = w;
                glyph_ptr->size.h = h;
                
                i32 advance = 0;
                i32 lsb = 0;
                stbtt_GetCodepointHMetrics(&stb_font, i, &advance, &lsb);
                
                *advance_ptr = advance*em_scale;
            }
            
        }
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }
    
}

void jd_Internal_DrawGlyph(jd_Renderer* renderer, jd_Font* font, jd_V2F window_pos, jd_V4F col, c8 glyph_char) {
    if (renderer->active_texture != font->texture) {
        jd_RendererDraw(renderer);
        renderer->active_texture = font->texture;
    }
    
    if (glyph_char > 1024) glyph_char = 0;
    
    jd_Glyph* g = font->glyphs + glyph_char;
    
    f32 dpi_scaling = renderer->dpi_scaling;
    
    f32 x = window_pos.x;
    f32 y = window_pos.y;
    f32 z = 0.0f;
    
    jd_V3F tx = { g->size.x * (1.0f / (f32)_JD_RENDER_FONT_TEXTURE_WIDTH), g->size.y * (1.0f / (f32)_JD_RENDER_FONT_TEXTURE_HEIGHT), (f32)glyph_char };
    
    jd_Rectangle rect;
    rect.min.x = x + (g->offset.x);
    rect.min.y = y + (g->offset.y);
    rect.max.x = rect.min.x + (g->size.x);
    rect.max.y = rect.min.y + (g->size.y);
    
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
    
    jd_DArrayPushBack(renderer->vertices, &top_right);
    jd_DArrayPushBack(renderer->vertices, &bottom_right);
    jd_DArrayPushBack(renderer->vertices, &bottom_left);
    jd_DArrayPushBack(renderer->vertices, &bottom_left);
    jd_DArrayPushBack(renderer->vertices, &top_left);
    jd_DArrayPushBack(renderer->vertices, &top_right);
}

jd_V2F jd_CalcStringBoxMax(jd_Renderer* renderer, jd_Font* font, jd_String str, f32 wrap_width) {
    jd_V2F max = {0.0f, font->line_adv};
    jd_V2F pos = {0.0f, 0.0f};
    
    for (u64 i = 0; i < str.count; i++) {
        c8 g = str.mem[i];
        f32 adv = 0.0f;
        if (g < 1024) {
            adv = font->advances[g];
        }
        
        if ((pos.x + adv > wrap_width)) {
            max.x = wrap_width;
            pos.x = 0;
            pos.y += font->line_adv;
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

void jd_DrawString(jd_Renderer* renderer, jd_Font* font, jd_String str, jd_V2F window_pos, jd_V4F color, f32 wrap_width) {
    jd_V2F pos = window_pos;
    pos.x *= renderer->dpi_scaling;
    pos.y *= renderer->dpi_scaling;
    pos.y += font->descent;
    jd_V2F starting_pos = pos;
    for (u64 i = 0; i < str.count; i++) {
        c8 g = str.mem[i];
        f32 adv = 0.0f;
        if (g < 1024) {
            adv = font->advances[g];
        }
        
        if ((pos.x - starting_pos.x) + adv > wrap_width) {
            pos.x = starting_pos.x;
            pos.y += font->line_adv;
        }
        
        jd_Internal_DrawGlyph(renderer, font, pos, color, g);
        
        pos.x += adv;
    }
}

void jd_DrawStringWithBG(jd_Renderer* renderer, jd_Font* font, jd_String str, jd_V2F window_pos, jd_V4F text_color, jd_V4F bg_color, f32 wrap_width) {
    jd_V2F max = jd_CalcStringBoxMax(renderer, font, str, wrap_width);
    jd_V2F box_pos = {window_pos.x, window_pos.y - max.y};
    jd_DrawRect(renderer, box_pos, max, bg_color);
    jd_DrawString(renderer, font, str, window_pos, text_color, wrap_width);
}

#endif

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
    
    jd_DArrayPushBack(renderer->vertices, &top_right);
    jd_DArrayPushBack(renderer->vertices, &bottom_right);
    jd_DArrayPushBack(renderer->vertices, &bottom_left);
    jd_DArrayPushBack(renderer->vertices, &bottom_left);
    jd_DArrayPushBack(renderer->vertices, &top_left);
    jd_DArrayPushBack(renderer->vertices, &top_right);
}

jd_Renderer* jd_RendererCreate(struct jd_Window* window) {
    u32 max_array_tex_layers = 256;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &max_array_tex_layers);
    jd_Arena* arena = jd_ArenaCreate(0, 0);
    jd_Renderer* renderer = jd_ArenaAlloc(arena, sizeof(*renderer));
    renderer->arena = arena;
    renderer->max_texture_layers = max_array_tex_layers;
    renderer->dpi_scaling = 1.0f;
#if 0
    renderer->fonts = jd_DArrayCreate(256, sizeof(jd_Font));
    renderer->default_font = jd_FontPushFromFile(renderer, jd_StrLit("assets/libmono.ttf"), 42);
#endif
    
#if 0
    renderer->active_texture = renderer->default_font->texture;
#endif
    renderer->vertices = jd_DArrayCreate(MEGABYTES(64) / sizeof(jd_GLVertex), sizeof(jd_GLVertex));
    renderer->window = window;
    jd_RenderObjects* objects = &renderer->objects;
    jd_ShaderCreate(renderer);
    
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(jd_GLVertex) * 3600, NULL, GL_DYNAMIC_DRAW);
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
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->vertices->view.count * sizeof(jd_GLVertex), jd_DArrayGetIndex(renderer->vertices, 0));
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, renderer->active_texture);
    glUseProgram(renderer->objects.shader);
    i32 tex_loc = glGetUniformLocation(renderer->objects.shader, "tex");
    i32 screen_conv_location = glGetUniformLocation(renderer->objects.shader, "screen_conv");
    glUniform1i(tex_loc, 0);
    glUniform2f(screen_conv_location, 2.0f / size.w, 2.0f / size.h);
    
    glBindVertexArray(renderer->objects.vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->objects.vbo);
    glDrawArrays(GL_TRIANGLES, 0, renderer->vertices->view.count);
    
    jd_DArrayClear(renderer->vertices);
}
