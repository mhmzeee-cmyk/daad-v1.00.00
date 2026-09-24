/**
 * @file symbol.h
 * @brief رمز (Symbol) في جدول الرموز
 */

#ifndef DAAD_SYMBOL_H
#define DAAD_SYMBOL_H

#include "type.h"

typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_CONSTANT,
    SYMBOL_FUNCTION,
    SYMBOL_PARAMETER,
    SYMBOL_STRUCT
} SymbolKind;

typedef struct {
    char* name;
    SymbolKind kind;
    DaadType* type;
    int scope_level;
    int is_mutable;
    int is_initialized;
    int is_variadic;
} Symbol;

Symbol* symbol_create(const char* name, SymbolKind kind, DaadType* type, int scope_level);
void symbol_destroy(Symbol* sym);

#endif