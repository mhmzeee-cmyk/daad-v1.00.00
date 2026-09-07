/**
 * @file constant_folding.c
 * @brief تنفيذ تقليص الثوابت
 */

#include "constant_folding.h"
#include "../ast/ast_builder.h"
#include "../ast/ast.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static int add_overflow_check(long long a, long long b, long long* result) {
    if ((b > 0 && a > LLONG_MAX - b) || (b < 0 && a < LLONG_MIN - b)) return 1;
    *result = a + b;
    return 0;
}

static int sub_overflow_check(long long a, long long b, long long* result) {
    if ((b > 0 && a < LLONG_MIN + b) || (b < 0 && a > LLONG_MAX + b)) return 1;
    *result = a - b;
    return 0;
}

static int mul_overflow_check(long long a, long long b, long long* result) {
    if (a == 0 || b == 0) { *result = 0; return 0; }
    if (a == 1) { *result = b; return 0; }
    if (b == 1) { *result = a; return 0; }
    if (a == -1) { *result = -b; return (b == LLONG_MIN); }
    if (b == -1) { *result = -a; return (a == LLONG_MIN); }
    if (a > 0) {
        if (b > 0) { if (a > LLONG_MAX / b) return 1; }
        else { if (b < LLONG_MIN / a) return 1; }
    } else {
        if (b > 0) { if (a < LLONG_MIN / b) return 1; }
        else { if (a < LLONG_MAX / b) return 1; }
    }
    *result = a * b;
    return 0;
}

ASTNode* constant_fold(ASTNode* node) {
    if (!node) return NULL;

    if (node->type == NODE_BINARY_EXPRESSION) {
        ASTNode* left = constant_fold(node->as.binary.left);
        ASTNode* right = constant_fold(node->as.binary.right);

        if (left && right &&
            left->type == NODE_LITERAL && right->type == NODE_LITERAL &&
            left->as.literal.kind == LITERAL_INT && right->as.literal.kind == LITERAL_INT) {

            long long result;
            int overflow = 0;
            switch (node->as.binary.op) {
                case OP_ADD: overflow = add_overflow_check(left->as.literal.as.int_value, right->as.literal.as.int_value, &result); break;
                case OP_SUB: overflow = sub_overflow_check(left->as.literal.as.int_value, right->as.literal.as.int_value, &result); break;
                case OP_MUL: overflow = mul_overflow_check(left->as.literal.as.int_value, right->as.literal.as.int_value, &result); break;
                case OP_DIV:
                    if (right->as.literal.as.int_value == 0) { ast_node_destroy(left); ast_node_destroy(right); return node; }
                    result = left->as.literal.as.int_value / right->as.literal.as.int_value;
                    break;
                case OP_MOD:
                    if (right->as.literal.as.int_value == 0) { ast_node_destroy(left); ast_node_destroy(right); return node; }
                    result = left->as.literal.as.int_value % right->as.literal.as.int_value;
                    break;
                case OP_BITWISE_AND: result = left->as.literal.as.int_value & right->as.literal.as.int_value; break;
                case OP_BITWISE_OR: result = left->as.literal.as.int_value | right->as.literal.as.int_value; break;
                case OP_BITWISE_XOR: result = left->as.literal.as.int_value ^ right->as.literal.as.int_value; break;
                case OP_SHIFT_LEFT: result = left->as.literal.as.int_value << right->as.literal.as.int_value; break;
                case OP_SHIFT_RIGHT: result = left->as.literal.as.int_value >> right->as.literal.as.int_value; break;
                case OP_EQUAL: result = left->as.literal.as.int_value == right->as.literal.as.int_value; break;
                case OP_NOT_EQUAL: result = left->as.literal.as.int_value != right->as.literal.as.int_value; break;
                case OP_GREATER: result = left->as.literal.as.int_value > right->as.literal.as.int_value; break;
                case OP_LESS: result = left->as.literal.as.int_value < right->as.literal.as.int_value; break;
                case OP_GREATER_EQUAL: result = left->as.literal.as.int_value >= right->as.literal.as.int_value; break;
                case OP_LESS_EQUAL: result = left->as.literal.as.int_value <= right->as.literal.as.int_value; break;
                default:
                    node->as.binary.left = left;
                    node->as.binary.right = right;
                    return node;
            }
            if (overflow) {
                node->as.binary.left = left;
                node->as.binary.right = right;
                return node;
            }
            ASTNode* folded = ast_build_literal_int(result);
            ast_node_destroy(left);
            ast_node_destroy(right);
            return folded;
        }

        if (left && right &&
            left->type == NODE_LITERAL && right->type == NODE_LITERAL &&
            left->as.literal.kind == LITERAL_FLOAT && right->as.literal.kind == LITERAL_FLOAT) {

            double l = left->as.literal.as.float_value;
            double r = right->as.literal.as.float_value;
            double result;
            switch (node->as.binary.op) {
                case OP_ADD: result = l + r; break;
                case OP_SUB: result = l - r; break;
                case OP_MUL: result = l * r; break;
                case OP_DIV:
                    if (r == 0.0) { ast_node_destroy(left); ast_node_destroy(right); return node; }
                    result = l / r;
                    break;
                case OP_EQUAL: result = l == r; break;
                case OP_NOT_EQUAL: result = l != r; break;
                case OP_GREATER: result = l > r; break;
                case OP_LESS: result = l < r; break;
                case OP_GREATER_EQUAL: result = l >= r; break;
                case OP_LESS_EQUAL: result = l <= r; break;
                default:
                    node->as.binary.left = left;
                    node->as.binary.right = right;
                    return node;
            }
            ASTNode* folded = ast_build_literal_float(result);
            ast_node_destroy(left);
            ast_node_destroy(right);
            return folded;
        }

        node->as.binary.left = left;
        node->as.binary.right = right;
    }

    if (node->type == NODE_UNARY_EXPRESSION && node->as.unary.operand) {
        ASTNode* operand = constant_fold(node->as.unary.operand);
        if (operand && operand->type == NODE_LITERAL && operand->as.literal.kind == LITERAL_INT) {
            if (node->as.unary.op == UNARY_NEGATE) {
                ASTNode* folded = ast_build_literal_int(-operand->as.literal.as.int_value);
                ast_node_destroy(operand);
                return folded;
            }
            if (node->as.unary.op == UNARY_BITWISE_NOT) {
                ASTNode* folded = ast_build_literal_int(~operand->as.literal.as.int_value);
                ast_node_destroy(operand);
                return folded;
            }
        }
        node->as.unary.operand = operand;
    }

    return node;
}

int constant_folding_is_foldable(ASTNode* node) {
    if (!node) return 0;
    if (node->type == NODE_LITERAL) return 1;
    if (node->type == NODE_BINARY_EXPRESSION) {
        return constant_folding_is_foldable(node->as.binary.left) &&
               constant_folding_is_foldable(node->as.binary.right);
    }
    return 0;
}