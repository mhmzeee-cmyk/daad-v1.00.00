/**
 * @file liveness.h
 * @brief تحليل الحياة - Liveness Analysis
 */

#ifndef DAAD_LIVENESS_H
#define DAAD_LIVENESS_H

#include "ir_function.h"
#include "ir_basic_block.h"

#define MAX_LIVE_REGS 512

typedef struct {
    int* regs;
    int count;
    int capacity;
} LiveSet;

typedef struct {
    LiveSet use;
    LiveSet def;
    LiveSet live_in;
    LiveSet live_out;
} BlockLiveness;

typedef struct {
    BlockLiveness* block_live;
    int block_count;
    int* all_regs;
    int all_reg_count;
} LivenessResult;

LivenessResult* liveness_analyze(IRFunction* func);
void liveness_destroy(LivenessResult* result);

int liveness_is_live_in(LivenessResult* result, IRBasicBlock* bb, int reg_id);
int liveness_is_live_out(LivenessResult* result, IRBasicBlock* bb, int reg_id);
int liveness_is_used(LivenessResult* result, IRBasicBlock* bb, int reg_id);
int liveness_is_defined(LivenessResult* result, IRBasicBlock* bb, int reg_id);

LiveSet* liveness_get_live_in(LivenessResult* result, IRBasicBlock* bb);
LiveSet* liveness_get_live_out(LivenessResult* result, IRBasicBlock* bb);

#endif
