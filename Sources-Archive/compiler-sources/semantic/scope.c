/**
 * @file scope.c
 * @brief تنفيذ النطاقات
 */

#include "scope.h"
#include <stdlib.h>

Scope* scope_create(Scope* parent, int level) {
    Scope* scope = (Scope*)malloc(sizeof(Scope));
    if (!scope) return NULL;
    scope->symbols = symbol_table_create();
    if (!scope->symbols) { free(scope); return NULL; }
    scope->parent = parent;
    scope->level = level;
    return scope;
}

void scope_destroy(Scope* scope) {
    if (!scope) return;
    symbol_table_destroy(scope->symbols);
    free(scope);
}

int scope_define(Scope* scope, Symbol* symbol) {
    if (!scope || !symbol) return 0;
    if (scope_lookup_current(scope, symbol->name)) return 0;
    symbol->scope_level = scope->level;
    symbol_table_insert(scope->symbols, symbol);
    return 1;
}

Symbol* scope_lookup(Scope* scope, const char* name) {
    if (!scope || !name) return NULL;
    Symbol* sym = scope_lookup_current(scope, name);
    if (sym) return sym;
    return scope_lookup(scope->parent, name);
}

Symbol* scope_lookup_current(Scope* scope, const char* name) {
    if (!scope || !name) return NULL;
    return symbol_table_lookup(scope->symbols, name);
}