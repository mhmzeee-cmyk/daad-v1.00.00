/**
 * @file ir_basic_block.c
 * @brief تنفيذ الكتل الأساسية
 */

#include "ir_basic_block.h"
#include <stdlib.h>
#include <string.h>

IRBasicBlock* ir_bb_create(const char* label, int id) {
    IRBasicBlock* bb = (IRBasicBlock*)calloc(1, sizeof(IRBasicBlock));
    if (!bb) return NULL;
    bb->label = label ? strdup(label) : NULL;
    bb->block_id = id;
    return bb;
}

void ir_bb_destroy(IRBasicBlock* bb) {
    if (!bb) return;
    free(bb->label);
    free(bb);
}

void ir_bb_add_instruction(IRBasicBlock* bb, IRInstruction inst) {
    if (!bb || bb->instruction_count >= IR_MAX_INSTRUCTIONS_PER_BLOCK) return;
    bb->instructions[bb->instruction_count++] = inst;
}

void ir_bb_add_successor(IRBasicBlock* bb, IRBasicBlock* successor) {
    if (!bb || !successor || bb->successor_count >= 4) return;
    bb->successors[bb->successor_count++] = successor;
}

void ir_bb_add_predecessor(IRBasicBlock* bb, IRBasicBlock* predecessor) {
    if (!bb || !predecessor || bb->predecessor_count >= 4) return;
    bb->predecessors[bb->predecessor_count++] = predecessor;
}

IRInstruction* ir_bb_get_last_instruction(IRBasicBlock* bb) {
    if (!bb || bb->instruction_count == 0) return NULL;
    return &bb->instructions[bb->instruction_count - 1];
}

int ir_bb_terminates(IRBasicBlock* bb) {
    IRInstruction* last = ir_bb_get_last_instruction(bb);
    if (!last) return 0;
    return last->opcode == IR_OP_JMP || last->opcode == IR_OP_JE ||
           last->opcode == IR_OP_JNE || last->opcode == IR_OP_JG ||
           last->opcode == IR_OP_JL || last->opcode == IR_OP_JGE ||
           last->opcode == IR_OP_JLE || last->opcode == IR_OP_RET;
}

void ir_bb_clear_visited(IRBasicBlock* bb) {
    if (bb) bb->visited = 0;
}
