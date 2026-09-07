/**
 * @file type_registry.h
 * @brief سجل الأنواع المدمجة في ض Core
 */

#ifndef DAAD_TYPE_REGISTRY_H
#define DAAD_TYPE_REGISTRY_H

#include "type.h"

typedef struct {
    DaadType* int_type;
    DaadType* float_type;
    DaadType* char_type;
    DaadType* bool_type;
    DaadType* void_type;
    DaadType* string_type;
} TypeRegistry;

TypeRegistry* type_registry_create(void);
DaadType* type_registry_get_int(TypeRegistry* reg);
DaadType* type_registry_get_float(TypeRegistry* reg);
DaadType* type_registry_get_char(TypeRegistry* reg);
DaadType* type_registry_get_bool(TypeRegistry* reg);
DaadType* type_registry_get_void(TypeRegistry* reg);
DaadType* type_registry_get_string(TypeRegistry* reg);
void type_registry_destroy(TypeRegistry* reg);

#endif