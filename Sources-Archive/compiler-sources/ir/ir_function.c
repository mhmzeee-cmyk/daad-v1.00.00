/**
 * @file ir_function.c
 * @brief تنفيذ الدوال في IR
 */

#include "ir_function.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

IRFunction* ir_function_create(const char* name, IRType return_type) {
    IRFunction* func = (IRFunction*)calloc(1, sizeof(IRFunction));
    if (!func) return NULL;
    func->name = name ? strdup(name) : NULL;
    func->return_type = return_type;
    func->next_reg = 1;
    func->next_block_id = 0;
    func->block_capacity = 16;
    func->blocks = (IRBasicBlock**)calloc(func->block_capacity, sizeof(IRBasicBlock*));
    if (!func->blocks) { free(func->name); free(func); return NULL; }
    func->alloca_capacity = 16;
    func->alloca_list = (IRValue*)calloc(func->alloca_capacity, sizeof(IRValue));
    if (!func->alloca_list) { free(func->blocks); free(func->name); free(func); return NULL; }
    return func;
}

void ir_function_destroy(IRFunction* func) {
    if (!func) return;
    free(func->name);
    for (int i = 0; i < func->param_count; i++) {
        free(func->param_names[i]);
    }
    for (int i = 0; i < func->block_count; i++) {
        ir_bb_destroy(func->blocks[i]);
    }
    free(func->blocks);
    free(func->alloca_list);
    free(func);
}

IRBasicBlock* ir_function_add_block(IRFunction* func, const char* label) {
    if (!func) return NULL;
    if (func->block_count >= func->block_capacity) {
        int new_cap = func->block_capacity * 2;
        IRBasicBlock** new_blocks = (IRBasicBlock**)realloc(func->blocks, new_cap * sizeof(IRBasicBlock*));
        if (!new_blocks) return NULL;
        func->blocks = new_blocks;
        func->block_capacity = new_cap;
    }
    char label_buf[64];
    if (label) {
        snprintf(label_buf, sizeof(label_buf), "%s", label);
    } else {
        snprintf(label_buf, sizeof(label_buf), "bb%d", func->next_block_id);
    }
    IRBasicBlock* bb = ir_bb_create(label_buf, func->next_block_id++);
    if (!bb) return NULL;
    func->blocks[func->block_count++] = bb;
    if (!func->entry) func->entry = bb;
    func->exit = bb;
    return bb;
}

void ir_function_add_param(IRFunction* func, const char* name, IRType type) {
    if (!func || func->param_count >= IR_MAX_PARAMS) return;
    IRValue param = ir_value_create_register(func->next_reg++, type);
    func->params[func->param_count] = param;
    func->param_types[func->param_count] = type;
    func->param_names[func->param_count] = name ? strdup(name) : NULL;
    func->param_count++;
}

IRValue ir_function_alloc_reg(IRFunction* func, IRType type) {
    if (!func) return ir_value_null();
    return ir_value_create_register(func->next_reg++, type);
}

void ir_function_add_alloca(IRFunction* func, IRValue val) {
    if (!func) return;
    if (func->alloca_count >= func->alloca_capacity) {
        int new_cap = func->alloca_capacity * 2;
        IRValue* new_list = (IRValue*)realloc(func->alloca_list, new_cap * sizeof(IRValue));
        if (!new_list) return;
        func->alloca_list = new_list;
        func->alloca_capacity = new_cap;
    }
    func->alloca_list[func->alloca_count++] = val;
}
