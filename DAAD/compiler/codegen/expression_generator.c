/**
 * @file expression_generator.c
 * @brief تنفيذ مولّد التعبيرات - محدّث مع Backend الموحد
 */

#include "expression_generator.h"
#include "../backend/backend_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

ExpressionGenerator* expr_gen_create(StackManager* stack) {
    ExpressionGenerator* gen = (ExpressionGenerator*)calloc(1, sizeof(ExpressionGenerator));
    if (!gen) return NULL;
    gen->stack = stack;
    gen->next_temp = 100;
    return gen;
}

void expr_gen_destroy(ExpressionGenerator* gen) {
    free(gen);
}

static const char* expr_reg_name(int reg_id) {
    if (reg_id >= 100) {
        static char buf[32];
        snprintf(buf, sizeof(buf), "%%r%d", reg_id);
        return buf;
    }
    return x86_reg_name(reg_id);
}

int expr_gen_generate_binary(ExpressionGenerator* gen, IROpcode op, IRValue left, IRValue right, IRValue* result, FILE* out) {
    if (!gen || !out) return 0;
    int reg_id = gen->next_temp++;
    result->kind = IR_VALUE_REGISTER;
    result->id = reg_id;
    if (left.kind == IR_VALUE_CONSTANT_INT) {
        fprintf(out, "  movq $%lld, %s\n", left.as.int_val, expr_reg_name(reg_id));
    } else if (left.kind == IR_VALUE_REGISTER) {
        fprintf(out, "  movq %s, %s\n", expr_reg_name(left.id), expr_reg_name(reg_id));
    }
    switch (op) {
        case IR_OP_ADD:
            if (right.kind == IR_VALUE_CONSTANT_INT) {
                fprintf(out, "  addq $%lld, %s\n", right.as.int_val, expr_reg_name(reg_id));
            } else if (right.kind == IR_VALUE_REGISTER) {
                fprintf(out, "  addq %s, %s\n", expr_reg_name(right.id), expr_reg_name(reg_id));
            }
            break;
        case IR_OP_SUB:
            if (right.kind == IR_VALUE_CONSTANT_INT) {
                fprintf(out, "  subq $%lld, %s\n", right.as.int_val, expr_reg_name(reg_id));
            } else if (right.kind == IR_VALUE_REGISTER) {
                fprintf(out, "  subq %s, %s\n", expr_reg_name(right.id), expr_reg_name(reg_id));
            }
            break;
        case IR_OP_MUL:
            fprintf(out, "  movq %s, %%rax\n", expr_reg_name(reg_id));
            if (right.kind == IR_VALUE_CONSTANT_INT) {
                fprintf(out, "  imulq $%lld, %%rax\n", right.as.int_val);
            } else if (right.kind == IR_VALUE_REGISTER) {
                fprintf(out, "  imulq %s, %%rax\n", expr_reg_name(right.id));
            }
            fprintf(out, "  movq %%rax, %s\n", expr_reg_name(reg_id));
            break;
        case IR_OP_DIV:
            fprintf(out, "  movq %s, %%rax\n", expr_reg_name(reg_id));
            if (right.kind == IR_VALUE_CONSTANT_INT) {
                fprintf(out, "  movq $%lld, %%rcx\n", right.as.int_val);
                fprintf(out, "  cqo\n");
                fprintf(out, "  idivq %%rcx\n");
            } else if (right.kind == IR_VALUE_REGISTER) {
                fprintf(out, "  cqo\n");
                fprintf(out, "  idivq %s\n", expr_reg_name(right.id));
            }
            fprintf(out, "  movq %%rax, %s\n", expr_reg_name(reg_id));
            break;
        case IR_OP_MOD:
            fprintf(out, "  movq %s, %%rax\n", expr_reg_name(reg_id));
            if (right.kind == IR_VALUE_CONSTANT_INT) {
                fprintf(out, "  movq $%lld, %%rcx\n", right.as.int_val);
                fprintf(out, "  cqo\n");
                fprintf(out, "  idivq %%rcx\n");
            } else if (right.kind == IR_VALUE_REGISTER) {
                fprintf(out, "  cqo\n");
                fprintf(out, "  idivq %s\n", expr_reg_name(right.id));
            }
            fprintf(out, "  movq %%rdx, %s\n", expr_reg_name(reg_id));
            break;
        case IR_OP_AND:
            if (right.kind == IR_VALUE_CONSTANT_INT) {
                fprintf(out, "  andq $%lld, %s\n", right.as.int_val, expr_reg_name(reg_id));
            } else if (right.kind == IR_VALUE_REGISTER) {
                fprintf(out, "  andq %s, %s\n", expr_reg_name(right.id), expr_reg_name(reg_id));
            }
            break;
        case IR_OP_OR:
            if (right.kind == IR_VALUE_CONSTANT_INT) {
                fprintf(out, "  orq $%lld, %s\n", right.as.int_val, expr_reg_name(reg_id));
            } else if (right.kind == IR_VALUE_REGISTER) {
                fprintf(out, "  orq %s, %s\n", expr_reg_name(right.id), expr_reg_name(reg_id));
            }
            break;
        case IR_OP_XOR:
            if (right.kind == IR_VALUE_CONSTANT_INT) {
                fprintf(out, "  xorq $%lld, %s\n", right.as.int_val, expr_reg_name(reg_id));
            } else if (right.kind == IR_VALUE_REGISTER) {
                fprintf(out, "  xorq %s, %s\n", expr_reg_name(right.id), expr_reg_name(reg_id));
            }
            break;
        case IR_OP_SHL:
            fprintf(out, "  movq %s, %%rcx\n", expr_reg_name(right.id));
            fprintf(out, "  shlq %%cl, %s\n", expr_reg_name(reg_id));
            break;
        case IR_OP_SHR:
            fprintf(out, "  movq %s, %%rcx\n", expr_reg_name(right.id));
            fprintf(out, "  shrq %%cl, %s\n", expr_reg_name(reg_id));
            break;
        case IR_OP_CMP:
            if (right.kind == IR_VALUE_CONSTANT_INT) {
                fprintf(out, "  cmpq $%lld, %s\n", right.as.int_val, expr_reg_name(reg_id));
            } else if (right.kind == IR_VALUE_REGISTER) {
                fprintf(out, "  cmpq %s, %s\n", expr_reg_name(right.id), expr_reg_name(reg_id));
            }
            fprintf(out, "  sete %%al\n");
            fprintf(out, "  movzbl %%al, %s\n", expr_reg_name(reg_id));
            break;
        default:
            break;
    }
    return 1;
}

int expr_gen_generate_unary(ExpressionGenerator* gen, IROpcode op, IRValue operand, IRValue* result, FILE* out) {
    if (!gen || !out) return 0;
    int reg_id = gen->next_temp++;
    result->kind = IR_VALUE_REGISTER;
    result->id = reg_id;
    if (operand.kind == IR_VALUE_CONSTANT_INT) {
        fprintf(out, "  movq $%lld, %s\n", operand.as.int_val, expr_reg_name(reg_id));
    } else if (operand.kind == IR_VALUE_REGISTER) {
        fprintf(out, "  movq %s, %s\n", expr_reg_name(operand.id), expr_reg_name(reg_id));
    }
    switch (op) {
        case IR_OP_NEG:
            fprintf(out, "  negq %s\n", expr_reg_name(reg_id));
            break;
        case IR_OP_NOT:
            fprintf(out, "  notq %s\n", expr_reg_name(reg_id));
            break;
        default:
            break;
    }
    return 1;
}

int expr_gen_generate_load(ExpressionGenerator* gen, IRValue addr, IRValue* result, FILE* out) {
    if (!gen || !out) return 0;
    int reg_id = gen->next_temp++;
    result->kind = IR_VALUE_REGISTER;
    result->id = reg_id;
    if (addr.kind == IR_VALUE_REGISTER) {
        fprintf(out, "  movq (%s), %s\n", expr_reg_name(addr.id), expr_reg_name(reg_id));
    }
    return 1;
}

int expr_gen_generate_store(ExpressionGenerator* gen, IRValue addr, IRValue value, FILE* out) {
    if (!gen || !out) return 0;
    if (addr.kind == IR_VALUE_REGISTER && value.kind == IR_VALUE_CONSTANT_INT) {
        fprintf(out, "  movq $%lld, (%s)\n", value.as.int_val, expr_reg_name(addr.id));
    } else if (addr.kind == IR_VALUE_REGISTER && value.kind == IR_VALUE_REGISTER) {
        fprintf(out, "  movq %s, (%s)\n", expr_reg_name(value.id), expr_reg_name(addr.id));
    }
    return 1;
}

const char* expr_gen_get_error(ExpressionGenerator* gen) {
    if (!gen) return "null generator";
    return gen->error_occurred ? gen->last_error : NULL;
}
