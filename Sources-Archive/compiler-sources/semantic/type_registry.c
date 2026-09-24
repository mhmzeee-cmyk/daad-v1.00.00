/**
 * @file type_registry.c
 * @brief تنفيذ سجل الأنواع المدمجة
 */

#include "type_registry.h"
#include <stdlib.h>

TypeRegistry* type_registry_create(void) {
    TypeRegistry* reg = (TypeRegistry*)malloc(sizeof(TypeRegistry));
    if (!reg) return NULL;

    reg->int_type = daad_type_create(TYPE_INT, "رقم", 8);
    reg->float_type = daad_type_create(TYPE_FLOAT, "رقم_عشري", 8);
    reg->char_type = daad_type_create(TYPE_CHAR, "حرف", 1);
    reg->bool_type = daad_type_create(TYPE_BOOL, "منطق", 1);
    reg->void_type = daad_type_create(TYPE_VOID, "فراغ", 0);
    reg->string_type = daad_type_create(TYPE_STRING, "نص", 8);

    if (!reg->int_type || !reg->float_type || !reg->char_type ||
        !reg->bool_type || !reg->void_type || !reg->string_type) {
        daad_type_destroy(reg->int_type);
        daad_type_destroy(reg->float_type);
        daad_type_destroy(reg->char_type);
        daad_type_destroy(reg->bool_type);
        daad_type_destroy(reg->void_type);
        daad_type_destroy(reg->string_type);
        free(reg);
        return NULL;
    }

    return reg;
}

DaadType* type_registry_get_int(TypeRegistry* reg) { return reg ? reg->int_type : NULL; }
DaadType* type_registry_get_float(TypeRegistry* reg) { return reg ? reg->float_type : NULL; }
DaadType* type_registry_get_char(TypeRegistry* reg) { return reg ? reg->char_type : NULL; }
DaadType* type_registry_get_bool(TypeRegistry* reg) { return reg ? reg->bool_type : NULL; }
DaadType* type_registry_get_void(TypeRegistry* reg) { return reg ? reg->void_type : NULL; }
DaadType* type_registry_get_string(TypeRegistry* reg) { return reg ? reg->string_type : NULL; }

void type_registry_destroy(TypeRegistry* reg) {
    if (!reg) return;
    daad_type_destroy(reg->int_type);
    daad_type_destroy(reg->float_type);
    daad_type_destroy(reg->char_type);
    daad_type_destroy(reg->bool_type);
    daad_type_destroy(reg->void_type);
    daad_type_destroy(reg->string_type);
    free(reg);
}
