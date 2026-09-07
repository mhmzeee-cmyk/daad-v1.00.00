/**
 * @file liveness.c
 * @brief تحليل الحياة - Liveness Analysis
 *
 * Backward dataflow analysis:
 *   LiveIn[B] = Use[B] ∪ (LiveOut[B] - Def[B])
 *   LiveOut[B] = ∪ LiveIn[S] for all successors S of B
 */

#include "liveness.h"
#include <stdlib.h>
#include <string.h>

static void live_set_init(LiveSet* s) {
    s->regs = NULL;
    s->count = 0;
    s->capacity = 0;
}

static void live_set_destroy(LiveSet* s) {
    free(s->regs);
}

static void live_set_add(LiveSet* s, int reg_id) {
    for (int i = 0; i < s->count; i++) {
        if (s->regs[i] == reg_id) return;
    }
    if (s->count >= s->capacity) {
        int new_cap = s->capacity == 0 ? 16 : s->capacity * 2;
        int* tmp = (int*)realloc(s->regs, new_cap * sizeof(int));
        if (!tmp) return;
        s->regs = tmp;
        s->capacity = new_cap;
    }
    s->regs[s->count++] = reg_id;
}

static int live_set_contains(LiveSet* s, int reg_id) {
    for (int i = 0; i < s->count; i++) {
        if (s->regs[i] == reg_id) return 1;
    }
    return 0;
}

static void live_set_union(LiveSet* dst, LiveSet* src) {
    for (int i = 0; i < src->count; i++) {
        live_set_add(dst, src->regs[i]);
    }
}

static void live_set_subtract(LiveSet* dst, LiveSet* sub) {
    int write = 0;
    for (int read = 0; read < dst->count; read++) {
        int found = 0;
        for (int j = 0; j < sub->count; j++) {
            if (dst->regs[read] == sub->regs[j]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            dst->regs[write++] = dst->regs[read];
        }
    }
    dst->count = write;
}

static int live_set_equals(LiveSet* a, LiveSet* b) {
    if (a->count != b->count) return 0;
    for (int i = 0; i < a->count; i++) {
        if (!live_set_contains(b, a->regs[i])) return 0;
    }
    return 1;
}

static void compute_use_def(IRBasicBlock* bb, LiveSet* use, LiveSet* def) {
    for (int i = 0; i < bb->instruction_count; i++) {
        IRInstruction* inst = &bb->instructions[i];

        for (int o = 0; o < inst->operand_count; o++) {
            if (inst->operands[o].kind == IR_VALUE_REGISTER) {
                if (!live_set_contains(def, inst->operands[o].id)) {
                    live_set_add(use, inst->operands[o].id);
                }
            }
        }

        if (inst->opcode == IR_OP_PHI) {
            for (int o = 0; o < inst->operand_count; o += 2) {
                if (o + 1 < inst->operand_count &&
                    inst->operands[o].kind == IR_VALUE_REGISTER) {
                    if (!live_set_contains(def, inst->operands[o].id)) {
                        live_set_add(use, inst->operands[o].id);
                    }
                }
            }
        }

        if (inst->result.kind == IR_VALUE_REGISTER) {
            live_set_add(def, inst->result.id);
        }
    }
}

LivenessResult* liveness_analyze(IRFunction* func) {
    if (!func) return NULL;

    LivenessResult* result = (LivenessResult*)calloc(1, sizeof(LivenessResult));
    if (!result) return NULL;

    result->block_count = func->block_count;
    result->block_live = (BlockLiveness*)calloc(result->block_count, sizeof(BlockLiveness));
    if (!result->block_live) {
        free(result);
        return NULL;
    }

    for (int i = 0; i < result->block_count; i++) {
        live_set_init(&result->block_live[i].use);
        live_set_init(&result->block_live[i].def);
        live_set_init(&result->block_live[i].live_in);
        live_set_init(&result->block_live[i].live_out);
    }

    for (int i = 0; i < result->block_count; i++) {
        compute_use_def(func->blocks[i],
                        &result->block_live[i].use,
                        &result->block_live[i].def);
    }

    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = result->block_count - 1; i >= 0; i--) {
            IRBasicBlock* bb = func->blocks[i];
            BlockLiveness* bl = &result->block_live[i];

            LiveSet old_in;
            live_set_init(&old_in);
            live_set_union(&old_in, &bl->live_in);

            live_set_destroy(&bl->live_in);
            live_set_init(&bl->live_in);
            live_set_union(&bl->live_in, &bl->use);

            LiveSet succ_union;
            live_set_init(&succ_union);
            for (int s = 0; s < bb->successor_count; s++) {
                int succ_idx = -1;
                for (int j = 0; j < result->block_count; j++) {
                    if (func->blocks[j] == bb->successors[s]) {
                        succ_idx = j;
                        break;
                    }
                }
                if (succ_idx >= 0) {
                    live_set_union(&succ_union, &result->block_live[succ_idx].live_in);
                }
            }

            LiveSet temp;
            live_set_init(&temp);
            live_set_union(&temp, &succ_union);
            live_set_subtract(&temp, &bl->def);
            live_set_union(&bl->live_in, &temp);

            live_set_destroy(&temp);
            live_set_destroy(&succ_union);

            if (!live_set_equals(&old_in, &bl->live_in)) {
                changed = 1;
            }
            live_set_destroy(&old_in);
        }
    }

    for (int i = 0; i < result->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        BlockLiveness* bl = &result->block_live[i];

        live_set_destroy(&bl->live_out);
        live_set_init(&bl->live_out);

        for (int s = 0; s < bb->successor_count; s++) {
            int succ_idx = -1;
            for (int j = 0; j < result->block_count; j++) {
                if (func->blocks[j] == bb->successors[s]) {
                    succ_idx = j;
                    break;
                }
            }
            if (succ_idx >= 0) {
                live_set_union(&bl->live_out, &result->block_live[succ_idx].live_in);
            }
        }
    }

    int max_reg = 0;
    for (int i = 0; i < result->block_count; i++) {
        BlockLiveness* bl = &result->block_live[i];
        for (int j = 0; j < bl->use.count; j++) {
            if (bl->use.regs[j] > max_reg) max_reg = bl->use.regs[j];
        }
        for (int j = 0; j < bl->def.count; j++) {
            if (bl->def.regs[j] > max_reg) max_reg = bl->def.regs[j];
        }
    }
    result->all_reg_count = max_reg + 1;
    result->all_regs = (int*)calloc(result->all_reg_count, sizeof(int));
    if (result->all_regs) {
        for (int i = 0; i < result->all_reg_count; i++) {
            result->all_regs[i] = i;
        }
    }

    return result;
}

void liveness_destroy(LivenessResult* result) {
    if (!result) return;
    if (result->block_live) {
        for (int i = 0; i < result->block_count; i++) {
            live_set_destroy(&result->block_live[i].use);
            live_set_destroy(&result->block_live[i].def);
            live_set_destroy(&result->block_live[i].live_in);
            live_set_destroy(&result->block_live[i].live_out);
        }
        free(result->block_live);
    }
    free(result->all_regs);
    free(result);
}

int liveness_is_live_in(LivenessResult* result, IRBasicBlock* bb, int reg_id) {
    if (!result || !bb) return 0;
    for (int i = 0; i < result->block_count; i++) {
        if (bb->block_id >= 0 && i == bb->block_id) {
            return live_set_contains(&result->block_live[i].live_in, reg_id);
        }
    }
    return 0;
}

int liveness_is_live_out(LivenessResult* result, IRBasicBlock* bb, int reg_id) {
    if (!result || !bb) return 0;
    for (int i = 0; i < result->block_count; i++) {
        if (bb->block_id >= 0 && i == bb->block_id) {
            return live_set_contains(&result->block_live[i].live_out, reg_id);
        }
    }
    return 0;
}

int liveness_is_used(LivenessResult* result, IRBasicBlock* bb, int reg_id) {
    if (!result || !bb) return 0;
    for (int i = 0; i < result->block_count; i++) {
        if (bb->block_id >= 0 && i == bb->block_id) {
            return live_set_contains(&result->block_live[i].use, reg_id);
        }
    }
    return 0;
}

int liveness_is_defined(LivenessResult* result, IRBasicBlock* bb, int reg_id) {
    if (!result || !bb) return 0;
    for (int i = 0; i < result->block_count; i++) {
        if (bb->block_id >= 0 && i == bb->block_id) {
            return live_set_contains(&result->block_live[i].def, reg_id);
        }
    }
    return 0;
}

LiveSet* liveness_get_live_in(LivenessResult* result, IRBasicBlock* bb) {
    if (!result || !bb) return NULL;
    for (int i = 0; i < result->block_count; i++) {
        if (bb->block_id >= 0 && i == bb->block_id) {
            return &result->block_live[i].live_in;
        }
    }
    return NULL;
}

LiveSet* liveness_get_live_out(LivenessResult* result, IRBasicBlock* bb) {
    if (!result || !bb) return NULL;
    for (int i = 0; i < result->block_count; i++) {
        if (bb->block_id >= 0 && i == bb->block_id) {
            return &result->block_live[i].live_out;
        }
    }
    return NULL;
}
