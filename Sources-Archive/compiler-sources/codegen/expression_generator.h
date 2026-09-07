/**
 * @file expression_generator.h
 * @brief مولّد التعبيرات
 */

#ifndef DAAD_EXPRESSION_GENERATOR_H
#define DAAD_EXPRESSION_GENERATOR_H

#include "../ir/ir_function.h"
#include "stack_manager.h"
#include <stdio.h>

typedef struct {
    StackManager* stack;
    int next_temp;
    char last_error[256];
    int error_occurred;
} ExpressionGenerator;

ExpressionGenerator* expr_gen_create(StackManager* stack);
void expr_gen_destroy(ExpressionGenerator* gen);
int expr_gen_generate_binary(ExpressionGenerator* gen, IROpcode op, IRValue left, IRValue right, IRValue* result, FILE* out);
int expr_gen_generate_unary(ExpressionGenerator* gen, IROpcode op, IRValue operand, IRValue* result, FILE* out);
int expr_gen_generate_load(ExpressionGenerator* gen, IRValue addr, IRValue* result, FILE* out);
int expr_gen_generate_store(ExpressionGenerator* gen, IRValue addr, IRValue value, FILE* out);
const char* expr_gen_get_error(ExpressionGenerator* gen);

#endif