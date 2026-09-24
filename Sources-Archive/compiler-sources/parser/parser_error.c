/**
 * @file parser_error.c
 * @brief تنفيذ دوال أخطاء التحليل النحوي
 *
 * @version 0.1.0
 * @date 2026-08-03
 */

#include "parser_error.h"
#include <stdlib.h>
#include <string.h>

#define PARSER_ERROR_INIT_CAPACITY 16

void parser_error_list_init(ParserErrorList* list) {
    if (!list) return;
    list->errors = NULL;
    list->count = 0;
    list->capacity = 0;
}

void parser_error_list_add(ParserErrorList* list, const char* code,
                           const char* msg, size_t line, size_t col,
                           const char* fn) {
    if (!list || !code || !msg) return;

    /* توسيع المصفوفة إذا لزم */
    if (list->count >= list->capacity) {
        int new_cap = list->capacity == 0
                          ? PARSER_ERROR_INIT_CAPACITY
                          : list->capacity * 2;
        ParserError** new_arr = (ParserError**)realloc(
            list->errors, (size_t)new_cap * sizeof(ParserError*));
        if (!new_arr) return;
        list->errors = new_arr;
        list->capacity = new_cap;
    }

    ParserError* err = (ParserError*)malloc(sizeof(ParserError));
    if (!err) return;

    err->code = code;
    err->message = strdup(msg);
    err->line = line;
    err->column = col;
    err->filename = fn;

    list->errors[list->count++] = err;
}

void parser_error_list_destroy(ParserErrorList* list) {
    if (!list) return;

    for (int i = 0; i < list->count; i++) {
        if (list->errors[i]) {
            free(list->errors[i]->message);
            free(list->errors[i]);
        }
    }
    free(list->errors);
    list->errors = NULL;
    list->count = 0;
    list->capacity = 0;
}

void parser_error_print(ParserError* err, FILE* file) {
    if (!err || !file) return;

    fprintf(file, "%s:%zu:%zu: %s: %s\n",
            err->filename ? err->filename : "<stdin>",
            err->line, err->column,
            err->code, err->message);
}
