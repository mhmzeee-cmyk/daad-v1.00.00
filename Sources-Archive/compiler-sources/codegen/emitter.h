/**
 * @file emitter.h
 * @brief مولّد الكود
 */

#ifndef DAAD_EMITTER_H
#define DAAD_EMITTER_H

#include "../ir/ir_module.h"
#include "../backend/backend_interface.h"
#include <stdio.h>

typedef struct {
    IRModule* module;
    BackendTarget target;
    FILE* output;
    char last_error[256];
    int instruction_count;
    int function_count;
    int error_occurred;
} Emitter;

Emitter* emitter_create(IRModule* module, BackendTarget target);
void emitter_destroy(Emitter* emit);
int emitter_emit(Emitter* emit, FILE* out);
int emitter_emit_function(IRFunction* func, BackendTarget target, FILE* out);
const char* emitter_get_error(Emitter* emit);

#endif