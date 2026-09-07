/**
 * @file keywords.h
 * @brief جدول البحث عن الكلمات المفتاحية في لغة ض Core
 * 
 * هذا الملف يحتوي على واجهة جدول البحث السريع للكلمات المفتاحية.
 * جميع الكلمات مأخوذة من docs/keywords.md.
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#ifndef DAAD_KEYWORDS_H
#define DAAD_KEYWORDS_H

#include "token/token_types.h"
#include <stddef.h>

/**
 * @brief بنية تمثل كلمة مفتاحية
 */
typedef struct {
    const char* text;       /* النص العربي */
    TokenType type;         /* نوع الـ Token المقابل */
    size_t length;          /* طول النص (بالبايت) */
} KeywordEntry;

/**
 * @brief البحث عن كلمة مفتاحية
 * @param text النص المراد البحث عنه
 * @param length طول النص
 * @return نوع الـ Token إذا وُجد، TOKEN_IDENTIFIER إذا لم يُوجد
 */
TokenType keywords_lookup(const char* text, size_t length);

/**
 * @brief التحقق من أن النص كلمة مفتاحية
 * @param text النص
 * @param length طول النص
 * @return 1 إذا كان كلمة مفتاحية، 0 خلاف ذلك
 */
int keywords_is_keyword(const char* text, size_t length);

#endif /* DAAD_KEYWORDS_H */
