/**
 * @file statement_generator.h
 * @brief مولّد الجمل - محدّث مع Backend الموحد
 */

#ifndef DAAD_STATEMENT_GENERATOR_H
#define DAAD_STATEMENT_GENERATOR_H

#include "../ir/ir_function.h"
#include "expression_generator.h"
#include "label_manager.h"
#include <stdio.h>

typedef struct {
    ExpressionGenerator* expr_gen;
    LabelManager* labels;
    int next_label;
    char last_error[256];
    int error_occurred;
} StatementGenerator;

StatementGenerator* stmt_gen_create(ExpressionGenerator* expr_gen, LabelManager* labels);
void stmt_gen_destroy(StatementGenerator* gen);
int stmt_gen_generate_return(StatementGenerator* gen, IRValue value, FILE* out);
int stmt_gen_generate_if(StatementGenerator* gen, IRValue cond, const char* true_label, const char* false_label, FILE* out);
int stmt_gen_generate_while(StatementGenerator* gen, IRValue cond, const char* loop_label, const char* exit_label, FILE* out);
int stmt_gen_generate_for(StatementGenerator* gen, IRValue cond, const char* loop_label, const char* exit_label, FILE* out);
int stmt_gen_generate_break(StatementGenerator* gen, const char* target, FILE* out);
int stmt_gen_generate_continue(StatementGenerator* gen, const char* target, FILE* out);
int stmt_gen_generate_jmp(StatementGenerator* gen, const char* target, FILE* out);
int stmt_gen_generate_label(StatementGenerator* gen, const char* label, FILE* out);
const char* stmt_gen_get_error(StatementGenerator* gen);

#endif
