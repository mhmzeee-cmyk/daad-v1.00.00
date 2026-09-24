/**
 * @file parser_error.h
 * @brief تعريف أنواع الأخطاء في المحلل النحوي (Parser) لغة ض Core
 *
 * @version 0.1.0
 * @date 2026-08-03
 */

#ifndef DAAD_PARSER_ERROR_H
#define DAAD_PARSER_ERROR_H

#include <stddef.h>
#include <stdio.h>

/**
 * @brief بنية الخطأ في التحليل النحوي
 */
typedef struct {
    const char* code;       /* كود الخطأ: P001, P002, إلخ */
    char* message;          /* رسالة الخطأ */
    size_t line;            /* رقم السطر */
    size_t column;          /* رقم العمود */
    const char* filename;   /* اسم الملف */
} ParserError;

/**
 * @brief قائمة أخطاء التحليل النحوي
 */
typedef struct {
    ParserError** errors;   /* مصفوفة مؤشرات الأخطاء */
    int count;              /* عدد الأخطاء الحالي */
    int capacity;           /* السعة الحالية */
} ParserErrorList;

/**
 * @brief تهيئة قائمة الأخطاء
 * @param list مؤشر إلى القائمة
 */
void parser_error_list_init(ParserErrorList* list);

/**
 * @brief إضافة خطأ إلى القائمة
 * @param list مؤشر إلى القائمة
 * @param code كود الخطأ
 * @param msg رسالة الخطأ
 * @param line رقم السطر
 * @param col رقم العمود
 * @param fn اسم الملف
 */
void parser_error_list_add(ParserErrorList* list, const char* code,
                           const char* msg, size_t line, size_t col,
                           const char* fn);

/**
 * @brief تحرير ذاكرة القائمة
 * @param list مؤشر إلى القائمة
 */
void parser_error_list_destroy(ParserErrorList* list);

/**
 * @brief طباعة خطأ واحد
 * @param err مؤشر إلى الخطأ
 * @param file ملف الإخراج
 */
void parser_error_print(ParserError* err, FILE* file);

#endif /* DAAD_PARSER_ERROR_H */
