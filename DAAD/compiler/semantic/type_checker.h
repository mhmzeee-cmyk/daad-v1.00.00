/**
 * @file type_checker.h
 * @brief فاحص الأنواع
 */

#ifndef DAAD_TYPE_CHECKER_H
#define DAAD_TYPE_CHECKER_H

#include "type.h"
#include "type_registry.h"
#include "semantic_error.h"
#include "../ast/ast_node.h"

typedef struct {
    TypeRegistry* registry;
    SemanticErrorList* errors;
} TypeChecker;

TypeChecker* type_checker_create(TypeRegistry* registry, SemanticErrorList* errors);
DaadType* type_checker_check_binary(TypeChecker* checker, BinaryOperator op, DaadType* left, DaadType* right, size_t line, size_t column, const char* filename);
DaadType* type_checker_check_unary(TypeChecker* checker, UnaryOperator op, DaadType* operand, size_t line, size_t column, const char* filename);
int type_checker_check_assignment(TypeChecker* checker, DaadType* target, DaadType* value, size_t line, size_t column, const char* filename);
int type_checker_check_return(TypeChecker* checker, DaadType* expected, DaadType* actual, size_t line, size_t column, const char* filename);
int type_checker_check_cast(TypeChecker* checker, DaadType* from, DaadType* to, size_t line, size_t column, const char* filename);
int type_checker_check_call(TypeChecker* checker, DaadType* func_type, DaadType** arg_types, int arg_count, size_t line, size_t column, const char* filename);
DaadType* type_checker_get_promoted_type(TypeChecker* checker, DaadType* left, DaadType* right);
void type_checker_destroy(TypeChecker* checker);

#endif