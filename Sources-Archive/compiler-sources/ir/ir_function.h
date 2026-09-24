/**
 * @file ir_function.h
 * @brief دالة في IR
 */

#ifndef DAAD_IR_FUNCTION_H
#define DAAD_IR_FUNCTION_H

#include "ir_basic_block.h"
#include "ir_value.h"

#define IR_MAX_PARAMS 32

typedef struct {
    char* name;
    IRType return_type;
    IRValue params[IR_MAX_PARAMS];
    IRType param_types[IR_MAX_PARAMS];
    char* param_names[IR_MAX_PARAMS];
    int param_count;
    IRBasicBlock* entry;
    IRBasicBlock* exit;
    IRBasicBlock** blocks;
    int block_count;
    int block_capacity;
    int next_reg;
    int next_block_id;
    IRValue* alloca_list;
    int alloca_count;
    int alloca_capacity;
} IRFunction;

IRFunction* ir_function_create(const char* name, IRType return_type);
void ir_function_destroy(IRFunction* func);
IRBasicBlock* ir_function_add_block(IRFunction* func, const char* label);
void ir_function_add_param(IRFunction* func, const char* name, IRType type);
IRValue ir_function_alloc_reg(IRFunction* func, IRType type);
void ir_function_add_alloca(IRFunction* func, IRValue val);

#endif