/**
 * @file ir_value.h
 * @brief قيم IR (Virtual Registers, Constants, Labels)
 */

#ifndef DAAD_IR_VALUE_H
#define DAAD_IR_VALUE_H

#include "ir_types.h"
#include <stddef.h>

typedef enum {
    IR_VALUE_REGISTER,
    IR_VALUE_CONSTANT_INT,
    IR_VALUE_CONSTANT_FLOAT,
    IR_VALUE_CONSTANT_STRING,
    IR_VALUE_LABEL,
    IR_VALUE_NULL
} IRValueKind;

typedef struct {
    IRValueKind kind;
    IRType type;
    int id;
    union {
        long long int_val;
        double float_val;
        const char* string_val;   /* label name for string constants (e.g. ".str0") */
        const char* label_val;
    } as;
    const char* ir_str;           /* actual string data for IR_VALUE_CONSTANT_STRING */
    size_t def_line;
    size_t def_col;
    const char* source_file;
} IRValue;

IRValue ir_value_create_register(int id, IRType type);
IRValue ir_value_create_int(long long val, IRType type);
IRValue ir_value_create_float(double val, IRType type);
IRValue ir_value_create_string(const char* val);
IRValue ir_value_create_string_data(const char* label, const char* data);
IRValue ir_value_create_label(const char* name);
IRValue ir_value_null(void);
const char* ir_value_name(IRValue val);
int ir_value_name_buf(IRValue val, char* buf, size_t buf_size);

#endif