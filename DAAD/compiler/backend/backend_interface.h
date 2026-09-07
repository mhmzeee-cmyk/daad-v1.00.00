/**
 * @file backend_interface.h
 * @brief واجهة Backend الموحدة - Unified Backend Interface
 *
 * Backend موحد يجمع:
 * - backend_interface (instruction emission)
 * - function_generator (prologue/epilogue)
 * - expression_generator (expressions)
 * - statement_generator (statements)
 * - stack_manager (stack layout)
 * - label_manager (labels)
 * - register_allocator (register allocation)
 * - emitter (top-level emission)
 */

#ifndef DAAD_BACKEND_INTERFACE_H
#define DAAD_BACKEND_INTERFACE_H

#include "../ir/ir_function.h"
#include "../ir/ir_module.h"
#include "../codegen/register_allocator.h"
#include "../codegen/stack_manager.h"
#include "../codegen/label_manager.h"
#include <stdio.h>

typedef enum {
    BACKEND_X86,
    BACKEND_ARM,
    BACKEND_RISCV,
    BACKEND_DHAD
} BackendTarget;

typedef struct {
    BackendTarget target;
    const char* name;
    int (*emit_function)(IRFunction* func, FILE* out);
    int (*emit_instruction)(IRInstruction* inst, FILE* out);
    const char* (*reg_name)(int reg_id);
    const char* (*type_suffix)(IRType type);
} Backend;

typedef struct {
    RegisterAllocator* reg_alloc;
    StackManager* stack;
    LabelManager* labels;
    int frame_size;
    int param_count;
    int local_offset;
    int alloca_counter;
    IRFunction* current_func;
    char last_error[256];
    int error_occurred;
    int* reg_to_offset;
    int reg_map_size;
    int max_reg_id;
    int callee_saved_regs[5];
    int callee_saved_count;
    int callee_saved_offsets[5];
} BackendContext;

BackendContext* backend_context_create(void);
void backend_context_destroy(BackendContext* ctx);
int backend_build_intervals(BackendContext* ctx, IRFunction* func);
int backend_allocate_registers(BackendContext* ctx);

Backend* backend_create_x86(void);
Backend* backend_create_arm(void);
Backend* backend_create_riscv(void);
Backend* backend_create_dhad(void);
Backend* backend_get(BackendTarget target);
void backend_destroy(Backend* backend);

int backend_emit_module(IRModule* module, BackendTarget target, FILE* out);

/* إصدار الثوابت النصية المجمّعة إلى قسم .rodata (يُستدعى مرة بعد emit_module) */
void backend_emit_string_data(FILE* out);
int backend_emit_function(IRFunction* func, BackendTarget target, FILE* out);
int backend_emit_instruction(IRInstruction* inst, FILE* out);
int backend_emit_prologue(IRFunction* func, FILE* out);
int backend_emit_epilogue(FILE* out);
int backend_emit_param_load(int param_index, IRValue* result, FILE* out);
int backend_emit_return(IRValue value, FILE* out);

const char* x86_reg_name(int reg_id);
const char* x86_type_suffix(IRType type);
int x86_calculate_frame_size(IRFunction* func);
int x86_get_param_offset(int param_index);

#endif
