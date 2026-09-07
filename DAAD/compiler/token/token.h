/**
 * @file token.h
 * @brief تعريف بنية الـ Token في لغة ض Core
 * 
 * هذا الملف يحتوي على بنية Token التي تمثل رمزاً واحداً من رموز اللغة.
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#ifndef DAAD_TOKEN_H
#define DAAD_TOKEN_H

#include "token_types.h"
#include <stddef.h>

/**
 * @brief بنية تمثل موقع في الملف
 */
typedef struct {
    size_t line;        /* رقم السطر (يبدأ من 1) */
    size_t column;      /* رقم العمود (يبدأ من 1) */
    size_t offset;      /* الإزاحة من بداية الملف (بالبايت) */
} Position;

/**
 * @brief بنية تمثل Token واحدة
 */
typedef struct {
    TokenType type;             /* نوع الـ Token */
    char* value;                /* القيمة النصية الأصلية */
    size_t length;              /* طول القيمة (بالأحرف) */
    
    Position start;             /* الموقع البداية */
    Position end;               /* الموقع النهاية */
    
    const char* filename;       /* اسم الملف */
    
    /* بيانات إضافية للأرقام */
    union {
        long long int_value;    /* قيمة عدد صحيح */
        double float_value;     /* قيمة عدد عشري */
    } literal;
    
    int has_literal;            /* هل يحتوي على قيمة حرفية؟ */
} Token;

/**
 * @brief إنشاء Token جديدة
 * @param type نوع الـ Token
 * @param value القيمة النصية
 * @param length طول القيمة
 * @return Token جديدة
 */
Token token_create(TokenType type, const char* value, size_t length);

/**
 * @brief تحرير ذاكرة الـ Token
 * @param token مؤشر إلى الـ Token
 */
void token_destroy(Token* token);

/**
 * @brief نسخ Token
 * @param src الـ Token المصدر
 * @return نسخة جديدة من الـ Token
 */
Token token_copy(const Token* src);

/**
 * @brief التحقق من أن الـ Token لها نوع محدد
 * @param token الـ Token
 * @param type النوع المطلوب
 * @return 1 إذا كان النوع مطابقاً، 0 خلاف ذلك
 */
int token_is_type(const Token* token, TokenType type);

/**
 * @brief التحقق من أن الـ Token تطابق نصاً معيناً
 * @param token الـ Token
 * @param text النص المطلوب
 * @return 1 إذا كان النص مطابقاً، 0 خلاف ذلك
 */
int token_is_value(const Token* token, const char* text);

#endif /* DAAD_TOKEN_H */
