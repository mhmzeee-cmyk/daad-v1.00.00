/**
 * @file type.c
 * @brief تنفيذ تمثيل الأنواع في نظام ض Core
 */

#include "type.h"
#include <stdlib.h>
#include <string.h>

DaadType* daad_type_create(DaadTypeKind kind, const char* name, size_t size) {
    DaadType* type = (DaadType*)malloc(sizeof(DaadType));
    if (!type) return NULL;

    type->kind = kind;
    type->name = name;
    type->size = size;
    type->is_const = 0;
    memset(&type->as, 0, sizeof(type->as));

    return type;
}

DaadType* daad_type_create_pointer(DaadType* pointee) {
    DaadType* type = daad_type_create(TYPE_POINTER, "مؤشر", 8);
    if (!type) return NULL;
    type->as.pointer.pointee = pointee;
    return type;
}

DaadType* daad_type_create_array(DaadType* element_type, size_t length) {
    DaadType* type = daad_type_create(TYPE_ARRAY, "مصفوفة", 0);
    if (!type) return NULL;
    type->as.array.element_type = element_type;
    type->as.array.length = length;
    type->size = element_type ? element_type->size * length : 0;
    return type;
}

DaadType* daad_type_create_struct(char** member_names, DaadType** member_types, size_t member_count) {
    DaadType* type = daad_type_create(TYPE_STRUCT, "هيكل", 0);
    if (!type) return NULL;

    type->as.struct_type.member_names = member_names;
    type->as.struct_type.member_types = member_types;
    type->as.struct_type.member_count = member_count;

    size_t total = 0;
    for (size_t i = 0; i < member_count; i++) {
        if (member_types[i]) total += member_types[i]->size;
    }
    type->size = total;

    return type;
}

DaadType* daad_type_create_function(DaadType** param_types, size_t param_count, DaadType* return_type) {
    DaadType* type = daad_type_create(TYPE_FUNCTION, "دالة", 8);
    if (!type) return NULL;
    type->as.function.param_types = param_types;
    type->as.function.param_count = param_count;
    type->as.function.return_type = return_type;
    return type;
}

int daad_type_equals(const DaadType* a, const DaadType* b) {
    if (!a || !b) return 0;
    if (a->kind != b->kind) return 0;

    switch (a->kind) {
        case TYPE_POINTER:
            return daad_type_equals(a->as.pointer.pointee, b->as.pointer.pointee);
        case TYPE_ARRAY:
            return a->as.array.length == b->as.array.length &&
                   daad_type_equals(a->as.array.element_type, b->as.array.element_type);
        case TYPE_FUNCTION:
            if (a->as.function.param_count != b->as.function.param_count) return 0;
            for (size_t i = 0; i < a->as.function.param_count; i++) {
                if (!daad_type_equals(a->as.function.param_types[i], b->as.function.param_types[i])) return 0;
            }
            return daad_type_equals(a->as.function.return_type, b->as.function.return_type);
        case TYPE_STRUCT:
            if (a->as.struct_type.member_count != b->as.struct_type.member_count) return 0;
            for (size_t i = 0; i < a->as.struct_type.member_count; i++) {
                if (strcmp(a->as.struct_type.member_names[i], b->as.struct_type.member_names[i]) != 0) return 0;
                if (!daad_type_equals(a->as.struct_type.member_types[i], b->as.struct_type.member_types[i])) return 0;
            }
            return 1;
        default:
            return a->kind == b->kind;
    }
}

int daad_type_is_compatible(const DaadType* from, const DaadType* to) {
    if (!from || !to) return 0;
    if (daad_type_equals(from, to)) return 1;

    if (from->kind == TYPE_POINTER && to->kind == TYPE_POINTER) {
        if (to->as.pointer.pointee &&
            to->as.pointer.pointee->kind == TYPE_VOID) return 1;
        if (from->as.pointer.pointee &&
            from->as.pointer.pointee->kind == TYPE_VOID) return 1;
    }

    if ((from->kind == TYPE_INT || from->kind == TYPE_FLOAT) &&
        (to->kind == TYPE_INT || to->kind == TYPE_FLOAT)) return 1;

    if ((from->kind == TYPE_INT || from->kind == TYPE_FLOAT) && to->kind == TYPE_BOOL) return 1;
    if (from->kind == TYPE_BOOL && (to->kind == TYPE_INT || to->kind == TYPE_FLOAT)) return 1;

    if (from->kind == TYPE_CHAR && to->kind == TYPE_INT) return 1;
    if (from->kind == TYPE_INT && to->kind == TYPE_CHAR) return 1;

    return 0;
}

int daad_type_is_numeric(const DaadType* type) {
    if (!type) return 0;
    return type->kind == TYPE_INT || type->kind == TYPE_FLOAT;
}

int daad_type_is_pointer(const DaadType* type) {
    if (!type) return 0;
    return type->kind == TYPE_POINTER;
}

int daad_type_is_void(const DaadType* type) {
    if (!type) return 0;
    return type->kind == TYPE_VOID;
}

void daad_type_destroy(DaadType* type) {
    if (!type) return;

    switch (type->kind) {
        case TYPE_POINTER:
            daad_type_destroy(type->as.pointer.pointee);
            break;
        case TYPE_ARRAY:
            daad_type_destroy(type->as.array.element_type);
            break;
        case TYPE_STRUCT:
            for (size_t i = 0; i < type->as.struct_type.member_count; i++) {
                free(type->as.struct_type.member_names[i]);
                daad_type_destroy(type->as.struct_type.member_types[i]);
            }
            free(type->as.struct_type.member_names);
            free(type->as.struct_type.member_types);
            break;
        case TYPE_FUNCTION:
            for (size_t i = 0; i < type->as.function.param_count; i++) {
                daad_type_destroy(type->as.function.param_types[i]);
            }
            free(type->as.function.param_types);
            daad_type_destroy(type->as.function.return_type);
            break;
        default:
            break;
    }

    free(type);
}