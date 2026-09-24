/**
 * @file ir_value.c
 * @brief تنفيذ قيم IR
 */

#include "ir_value.h"
#include <stdio.h>
#include <stdlib.h>

IRValue ir_value_create_register(int id, IRType type) {
    IRValue v;
    v.kind = IR_VALUE_REGISTER;
    v.type = type;
    v.id = id;
    v.as.int_val = 0;
    v.ir_str = NULL;
    v.def_line = 0;
    v.def_col = 0;
    v.source_file = NULL;
    return v;
}

IRValue ir_value_create_int(long long val, IRType type) {
    IRValue v;
    v.kind = IR_VALUE_CONSTANT_INT;
    v.type = type;
    v.id = -1;
    v.as.int_val = val;
    v.ir_str = NULL;
    v.def_line = 0;
    v.def_col = 0;
    v.source_file = NULL;
    return v;
}

IRValue ir_value_create_float(double val, IRType type) {
    IRValue v;
    v.kind = IR_VALUE_CONSTANT_FLOAT;
    v.type = type;
    v.id = -1;
    v.as.float_val = val;
    v.ir_str = NULL;
    v.def_line = 0;
    v.def_col = 0;
    v.source_file = NULL;
    return v;
}

IRValue ir_value_create_string(const char* val) {
    IRValue v;
    v.kind = IR_VALUE_CONSTANT_STRING;
    v.type = ir_type_ptr();
    v.id = -1;
    v.as.string_val = val;
    v.ir_str = NULL;
    v.def_line = 0;
    v.def_col = 0;
    v.source_file = NULL;
    return v;
}

IRValue ir_value_create_string_data(const char* label, const char* data) {
    IRValue v;
    v.kind = IR_VALUE_CONSTANT_STRING;
    v.type = ir_type_ptr();
    v.id = -1;
    v.as.string_val = label;
    v.ir_str = data;
    v.def_line = 0;
    v.def_col = 0;
    v.source_file = NULL;
    return v;
}

IRValue ir_value_create_label(const char* name) {
    IRValue v;
    v.kind = IR_VALUE_LABEL;
    v.type = ir_type_label();
    v.id = -1;
    v.as.label_val = name;
    v.ir_str = NULL;
    v.def_line = 0;
    v.def_col = 0;
    v.source_file = NULL;
    return v;
}

IRValue ir_value_null(void) {
    IRValue v;
    v.kind = IR_VALUE_NULL;
    v.type = ir_type_ptr();
    v.id = -1;
    v.as.int_val = 0;
    v.ir_str = NULL;
    v.def_line = 0;
    v.def_col = 0;
    v.source_file = NULL;
    return v;
}

const char* ir_value_name(IRValue val) {
    static char buf[64];
    ir_value_name_buf(val, buf, sizeof(buf));
    return buf;
}

int ir_value_name_buf(IRValue val, char* buf, size_t buf_size) {
    if (!buf || buf_size == 0) return 0;
    switch (val.kind) {
        case IR_VALUE_REGISTER:
            return snprintf(buf, buf_size, "r%d", val.id);
        case IR_VALUE_CONSTANT_INT:
            return snprintf(buf, buf_size, "%lld", val.as.int_val);
        case IR_VALUE_CONSTANT_FLOAT:
            return snprintf(buf, buf_size, "%f", val.as.float_val);
        case IR_VALUE_CONSTANT_STRING:
            if (val.as.string_val) {
                return snprintf(buf, buf_size, "%s", val.as.string_val);
            }
            return snprintf(buf, buf_size, "\"\"");
        case IR_VALUE_LABEL:
            if (val.as.label_val) {
                return snprintf(buf, buf_size, "%s", val.as.label_val);
            }
            return snprintf(buf, buf_size, "label");
        case IR_VALUE_NULL:
            return snprintf(buf, buf_size, "null");
    }
    return snprintf(buf, buf_size, "?");
}
