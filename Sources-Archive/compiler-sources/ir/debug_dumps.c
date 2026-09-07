/**
 * @file debug_dumps.c
 * @brief تنفيذ Debug Dumps for IR, CFG, SSA, Backend
 */

#include "debug_dumps.h"
#include <stdio.h>
#include <string.h>

static const char* type_to_string(IRType t) {
    switch (t.kind) {
        case IR_TYPE_VOID: return "void";
        case IR_TYPE_I8: return "i8";
        case IR_TYPE_I16: return "i16";
        case IR_TYPE_I32: return "i32";
        case IR_TYPE_I64: return "i64";
        case IR_TYPE_F32: return "f32";
        case IR_TYPE_F64: return "f64";
        case IR_TYPE_PTR: return "ptr";
        default: return "?";
    }
}

void debug_print_ir_function(IRFunction* func, FILE* out) {
    if (!func || !out) return;
    fprintf(out, "function @%s(", func->name);
    for (int i = 0; i < func->param_count; i++) {
        if (i > 0) fprintf(out, ", ");
        fprintf(out, "%s", func->param_names[i] ? func->param_names[i] : "?");
    }
    fprintf(out, ") -> %s {\n", type_to_string(func->return_type));
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        if (bb->label) {
            fprintf(out, ".%s:\n", bb->label);
        }
        for (int j = 0; j < bb->instruction_count; j++) {
            fprintf(out, "  ");
            debug_print_ir_instruction(&bb->instructions[j], out);
            fprintf(out, "\n");
        }
    }
    fprintf(out, "}\n");
}

void debug_print_ir_module(IRModule* module, FILE* out) {
    if (!module || !out) return;
    fprintf(out, "module @%s\n", module->name);
    for (int i = 0; i < module->global_count; i++) {
        IRGlobal* g = &module->globals[i];
        fprintf(out, "global @%s : %s", g->name, type_to_string(g->type));
        if (g->is_constant) fprintf(out, " (const)");
        fprintf(out, "\n");
    }
    for (int i = 0; i < module->function_count; i++) {
        fprintf(out, "\n");
        debug_print_ir_function(module->functions[i], out);
    }
}

void debug_print_ir_instruction(IRInstruction* inst, FILE* out) {
    if (!inst || !out) return;
    if (inst->result.kind != IR_VALUE_NULL) {
        debug_print_ir_value(inst->result, out);
        fprintf(out, " = ");
    }
    fprintf(out, "%s", ir_opcode_name(inst->opcode));
    for (int i = 0; i < inst->operand_count; i++) {
        fprintf(out, " ");
        debug_print_ir_value(inst->operands[i], out);
    }
}

void debug_print_ir_value(IRValue val, FILE* out) {
    if (!out) return;
    switch (val.kind) {
        case IR_VALUE_REGISTER:
            fprintf(out, "r%d", val.id);
            break;
        case IR_VALUE_CONSTANT_INT:
            fprintf(out, "%lld", val.as.int_val);
            break;
        case IR_VALUE_CONSTANT_FLOAT:
            fprintf(out, "%f", val.as.float_val);
            break;
        case IR_VALUE_CONSTANT_STRING:
            fprintf(out, "\"%s\"", val.as.string_val);
            break;
        case IR_VALUE_LABEL:
            fprintf(out, "%s", val.as.label_val);
            break;
        case IR_VALUE_NULL:
            fprintf(out, "null");
            break;
        default:
            fprintf(out, "?");
            break;
    }
}

void debug_print_cfg(CFG* cfg, FILE* out) {
    if (!cfg || !out) return;
    fprintf(out, "CFG (%d blocks):\n", cfg->block_count);
    for (int i = 0; i < cfg->block_count; i++) {
        IRBasicBlock* bb = cfg->blocks[i];
        fprintf(out, "  Block %d [%s]:", i, bb->label ? bb->label : "?");
        fprintf(out, " pred=");
        for (int p = 0; p < bb->predecessor_count; p++) {
            for (int j = 0; j < cfg->block_count; j++) {
                if (cfg->blocks[j] == bb->predecessors[p]) {
                    fprintf(out, "%d ", j);
                    break;
                }
            }
        }
        fprintf(out, " succ=");
        for (int s = 0; s < bb->successor_count; s++) {
            for (int j = 0; j < cfg->block_count; j++) {
                if (cfg->blocks[j] == bb->successors[s]) {
                    fprintf(out, "%d ", j);
                    break;
                }
            }
        }
        fprintf(out, "\n");
    }
}

void debug_print_cfg_dominators(CFG* cfg, FILE* out) {
    if (!cfg || !out) return;
    fprintf(out, "Dominators:\n");
    for (int i = 0; i < cfg->block_count; i++) {
        fprintf(out, "  Block %d [%s]: idom=%d\n", i,
            cfg->blocks[i]->label ? cfg->blocks[i]->label : "?",
            cfg->idom[i]);
    }
}

void debug_print_cfg_post_dominators(CFG* cfg, FILE* out) {
    if (!cfg || !out) return;
    fprintf(out, "Post-dominators:\n");
    for (int i = 0; i < cfg->block_count; i++) {
        fprintf(out, "  Block %d [%s]: post_dom=%d\n", i,
            cfg->blocks[i]->label ? cfg->blocks[i]->label : "?",
            cfg->post_dominators[i]);
    }
}

void debug_print_cfg_dominance_frontiers(CFG* cfg, FILE* out) {
    if (!cfg || !out) return;
    fprintf(out, "Dominance Frontiers:\n");
    for (int i = 0; i < cfg->block_count; i++) {
        fprintf(out, "  Block %d [%s]: DF={", i,
            cfg->blocks[i]->label ? cfg->blocks[i]->label : "?");
        for (int j = 0; j < cfg->frontier_counts[i]; j++) {
            if (j > 0) fprintf(out, ", ");
            fprintf(out, "%d", cfg->dominance_frontiers[i][j]);
        }
        fprintf(out, "}\n");
    }
}

void debug_print_ssa(SSAContext* ctx, FILE* out) {
    if (!ctx || !out) return;
    fprintf(out, "SSA Context:\n");
    fprintf(out, "  Function: %s\n", ctx->func->name);
    fprintf(out, "  Next SSA ID: %d\n", ctx->next_ssa_id);
    debug_print_ssa_renames(ctx, out);
}

void debug_print_ssa_renames(SSAContext* ctx, FILE* out) {
    if (!ctx || !out) return;
    fprintf(out, "  SSA Renames:\n");
    for (int i = 0; i < ctx->rename_count; i++) {
        fprintf(out, "    r%d -> r%d (v%d)\n",
            ctx->renames[i].original_id,
            ctx->renames[i].ssa_id,
            ctx->renames[i].version);
    }
}

void debug_print_register_allocator(RegisterAllocator* alloc, FILE* out) {
    if (!alloc || !out) return;
    fprintf(out, "Register Allocator:\n");
    fprintf(out, "  Intervals: %d\n", alloc->interval_count);
    fprintf(out, "  Frame Size: %d\n", alloc->frame_size);
    fprintf(out, "  Spill Slots: %d\n", alloc->spill_slots);
    for (int i = 0; i < alloc->interval_count; i++) {
        LiveInterval* iv = &alloc->intervals[i];
        fprintf(out, "  r%d [%d-%d] -> %s", iv->ir_reg, iv->start, iv->end,
            iv->phys_reg != REG_NONE ? reg_alloc_phys_name(iv->phys_reg) : "SPILL");
        if (iv->is_spilled) {
            fprintf(out, " (spilled at %d)", iv->spill_offset);
        }
        fprintf(out, "\n");
    }
}

void debug_print_stack_manager(StackManager* mgr, FILE* out) {
    if (!mgr || !out) return;
    fprintf(out, "Stack Manager:\n");
    fprintf(out, "  Frame Size: %d\n", stack_manager_get_frame_size(mgr));
    fprintf(out, "  Local Count: %d\n", stack_manager_get_local_count(mgr));
    fprintf(out, "  Spill Count: %d\n", stack_manager_get_spill_count(mgr));
    fprintf(out, "  Param Count: %d\n", stack_manager_get_param_count(mgr));
    for (int i = 0; i < mgr->count; i++) {
        fprintf(out, "  [%d] %s: offset=%d\n", i,
            mgr->names[i] ? mgr->names[i] : "?",
            mgr->offsets[i]);
    }
}

void debug_print_backend_context(BackendContext* ctx, FILE* out) {
    if (!ctx || !out) return;
    fprintf(out, "Backend Context:\n");
    fprintf(out, "  Frame Size: %d\n", ctx->frame_size);
    fprintf(out, "  Param Count: %d\n", ctx->param_count);
    fprintf(out, "  Local Offset: %d\n", ctx->local_offset);
    if (ctx->reg_alloc) {
        debug_print_register_allocator(ctx->reg_alloc, out);
    }
    if (ctx->stack) {
        debug_print_stack_manager(ctx->stack, out);
    }
}
