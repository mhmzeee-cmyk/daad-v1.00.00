/**
 * @file lexer.h
 * @brief المحلل البصري (Lexer) لغة ض Core
 * 
 * هذا الملف يحتوي على واجهة Lexer المسؤول عن تحويل النص إلى Tokens.
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#ifndef DAAD_LEXER_H
#define DAAD_LEXER_H

#include "token/token.h"
#include <stddef.h>
#include <stdio.h>

/**
 * @brief الحد الأقصى لعدد الأخطاء
 */
#define LEXER_MAX_ERRORS 100

/**
 * @brief الحد الأقصى لطول النص المُخزّن مؤقتاً
 */
#define LEXER_MAX_BUFFER 1024

/**
 * @brief بنية الخطأ
 */
typedef struct {
    char code[8];           /* كود الخطأ: L001, L002, إلخ */
    char message[256];      /* رسالة الخطأ */
    Position position;      /* موقع الخطأ */
} LexerError;

/**
 * @brief بنية المحلل البصري
 */
typedef struct {
    /* المدخل */
    const char* source;         /* النص المصدري */
    size_t source_length;       /* طول النص */
    const char* filename;       /* اسم الملف */
    int source_owned;           /* هل يملك الـ Lexer مصدر النص؟ (يُحرر في lexer_destroy) */
    
    /* الحالة الحالية */
    size_t position;            /* الموقع الحالي بالبايت */
    size_t line;                /* رقم السطر الحالي */
    size_t column;              /* رقم العمود الحالي */
    size_t token_start;         /* بداية الـ Token الحالية */
    size_t token_start_line;    /* سطر بداية الـ Token */
    size_t token_start_column;  /* عمود بداية الـ Token */
    
    /* المخزن المؤقت */
    char buffer[LEXER_MAX_BUFFER];
    size_t buffer_length;
    
    /* الأخطاء */
    LexerError errors[LEXER_MAX_ERRORS];
    size_t error_count;
    int has_errors;
    
    /* خيارات */
    int skip_whitespace;        /* تجاهل المسافات */
    int skip_comments;          /* تجاهل التعليقات */
} Lexer;

/**
 * @brief إنشاء Lexer جديد من نص
 * @param source النص المصدري
 * @param filename اسم الملف (اختياري)
 * @return مؤشر إلى الـ Lexer الجديد
 */
Lexer* lexer_create(const char* source, const char* filename);

/**
 * @brief إنشاء Lexer جديد من ملف
 * @param file مؤشر إلى الملف المفتوح
 * @param filename اسم الملف
 * @return مؤشر إلى الـ Lexer الجديد
 */
Lexer* lexer_create_from_file(FILE* file, const char* filename);

/**
 * @brief تحرير ذاكرة الـ Lexer
 * @param lexer مؤشر إلى الـ Lexer
 */
void lexer_destroy(Lexer* lexer);

/**
 * @brief الحصول على التوكن التالي
 * @param lexer مؤشر إلى الـ Lexer
 * @return الـ Token التالية
 */
Token lexer_next_token(Lexer* lexer);

/**
 * @brief الاطلاع على التوكن التالية بدون أخذها
 * @param lexer مؤشر إلى الـ Lexer
 * @return الـ Token التالية
 */
Token lexer_peek_token(Lexer* lexer);

/**
 * @brief تحليل جميع الـ Tokens
 * @param lexer مؤشر إلى الـ Lexer
 * @return مصفوفة من الـ Tokens (يجب تحريرها لاحقاً)
 */
Token* lexer_tokenize(Lexer* lexer, size_t* count);

/**
 * @brief إعادة تعيين الـ Lexer
 * @param lexer مؤشر إلى الـ Lexer
 */
void lexer_reset(Lexer* lexer);

/**
 * @brief التحقق من وجود أخطاء
 * @param lexer مؤشر إلى الـ Lexer
 * @return 1 إذا وُجدت أخطاء، 0 خلاف ذلك
 */
int lexer_has_errors(const Lexer* lexer);

/**
 * @brief الحصول على عدد الأخطاء
 * @param lexer مؤشر إلى الـ Lexer
 * @return عدد الأخطاء
 */
size_t lexer_error_count(const Lexer* lexer);

/**
 * @brief الحصول على خطأ معين
 * @param lexer مؤشر إلى الـ Lexer
 * @param index فهرس الخطأ
 * @return مؤشر إلى الخطأ، أو NULL إذا كان الفهرس غير صالح
 */
const LexerError* lexer_get_error(const Lexer* lexer, size_t index);

/**
 * @brief طباعة جميع الأخطاء
 * @param lexer مؤشر إلى الـ Lexer
 * @param file ملف الإخراج
 */
void lexer_print_errors(const Lexer* lexer, FILE* file);

#endif /* DAAD_LEXER_H */
