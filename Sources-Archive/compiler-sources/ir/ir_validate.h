/**
 * @file ir_validate.h
 * @brief التحقق من صحة IR
 */

#ifndef DAAD_IR_VALIDATE_H
#define DAAD_IR_VALIDATE_H

#include "ir_module.h"
#include "ir_function.h"
#include "ir_basic_block.h"
#include "ir_instruction.h"

typedef struct {
    int is_valid;
    char error_message[256];
    size_t error_line;
    size_t error_col;
} IRValidationResult;

IRValidationResult ir_validate_module(IRModule* module);
IRValidationResult ir_validate_function(IRFunction* func);
IRValidationResult ir_validate_basic_block(IRBasicBlock* bb, IRFunction* func);
IRValidationResult ir_validate_instruction(IRInstruction* inst, IRFunction* func);

#endif
