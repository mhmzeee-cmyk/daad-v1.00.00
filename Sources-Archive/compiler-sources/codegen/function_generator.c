/**
 * @file function_generator.c
 * @brief تنفيذ مولّد الدوال - محدّث مع Backend الموحد
 *
 * هذا الملف أصبح wrapper حول backend_interface الموحد
 */

#include "function_generator.h"
#include "../backend/backend_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FunctionGenerator* func_gen_create(StackManager* stack, LabelManager* labels) {
    FunctionGenerator* gen = (FunctionGenerator*)calloc(1, sizeof(FunctionGenerator));
    if (!gen) return NULL;
    gen->stack = stack;
    gen->labels = labels;
    gen->error_occurred = 0;
    return gen;
}

void func_gen_destroy(FunctionGenerator* gen) {
    free(gen);
}

int func_gen_generate(FunctionGenerator* gen, IRFunction* func, FILE* out) {
    if (!gen || !func || !out) return 0;
    gen->func = func;
    gen->output = out;
    int inst_count = backend_emit_function(func, BACKEND_X86, out);
    return inst_count;
}

const char* func_gen_get_error(FunctionGenerator* gen) {
    if (!gen) return "null generator";
    return gen->error_occurred ? gen->last_error : NULL;
}
