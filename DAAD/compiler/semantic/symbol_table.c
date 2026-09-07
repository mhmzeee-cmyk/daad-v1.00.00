/**
 * @file symbol_table.c
 * @brief تنفيذ جدول الرموز
 */

#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>

static unsigned int hash_string(const char* str) {
    unsigned int hash = 5381;
    while (*str) {
        hash = ((hash << 5) + hash) + (unsigned char)*str;
        str++;
    }
    return hash % SYMBOL_TABLE_CAPACITY;
}

SymbolTable* symbol_table_create(void) {
    SymbolTable* table = (SymbolTable*)calloc(1, sizeof(SymbolTable));
    return table;
}

void symbol_table_insert(SymbolTable* table, Symbol* symbol) {
    if (!table || !symbol) return;
    unsigned int index = hash_string(symbol->name);
    SymbolEntry* entry = (SymbolEntry*)malloc(sizeof(SymbolEntry));
    if (!entry) return;
    entry->symbol = symbol;
    entry->next = table->buckets[index];
    table->buckets[index] = entry;
    table->size++;
}

Symbol* symbol_table_lookup(SymbolTable* table, const char* name) {
    if (!table || !name) return NULL;
    unsigned int index = hash_string(name);
    SymbolEntry* entry = table->buckets[index];
    while (entry) {
        if (strcmp(entry->symbol->name, name) == 0) return entry->symbol;
        entry = entry->next;
    }
    return NULL;
}

void symbol_table_remove(SymbolTable* table, const char* name) {
    if (!table || !name) return;
    unsigned int index = hash_string(name);
    SymbolEntry** prev = &table->buckets[index];
    SymbolEntry* entry = *prev;
    while (entry) {
        if (strcmp(entry->symbol->name, name) == 0) {
            *prev = entry->next;
            symbol_destroy(entry->symbol);
            free(entry);
            table->size--;
            return;
        }
        prev = &entry->next;
        entry = entry->next;
    }
}

void symbol_table_destroy(SymbolTable* table) {
    if (!table) return;
    for (size_t i = 0; i < SYMBOL_TABLE_CAPACITY; i++) {
        SymbolEntry* entry = table->buckets[i];
        while (entry) {
            SymbolEntry* next = entry->next;
            symbol_destroy(entry->symbol);
            free(entry);
            entry = next;
        }
    }
    free(table);
}