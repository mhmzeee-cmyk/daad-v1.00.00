/**
 * @file emitter.c
 * @brief تنفيذ مولّد الكود - محدّث مع Backend الموحد
 */

#include "emitter.h"
#include "../backend/backend_interface.h"
#include <stdlib.h>
#include <string.h>

Emitter* emitter_create(IRModule* module, BackendTarget target) {
    Emitter* emit = (Emitter*)calloc(1, sizeof(Emitter));
    if (!emit) return NULL;
    emit->module = module;
    emit->target = target;
    emit->output = NULL;
    emit->error_occurred = 0;
    return emit;
}

void emitter_destroy(Emitter* emit) {
    free(emit);
}

int emitter_emit_function(IRFunction* func, BackendTarget target, FILE* out) {
    return backend_emit_function(func, target, out);
}

int emitter_emit(Emitter* emit, FILE* out) {
    if (!emit || !out) return 0;
    emit->output = out;
    emit->instruction_count = 0;
    emit->function_count = 0;
    for (int i = 0; i < emit->module->function_count; i++) {
        IRFunction* func = emit->module->functions[i];
        emit->instruction_count += emitter_emit_function(func, emit->target, out);
        emit->function_count++;
        fprintf(out, "\n");
    }
    return emit->instruction_count;
}

const char* emitter_get_error(Emitter* emit) {
    if (!emit) return "null emitter";
    return emit->error_occurred ? emit->last_error : NULL;
}
