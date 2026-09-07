/**
 * @file ssa.h
 * @brief SSA (Static Single Assignment) Support
 */

#ifndef DAAD_SSA_H
#define DAAD_SSA_H

#include "../ir/ir_function.h"

typedef struct {
    int original_id;
    int ssa_id;
    int version;
} SSARename;

typedef struct {
    IRFunction* func;
    SSARename* renames;
    int rename_count;
    int rename_capacity;
    int next_ssa_id;
} SSAContext;

SSAContext* ssa_create(IRFunction* func);
void ssa_destroy(SSAContext* ctx);
void ssa_convert_to_ssa(SSAContext* ctx);
void ssa_convert_from_ssa(SSAContext* ctx);
int ssa_validate(SSAContext* ctx);
void ssa_insert_phi_nodes(SSAContext* ctx);
void ssa_rename_variables(SSAContext* ctx);

#endif