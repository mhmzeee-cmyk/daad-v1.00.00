/**
 * @file ir_types.c
 * @brief تنفيذ أنواع IR
 */

#include "ir_types.h"

IRType ir_type_create(IRTypeKind kind, int bit_width) {
    IRType t;
    t.kind = kind;
    t.bit_width = bit_width;
    return t;
}

IRType ir_type_void(void) { return ir_type_create(IR_TYPE_VOID, 0); }
IRType ir_type_i8(void) { return ir_type_create(IR_TYPE_I8, 8); }
IRType ir_type_i16(void) { return ir_type_create(IR_TYPE_I16, 16); }
IRType ir_type_i32(void) { return ir_type_create(IR_TYPE_I32, 32); }
IRType ir_type_i64(void) { return ir_type_create(IR_TYPE_I64, 64); }
IRType ir_type_f32(void) { return ir_type_create(IR_TYPE_F32, 32); }
IRType ir_type_f64(void) { return ir_type_create(IR_TYPE_F64, 64); }
IRType ir_type_ptr(void) { return ir_type_create(IR_TYPE_PTR, 64); }
IRType ir_type_label(void) { return ir_type_create(IR_TYPE_LABEL, 0); }

int ir_type_equals(IRType a, IRType b) {
    return a.kind == b.kind && a.bit_width == b.bit_width;
}

int ir_type_is_integer(IRType t) {
    return t.kind == IR_TYPE_I8 || t.kind == IR_TYPE_I16 ||
           t.kind == IR_TYPE_I32 || t.kind == IR_TYPE_I64;
}

int ir_type_is_float(IRType t) {
    return t.kind == IR_TYPE_F32 || t.kind == IR_TYPE_F64;
}

int ir_type_is_numeric(IRType t) {
    return ir_type_is_integer(t) || ir_type_is_float(t);
}

int ir_type_size_bytes(IRType t) {
    switch (t.kind) {
        case IR_TYPE_I8: return 1;
        case IR_TYPE_I16: return 2;
        case IR_TYPE_I32: return 4;
        case IR_TYPE_I64: case IR_TYPE_PTR: return 8;
        case IR_TYPE_F32: return 4;
        case IR_TYPE_F64: return 8;
        default: return 0;
    }
}