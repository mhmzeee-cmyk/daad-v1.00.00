/**
 * @file lexer.c
 * @brief تنفيذ المحلل البصري (Lexer) لغة ض Core
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#include "lexer.h"
#include "unicode.h"
#include "keywords.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

/* ===== دوال مساعدة داخلية ===== */

static void add_error(Lexer* lexer, const char* code, const char* message) {
    if (lexer->error_count >= LEXER_MAX_ERRORS) return;
    
    LexerError* error = &lexer->errors[lexer->error_count];
    strncpy(error->code, code, sizeof(error->code) - 1);
    error->code[sizeof(error->code) - 1] = '\0';
    strncpy(error->message, message, sizeof(error->message) - 1);
    error->message[sizeof(error->message) - 1] = '\0';
    error->position.line = lexer->token_start_line;
    error->position.column = lexer->token_start_column;
    error->position.offset = lexer->token_start;
    
    lexer->error_count++;
    lexer->has_errors = 1;
}

/**
 * @brief قراءة بايت واحد من المصدر
 * 
 * Ownership: لا يملك القيمة، يُرجع مؤشراً داخل source.
 */
static char current_char(const Lexer* lexer) {
    if (lexer->position >= lexer->source_length) return '\0';
    return lexer->source[lexer->position];
}

/**
 * @brief الاطلاع على بايت بمعلومffset
 */
static char peek_char(const Lexer* lexer, size_t offset) {
    size_t pos = lexer->position + offset;
    if (pos >= lexer->source_length) return '\0';
    return lexer->source[pos];
}

/**
 * @brief التقدم با بايت واحد (updates line/column)
 */
static void advance(Lexer* lexer) {
    if (lexer->position < lexer->source_length) {
        if (lexer->source[lexer->position] == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }
        lexer->position++;
    }
}

/**
 * @brief التقدم برمز Unicode كامل ( updates line/column by 1 visual char)
 */
static void advance_rune(Lexer* lexer, size_t byte_count) {
    for (size_t i = 0; i < byte_count; i++) {
        advance(lexer);
    }
}

static void start_token(Lexer* lexer) {
    lexer->token_start = lexer->position;
    lexer->token_start_line = lexer->line;
    lexer->token_start_column = lexer->column;
    lexer->buffer_length = 0;
}

/**
 * @brief إضافة بايت واحد للمخزن المؤقت
 * 
 * Ownership: ينسخ القيمة إلى buffer الداخلي.
 */
static void buffer_char(Lexer* lexer, char c) {
    if (lexer->buffer_length < LEXER_MAX_BUFFER - 1) {
        lexer->buffer[lexer->buffer_length++] = c;
        lexer->buffer[lexer->buffer_length] = '\0';
    }
}

/**
 * @brief إضافة رمز Unicode للمخزن المؤقت
 */
static void buffer_rune(Lexer* lexer, Rune rune) {
    char utf8[5];
    size_t count = unicode_write_rune(rune, utf8, sizeof(utf8));
    for (size_t i = 0; i < count; i++) {
        buffer_char(lexer, utf8[i]);
    }
}

/**
 * @brief إنشاء Token من المخزن المؤقت
 * 
 * Ownership: يُرجع Token تملك value عبر strdup.
 * المستدعي مسؤول عن تحرير token.value عبر token_destroy.
 */
static Token make_token(const Lexer* lexer, TokenType type) {
    Token token;
    token.type = type;
    token.value = strdup(lexer->buffer);
    if (!token.value) {
        token.type = TOKEN_ERROR;
        token.value = strdup("ذاكرة غير كافية");
    }
    token.length = lexer->buffer_length;
    token.start.line = lexer->token_start_line;
    token.start.column = lexer->token_start_column;
    token.start.offset = lexer->token_start;
    token.end.line = lexer->line;
    token.end.column = lexer->column;
    token.end.offset = lexer->position;
    token.filename = lexer->filename;
    token.has_literal = 0;
    token.literal.int_value = 0;
    return token;
}

/**
 * @brief إنشاء Token خطأ
 * 
 * Ownership: يُرجع Token تملك value عبر strdup.
 */
static Token make_error_token(const Lexer* lexer, const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.value = strdup(message);
    token.length = strlen(message);
    token.start.line = lexer->token_start_line;
    token.start.column = lexer->token_start_column;
    token.start.offset = lexer->token_start;
    token.end.line = lexer->line;
    token.end.column = lexer->column;
    token.end.offset = lexer->position;
    token.filename = lexer->filename;
    token.has_literal = 0;
    token.literal.int_value = 0;
    return token;
}

/* ===== دوال التحليل ===== */

static void skip_whitespace(Lexer* lexer) {
    while (lexer->position < lexer->source_length) {
        char c = current_char(lexer);
        if (c == ' ' || c == '\t' || c == '\r') {
            advance(lexer);
        } else {
            break;
        }
    }
}

static void skip_single_line_comment(Lexer* lexer) {
    while (lexer->position < lexer->source_length) {
        if (current_char(lexer) == '\n') break;
        advance(lexer);
    }
}

static void skip_multi_line_comment(Lexer* lexer) {
    int depth = 1;
    while (lexer->position < lexer->source_length && depth > 0) {
        if (current_char(lexer) == '#' && peek_char(lexer, 1) == '[') {
            depth++;
            advance(lexer);
            advance(lexer);
        } else if (current_char(lexer) == ']' && peek_char(lexer, 1) == '#') {
            depth--;
            advance(lexer);
            advance(lexer);
        } else {
            advance(lexer);
        }
    }
}

static Token read_string(Lexer* lexer) {
    start_token(lexer);
    advance(lexer); /* تجاوز علامة الاقتباس المزدوجة الأولى */
    
    while (lexer->position < lexer->source_length) {
        char c = current_char(lexer);
        
        if (c == '"') {
            advance(lexer);
            return make_token(lexer, TOKEN_STRING);
        }
        
        if (c == '\\') {
            advance(lexer);
            char escape = current_char(lexer);
            switch (escape) {
                case 'n': buffer_char(lexer, '\n'); break;
                case 't': buffer_char(lexer, '\t'); break;
                case '\\': buffer_char(lexer, '\\'); break;
                case '"': buffer_char(lexer, '"'); break;
                case '\'': buffer_char(lexer, '\''); break;
                case '0': buffer_char(lexer, '\0'); break;
                default:
                    buffer_char(lexer, '\\');
                    buffer_char(lexer, escape);
                    break;
            }
            advance(lexer);
        } else {
            Rune rune;
            size_t bytes = unicode_read_rune(lexer->source + lexer->position, 
                                            lexer->source_length - lexer->position, 
                                            &rune);
            if (bytes > 0) {
                buffer_rune(lexer, rune);
                advance_rune(lexer, bytes);
            } else {
                advance(lexer);
            }
        }
    }
    
    add_error(lexer, "L003", "نص غير مكتمل: علامة اقتباس بدون إغلاق");
    return make_error_token(lexer, "L003");
}

static Token read_character(Lexer* lexer) {
    start_token(lexer);
    advance(lexer); /* تجاوز علامة الاقتباس المفردة الأولى */
    
    if (lexer->position >= lexer->source_length) {
        add_error(lexer, "L003", "حرف غير مكتمل: علامة اقتباس بدون إغلاق");
        return make_error_token(lexer, "L003");
    }
    
    char c = current_char(lexer);
    if (c == '\\') {
        advance(lexer); /* تجاوز علامة backslash */
        c = current_char(lexer);
        switch (c) {
            case 'n': buffer_char(lexer, '\n'); break;
            case 't': buffer_char(lexer, '\t'); break;
            case '\\': buffer_char(lexer, '\\'); break;
            case '"': buffer_char(lexer, '"'); break;
            case '\'': buffer_char(lexer, '\''); break;
            case '0':
                /* \0: لا يمكن تمثيله في buffer العادي، نحفظ القيمة في literal */
                buffer_char(lexer, '\0');
                break;
            default:
                buffer_char(lexer, '\\');
                buffer_char(lexer, c);
                break;
        }
        advance(lexer); /* تجاوز الحرف المُهرب */
    } else {
        Rune rune;
        size_t bytes = unicode_read_rune(lexer->source + lexer->position,
                                        lexer->source_length - lexer->position,
                                        &rune);
        if (bytes > 0) {
            buffer_rune(lexer, rune);
            advance_rune(lexer, bytes);
        } else {
            buffer_char(lexer, c);
            advance(lexer);
        }
    }
    
    if (lexer->position >= lexer->source_length || current_char(lexer) != '\'') {
        add_error(lexer, "L003", "حرف غير مكتمل: علامة اقتباس بدون إغلاق");
        return make_error_token(lexer, "L003");
    }
    
    advance(lexer);
    
    Token token = make_token(lexer, TOKEN_CHARACTER);
    
    /* إذا كان الحرف \0، نحفظ القيمة في literal لأن buffer لا يمكنه تمثيلها */
    if (token.length == 0 || (token.value && token.value[0] == '\0')) {
        token.has_literal = 1;
        token.literal.int_value = 0;
    }
    
    return token;
}

static Token read_number(Lexer* lexer) {
    start_token(lexer);
    
    Rune first_rune = 0;
    size_t first_bytes = unicode_read_rune(lexer->source + lexer->position,
                                          lexer->source_length - lexer->position,
                                          &first_rune);
    
    /* فحص الأرقام العربية-الهندية */
    if (first_bytes > 0 && unicode_is_arabic_digit(first_rune)) {
        /* تحويل الأرقام العربية-الهندية إلى غربية */
        int has_digits = 0;
        while (lexer->position < lexer->source_length) {
            Rune r;
            size_t b = unicode_read_rune(lexer->source + lexer->position,
                                       lexer->source_length - lexer->position,
                                       &r);
            if (b > 0 && unicode_is_arabic_digit(r)) {
                char western = (char)unicode_to_western_digit(r);
                buffer_char(lexer, western);
                advance_rune(lexer, b);
                has_digits = 1;
            } else if (b > 0 && r == '_') {
                advance_rune(lexer, b);
            } else if (b > 0 && r == '.') {
                char next = peek_char(lexer, 1);
                if (next >= '0' && next <= '9') {
                    buffer_char(lexer, '.');
                    advance(lexer);
                } else {
                    break;
                }
            } else {
                break;
            }
        }
        
        if (!has_digits) {
            add_error(lexer, "L004", "رقم غير صالح");
            return make_error_token(lexer, "L004");
        }
        
        Token token = make_token(lexer, TOKEN_INTEGER);
        token.has_literal = 1;
        errno = 0;
        token.literal.int_value = strtoll(token.value, NULL, 10);
        if (errno == ERANGE) {
            add_error(lexer, "L011", "عدد صحيح أكبر من الحد المسموح");
        }
        return token;
    }
    
    /* التحقق من الأرقام غير العشرية */
    char first = current_char(lexer);
    if (first == '0') {
        char next = peek_char(lexer, 1);
        
        /* hex: 0xFF */
        if (next == 'x' || next == 'X') {
            buffer_char(lexer, '0');
            buffer_char(lexer, 'x');
            advance(lexer);
            advance(lexer);
            
            int has_digits = 0;
            while (lexer->position < lexer->source_length) {
                char c = current_char(lexer);
                if ((c >= '0' && c <= '9') || 
                    (c >= 'a' && c <= 'f') || 
                    (c >= 'A' && c <= 'F')) {
                    buffer_char(lexer, c);
                    advance(lexer);
                    has_digits = 1;
                } else if (c == '_') {
                    advance(lexer);
                } else {
                    break;
                }
            }
            
            if (!has_digits) {
                add_error(lexer, "L008", "عدد Hex غير صالح: لا توجد أرقام بعد 0x");
                return make_error_token(lexer, "L008");
            }
            
            Token token = make_token(lexer, TOKEN_INTEGER);
            token.has_literal = 1;
            errno = 0;
            token.literal.int_value = strtoll(token.value, NULL, 0);
            if (errno == ERANGE) {
                add_error(lexer, "L011", "عدد صحيح أكبر من الحد المسموح");
            }
            return token;
        }
        
        /* binary: 0b1010 */
        if (next == 'b' || next == 'B') {
            buffer_char(lexer, '0');
            buffer_char(lexer, 'b');
            advance(lexer);
            advance(lexer);
            
            int has_digits = 0;
            while (lexer->position < lexer->source_length) {
                char c = current_char(lexer);
                if (c == '0' || c == '1') {
                    buffer_char(lexer, c);
                    advance(lexer);
                    has_digits = 1;
                } else if (c == '_') {
                    advance(lexer);
                } else {
                    break;
                }
            }
            
            if (!has_digits) {
                add_error(lexer, "L009", "عدد Binary غير صالح: لا توجد أرقام بعد 0b");
                return make_error_token(lexer, "L009");
            }
            
            return make_token(lexer, TOKEN_INTEGER);
        }
        
        /* octal: 0o77 */
        if (next == 'o' || next == 'O') {
            buffer_char(lexer, '0');
            buffer_char(lexer, 'o');
            advance(lexer);
            advance(lexer);
            
            int has_digits = 0;
            while (lexer->position < lexer->source_length) {
                char c = current_char(lexer);
                if (c >= '0' && c <= '7') {
                    buffer_char(lexer, c);
                    advance(lexer);
                    has_digits = 1;
                } else {
                    break;
                }
            }
            
            if (!has_digits) {
                add_error(lexer, "L010", "عدد Octal غير صالح: لا توجد أرقام بعد 0o");
                return make_error_token(lexer, "L010");
            }
            
            return make_token(lexer, TOKEN_INTEGER);
        }
    }
    
    /* أرقام عشرية أو عشبية */
    int has_dot = 0;
    int has_exponent = 0;
    int has_digits = 0;
    
    while (lexer->position < lexer->source_length) {
        char c = current_char(lexer);
        
        if (c >= '0' && c <= '9') {
            buffer_char(lexer, c);
            advance(lexer);
            has_digits = 1;
        } else if (c == '_' ) {
            advance(lexer);
        } else if (c == '.' && !has_dot && !has_exponent) {
            char next = peek_char(lexer, 1);
            if (next >= '0' && next <= '9') {
                has_dot = 1;
                buffer_char(lexer, c);
                advance(lexer);
            } else {
                break;
            }
        } else if ((c == 'e' || c == 'E') && !has_exponent && has_digits) {
            has_exponent = 1;
            has_dot = 1; /*科学ي = عدد عشري */
            buffer_char(lexer, c);
            advance(lexer);
            
            /* فحص الإشارة الاختيارية */
            if (lexer->position < lexer->source_length) {
                c = current_char(lexer);
                if (c == '+' || c == '-') {
                    buffer_char(lexer, c);
                    advance(lexer);
                }
            }
            
            /* يجب أن يلي至少 رقم واحد بعد e */
            int has_exp_digits = 0;
            while (lexer->position < lexer->source_length) {
                c = current_char(lexer);
                if (c >= '0' && c <= '9') {
                    buffer_char(lexer, c);
                    advance(lexer);
                    has_exp_digits = 1;
                } else if (c == '_') {
                    advance(lexer);
                } else {
                    break;
                }
            }
            
            if (!has_exp_digits) {
                add_error(lexer, "L012", "عدد علمي غير صالح: لا توجد أرقام بعد e");
                return make_error_token(lexer, "L012");
            }
        } else {
            break;
        }
    }
    
    if (!has_digits) {
        add_error(lexer, "L004", "رقم غير صالح");
        return make_error_token(lexer, "L004");
    }
    
    TokenType type = has_dot ? TOKEN_FLOAT : TOKEN_INTEGER;
    Token token = make_token(lexer, type);
    
    /* تحويل القيمة */
    token.has_literal = 1;
    if (type == TOKEN_INTEGER) {
        errno = 0;
        token.literal.int_value = strtoll(token.value, NULL, 0);
        if (errno == ERANGE) {
            add_error(lexer, "L011", "عدد صحيح أكبر من الحد المسموح");
        }
    } else {
        errno = 0;
        token.literal.float_value = strtod(token.value, NULL);
        if (errno == ERANGE) {
            add_error(lexer, "L013", "عدد عشري أكبر من الحد المسموح");
        }
    }
    
    return token;
}

static Token read_identifier_or_keyword(Lexer* lexer) {
    start_token(lexer);
    
    Rune rune;
    size_t bytes = unicode_read_rune(lexer->source + lexer->position,
                                    lexer->source_length - lexer->position,
                                    &rune);
    
    if (bytes == 0 || !unicode_is_identifier_start(rune)) {
        return make_error_token(lexer, "معرف غير صالح");
    }
    
    buffer_rune(lexer, rune);
    advance_rune(lexer, bytes);
    
    while (lexer->position < lexer->source_length) {
        bytes = unicode_read_rune(lexer->source + lexer->position,
                                lexer->source_length - lexer->position,
                                &rune);
        
        if (bytes == 0 || !unicode_is_identifier_char(rune)) break;
        
        buffer_rune(lexer, rune);
        advance_rune(lexer, bytes);
    }
    
    /* البحث في الكلمات المفتاحية */
    TokenType keyword_type = keywords_lookup(lexer->buffer, lexer->buffer_length);
    if (keyword_type != TOKEN_IDENTIFIER) {
        return make_token(lexer, keyword_type);
    }
    
    return make_token(lexer, TOKEN_IDENTIFIER);
}

/* ===== الواجهة العامة ===== */

/**
 * @brief إنشاء Lexer من نص
 * 
 * Ownership: لا يملك مصدر النص (source). caller مسؤول عن بقاءه.
 */
Lexer* lexer_create(const char* source, const char* filename) {
    if (!source) return NULL;
    
    Lexer* lexer = (Lexer*)calloc(1, sizeof(Lexer));
    if (!lexer) return NULL;
    
    lexer->source = source;
    lexer->source_length = strlen(source);
    lexer->filename = filename ? strdup(filename) : strdup("<stdin>");
    lexer->source_owned = 0;
    
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    
    lexer->skip_whitespace = 1;
    lexer->skip_comments = 1;
    
    return lexer;
}

/**
 * @brief إنشاء Lexer من ملف
 * 
 * Ownership: يملك مصدر النص (يُحرر في lexer_destroy).
 */
Lexer* lexer_create_from_file(FILE* file, const char* filename) {
    if (!file) return NULL;
    
    /* قراءة الملف بالكامل */
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (size <= 0) return NULL;
    
    char* source = (char*)malloc((size_t)size + 1);
    if (!source) return NULL;
    
    size_t read = fread(source, 1, (size_t)size, file);
    source[read] = '\0';
    
    Lexer* lexer = lexer_create(source, filename);
    if (!lexer) {
        free(source);
        return NULL;
    }
    
    /* الـ Lexer يملك مصدر النص الآن */
    lexer->source_owned = 1;
    
    return lexer;
}

/**
 * @brief تحرير ذاكرة الـ Lexer
 * 
 * Ownership: يحرر filename وsource (إذا كان مملوكاً).
 */
void lexer_destroy(Lexer* lexer) {
    if (!lexer) return;
    
    if (lexer->filename) {
        free((void*)lexer->filename);
    }
    
    if (lexer->source_owned && lexer->source) {
        free((void*)lexer->source);
    }
    
    free(lexer);
}

Token lexer_next_token(Lexer* lexer) {
    if (!lexer) {
        Token error = {0};
        error.type = TOKEN_ERROR;
        return error;
    }
    
    /* تجاهل المسافات والتعليقات */
    if (lexer->skip_whitespace) {
        skip_whitespace(lexer);
    }
    
    /* التحقق من نهاية الملف */
    if (lexer->position >= lexer->source_length) {
        Token eof = {0};
        eof.type = TOKEN_EOF;
        eof.value = NULL;
        eof.length = 0;
        eof.start.line = lexer->line;
        eof.start.column = lexer->column;
        eof.start.offset = lexer->position;
        eof.end = eof.start;
        eof.filename = lexer->filename;
        return eof;
    }
    
    char c = current_char(lexer);
    char next = peek_char(lexer, 1);
    
    /* نهاية السطر */
    if (c == '\n') {
        start_token(lexer);
        advance(lexer);
        return make_token(lexer, TOKEN_NEWLINE);
    }
    
    /* التعليقات */
    if (c == '#') {
        if (next == '[') {
            /* تعليق متعدد الأسطر */
            if (lexer->skip_comments) {
                start_token(lexer);
                advance(lexer);
                advance(lexer);
                skip_multi_line_comment(lexer);
                return lexer_next_token(lexer);
            } else {
                start_token(lexer);
                advance(lexer);
                advance(lexer);
                while (lexer->position < lexer->source_length) {
                    if (current_char(lexer) == ']' && peek_char(lexer, 1) == '#') {
                        advance(lexer);
                        advance(lexer);
                        break;
                    }
                    advance(lexer);
                }
                return make_token(lexer, TOKEN_COMMENT_MULTI_START);
            }
        } else {
            /* تعليق سطر واحد */
            if (lexer->skip_comments) {
                start_token(lexer);
                skip_single_line_comment(lexer);
                return lexer_next_token(lexer);
            } else {
                start_token(lexer);
                skip_single_line_comment(lexer);
                return make_token(lexer, TOKEN_COMMENT_SINGLE);
            }
        }
    }
    
    /* النصوص */
    if (c == '"') {
        return read_string(lexer);
    }
    
    /* الأحرف */
    if (c == '\'') {
        return read_character(lexer);
    }
    
    /* الأرقام */
    if (c >= '0' && c <= '9') {
        return read_number(lexer);
    }
    
    /* الأرقام العربية-الهندية والفاصلة العربية */
    {
        Rune rune;
        size_t bytes = unicode_read_rune(lexer->source + lexer->position,
                                        lexer->source_length - lexer->position,
                                        &rune);
        if (bytes > 0) {
            /* أرقام عربية-هندية */
            if (unicode_is_arabic_digit(rune)) {
                return read_number(lexer);
            }
            /* الفاصلة المنقوطة العربية ؛ (U+061B) */
            if (rune == 0x061B) {
                start_token(lexer);
                buffer_rune(lexer, rune);
                advance_rune(lexer, bytes);
                return make_token(lexer, TOKEN_SEMICOLON);
            }
            /* الفاصلة العربية ، (U+060C) */
            if (rune == 0x060C) {
                start_token(lexer);
                buffer_rune(lexer, rune);
                advance_rune(lexer, bytes);
                return make_token(lexer, TOKEN_COMMA);
            }
        }
    }
    
    /* المعرفات والكلمات المفتاحية */
    {
        Rune rune;
        size_t bytes = unicode_read_rune(lexer->source + lexer->position,
                                        lexer->source_length - lexer->position,
                                        &rune);
        if (bytes > 0 && unicode_is_identifier_start(rune)) {
            return read_identifier_or_keyword(lexer);
        }
    }
    
    /* الرموز البسيطة */
    start_token(lexer);
    
    /* السهمان */
    if (c == '-' && next == '>') {
        buffer_char(lexer, '-');
        buffer_char(lexer, '>');
        advance(lexer);
        advance(lexer);
        return make_token(lexer, TOKEN_ARROW);
    }
    
    if (c == ':' && next == '-') {
        buffer_char(lexer, ':');
        buffer_char(lexer, '-');
        advance(lexer);
        advance(lexer);
        return make_token(lexer, TOKEN_COLON_ARROW);
    }
    
    /* المعاملات المزدوجة */
    if (c == '=' && next == '=') {
        buffer_char(lexer, '=');
        buffer_char(lexer, '=');
        advance(lexer);
        advance(lexer);
        return make_token(lexer, TOKEN_EQUAL);
    }
    
    if (c == '!' && next == '=') {
        buffer_char(lexer, '!');
        buffer_char(lexer, '=');
        advance(lexer);
        advance(lexer);
        return make_token(lexer, TOKEN_NOT_EQUAL);
    }
    
    if (c == '>' && next == '=') {
        buffer_char(lexer, '>');
        buffer_char(lexer, '=');
        advance(lexer);
        advance(lexer);
        return make_token(lexer, TOKEN_GREATER_EQUAL);
    }
    
    if (c == '<' && next == '=') {
        buffer_char(lexer, '<');
        buffer_char(lexer, '=');
        advance(lexer);
        advance(lexer);
        return make_token(lexer, TOKEN_LESS_EQUAL);
    }
    
    if (c == '&' && next == '&') {
        buffer_char(lexer, '&');
        buffer_char(lexer, '&');
        advance(lexer);
        advance(lexer);
        return make_token(lexer, TOKEN_AND);
    }
    
    if (c == '|' && next == '|') {
        buffer_char(lexer, '|');
        buffer_char(lexer, '|');
        advance(lexer);
        advance(lexer);
        return make_token(lexer, TOKEN_OR);
    }
    
    if (c == '<' && next == '<') {
        buffer_char(lexer, '<');
        buffer_char(lexer, '<');
        advance(lexer);
        advance(lexer);
        if (current_char(lexer) == '=') {
            buffer_char(lexer, '=');
            advance(lexer);
            return make_token(lexer, TOKEN_SHIFT_LEFT_ASSIGN);
        }
        return make_token(lexer, TOKEN_SHIFT_LEFT);
    }
    
    if (c == '>' && next == '>') {
        buffer_char(lexer, '>');
        buffer_char(lexer, '>');
        advance(lexer);
        advance(lexer);
        if (current_char(lexer) == '=') {
            buffer_char(lexer, '=');
            advance(lexer);
            return make_token(lexer, TOKEN_SHIFT_RIGHT_ASSIGN);
        }
        return make_token(lexer, TOKEN_SHIFT_RIGHT);
    }
    
    if (c == '+' && next == '=') {
        buffer_char(lexer, '+');
        buffer_char(lexer, '=');
        advance(lexer);
        advance(lexer);
        return make_token(lexer, TOKEN_PLUS_ASSIGN);
    }
    
    if (c == '-' && next == '=') {
        buffer_char(lexer, '-');
        buffer_char(lexer, '=');
        advance(lexer);
        advance(lexer);
        return make_token(lexer, TOKEN_MINUS_ASSIGN);
    }
    
    if (c == '*' && next == '=') {
        buffer_char(lexer, '*');
        buffer_char(lexer, '=');
        advance(lexer);
        advance(lexer);
        return make_token(lexer, TOKEN_STAR_ASSIGN);
    }
    
    if (c == '/' && next == '=') {
        buffer_char(lexer, '/');
        buffer_char(lexer, '=');
        advance(lexer);
        advance(lexer);
        return make_token(lexer, TOKEN_SLASH_ASSIGN);
    }
    
    if (c == '%' && next == '=') {
        buffer_char(lexer, '%');
        buffer_char(lexer, '=');
        advance(lexer);
        advance(lexer);
        return make_token(lexer, TOKEN_PERCENT_ASSIGN);
    }
    
    /* المعاملات البسيطة */
    buffer_char(lexer, c);
    advance(lexer);
    
    switch (c) {
        case '+': return make_token(lexer, TOKEN_PLUS);
        case '-': return make_token(lexer, TOKEN_MINUS);
        case '*': return make_token(lexer, TOKEN_STAR);
        case '/': return make_token(lexer, TOKEN_SLASH);
        case '%': return make_token(lexer, TOKEN_PERCENT);
        case '=': return make_token(lexer, TOKEN_ASSIGN);
        case '!': return make_token(lexer, TOKEN_NOT);
        case '>': return make_token(lexer, TOKEN_GREATER);
        case '<': return make_token(lexer, TOKEN_LESS);
        case '&': return make_token(lexer, TOKEN_BITWISE_AND);
        case '|': return make_token(lexer, TOKEN_BITWISE_OR);
        case '^': return make_token(lexer, TOKEN_BITWISE_XOR);
        case '~': return make_token(lexer, TOKEN_BITWISE_NOT);
        case ':': return make_token(lexer, TOKEN_COLON);
        case ',': return make_token(lexer, TOKEN_COMMA);
        case '.': return make_token(lexer, TOKEN_DOT);
        case '@': return make_token(lexer, TOKEN_AT);
        case '(': return make_token(lexer, TOKEN_LPAREN);
        case ')': return make_token(lexer, TOKEN_RPAREN);
        case '{': return make_token(lexer, TOKEN_LBRACE);
        case '}': return make_token(lexer, TOKEN_RBRACE);
        case '[': return make_token(lexer, TOKEN_LBRACKET);
        case ']': return make_token(lexer, TOKEN_RBRACKET);
        default: break;
    }
    
    /* رمز غير معروف */
    add_error(lexer, "L006", "رمز غير معروف في اللغة");
    return make_error_token(lexer, "L006");
}

/**
 * @brief الاطلاع على التوكن التالي دون أخذها
 * 
 * يحفظ ويستعيد كامل الحالة الداخلية.
 * Ownership: القيمة المُعادة مملوكة للمستدعي (يجب تحريرها عبر token_destroy).
 */
Token lexer_peek_token(Lexer* lexer) {
    if (!lexer) {
        Token error = {0};
        error.type = TOKEN_ERROR;
        return error;
    }
    
    /* حفظ كامل الحالة */
    size_t saved_position = lexer->position;
    size_t saved_line = lexer->line;
    size_t saved_column = lexer->column;
    size_t saved_token_start = lexer->token_start;
    size_t saved_token_start_line = lexer->token_start_line;
    size_t saved_token_start_column = lexer->token_start_column;
    size_t saved_buffer_length = lexer->buffer_length;
    size_t saved_error_count = lexer->error_count;
    int saved_has_errors = lexer->has_errors;
    
    Token token = lexer_next_token(lexer);
    
    /* استعادة كامل الحالة */
    lexer->position = saved_position;
    lexer->line = saved_line;
    lexer->column = saved_column;
    lexer->token_start = saved_token_start;
    lexer->token_start_line = saved_token_start_line;
    lexer->token_start_column = saved_token_start_column;
    lexer->buffer_length = saved_buffer_length;
    lexer->error_count = saved_error_count;
    lexer->has_errors = saved_has_errors;
    
    return token;
}

/**
 * @brief تحليل جميع الـ Tokens
 * 
 * Ownership: يُرجع مصفوفة Tokens تملك value عبر strdup.
 * المستدعي مسؤول عن تحرير كل token عبر token_destroy ثم free للمصفوفة.
 */
Token* lexer_tokenize(Lexer* lexer, size_t* count) {
    if (!lexer || !count) return NULL;
    
    size_t capacity = 256;
    size_t size = 0;
    Token* tokens = (Token*)malloc(capacity * sizeof(Token));
    
    if (!tokens) return NULL;
    
    while (1) {
        Token token = lexer_next_token(lexer);
        
        if (size >= capacity) {
            size_t new_capacity = capacity * 2;
            Token* new_tokens = (Token*)realloc(tokens, new_capacity * sizeof(Token));
            if (!new_tokens) {
                /* تحرير جميع الـ Tokens المحفوظة قبل الإخراج */
                for (size_t i = 0; i < size; i++) {
                    token_destroy(&tokens[i]);
                }
                free(tokens);
                token_destroy(&token); /* تحرير الـ Token الحالية */
                *count = 0;
                return NULL;
            }
            tokens = new_tokens;
            capacity = new_capacity;
        }
        
        tokens[size++] = token;
        
        if (token.type == TOKEN_EOF) break;
    }
    
    *count = size;
    return tokens;
}

void lexer_reset(Lexer* lexer) {
    if (!lexer) return;
    
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->buffer_length = 0;
    lexer->error_count = 0;
    lexer->has_errors = 0;
}

int lexer_has_errors(const Lexer* lexer) {
    return lexer ? lexer->has_errors : 0;
}

size_t lexer_error_count(const Lexer* lexer) {
    return lexer ? lexer->error_count : 0;
}

const LexerError* lexer_get_error(const Lexer* lexer, size_t index) {
    if (!lexer || index >= lexer->error_count) return NULL;
    return &lexer->errors[index];
}

void lexer_print_errors(const Lexer* lexer, FILE* file) {
    if (!lexer || !file) return;
    
    for (size_t i = 0; i < lexer->error_count; i++) {
        const LexerError* error = &lexer->errors[i];
        fprintf(file, "%s:%zu:%zu: %s: %s\n",
                lexer->filename ? lexer->filename : "<stdin>",
                error->position.line,
                error->position.column,
                error->code,
                error->message);
    }
}
