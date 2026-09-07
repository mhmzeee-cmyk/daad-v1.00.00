/**
 * @file type.h
 * @brief تمثيل الأنواع في نظام ض Core
 *
 * يحتوي على بنية DaadType التي تمثل جميع أنواع البيانات في اللغة.
 *
 * @version 0.1.0
 * @date 2026-08-04
 */

#ifndef DAAD_SEMANTIC_TYPE_H
#define DAAD_SEMANTIC_TYPE_H

#include <stddef.h>

/**
 * @brief أنواع البيانات الأساسية
 */
typedef enum {
    TYPE_VOID,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_POINTER,
    TYPE_ARRAY,
    TYPE_STRUCT,
    TYPE_FUNCTION
} DaadTypeKind;

/**
 * @brief بنية تمثل نوع بيانات في ض Core
 */
typedef struct DaadType {
    DaadTypeKind kind;
    const char* name;
    size_t size;
    int is_const;

    union {
        /* TYPE_POINTER */
        struct {
            struct DaadType* pointee;
        } pointer;

        /* TYPE_ARRAY */
        struct {
            struct DaadType* element_type;
            size_t length;
        } array;

        /* TYPE_STRUCT */
        struct {
            char** member_names;
            struct DaadType** member_types;
            size_t member_count;
        } struct_type;

        /* TYPE_FUNCTION */
        struct {
            struct DaadType** param_types;
            size_t param_count;
            struct DaadType* return_type;
        } function;
    } as;
} DaadType;

/**
 * @brief إنشاء نوع أساسي
 * @param kind نوع البيانات
 * @param name اسم النوع
 * @param size حجم النوع بالبايت
 * @return مؤشر إلى النوع الجديد
 */
DaadType* daad_type_create(DaadTypeKind kind, const char* name, size_t size);

/**
 * @brief إنشاء نوع مؤشر
 * @param pointee نوع البيانات المؤشر إليها
 * @return مؤشر إلى نوع المؤشر
 */
DaadType* daad_type_create_pointer(DaadType* pointee);

/**
 * @brief إنشاء نوع مصفوفة
 * @param element_type نوع العناصر
 * @param length عدد العناصر
 * @return مؤشر إلى نوع المصفوفة
 */
DaadType* daad_type_create_array(DaadType* element_type, size_t length);

/**
 * @brief إنشاء نوع هيكل
 * @param member_names أسماء الأعضاء
 * @param member_types أنواع الأعضاء
 * @param member_count عدد الأعضاء
 * @return مؤشر إلى نوع الهيكل
 */
DaadType* daad_type_create_struct(char** member_names, DaadType** member_types, size_t member_count);

/**
 * @brief إنشاء نوع دالة
 * @param param_types أنواع المعاملات
 * @param param_count عدد المعاملات
 * @param return_type نوع الإرجاع
 * @return مؤشر إلى نوع الدالة
 */
DaadType* daad_type_create_function(DaadType** param_types, size_t param_count, DaadType* return_type);

/**
 * @brief التحقق من تساوي نوعين
 * @param a النوع الأول
 * @param b النوع الثاني
 * @return 1 إذا كانا متساويين، 0 خلاف ذلك
 */
int daad_type_equals(const DaadType* a, const DaadType* b);

/**
 * @brief التحقق من إمكانية التحويل الضمني
 * @param from النوع المصدر
 * @param to النوع الهدف
 * @return 1 إذا كان التحويل ممكناً، 0 خلاف ذلك
 */
int daad_type_is_compatible(const DaadType* from, const DaadType* to);

/**
 * @brief التحقق من أن النوع رقمي
 * @param type النوع
 * @return 1 إذا كان نوعاً عددياً، 0 خلاف ذلك
 */
int daad_type_is_numeric(const DaadType* type);

/**
 * @brief التحقق من أن النوع مؤشر
 * @param type النوع
 * @return 1 إذا كان مؤشراً، 0 خلاف ذلك
 */
int daad_type_is_pointer(const DaadType* type);

/**
 * @brief التحقق من أن النوع صفرية
 * @param type النوع
 * @return 1 إذا كان void، 0 خلاف ذلك
 */
int daad_type_is_void(const DaadType* type);

/**
 * @brief تحرير ذاكرة النوع
 * @param type النوع المراد تحريره
 */
void daad_type_destroy(DaadType* type);

#endif /* DAAD_SEMANTIC_TYPE_H */