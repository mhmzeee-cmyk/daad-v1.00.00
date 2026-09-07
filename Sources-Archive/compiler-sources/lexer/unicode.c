/**
 * @file unicode.c
 * @brief تنفيذ أدوات Unicode
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#include "unicode.h"
#include <string.h>

size_t unicode_read_rune(const char* str, size_t length, Rune* rune) {
    if (!str || length == 0 || !rune) return 0;
    
    uint8_t* bytes = (uint8_t*)str;
    size_t byte_count = unicode_utf8_byte_count(bytes[0]);
    
    if (byte_count == 0 || byte_count > length) return 0;
    
    /* التحقق من صحة UTF-8 */
    for (size_t i = 1; i < byte_count; i++) {
        if ((bytes[i] & 0xC0) != 0x80) return 0;
    }
    
    /* فك الترميز */
    Rune result = 0;
    switch (byte_count) {
        case 1:
            result = bytes[0];
            break;
        case 2:
            result = (bytes[0] & 0x1F) << 6;
            result |= (bytes[1] & 0x3F);
            break;
        case 3:
            result = (bytes[0] & 0x0F) << 12;
            result |= (bytes[1] & 0x3F) << 6;
            result |= (bytes[2] & 0x3F);
            break;
        case 4:
            result = (bytes[0] & 0x07) << 18;
            result |= (bytes[1] & 0x3F) << 12;
            result |= (bytes[2] & 0x3F) << 6;
            result |= (bytes[3] & 0x3F);
            break;
        default:
            return 0;
    }
    
    *rune = result;
    return byte_count;
}

size_t unicode_write_rune(Rune rune, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return 0;
    
    if (rune <= 0x7F) {
        if (buffer_size < 1) return 0;
        buffer[0] = (char)rune;
        return 1;
    }
    else if (rune <= 0x7FF) {
        if (buffer_size < 2) return 0;
        buffer[0] = (char)(0xC0 | (rune >> 6));
        buffer[1] = (char)(0x80 | (rune & 0x3F));
        return 2;
    }
    else if (rune <= 0xFFFF) {
        if (buffer_size < 3) return 0;
        buffer[0] = (char)(0xE0 | (rune >> 12));
        buffer[1] = (char)(0x80 | ((rune >> 6) & 0x3F));
        buffer[2] = (char)(0x80 | (rune & 0x3F));
        return 3;
    }
    else if (rune <= 0x10FFFF) {
        if (buffer_size < 4) return 0;
        buffer[0] = (char)(0xF0 | (rune >> 18));
        buffer[1] = (char)(0x80 | ((rune >> 12) & 0x3F));
        buffer[2] = (char)(0x80 | ((rune >> 6) & 0x3F));
        buffer[3] = (char)(0x80 | (rune & 0x3F));
        return 4;
    }
    
    return 0;
}

CharCategory unicode_category(Rune rune) {
    /* الأرقام الغربية */
    if (rune >= '0' && rune <= '9') {
        return CHAR_CATEGORY_DIGIT;
    }
    
    /* الأرقام العربية-الهندية */
    if (rune >= 0x0660 && rune <= 0x0669) {
        return CHAR_CATEGORY_ARABIC_DIGIT;
    }
    
    /* الأحرف الإنجليزية الصغيرة */
    if (rune >= 'a' && rune <= 'z') {
        return CHAR_CATEGORY_LATIN_LOWER;
    }
    
    /* الأحرف الإنجليزية الكبيرة */
    if (rune >= 'A' && rune <= 'Z') {
        return CHAR_CATEGORY_LATIN_UPPER;
    }
    
    /* الأحرف العربية */
    if ((rune >= 0x0621 && rune <= 0x063A) ||
        (rune >= 0x0641 && rune <= 0x064A)) {
        return CHAR_CATEGORY_ARABIC;
    }
    
    /* الرموز الخاصة */
    switch (rune) {
        case '_':  return CHAR_CATEGORY_UNDERSCORE;
        case '.':  return CHAR_CATEGORY_DOT;
        case ':':  return CHAR_CATEGORY_COLON;
        case 0x060C: return CHAR_CATEGORY_COMMA;  /* ، Arabic comma */
        case 0x061B: return CHAR_CATEGORY_SEMICOLON; /* ؛ */
        case ',':  return CHAR_CATEGORY_COMMA;
        case '(':  return CHAR_CATEGORY_LPAREN;
        case ')':  return CHAR_CATEGORY_RPAREN;
        case '{':  return CHAR_CATEGORY_LBRACE;
        case '}':  return CHAR_CATEGORY_RBRACE;
        case '[':  return CHAR_CATEGORY_LBRACKET;
        case ']':  return CHAR_CATEGORY_RBRACKET;
        case '+':  return CHAR_CATEGORY_PLUS;
        case '-':  return CHAR_CATEGORY_MINUS;
        case '*':  return CHAR_CATEGORY_STAR;
        case '/':  return CHAR_CATEGORY_SLASH;
        case '%':  return CHAR_CATEGORY_PERCENT;
        case '=':  return CHAR_CATEGORY_ASSIGN;
        case '!':  return CHAR_CATEGORY_NOT;
        case '>':  return CHAR_CATEGORY_GREATER;
        case '<':  return CHAR_CATEGORY_LESS;
        case '&':  return CHAR_CATEGORY_AND;
        case '|':  return CHAR_CATEGORY_OR;
        case '^':  return CHAR_CATEGORY_XOR;
        case '~':  return CHAR_CATEGORY_TILDE;
        case '@':  return CHAR_CATEGORY_AT;
        case '#':  return CHAR_CATEGORY_HASH;
        case '\'': return CHAR_CATEGORY_SINGLE_QUOTE;
        case '"':  return CHAR_CATEGORY_DOUBLE_QUOTE;
        case '\n': return CHAR_CATEGORY_NEWLINE;
        case ' ':  return CHAR_CATEGORY_SPACE;
        case '\t': return CHAR_CATEGORY_TAB;
    }
    
    return CHAR_CATEGORY_OTHER;
}

int unicode_is_arabic(Rune rune) {
    return (rune >= 0x0621 && rune <= 0x063A) ||
           (rune >= 0x0641 && rune <= 0x064A) ||
           (rune >= 0x064B && rune <= 0x065F);
}

int unicode_is_latin(Rune rune) {
    return ((rune >= 'a' && rune <= 'z') || 
            (rune >= 'A' && rune <= 'Z'));
}

int unicode_is_digit(Rune rune) {
    return (rune >= '0' && rune <= '9');
}

int unicode_is_arabic_digit(Rune rune) {
    return (rune >= 0x0660 && rune <= 0x0669);
}

int unicode_is_identifier_char(Rune rune) {
    return unicode_is_arabic(rune) || 
           unicode_is_latin(rune) || 
           unicode_is_digit(rune) ||
           unicode_is_arabic_digit(rune) ||
           rune == '_';
}

int unicode_is_identifier_start(Rune rune) {
    return unicode_is_arabic(rune) || 
           unicode_is_latin(rune) || 
           rune == '_';
}

Rune unicode_to_western_digit(Rune rune) {
    if (rune >= 0x0660 && rune <= 0x0669) {
        return rune - 0x0660 + '0';
    }
    return rune;
}

size_t unicode_utf8_byte_count(uint8_t first_byte) {
    if (first_byte <= 0x7F) return 1;
    if ((first_byte & 0xE0) == 0xC0) return 2;
    if ((first_byte & 0xF0) == 0xE0) return 3;
    if ((first_byte & 0xF8) == 0xF0) return 4;
    return 0;
}

int unicode_validate_utf8(const char* str, size_t length) {
    if (!str || length == 0) return 1;
    
    size_t i = 0;
    while (i < length) {
        size_t count = unicode_utf8_byte_count((uint8_t)str[i]);
        if (count == 0) return 0;
        
        if (i + count > length) return 0;
        
        for (size_t j = 1; j < count; j++) {
            if (((uint8_t)str[i + j] & 0xC0) != 0x80) return 0;
        }
        
        if (count == 3) {
            uint32_t cp = ((uint8_t)str[i] & 0x0F) << 12 |
                          ((uint8_t)str[i + 1] & 0x3F) << 6 |
                          ((uint8_t)str[i + 2] & 0x3F);
            if (cp >= 0xD800 && cp <= 0xDFFF) return 0;
        }
        if (count == 4) {
            uint32_t cp = ((uint8_t)str[i] & 0x07) << 18 |
                          ((uint8_t)str[i + 1] & 0x3F) << 12 |
                          ((uint8_t)str[i + 2] & 0x3F) << 6 |
                          ((uint8_t)str[i + 3] & 0x3F);
            if (cp > 0x10FFFF) return 0;
            if (cp >= 0xD800 && cp <= 0xDFFF) return 0;
        }
        
        i += count;
    }
    
    return 1;
}
