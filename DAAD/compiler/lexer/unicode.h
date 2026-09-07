/**
 * @file unicode.h
 * @brief أدوات Unicode للمعالجة في لغة ض Core
 * 
 * هذا الملف يحتوي على دوال معالجة Unicode المطلوبة لـ Lexer.
 * يدعم العربية والإنجليزية والأرقام العربية-الهندسية.
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#ifndef DAAD_UNICODE_H
#define DAAD_UNICODE_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief رمز Unicode
 */
typedef uint32_t Rune;

/**
 * @brief الفئات المشفرة
 */
typedef enum {
    CHAR_CATEGORY_NONE,          /* غير مصنف */
    CHAR_CATEGORY_DIGIT,         /* رقم غربي: 0-9 */
    CHAR_CATEGORY_ARABIC_DIGIT,  /* رقم عربي-هندي: ٠-٩ */
    CHAR_CATEGORY_LATIN_LOWER,   /* حرف إنجليزي صغير: a-z */
    CHAR_CATEGORY_LATIN_UPPER,   /* حرف إنجليزي كبير: A-Z */
    CHAR_CATEGORY_ARABIC,        /* حرف عربي */
    CHAR_CATEGORY_UNDERSCORE,    /* شرطة سفلية: _ */
    CHAR_CATEGORY_DOT,           /* نقطة: . */
    CHAR_CATEGORY_COLON,         /* نقطتين: : */
    CHAR_CATEGORY_SEMICOLON,     /* فاصلة منقوطة عربية: ؛ */
    CHAR_CATEGORY_COMMA,         /* فاصلة: , */
    CHAR_CATEGORY_LPAREN,        /* قوس يسار: ( */
    CHAR_CATEGORY_RPAREN,        /* قوس يمين: ) */
    CHAR_CATEGORY_LBRACE,        /* قوس يسار معقوف: { */
    CHAR_CATEGORY_RBRACE,        /* قوس يمين معقوف: } */
    CHAR_CATEGORY_LBRACKET,      /* قوس يسار مربع: [ */
    CHAR_CATEGORY_RBRACKET,      /* قوس يمين مربع: ] */
    CHAR_CATEGORY_PLUS,          /* علامة زائد: + */
    CHAR_CATEGORY_MINUS,         /* علامة سالب: - */
    CHAR_CATEGORY_STAR,          /* علامة ضرب: * */
    CHAR_CATEGORY_SLASH,         /* علامة قسمة: / */
    CHAR_CATEGORY_PERCENT,       /* علامة باقي: % */
    CHAR_CATEGORY_ASSIGN,        /* علامة تعيين: = */
    CHAR_CATEGORY_NOT,           /* علامة نفي: ! */
    CHAR_CATEGORY_GREATER,       /* أكبر من: > */
    CHAR_CATEGORY_LESS,          /* أصغر من: < */
    CHAR_CATEGORY_AND,           /* علامة و: & */
    CHAR_CATEGORY_OR,            /* علامة أو: | */
    CHAR_CATEGORY_XOR,           /* علامة XOR: ^ */
    CHAR_CATEGORY_TILDE,         /* علامة تيلد: ~ */
    CHAR_CATEGORY_AT,            /* علامة arobase: @ */
    CHAR_CATEGORY_HASH,          /* علامة تجديف: # */
    CHAR_CATEGORY_SINGLE_QUOTE,  /* اقتباس مفرد: ' */
    CHAR_CATEGORY_DOUBLE_QUOTE,  /* اقتباس مزدوج: " */
    CHAR_CATEGORY_NEWLINE,       /* نهاية سطر: \n */
    CHAR_CATEGORY_SPACE,         /* مسافة */
    CHAR_CATEGORY_TAB,           /* تبويب: \t */
    CHAR_CATEGORY_OTHER          /* حرف آخر */
} CharCategory;

/**
 * @brief قراءة رمز Unicode من نص UTF-8
 * @param str النص
 * @param length طول النص بالبايت
 * @param[out] rune الرمز المُقرأ
 * @return عدد البايتات المُقروءة، 0 إذا كان الخطأ
 */
size_t unicode_read_rune(const char* str, size_t length, Rune* rune);

/**
 * @brief كتابة رمز Unicode كنص UTF-8
 * @param rune الرمز
 * @param buffer المخزن
 * @param buffer_size حجم المخزن
 * @return عدد البايتات المكتوبة
 */
size_t unicode_write_rune(Rune rune, char* buffer, size_t buffer_size);

/**
 * @brief الحصول على فئة الحرف
 * @param rune الرمز
 * @return فئة الحرف
 */
CharCategory unicode_category(Rune rune);

/**
 * @brief التحقق من أن الحرف حرف عربي
 * @param rune الرمز
 * @return 1 إذا كان عربياً، 0 خلاف ذلك
 */
int unicode_is_arabic(Rune rune);

/**
 * @brief التحقق من أن الحرف حرف إنجليزي
 * @param rune الرمز
 * @return 1 إذا كان إنجليزياً، 0 خلاف ذلك
 */
int unicode_is_latin(Rune rune);

/**
 * @brief التحقق من أن الحرف رقم
 * @param rune الرمز
 * @return 1 إذا كان رقماً، 0 خلاف ذلك
 */
int unicode_is_digit(Rune rune);

/**
 * @brief التحقق من أن الحرف رقم عربي-هندي
 * @param rune الرمز
 * @return 1 إذا كان رقماً عربياً، 0 خلاف ذلك
 */
int unicode_is_arabic_digit(Rune rune);

/**
 * @brief التحقق من أن الحرف حرف أو رقم أو شرطة سفلية
 * @param rune الرمز
 * @return 1 إذا كان صالحاً لاسم معرف، 0 خلاف ذلك
 */
int unicode_is_identifier_char(Rune rune);

/**
 * @brief التحقق من أن الحرف في بداية معرف صالح
 * @param rune الرمز
 * @return 1 إذا كان صالحاً لبداية معرف، 0 خلاف ذلك
 */
int unicode_is_identifier_start(Rune rune);

/**
 * @brief تحويل رقم عربي-هندي إلى غربي
 * @param rune الرمز العربي
 * @return الرمز الغربي المقابل، أو rune إذا لم يكن عربياً
 */
Rune unicode_to_western_digit(Rune rune);

/**
 * @brief الحصول على حجم البايت لرمز UTF-8
 * @param first_byte أول بايت
 * @return عدد البايتات (1-4)
 */
size_t unicode_utf8_byte_count(uint8_t first_byte);

/**
 * @brief التحقق من صحة ترميز UTF-8
 * @param str النص
 * @param length طول النص
 * @return 1 إذا كان صالحاً، 0 خلاف ذلك
 */
int unicode_validate_utf8(const char* str, size_t length);

#endif /* DAAD_UNICODE_H */
