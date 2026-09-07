/**
 * @file ssa.c
 * @brief تنفيذ SSA
 */

#include "ssa.h"
#include "../ir/ir_types.h"
#include <stdlib.h>
#include <string.h>

SSAContext* ssa_create(IRFunction* func) {
    SSAContext* ctx = (SSAContext*)calloc(1, sizeof(SSAContext));
    if (!ctx) return NULL;
    ctx->func = func;
    ctx->rename_capacity = 256;
    ctx->renames = (SSARename*)calloc(ctx->rename_capacity, sizeof(SSARename));
    if (!ctx->renames) { free(ctx); return NULL; }
    ctx->next_ssa_id = 1;
    return ctx;
}

void ssa_destroy(SSAContext* ctx) {
    if (!ctx) return;
    free(ctx->renames);
    free(ctx);
}

void ssa_rename_variables(SSAContext* ctx) {
    if (!ctx) return;
    for (int i = 0; i < ctx->func->block_count; i++) {
        IRBasicBlock* bb = ctx->func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->result.kind == IR_VALUE_REGISTER) {
                int old_id = inst->result.id;
                inst->result.id = ctx->next_ssa_id++;
                if (ctx->rename_count >= ctx->rename_capacity) {
                    int new_cap = ctx->rename_capacity * 2;
                    SSARename* tmp = (SSARename*)realloc(ctx->renames, new_cap * sizeof(SSARename));
                    if (!tmp) return;
                    ctx->renames = tmp;
                    ctx->rename_capacity = new_cap;
                }
                ctx->renames[ctx->rename_count].original_id = old_id;
                ctx->renames[ctx->rename_count].ssa_id = inst->result.id;
                ctx->renames[ctx->rename_count].version = 0;
                ctx->rename_count++;
            }
            for (int o = 0; o < inst->operand_count; o++) {
                if (inst->operands[o].kind == IR_VALUE_REGISTER) {
                    for (int r = ctx->rename_count - 1; r >= 0; r--) {
                        if (ctx->renames[r].original_id == inst->operands[o].id) {
                            inst->operands[o].id = ctx->renames[r].ssa_id;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void ssa_insert_phi_nodes(SSAContext* ctx) {
    if (!ctx || !ctx->func) return;
    int n = ctx->func->block_count;
    if (n < 2) return;

    int* block_defs = (int*)calloc(n * 16, sizeof(int));
    int* block_def_counts = (int*)calloc(n, sizeof(int));
    if (!block_defs || !block_def_counts) { free(block_defs); free(block_def_counts); return; }

    for (int i = 0; i < n; i++) {
        IRBasicBlock* bb = ctx->func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->result.kind == IR_VALUE_REGISTER && block_def_counts[i] < 16) {
                block_defs[i * 16 + block_def_counts[i]++] = inst->result.id;
            }
        }
    }

    int global_vars[256];
    int global_var_count = 0;
    for (int i = 0; i < n && global_var_count < 256; i++) {
        for (int j = 0; j < block_def_counts[i] && global_var_count < 256; j++) {
            int vid = block_defs[i * 16 + j];
            int found = 0;
            for (int k = 0; k < global_var_count; k++) {
                if (global_vars[k] == vid) { found = 1; break; }
            }
            if (!found) global_vars[global_var_count++] = vid;
        }
    }

    for (int i = 0; i < n; i++) {
        IRBasicBlock* bb = ctx->func->blocks[i];
        if (bb->predecessor_count < 2) continue;

        for (int v = 0; v < global_var_count; v++) {
            int var_id = global_vars[v];
            int defined_here = 0;
            for (int d = 0; d < block_def_counts[i]; d++) {
                if (block_defs[i * 16 + d] == var_id) { defined_here = 1; break; }
            }
            if (defined_here) continue;

            int defined_in_pred = 0;
            for (int p = 0; p < bb->predecessor_count; p++) {
                IRBasicBlock* pred = bb->predecessors[p];
                int pred_idx = -1;
                for (int k = 0; k < n; k++) {
                    if (ctx->func->blocks[k] == pred) { pred_idx = k; break; }
                }
                if (pred_idx < 0) continue;
                for (int d = 0; d < block_def_counts[pred_idx]; d++) {
                    if (block_defs[pred_idx * 16 + d] == var_id) { defined_in_pred = 1; break; }
                }
                if (defined_in_pred) break;
            }
            if (!defined_in_pred) continue;

            IRValue result = ir_value_create_register(ctx->next_ssa_id++, ir_type_i64());
            IRValue incomings[4];
            int inc_count = bb->predecessor_count < 4 ? bb->predecessor_count : 4;
            for (int p = 0; p < inc_count; p++) {
                incomings[p] = ir_value_create_register(var_id, ir_type_i64());
            }
            IRInstruction phi = ir_inst_phi(result, incomings, inc_count);
            for (int j = bb->instruction_count; j > 0; j--) {
                bb->instructions[j] = bb->instructions[j - 1];
            }
            bb->instructions[0] = phi;
            bb->instruction_count++;
        }
    }

    free(block_defs);
    free(block_def_counts);
}

void ssa_convert_to_ssa(SSAContext* ctx) {
    if (!ctx) return;
    ssa_insert_phi_nodes(ctx);
    ssa_rename_variables(ctx);
}

void ssa_convert_from_ssa(SSAContext* ctx) {
    if (!ctx || !ctx->func) return;
    for (int i = 0; i < ctx->func->block_count; i++) {
        IRBasicBlock* bb = ctx->func->blocks[i];
        for (int j = bb->instruction_count - 1; j >= 0; j--) {
            if (bb->instructions[j].opcode != IR_OP_PHI) continue;
            IRInstruction phi = bb->instructions[j];
            IRValue dest = phi.result;
            for (int p = 0; p < phi.operand_count && p < bb->predecessor_count; p++) {
                IRBasicBlock* pred = bb->predecessors[p];
                if (!pred) continue;
                IRInstruction mov;
                mov.opcode = IR_OP_MOV;
                mov.result = dest;
                mov.operands[0] = phi.operands[p];
                mov.operand_count = 1;
                mov.compare_op = IR_CMP_EQ;
                mov.flags = 0;
                mov.source_line = 0;
                mov.source_col = 0;
                mov.source_file = NULL;
                ir_bb_add_instruction(pred, mov);
            }
            for (int k = j; k < bb->instruction_count - 1; k++) {
                bb->instructions[k] = bb->instructions[k + 1];
            }
            bb->instruction_count--;
        }
    }
}

int ssa_validate(SSAContext* ctx) {
    if (!ctx) return 0;
    int valid = 1;
    for (int i = 0; i < ctx->func->block_count; i++) {
        IRBasicBlock* bb = ctx->func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->opcode == IR_OP_PHI) {
                if (inst->result.kind != IR_VALUE_REGISTER) valid = 0;
            }
        }
    }
    return valid;
}