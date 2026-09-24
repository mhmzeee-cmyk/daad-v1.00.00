/**
 * @file ir_module.h
 * @brief وحدة IR (أعلى مستوى)
 */

#ifndef DAAD_IR_MODULE_H
#define DAAD_IR_MODULE_H

#include "ir_function.h"

#define IR_MAX_GLOBALS 256
#define IR_MAX_STRINGS 1024

typedef struct {
    char* name;
    IRType type;
    IRValue init_val;
    int is_constant;
} IRGlobal;

typedef struct {
    const char* str;
    char label[64];
} IRStringEntry;

typedef struct {
    char* name;
    IRFunction** functions;
    int function_count;
    int function_capacity;
    IRGlobal globals[IR_MAX_GLOBALS];
    int global_count;
    IRStringEntry strings[IR_MAX_STRINGS];
    int string_count;
} IRModule;

IRModule* ir_module_create(const char* name);
void ir_module_destroy(IRModule* module);
IRFunction* ir_module_add_function(IRModule* module, const char* name, IRType return_type);
void ir_module_add_global(IRModule* module, const char* name, IRType type, IRValue init_val, int is_const);
const char* ir_module_register_string(IRModule* module, const char* str);

#endif