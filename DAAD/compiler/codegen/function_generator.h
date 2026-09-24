/**
 * @file function_generator.h
 * @brief مولّد الدوال
 */

#ifndef DAAD_FUNCTION_GENERATOR_H
#define DAAD_FUNCTION_GENERATOR_H

#include "../ir/ir_function.h"
#include "stack_manager.h"
#include "label_manager.h"
#include <stdio.h>

typedef struct {
    StackManager* stack;
    LabelManager* labels;
    IRFunction* func;
    FILE* output;
    char last_error[256];
    int error_occurred;
} FunctionGenerator;

FunctionGenerator* func_gen_create(StackManager* stack, LabelManager* labels);
void func_gen_destroy(FunctionGenerator* gen);
int func_gen_generate(FunctionGenerator* gen, IRFunction* func, FILE* out);
const char* func_gen_get_error(FunctionGenerator* gen);

#endif