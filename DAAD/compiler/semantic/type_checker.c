/**
 * @file type_checker.c
 * @brief تنفيذ فاحص الأنواع
 */

#include "type_checker.h"
#include "type_registry.h"
#include <stdlib.h>

TypeChecker* type_checker_create(TypeRegistry* registry, SemanticErrorList* errors) {
    TypeChecker* checker = (TypeChecker*)malloc(sizeof(TypeChecker));
    if (!checker) return NULL;
    checker->registry = registry;
    checker->errors = errors;
    return checker;
}

DaadType* type_checker_check_binary(TypeChecker* checker, BinaryOperator op, DaadType* left, DaadType* right, size_t line, size_t column, const char* filename) {
    if (!checker || !left || !right) return NULL;

    if (op == OP_ADD || op == OP_SUB || op == OP_MUL || op == OP_DIV || op == OP_MOD) {
        if (!daad_type_is_numeric(left) || !daad_type_is_numeric(right)) {
            semantic_error_add(checker->errors, S021_INVALID_BINARY_OP, line, column, filename, "العمليات الحسابية تتطلب أرقاماً");
            return NULL;
        }
        return type_checker_get_promoted_type(checker, left, right);
    }

    if (op == OP_EQUAL || op == OP_NOT_EQUAL || op == OP_GREATER || op == OP_LESS || op == OP_GREATER_EQUAL || op == OP_LESS_EQUAL) {
        if (daad_type_is_numeric(left) && daad_type_is_numeric(right)) return checker->registry->bool_type;
        if (left->kind == TYPE_BOOL && right->kind == TYPE_BOOL) return checker->registry->bool_type;
        if (left->kind == TYPE_CHAR && right->kind == TYPE_CHAR) return checker->registry->bool_type;
        return checker->registry->bool_type;
    }

    if (op == OP_AND || op == OP_OR) {
        if (left->kind != TYPE_BOOL || right->kind != TYPE_BOOL) {
            semantic_error_add(checker->errors, S021_INVALID_BINARY_OP, line, column, filename, "العمليات المنطقية تتطلب قيم منطقية");
            return NULL;
        }
        return checker->registry->bool_type;
    }

    if (op == OP_BITWISE_AND || op == OP_BITWISE_OR || op == OP_BITWISE_XOR) {
        if (!daad_type_is_numeric(left) || !daad_type_is_numeric(right)) {
            semantic_error_add(checker->errors, S021_INVALID_BINARY_OP, line, column, filename, "العمليات البتية تتطلب أرقاماً");
            return NULL;
        }
        return checker->registry->int_type;
    }

    if (op == OP_SHIFT_LEFT || op == OP_SHIFT_RIGHT) {
        if (!daad_type_is_numeric(left) || !daad_type_is_numeric(right)) {
            semantic_error_add(checker->errors, S021_INVALID_BINARY_OP, line, column, filename, "عمليات التحريك تتطلب أرقاماً");
            return NULL;
        }
        return left;
    }

    return NULL;
}

DaadType* type_checker_check_unary(TypeChecker* checker, UnaryOperator op, DaadType* operand, size_t line, size_t column, const char* filename) {
    if (!checker || !operand) return NULL;

    if (op == UNARY_NOT) {
        if (operand->kind != TYPE_BOOL) {
            semantic_error_add(checker->errors, S020_INVALID_UNARY_OP, line, column, filename, "ليس يتطلب قيمة منطقية");
            return NULL;
        }
        return checker->registry->bool_type;
    }

    if (op == UNARY_NEGATE) {
        if (!daad_type_is_numeric(operand)) {
            semantic_error_add(checker->errors, S020_INVALID_UNARY_OP, line, column, filename, "النفي يطلب رقماً");
            return NULL;
        }
        return operand;
    }

    if (op == UNARY_BITWISE_NOT) {
        if (!daad_type_is_numeric(operand)) {
            semantic_error_add(checker->errors, S020_INVALID_UNARY_OP, line, column, filename, "ليس_بتية يطلب رقماً");
            return NULL;
        }
        return checker->registry->int_type;
    }

    /* Phase 12.5: &expr — address-of: returns pointer to operand type */
    if (op == UNARY_ADDRESS_OF) {
        DaadType* ptr_type = daad_type_create_pointer(operand);
        return ptr_type;
    }

    /* Phase 12.5: *expr — dereference: operand must be a pointer, returns pointee type */
    if (op == UNARY_DEREF) {
        if (operand->kind != TYPE_POINTER) {
            semantic_error_add(checker->errors, S020_INVALID_UNARY_OP, line, column, filename, "ال解除 يتطلب مؤشراً");
            return NULL;
        }
        return operand->as.pointer.pointee;
    }

    return NULL;
}

int type_checker_check_assignment(TypeChecker* checker, DaadType* target, DaadType* value, size_t line, size_t column, const char* filename) {
    if (!checker || !target || !value) return 0;
    if (daad_type_is_compatible(value, target)) return 1;
    if (daad_type_is_numeric(value) && daad_type_is_numeric(target)) return 1;
    semantic_error_add(checker->errors, S007_TYPE_MISMATCH, line, column, filename, "النوع غير متوافق في الإسناد");
    return 0;
}

int type_checker_check_return(TypeChecker* checker, DaadType* expected, DaadType* actual, size_t line, size_t column, const char* filename) {
    if (!checker) return 0;
    if (!expected && !actual) return 1;
    if (!expected || !actual) {
        semantic_error_add(checker->errors, S009_RETURN_TYPE_MISMATCH, line, column, filename, "نوع الإرجاع غير متطابق");
        return 0;
    }
    if (daad_type_is_compatible(actual, expected)) return 1;
    semantic_error_add(checker->errors, S009_RETURN_TYPE_MISMATCH, line, column, filename, "نوع الإرجاع غير متطابق");
    return 0;
}

int type_checker_check_cast(TypeChecker* checker, DaadType* from, DaadType* to, size_t line, size_t column, const char* filename) {
    if (!checker || !from || !to) return 0;
    if (daad_type_is_compatible(from, to)) return 1;
    semantic_error_add(checker->errors, S029_INVALID_CAST, line, column, filename, "التحويل غير مدعوم");
    return 0;
}

DaadType* type_checker_get_promoted_type(TypeChecker* checker, DaadType* left, DaadType* right) {
    if (!left || !right) return NULL;
    if (!checker) return left;
    if (left->kind == TYPE_FLOAT || right->kind == TYPE_FLOAT) return checker->registry->float_type;
    if (left->kind == TYPE_INT) return left;
    if (right->kind == TYPE_INT) return right;
    return left;
}

int type_checker_check_call(TypeChecker* checker, DaadType* func_type, DaadType** arg_types, int arg_count, size_t line, size_t column, const char* filename) {
    if (!checker || !func_type) return 0;

    if (func_type->kind != TYPE_FUNCTION) {
        semantic_error_add(checker->errors, S010_NOT_CALLABLE, line, column, filename, "العنصر المستدعي ليس دالة");
        return 0;
    }

    if (arg_count < (int)func_type->as.function.param_count) {
        semantic_error_add(checker->errors, S008_ARGUMENT_COUNT_MISMATCH, line, column, filename, "عدد المعاملات غير صحيح: متوقع %d، تم تمرير %d", (int)func_type->as.function.param_count, arg_count);
        return 0;
    }

    for (int i = 0; i < arg_count; i++) {
        if (arg_types[i] && func_type->as.function.param_types[i]) {
            if (!daad_type_is_compatible(arg_types[i], func_type->as.function.param_types[i])) {
                if (func_type->as.function.param_types[i]->kind == TYPE_STRING ||
                    func_type->as.function.param_types[i]->kind == TYPE_VOID) {
                    continue;
                }
                semantic_error_add(checker->errors, S007_TYPE_MISMATCH, line, column, filename, "نوع المعامل %d غير متوافق", i + 1);
                return 0;
            }
        }
    }

    return 1;
}

void type_checker_destroy(TypeChecker* checker) {
    if (checker) free(checker);
}