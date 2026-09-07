/**
 * @file ir_module.c
 * @brief تنفيذ الوحدة
 */

#include "ir_module.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

IRModule* ir_module_create(const char* name) {
    IRModule* module = (IRModule*)calloc(1, sizeof(IRModule));
    if (!module) return NULL;
    module->name = name ? strdup(name) : NULL;
    module->function_capacity = 16;
    module->functions = (IRFunction**)calloc(module->function_capacity, sizeof(IRFunction*));
    if (!module->functions) { free(module->name); free(module); return NULL; }
    return module;
}

void ir_module_destroy(IRModule* module) {
    if (!module) return;
    free(module->name);
    for (int i = 0; i < module->function_count; i++) {
        ir_function_destroy(module->functions[i]);
    }
    free(module->functions);
    for (int i = 0; i < module->global_count; i++) {
        free(module->globals[i].name);
    }
    free(module);
}

IRFunction* ir_module_add_function(IRModule* module, const char* name, IRType return_type) {
    if (!module) return NULL;
    if (module->function_count >= module->function_capacity) {
        int new_cap = module->function_capacity * 2;
        IRFunction** tmp = (IRFunction**)realloc(module->functions, new_cap * sizeof(IRFunction*));
        if (!tmp) return NULL;
        module->functions = tmp;
        module->function_capacity = new_cap;
    }
    IRFunction* func = ir_function_create(name, return_type);
    if (!func) return NULL;
    module->functions[module->function_count++] = func;
    return func;
}

void ir_module_add_global(IRModule* module, const char* name, IRType type, IRValue init_val, int is_const) {
    if (!module || module->global_count >= IR_MAX_GLOBALS) return;
    IRGlobal* g = &module->globals[module->global_count++];
    g->name = name ? strdup(name) : NULL;
    g->type = type;
    g->init_val = init_val;
    g->is_constant = is_const;
}

const char* ir_module_register_string(IRModule* module, const char* str) {
    if (!module || !str) return NULL;
    for (int i = 0; i < module->string_count; i++) {
        if (strcmp(module->strings[i].str, str) == 0) {
            return module->strings[i].label;
        }
    }
    if (module->string_count >= IR_MAX_STRINGS) return NULL;
    IRStringEntry* entry = &module->strings[module->string_count++];
    entry->str = str;
    snprintf(entry->label, sizeof(entry->label), ".str%d", module->string_count - 1);
    return entry->label;
}
