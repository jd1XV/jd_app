/* date = March 11th 2024 8:27 pm */

#ifndef JD_UNICODE_H
#define JD_UNICODE_H

#ifndef JD_DEFS_H
#include "jd_defs.h"
#endif

#ifndef JD_STRING_H
#include "jd_string.h"
#endif

typedef enum jd_UnicodeTF {
    jd_UnicodeTF_UTF8,
    jd_UnicodeTF_UTF16,
    jd_UnicodeTF_UTF32,
    jd_UnicodeTF_Count
} jd_UnicodeTF;

typedef struct jd_UTFDecodedString {
    jd_UnicodeTF tf;
    union {
        u8*  utf8;
        u16* utf16;
        u32* utf32;
    }
    u64 count;
};

// 0xxxxxxx
// 110xxxxx 10xxxxxx
// 1110xxxx 10xxxxxx 10xxxxxx
// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

#define _jd_PackU64_u8(a,b,c,d, e,f,g,h) \
(a << 56) + (b << 48) + (c << 40) + (d << 32) + (e << 24) + (f << 16) + (g << 8) + h

jd_UTFDecodedString jd_UnicodeDecodeUTF8String(jd_Arena* arena, jd_UnicodeTF tf, jd_String input, b32 validate) {
    jd_UTFDecodedString dec_str = {
        .tf = tf,
    };
    
    u64 mask_ascii = 0x8080808080808080ull;
    
    u8 needed_continuations = 0;
    
    switch (tf) {
        default: {
            jd_LogError("Unknown Unicode Transformation Format. \
                                                                                                                                                                                                                                                                                                            Valid choices: jd_UnicodeTF_[UTF8|UTF16|UTF32]", jd_Error_API_Misuse, jd_Error_Common);
            return dec_str;
        }
        
        case jd_UnicodeTF_UTF32: {
            
            dec_str.utf32 = jd_ArenaAlloc(arena, input.count * 4);
            
            u8 continuation_table[] = {
                1, 1, 1, 1, 
                1, 1, 1, 1,
                1, 1, 1, 1,
                1, 1, 1, 1,
                0, 0, 0, 0,
                0, 0, 0, 0,
                2, 2, 2, 2,
                3, 3,
                4,
                0
            };
            
            for (u32 i = 0; i + 8 < input.count; i += 8) {
                u64 chunk = *(u64*)input.mem + i;
                u8* chunk_bytes = &chunk;
                u64 mask = 0;
                if ((mask = (chunk & mask_ascii)) == 0) {
                    dec_str.utf32[i]     = (u32)(chunk_bytes[i]);
                    dec_str.utf32[i + 1] = (u32)(chunk_bytes[i + 1]);
                    dec_str.utf32[i + 2] = (u32)(chunk_bytes[i + 2]);
                    dec_str.utf32[i + 3] = (u32)(chunk_bytes[i + 3]);
                    dec_str.utf32[i + 4] = (u32)(chunk_bytes[i + 4]);
                    dec_str.utf32[i + 5] = (u32)(chunk_bytes[i + 5]);
                    dec_str.utf32[i + 6] = (u32)(chunk_bytes[i + 6]);
                    dec_str.utf32[i + 7] = (u32)(chunk_bytes[i + 7]);
                } else {
                    switch (continuations[i + j]) {
                        case 0: { // continuation character
                            if (needed_continuations <= 0) {
                                jd_LogError("Unicode continuation character found where it doesn't belong", jd_Error_BadInput, jd_Error_Common);
                                return dec_str;
                                needed_continuations = 0;
                            }
                            needed_continuations -= 1;
                        } break;
                        
                        case 1: {
                            
                        } break;
                        
                        case 2: {
                            needed_continuations = 1;
                        } break;
                        
                        case 3: {
                            needed_continuations = 2;
                        } break;
                        
                        case 4: {
                            needed_continuations = 3;
                        }
                    }
                }
            }
            
            
        }
    }
    
    
    return dec_str;
}


#endif //JD_UNICODE_H
