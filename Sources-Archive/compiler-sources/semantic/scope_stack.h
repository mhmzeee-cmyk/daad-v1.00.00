/**
 * @file scope_stack.h
 * @brief كومة النطاقات
 */

#ifndef DAAD_SCOPE_STACK_H
#define DAAD_SCOPE_STACK_H

#include "scope.h"

#define MAX_SCOPE_DEPTH 64

typedef struct {
    Scope* scopes[MAX_SCOPE_DEPTH];
    int top;
    int current_level;
} ScopeStack;

ScopeStack* scope_stack_create(void);
void scope_stack_destroy(ScopeStack* stack);
void scope_stack_push(ScopeStack* stack);
void scope_stack_pop(ScopeStack* stack);
Scope* scope_stack_peek(ScopeStack* stack);
Scope* scope_stack_get_global(ScopeStack* stack);
int scope_stack_define(ScopeStack* stack, Symbol* symbol);
Symbol* scope_stack_lookup(ScopeStack* stack, const char* name);

#endif