/**
 * @file optimizer_pass.h
 * @brief نظام الـ Passes في المُحسّن
 */

#ifndef DAAD_OPTIMIZER_PASS_H
#define DAAD_OPTIMIZER_PASS_H

#include "../ir/ir_function.h"

typedef enum {
    OPT_PASS_CONSTANT_FOLDING,
    OPT_PASS_CONSTANT_PROPAGATION,
    OPT_PASS_COPY_PROPAGATION,
    OPT_PASS_DEAD_CODE_ELIMINATION,
    OPT_PASS_DEAD_STORE_ELIMINATION,
    OPT_PASS_ALGEBRAIC_SIMPLIFICATION,
    OPT_PASS_STRENGTH_REDUCTION,
    OPT_PASS_JUMP_OPTIMIZATION,
    OPT_PASS_REMOVE_EMPTY_BLOCKS,
    OPT_PASS_MERGE_BLOCKS,
    OPT_PASS_PEEPHOLE
} OptPassType;

typedef struct OptPass {
    OptPassType type;
    const char* name;
    int (*run)(IRFunction* func, struct OptPass* pass);
    struct OptPass* next;
} OptPass;

typedef struct {
    OptPass* passes;
    int pass_count;
    int total_transformations;
} Optimizer;

Optimizer* optimizer_create(void);
void optimizer_destroy(Optimizer* opt);
void optimizer_add_pass(Optimizer* opt, OptPassType type);
void optimizer_add_default_passes(Optimizer* opt);
int optimizer_run_all(Optimizer* opt, IRFunction* func);
int optimizer_run_single(Optimizer* opt, IRFunction* func, OptPassType type);
const char* optimizer_pass_name(OptPassType type);

#endif