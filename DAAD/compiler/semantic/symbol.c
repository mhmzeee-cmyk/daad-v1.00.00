/**
 * @file symbol.c
 * @brief تنفيذ الرموز
 */

#include "symbol.h"
#include <stdlib.h>
#include <string.h>

Symbol* symbol_create(const char* name, SymbolKind kind, DaadType* type, int scope_level) {
    if (!name) return NULL;
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    if (!sym) return NULL;

    sym->name = strdup(name);
    sym->kind = kind;
    sym->type = type;
    sym->scope_level = scope_level;
    sym->is_mutable = (kind == SYMBOL_VARIABLE || kind == SYMBOL_PARAMETER);
    sym->is_initialized = 0;

    return sym;
}

void symbol_destroy(Symbol* sym) {
    if (!sym) return;
    free(sym->name);
    free(sym);
}
