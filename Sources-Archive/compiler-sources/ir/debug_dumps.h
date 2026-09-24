/**
 * @file debug_dumps.h
 * @brief Debug Dumps for IR, CFG, SSA, Backend
 */

#ifndef DAAD_DEBUG_DUMPS_H
#define DAAD_DEBUG_DUMPS_H

#include "../ir/ir_function.h"
#include "../ir/ir_module.h"
#include "../cfg/cfg.h"
#include "../ssa/ssa.h"
#include "../codegen/register_allocator.h"
#include "../codegen/stack_manager.h"
#include "../backend/backend_interface.h"
#include <stdio.h>

void debug_print_ir_function(IRFunction* func, FILE* out);
void debug_print_ir_module(IRModule* module, FILE* out);
void debug_print_ir_instruction(IRInstruction* inst, FILE* out);
void debug_print_ir_value(IRValue val, FILE* out);

void debug_print_cfg(CFG* cfg, FILE* out);
void debug_print_cfg_dominators(CFG* cfg, FILE* out);
void debug_print_cfg_post_dominators(CFG* cfg, FILE* out);
void debug_print_cfg_dominance_frontiers(CFG* cfg, FILE* out);

void debug_print_ssa(SSAContext* ctx, FILE* out);
void debug_print_ssa_renames(SSAContext* ctx, FILE* out);

void debug_print_register_allocator(RegisterAllocator* alloc, FILE* out);
void debug_print_stack_manager(StackManager* mgr, FILE* out);
void debug_print_backend_context(BackendContext* ctx, FILE* out);

#endif
