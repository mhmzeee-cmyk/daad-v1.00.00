/**
 * @file parser_recovery.c
 * @brief استرداد الأخطاء في وضع الذعر (Panic Mode Recovery)
 *
 * @version 0.1.0
 * @date 2026-08-03
 */

#include "parser_internal.h"

/**
 * @brief التحقق من أن الـ Token الحالية تشكل نقطة تزامن
 */
static int is_synchronization_point(Parser* p) {
    Token* tok = parser_current(p);

    switch (tok->type) {
        case TOKEN_KEYWORD_END:       /* نهاية */
        case TOKEN_SEMICOLON:         /* ؛ */
        case TOKEN_NEWLINE:
        case TOKEN_EOF:
        case TOKEN_KEYWORD_FUNCTION:  /* دالة */
        case TOKEN_KEYWORD_FORWARD:   /* أمامية */
        case TOKEN_KEYWORD_VARIABLE:  /* متغير */
        case TOKEN_KEYWORD_CONSTANT:  /* ثابت */
        case TOKEN_KEYWORD_IF:        /* إذا */
        case TOKEN_KEYWORD_WHILE:     /* طالما */
        case TOKEN_KEYWORD_FOR:       /* لكل */
        case TOKEN_KEYWORD_RETURN:    /* ارجع */
        case TOKEN_KEYWORD_EXIT:      /* توقف */
        case TOKEN_KEYWORD_PRINT:     /* اطبع */
        case TOKEN_KEYWORD_INPUT:     /* أدخل */
            return 1;
        default:
            return 0;
    }
}

/**
 * @brief التقدم وتخطي الـ Tokens حتى الوصول لنقطة تزامن
 *
 * يُستخدم عند حدوث خطأ لمواصلة التحليل بأقل الأضرار.
 * يتجاوز الـ Tokens حتى يجد:
 *   - نهاية سطر (TOKEN_NEWLINE)
 *   - فاصلة منقوطة (TOKEN_SEMICOLON)
 *   - كلمة نهاية (TOKEN_KEYWORD_END)
 *   - بداية تعليمة جديدة (دالة، متغير، ثابت، إذا، طالما، لكل، إلخ)
 */
void parser_synchronize(Parser* p) {
    if (!p) return;

    p->panic_mode = 0;

    while (!parser_is_at_end(p)) {
        Token* tok = parser_current(p);

        /* إذا كانت الـ Token الحالية نقطة تزامن، نتوقف */
        if (is_synchronization_point(p)) {
            /* إذا كانت كلمة نهاية أو دالة، نتوقف هنا */
            if (tok->type == TOKEN_KEYWORD_END ||
                tok->type == TOKEN_KEYWORD_FUNCTION ||
                tok->type == TOKEN_KEYWORD_FORWARD ||
                tok->type == TOKEN_KEYWORD_VARIABLE ||
                tok->type == TOKEN_KEYWORD_CONSTANT) {
                return;
            }
            /* إذا كانت ؛ أو newline أو EOF، نتقدم واحدة وتوقف */
            parser_advance(p);
            return;
        }

        parser_advance(p);
    }
}
