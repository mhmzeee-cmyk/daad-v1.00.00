/**
 * @file scope_stack.c
 * @brief تنفيذ كومة النطاقات
 */

#include "scope_stack.h"
#include <stdlib.h>

ScopeStack* scope_stack_create(void) {
    ScopeStack* stack = (ScopeStack*)calloc(1, sizeof(ScopeStack));
    if (!stack) return NULL;
    stack->top = -1;
    stack->current_level = 0;
    scope_stack_push(stack);
    return stack;
}

void scope_stack_destroy(ScopeStack* stack) {
    if (!stack) return;
    while (stack->top >= 0) scope_stack_pop(stack);
    free(stack);
}

void scope_stack_push(ScopeStack* stack) {
    if (!stack || stack->top >= MAX_SCOPE_DEPTH - 1) return;
    stack->top++;
    Scope* parent = stack->top > 0 ? stack->scopes[stack->top - 1] : NULL;
    stack->scopes[stack->top] = scope_create(parent, stack->current_level);
    stack->current_level++;
}

void scope_stack_pop(ScopeStack* stack) {
    if (!stack || stack->top < 0) return;
    scope_destroy(stack->scopes[stack->top]);
    stack->scopes[stack->top] = NULL;
    stack->top--;
    if (stack->current_level > 0) stack->current_level--;
}

Scope* scope_stack_peek(ScopeStack* stack) {
    if (!stack || stack->top < 0) return NULL;
    return stack->scopes[stack->top];
}

Scope* scope_stack_get_global(ScopeStack* stack) {
    if (!stack || stack->top < 0) return NULL;
    return stack->scopes[0];
}

int scope_stack_define(ScopeStack* stack, Symbol* symbol) {
    Scope* scope = scope_stack_peek(stack);
    return scope_define(scope, symbol);
}

Symbol* scope_stack_lookup(ScopeStack* stack, const char* name) {
    Scope* scope = scope_stack_peek(stack);
    return scope_lookup(scope, name);
}