/**
 * @file ir_basic_block.h
 * @brief الكتل الأساسية في IR
 */

#ifndef DAAD_IR_BASIC_BLOCK_H
#define DAAD_IR_BASIC_BLOCK_H

#include "ir_instruction.h"

#define IR_MAX_INSTRUCTIONS_PER_BLOCK 256

typedef struct IRBasicBlock {
    char* label;
    IRInstruction instructions[IR_MAX_INSTRUCTIONS_PER_BLOCK];
    int instruction_count;
    struct IRBasicBlock* successors[4];
    int successor_count;
    struct IRBasicBlock* predecessors[4];
    int predecessor_count;
    int visited;
    int block_id;
} IRBasicBlock;

IRBasicBlock* ir_bb_create(const char* label, int id);
void ir_bb_destroy(IRBasicBlock* bb);
void ir_bb_add_instruction(IRBasicBlock* bb, IRInstruction inst);
void ir_bb_add_successor(IRBasicBlock* bb, IRBasicBlock* successor);
void ir_bb_add_predecessor(IRBasicBlock* bb, IRBasicBlock* predecessor);
IRInstruction* ir_bb_get_last_instruction(IRBasicBlock* bb);
int ir_bb_terminates(IRBasicBlock* bb);
void ir_bb_clear_visited(IRBasicBlock* bb);

#endif