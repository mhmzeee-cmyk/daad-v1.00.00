/**
 * @file ir_verifier.h
 * @brief مدقق IR - IR Verifier
 */

#ifndef DAAD_IR_VERIFIER_H
#define DAAD_IR_VERIFIER_H

#include "../ir/ir_function.h"
#include "../ir/ir_module.h"
#include "../cfg/cfg.h"

typedef struct {
    int errors_found;
    int warnings_found;
    char last_error[512];
    char last_warning[512];
} IRVerifier;

IRVerifier* ir_verifier_create(void);
void ir_verifier_destroy(IRVerifier* verifier);

int ir_verifier_verify_function(IRVerifier* verifier, IRFunction* func);
int ir_verifier_verify_module(IRVerifier* verifier, IRModule* module);
int ir_verifier_verify_cfg(IRVerifier* verifier, CFG* cfg);
int ir_verifier_verify_instructions(IRVerifier* verifier, IRFunction* func);
int ir_verifier_verify_operands(IRVerifier* verifier, IRFunction* func);
int ir_verifier_verify_registers(IRVerifier* verifier, IRFunction* func);
int ir_verifier_verify_phi(IRVerifier* verifier, IRFunction* func);

const char* ir_verifier_get_error(IRVerifier* verifier);
const char* ir_verifier_get_warning(IRVerifier* verifier);

#endif
