/**
 * @file semantic_error.c
 * @brief تنفيذ أخطاء التحليل الدلالي
 */

#include "semantic_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

SemanticErrorList* semantic_error_list_create(void) {
    SemanticErrorList* list = (SemanticErrorList*)malloc(sizeof(SemanticErrorList));
    if (!list) return NULL;
    list->errors = NULL;
    list->size = 0;
    list->capacity = 0;
    return list;
}

void semantic_error_add(SemanticErrorList* list, SemanticErrorCode code, size_t line, size_t column, const char* filename, const char* fmt, ...) {
    if (!list) return;

    if (list->size >= list->capacity) {
        size_t new_cap = list->capacity == 0 ? 16 : list->capacity * 2;
        SemanticError* tmp = (SemanticError*)realloc(list->errors, new_cap * sizeof(SemanticError));
        if (!tmp) return;
        list->errors = tmp;
        list->capacity = new_cap;
    }

    SemanticError* err = &list->errors[list->size];
    err->code = code;
    err->line = line;
    err->column = column;
    err->filename = filename;

    va_list args;
    va_start(args, fmt);
    vsnprintf(err->message, sizeof(err->message), fmt, args);
    va_end(args);

    list->size++;
}

void semantic_error_list_destroy(SemanticErrorList* list) {
    if (!list) return;
    free(list->errors);
    free(list);
}

const char* semantic_error_code_name(SemanticErrorCode code) {
    switch (code) {
        case S001_UNDEFINED_VARIABLE: return "S001";
        case S002_UNDEFINED_FUNCTION: return "S002";
        case S003_UNDEFINED_TYPE: return "S003";
        case S004_DUPLICATE_VARIABLE: return "S004";
        case S005_DUPLICATE_FUNCTION: return "S005";
        case S006_DUPLICATE_PARAMETER: return "S006";
        case S007_TYPE_MISMATCH: return "S007";
        case S008_ARGUMENT_COUNT_MISMATCH: return "S008";
        case S009_RETURN_TYPE_MISMATCH: return "S009";
        case S010_NOT_CALLABLE: return "S010";
        case S011_NOT_INDEXABLE: return "S011";
        case S012_INVALID_INDEX_TYPE: return "S012";
        case S013_CANNOT_ASSIGN: return "S013";
        case S014_CONSTANT_REASSIGNMENT: return "S014";
        case S015_VOID_VARIABLE: return "S015";
        case S016_MISSING_RETURN: return "S016";
        case S017_BREAK_OUTSIDE_LOOP: return "S017";
        case S018_CONTINUE_OUTSIDE_LOOP: return "S018";
        case S019_DIVISION_BY_ZERO: return "S019";
        case S020_INVALID_UNARY_OP: return "S020";
        case S021_INVALID_BINARY_OP: return "S021";
        case S022_MEMBER_ACCESS_ON_NON_STRUCT: return "S022";
        case S023_UNDEFINED_MEMBER: return "S023";
        case S024_ARRAY_SIZE_NOT_INT: return "S024";
        case S025_ARRAY_INIT_TYPE_MISMATCH: return "S025";
        case S026_IMPLICIT_CONVERSION: return "S026";
        case S027_UNUSED_VARIABLE: return "S027";
        case S028_UNINITIALIZED_VARIABLE: return "S028";
        case S029_INVALID_CAST: return "S029";
        case S030_INVALID_POINTER_OP: return "S030";
        case S031_CANNOT_DEREFERENCE: return "S031";
        case S032_CANNOT_TAKE_ADDRESS: return "S032";
        case S033_INCOMPATIBLE_POINTER_TYPES: return "S033";
        case S034_STRUCT_INIT_FIELD_COUNT: return "S034";
        case S035_STRUCT_INIT_FIELD_TYPE: return "S035";
        case S036_FOR_INIT_NOT_DECL: return "S036";
        case S037_FUNCTION_REDEFINITION: return "S037";
        case S038_WRONG_RETURN_TYPE: return "S038";
        case S039_EXPRESSION_NOT_CONSTANT: return "S039";
        case S040_INVALID_ASSIGNMENT_TARGET: return "S040";
        default: return "S???";
    }
}