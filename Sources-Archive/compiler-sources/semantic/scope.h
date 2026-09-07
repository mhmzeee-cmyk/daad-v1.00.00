/**
 * @file scope.h
 * @brief نطاق (Scope) في التحليل الدلالي
 */

#ifndef DAAD_SCOPE_H
#define DAAD_SCOPE_H

#include "symbol_table.h"

typedef struct Scope {
    SymbolTable* symbols;
    struct Scope* parent;
    int level;
} Scope;

Scope* scope_create(Scope* parent, int level);
void scope_destroy(Scope* scope);
int scope_define(Scope* scope, Symbol* symbol);
Symbol* scope_lookup(Scope* scope, const char* name);
Symbol* scope_lookup_current(Scope* scope, const char* name);

#endif