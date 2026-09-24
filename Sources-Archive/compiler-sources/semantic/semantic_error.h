/**
 * @file semantic_error.h
 * @brief أخطاء التحليل الدلالي
 */

#ifndef DAAD_SEMANTIC_ERROR_H
#define DAAD_SEMANTIC_ERROR_H

#include <stddef.h>

typedef enum {
    S001_UNDEFINED_VARIABLE,
    S002_UNDEFINED_FUNCTION,
    S003_UNDEFINED_TYPE,
    S004_DUPLICATE_VARIABLE,
    S005_DUPLICATE_FUNCTION,
    S006_DUPLICATE_PARAMETER,
    S007_TYPE_MISMATCH,
    S008_ARGUMENT_COUNT_MISMATCH,
    S009_RETURN_TYPE_MISMATCH,
    S010_NOT_CALLABLE,
    S011_NOT_INDEXABLE,
    S012_INVALID_INDEX_TYPE,
    S013_CANNOT_ASSIGN,
    S014_CONSTANT_REASSIGNMENT,
    S015_VOID_VARIABLE,
    S016_MISSING_RETURN,
    S017_BREAK_OUTSIDE_LOOP,
    S018_CONTINUE_OUTSIDE_LOOP,
    S019_DIVISION_BY_ZERO,
    S020_INVALID_UNARY_OP,
    S021_INVALID_BINARY_OP,
    S022_MEMBER_ACCESS_ON_NON_STRUCT,
    S023_UNDEFINED_MEMBER,
    S024_ARRAY_SIZE_NOT_INT,
    S025_ARRAY_INIT_TYPE_MISMATCH,
    S026_IMPLICIT_CONVERSION,
    S027_UNUSED_VARIABLE,
    S028_UNINITIALIZED_VARIABLE,
    S029_INVALID_CAST,
    S030_INVALID_POINTER_OP,
    S031_CANNOT_DEREFERENCE,
    S032_CANNOT_TAKE_ADDRESS,
    S033_INCOMPATIBLE_POINTER_TYPES,
    S034_STRUCT_INIT_FIELD_COUNT,
    S035_STRUCT_INIT_FIELD_TYPE,
    S036_FOR_INIT_NOT_DECL,
    S037_FUNCTION_REDEFINITION,
    S038_WRONG_RETURN_TYPE,
    S039_EXPRESSION_NOT_CONSTANT,
    S040_INVALID_ASSIGNMENT_TARGET
} SemanticErrorCode;

typedef struct {
    SemanticErrorCode code;
    size_t line;
    size_t column;
    char message[512];
    const char* filename;
} SemanticError;

typedef struct {
    SemanticError* errors;
    size_t size;
    size_t capacity;
} SemanticErrorList;

SemanticErrorList* semantic_error_list_create(void);
void semantic_error_add(SemanticErrorList* list, SemanticErrorCode code, size_t line, size_t column, const char* filename, const char* fmt, ...);
void semantic_error_list_destroy(SemanticErrorList* list);
const char* semantic_error_code_name(SemanticErrorCode code);

#endif