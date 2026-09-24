/**
 * @file ir_types.h
 * @brief أنواع IR في ض Core
 */

#ifndef DAAD_IR_TYPES_H
#define DAAD_IR_TYPES_H

typedef enum {
    IR_TYPE_VOID,
    IR_TYPE_I8,
    IR_TYPE_I16,
    IR_TYPE_I32,
    IR_TYPE_I64,
    IR_TYPE_F32,
    IR_TYPE_F64,
    IR_TYPE_PTR,
    IR_TYPE_LABEL
} IRTypeKind;

typedef struct {
    IRTypeKind kind;
    int bit_width;
} IRType;

IRType ir_type_create(IRTypeKind kind, int bit_width);
IRType ir_type_void(void);
IRType ir_type_i8(void);
IRType ir_type_i16(void);
IRType ir_type_i32(void);
IRType ir_type_i64(void);
IRType ir_type_f32(void);
IRType ir_type_f64(void);
IRType ir_type_ptr(void);
IRType ir_type_label(void);
int ir_type_equals(IRType a, IRType b);
int ir_type_is_integer(IRType t);
int ir_type_is_float(IRType t);
int ir_type_is_numeric(IRType t);
int ir_type_size_bytes(IRType t);

#endif