/**
 * @file symbol_table.h
 * @brief جدول الرموز (Hash Map)
 */

#ifndef DAAD_SYMBOL_TABLE_H
#define DAAD_SYMBOL_TABLE_H

#include "symbol.h"
#include <stddef.h>

#define SYMBOL_TABLE_CAPACITY 256

typedef struct SymbolEntry {
    Symbol* symbol;
    struct SymbolEntry* next;
} SymbolEntry;

typedef struct {
    SymbolEntry* buckets[SYMBOL_TABLE_CAPACITY];
    size_t size;
} SymbolTable;

SymbolTable* symbol_table_create(void);
void symbol_table_insert(SymbolTable* table, Symbol* symbol);
Symbol* symbol_table_lookup(SymbolTable* table, const char* name);
void symbol_table_remove(SymbolTable* table, const char* name);
void symbol_table_destroy(SymbolTable* table);

#endif