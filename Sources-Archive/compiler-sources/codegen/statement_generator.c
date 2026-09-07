/**
 * @file statement_generator.c
 * @brief تنفيذ مولّد الجمل - محدّث مع Backend الموحد
 */

#include "statement_generator.h"
#include "../backend/backend_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

StatementGenerator* stmt_gen_create(ExpressionGenerator* expr_gen, LabelManager* labels) {
    StatementGenerator* gen = (StatementGenerator*)calloc(1, sizeof(StatementGenerator));
    if (!gen) return NULL;
    gen->expr_gen = expr_gen;
    gen->labels = labels;
    gen->next_label = 100;
    return gen;
}

void stmt_gen_destroy(StatementGenerator* gen) {
    free(gen);
}

static const char* stmt_reg_name(int reg_id) {
    if (reg_id >= 100) {
        static char buf[32];
        snprintf(buf, sizeof(buf), "%%r%d", reg_id);
        return buf;
    }
    return x86_reg_name(reg_id);
}

int stmt_gen_generate_return(StatementGenerator* gen, IRValue value, FILE* out) {
    if (!gen || !out) return 0;
    if (value.kind == IR_VALUE_CONSTANT_INT) {
        fprintf(out, "  movq $%lld, %%rax\n", value.as.int_val);
    } else if (value.kind == IR_VALUE_REGISTER) {
        fprintf(out, "  movq %s, %%rax\n", stmt_reg_name(value.id));
    }
    fprintf(out, "  leave\n");
    fprintf(out, "  ret\n");
    return 1;
}

int stmt_gen_generate_if(StatementGenerator* gen, IRValue cond, const char* true_label, const char* false_label, FILE* out) {
    if (!gen || !out) return 0;
    if (cond.kind == IR_VALUE_REGISTER) {
        fprintf(out, "  testq %s, %s\n", stmt_reg_name(cond.id), stmt_reg_name(cond.id));
    } else if (cond.kind == IR_VALUE_CONSTANT_INT) {
        fprintf(out, "  movq $%lld, %%rax\n", cond.as.int_val);
        fprintf(out, "  testq %%rax, %%rax\n");
    }
    if (true_label) {
        fprintf(out, "  jnz %s\n", true_label);
    }
    if (false_label) {
        fprintf(out, "  jmp %s\n", false_label);
    }
    return 1;
}

int stmt_gen_generate_while(StatementGenerator* gen, IRValue cond, const char* loop_label, const char* exit_label, FILE* out) {
    if (!gen || !out) return 0;
    fprintf(out, "%s:\n", loop_label);
    if (cond.kind == IR_VALUE_REGISTER) {
        fprintf(out, "  testq %s, %s\n", stmt_reg_name(cond.id), stmt_reg_name(cond.id));
    } else if (cond.kind == IR_VALUE_CONSTANT_INT) {
        fprintf(out, "  movq $%lld, %%rax\n", cond.as.int_val);
        fprintf(out, "  testq %%rax, %%rax\n");
    }
    fprintf(out, "  jz %s\n", exit_label);
    return 1;
}

int stmt_gen_generate_for(StatementGenerator* gen, IRValue cond, const char* loop_label, const char* exit_label, FILE* out) {
    if (!gen || !out) return 0;
    (void)loop_label;
    if (cond.kind == IR_VALUE_REGISTER) {
        fprintf(out, "  testq %s, %s\n", stmt_reg_name(cond.id), stmt_reg_name(cond.id));
    } else if (cond.kind == IR_VALUE_CONSTANT_INT) {
        fprintf(out, "  movq $%lld, %%rax\n", cond.as.int_val);
        fprintf(out, "  testq %%rax, %%rax\n");
    }
    fprintf(out, "  jz %s\n", exit_label);
    return 1;
}

int stmt_gen_generate_break(StatementGenerator* gen, const char* target, FILE* out) {
    if (!gen || !out || !target) return 0;
    fprintf(out, "  jmp %s\n", target);
    return 1;
}

int stmt_gen_generate_continue(StatementGenerator* gen, const char* target, FILE* out) {
    if (!gen || !out || !target) return 0;
    fprintf(out, "  jmp %s\n", target);
    return 1;
}

int stmt_gen_generate_jmp(StatementGenerator* gen, const char* target, FILE* out) {
    if (!gen || !out || !target) return 0;
    fprintf(out, "  jmp %s\n", target);
    return 1;
}

int stmt_gen_generate_label(StatementGenerator* gen, const char* label, FILE* out) {
    if (!gen || !out || !label) return 0;
    fprintf(out, "%s:\n", label);
    return 1;
}

const char* stmt_gen_get_error(StatementGenerator* gen) {
    if (!gen) return "null generator";
    return gen->error_occurred ? gen->last_error : NULL;
}
