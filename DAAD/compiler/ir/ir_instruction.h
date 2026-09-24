/**
 * @file ir_instruction.h
 * @brief تعليمات IR
 */

#ifndef DAAD_IR_INSTRUCTION_H
#define DAAD_IR_INSTRUCTION_H

#include "ir_value.h"
#include <stddef.h>

typedef enum {
    IR_OP_NOP,
    IR_OP_LOAD,
    IR_OP_STORE,
    IR_OP_MOV,
    IR_OP_ADD,
    IR_OP_SUB,
    IR_OP_MUL,
    IR_OP_DIV,
    IR_OP_MOD,
    IR_OP_NEG,
    IR_OP_NOT,
    IR_OP_AND,
    IR_OP_OR,
    IR_OP_XOR,
    IR_OP_SHL,
    IR_OP_SHR,
    IR_OP_CMP,
    IR_OP_JMP,
    IR_OP_JE,
    IR_OP_JNE,
    IR_OP_JG,
    IR_OP_JL,
    IR_OP_JGE,
    IR_OP_JLE,
    IR_OP_CALL,
    IR_OP_RET,
    IR_OP_PUSH,
    IR_OP_POP,
    IR_OP_PHI,
    IR_OP_CAST,
    IR_OP_ALLOCA,
    IR_OP_LOAD_ELEMENT,
    IR_OP_STORE_ELEMENT,
    IR_OP_MEMBER_ACCESS,
    IR_OP_LEA
} IROpcode;

typedef enum {
    IR_CMP_EQ,
    IR_CMP_NE,
    IR_CMP_GT,
    IR_CMP_LT,
    IR_CMP_GE,
    IR_CMP_LE
} IRCompareOp;

typedef struct {
    IROpcode opcode;
    IRValue result;
    IRValue operands[8];
    int operand_count;
    IRCompareOp compare_op;
    int flags;
    size_t source_line;
    size_t source_col;
    const char* source_file;
} IRInstruction;

IRInstruction ir_inst_create(IROpcode opcode);
IRInstruction ir_inst_create_1(IROpcode opcode, IRValue result, IRValue op1);
IRInstruction ir_inst_create_2(IROpcode opcode, IRValue result, IRValue op1, IRValue op2);
IRInstruction ir_inst_create_3(IROpcode opcode, IRValue result, IRValue op1, IRValue op2, IRValue op3);
IRInstruction ir_inst_nop(void);
IRInstruction ir_inst_ret(IRValue val);
IRInstruction ir_inst_jmp(const char* label);
IRInstruction ir_inst_cond_jmp(IRCompareOp cmp, IRValue left, IRValue right, const char* label_true, const char* label_false);
IRInstruction ir_inst_call(IRValue result, const char* func_name, IRValue* args, int arg_count);
IRInstruction ir_inst_phi(IRValue result, IRValue* incomings, int count);
const char* ir_opcode_name(IROpcode op);
const char* ir_cmp_name(IRCompareOp cmp);

#endif