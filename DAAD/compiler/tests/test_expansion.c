/**
 * @file test_expansion.c
 * @brief اختبارات توسعة التغطية - يضيف 300+ اختبار
 */

#include "../ir/ir_types.h"
#include "../ir/ir_value.h"
#include "../ir/ir_instruction.h"
#include "../ir/ir_basic_block.h"
#include "../ir/ir_function.h"
#include "../ir/ir_module.h"
#include "../ir/ir_printer.h"
#include "../ir/ir_builder.h"
#include "../ir/ir_verifier.h"
#include "../cfg/cfg.h"
#include "../optimizer/optimizer_pass.h"
#include "../ssa/ssa.h"
#include "../codegen/register_allocator.h"
#include "../codegen/stack_manager.h"
#include "../codegen/label_manager.h"
#include "../backend/backend_interface.h"
#include "../semantic/scope.h"
#include "../semantic/scope_stack.h"
#include "../semantic/symbol.h"
#include "../semantic/type.h"
#include "../semantic/type_registry.h"
#include "../semantic/type_checker.h"
#include "../semantic/semantic_error.h"
#include "../ir/liveness.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_count = 0;
static int pass_count = 0;

#define ASSERT(cond, msg) do { \
    test_count++; \
    if (cond) { pass_count++; printf("  PASS: %s\n", msg); } \
    else { printf("  FAIL: %s\n", msg); } \
} while(0)

static void test_optimizer_constant_folding_all_ops(void) {
    printf("\n--- Optimizer: Constant Folding All Ops ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("cf_all", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");

    IRInstruction add = ir_inst_create_2(IR_OP_ADD, ir_value_create_register(1, i64),
        ir_value_create_int(10, i64), ir_value_create_int(20, i64));
    ir_bb_add_instruction(bb, add);
    IRInstruction sub = ir_inst_create_2(IR_OP_SUB, ir_value_create_register(2, i64),
        ir_value_create_int(50, i64), ir_value_create_int(15, i64));
    ir_bb_add_instruction(bb, sub);
    IRInstruction mul = ir_inst_create_2(IR_OP_MUL, ir_value_create_register(3, i64),
        ir_value_create_int(6, i64), ir_value_create_int(7, i64));
    ir_bb_add_instruction(bb, mul);
    IRInstruction div = ir_inst_create_2(IR_OP_DIV, ir_value_create_register(4, i64),
        ir_value_create_int(100, i64), ir_value_create_int(5, i64));
    ir_bb_add_instruction(bb, div);
    IRInstruction mod = ir_inst_create_2(IR_OP_MOD, ir_value_create_register(5, i64),
        ir_value_create_int(17, i64), ir_value_create_int(3, i64));
    ir_bb_add_instruction(bb, mod);
    IRInstruction and_i = ir_inst_create_2(IR_OP_AND, ir_value_create_register(6, i64),
        ir_value_create_int(0xFF, i64), ir_value_create_int(0x0F, i64));
    ir_bb_add_instruction(bb, and_i);
    IRInstruction or_i = ir_inst_create_2(IR_OP_OR, ir_value_create_register(7, i64),
        ir_value_create_int(0xF0, i64), ir_value_create_int(0x0F, i64));
    ir_bb_add_instruction(bb, or_i);
    IRInstruction xor_i = ir_inst_create_2(IR_OP_XOR, ir_value_create_register(8, i64),
        ir_value_create_int(0xFF, i64), ir_value_create_int(0xAA, i64));
    ir_bb_add_instruction(bb, xor_i);
    IRInstruction shl = ir_inst_create_2(IR_OP_SHL, ir_value_create_register(9, i64),
        ir_value_create_int(1, i64), ir_value_create_int(4, i64));
    ir_bb_add_instruction(bb, shl);
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));

    Optimizer* opt = optimizer_create();
    optimizer_add_pass(opt, OPT_PASS_CONSTANT_FOLDING);
    int changed = optimizer_run_single(opt, func, OPT_PASS_CONSTANT_FOLDING);
    optimizer_destroy(opt);

    ASSERT(changed == 9, "constant folding changed 9 instructions");
    ASSERT(bb->instructions[0].opcode == IR_OP_MOV, "add folded to mov");
    ASSERT(bb->instructions[0].operands[0].as.int_val == 30, "10+20=30");
    ASSERT(bb->instructions[1].operands[0].as.int_val == 35, "50-15=35");
    ASSERT(bb->instructions[2].operands[0].as.int_val == 42, "6*7=42");
    ASSERT(bb->instructions[3].operands[0].as.int_val == 20, "100/5=20");
    ASSERT(bb->instructions[4].operands[0].as.int_val == 2, "17%3=2");
    ASSERT(bb->instructions[5].operands[0].as.int_val == 0x0F, "0xFF & 0x0F");
    ASSERT(bb->instructions[6].operands[0].as.int_val == 0xFF, "0xF0 | 0x0F");
    ASSERT(bb->instructions[7].operands[0].as.int_val == 0x55, "0xFF ^ 0xAA");
    ASSERT(bb->instructions[8].operands[0].as.int_val == 16, "1 << 4");

    ir_function_destroy(func);
}

static void test_optimizer_constant_folding_div_by_zero(void) {
    printf("\n--- Optimizer: Div by Zero Safety ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("cf_div0", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");

    IRInstruction div = ir_inst_create_2(IR_OP_DIV, ir_value_create_register(1, i64),
        ir_value_create_int(10, i64), ir_value_create_int(0, i64));
    ir_bb_add_instruction(bb, div);
    IRInstruction mod = ir_inst_create_2(IR_OP_MOD, ir_value_create_register(2, i64),
        ir_value_create_int(10, i64), ir_value_create_int(0, i64));
    ir_bb_add_instruction(bb, mod);
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));

    Optimizer* opt = optimizer_create();
    optimizer_add_pass(opt, OPT_PASS_CONSTANT_FOLDING);
    int changed = optimizer_run_single(opt, func, OPT_PASS_CONSTANT_FOLDING);
    optimizer_destroy(opt);

    ASSERT(changed == 0, "div by zero not folded");
    ASSERT(bb->instructions[0].opcode == IR_OP_DIV, "div kept");
    ASSERT(bb->instructions[1].opcode == IR_OP_MOD, "mod kept");

    ir_function_destroy(func);
}

static void test_optimizer_algebraic(void) {
    printf("\n--- Optimizer: Algebraic Simplification ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("alg", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");

    IRValue r1 = ir_value_create_register(1, i64);
    IRInstruction add_zero = ir_inst_create_2(IR_OP_ADD, r1,
        ir_value_create_register(10, i64), ir_value_create_int(0, i64));
    ir_bb_add_instruction(bb, add_zero);

    IRValue r2 = ir_value_create_register(2, i64);
    IRInstruction mul_one = ir_inst_create_2(IR_OP_MUL, r2,
        ir_value_create_register(11, i64), ir_value_create_int(1, i64));
    ir_bb_add_instruction(bb, mul_one);

    IRValue r3 = ir_value_create_register(3, i64);
    IRInstruction mul_zero = ir_inst_create_2(IR_OP_MUL, r3,
        ir_value_create_register(12, i64), ir_value_create_int(0, i64));
    ir_bb_add_instruction(bb, mul_zero);

    IRValue r4 = ir_value_create_register(4, i64);
    IRInstruction sub_zero = ir_inst_create_2(IR_OP_SUB, r4,
        ir_value_create_register(13, i64), ir_value_create_int(0, i64));
    ir_bb_add_instruction(bb, sub_zero);

    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));

    Optimizer* opt = optimizer_create();
    optimizer_add_pass(opt, OPT_PASS_ALGEBRAIC_SIMPLIFICATION);
    int changed = optimizer_run_single(opt, func, OPT_PASS_ALGEBRAIC_SIMPLIFICATION);
    optimizer_destroy(opt);

    ASSERT(changed == 4, "algebraic changed 4");
    ASSERT(bb->instructions[0].opcode == IR_OP_MOV, "add x,0 -> mov");
    ASSERT(bb->instructions[1].opcode == IR_OP_MOV, "mul x,1 -> mov");
    ASSERT(bb->instructions[2].operands[0].as.int_val == 0, "mul x,0 -> mov 0");
    ASSERT(bb->instructions[3].opcode == IR_OP_MOV, "sub x,0 -> mov");

    ir_function_destroy(func);
}

static void test_optimizer_strength_reduction(void) {
    printf("\n--- Optimizer: Strength Reduction ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("str", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");

    IRInstruction mul2 = ir_inst_create_2(IR_OP_MUL, ir_value_create_register(1, i64),
        ir_value_create_register(10, i64), ir_value_create_int(2, i64));
    ir_bb_add_instruction(bb, mul2);
    IRInstruction mul4 = ir_inst_create_2(IR_OP_MUL, ir_value_create_register(2, i64),
        ir_value_create_register(11, i64), ir_value_create_int(4, i64));
    ir_bb_add_instruction(bb, mul4);
    IRInstruction mul8 = ir_inst_create_2(IR_OP_MUL, ir_value_create_register(3, i64),
        ir_value_create_register(12, i64), ir_value_create_int(8, i64));
    ir_bb_add_instruction(bb, mul8);
    IRInstruction mul3 = ir_inst_create_2(IR_OP_MUL, ir_value_create_register(4, i64),
        ir_value_create_register(13, i64), ir_value_create_int(3, i64));
    ir_bb_add_instruction(bb, mul3);
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));

    Optimizer* opt = optimizer_create();
    optimizer_add_pass(opt, OPT_PASS_STRENGTH_REDUCTION);
    int changed = optimizer_run_single(opt, func, OPT_PASS_STRENGTH_REDUCTION);
    optimizer_destroy(opt);

    ASSERT(changed == 3, "strength reduction changed 3");
    ASSERT(bb->instructions[0].opcode == IR_OP_SHL, "mul 2 -> shl 1");
    ASSERT(bb->instructions[0].operands[1].as.int_val == 1, "shl by 1");
    ASSERT(bb->instructions[1].opcode == IR_OP_SHL, "mul 4 -> shl 2");
    ASSERT(bb->instructions[1].operands[1].as.int_val == 2, "shl by 2");
    ASSERT(bb->instructions[2].opcode == IR_OP_SHL, "mul 8 -> shl 3");
    ASSERT(bb->instructions[2].operands[1].as.int_val == 3, "shl by 3");
    ASSERT(bb->instructions[3].opcode == IR_OP_MUL, "mul 3 kept (not power of 2)");

    ir_function_destroy(func);
}

static void test_optimizer_copy_propagation(void) {
    printf("\n--- Optimizer: Copy Propagation ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("copy", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");

    IRInstruction mov1 = ir_inst_create_2(IR_OP_MOV, ir_value_create_register(1, i64),
        ir_value_create_register(10, i64), ir_value_null());
    ir_bb_add_instruction(bb, mov1);
    IRInstruction add = ir_inst_create_2(IR_OP_ADD, ir_value_create_register(2, i64),
        ir_value_create_register(1, i64), ir_value_create_int(5, i64));
    ir_bb_add_instruction(bb, add);
    IRInstruction add2 = ir_inst_create_2(IR_OP_ADD, ir_value_create_register(3, i64),
        ir_value_create_register(1, i64), ir_value_create_int(10, i64));
    ir_bb_add_instruction(bb, add2);
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));

    Optimizer* opt = optimizer_create();
    optimizer_add_pass(opt, OPT_PASS_COPY_PROPAGATION);
    int changed = optimizer_run_single(opt, func, OPT_PASS_COPY_PROPAGATION);
    optimizer_destroy(opt);

    ASSERT(changed == 2, "copy propagation changed 2");
    ASSERT(bb->instructions[1].operands[0].id == 10, "add uses r10 not r1");
    ASSERT(bb->instructions[2].operands[0].id == 10, "add2 uses r10 not r1");

    ir_function_destroy(func);
}

static void test_optimizer_dead_code(void) {
    printf("\n--- Optimizer: Dead Code Elimination ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("dce", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");

    IRInstruction dead1 = ir_inst_create_2(IR_OP_ADD, ir_value_create_register(1, i64),
        ir_value_create_int(1, i64), ir_value_create_int(2, i64));
    ir_bb_add_instruction(bb, dead1);
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));

    Optimizer* opt = optimizer_create();
    optimizer_add_pass(opt, OPT_PASS_DEAD_CODE_ELIMINATION);
    int changed = optimizer_run_single(opt, func, OPT_PASS_DEAD_CODE_ELIMINATION);
    optimizer_destroy(opt);

    ASSERT(changed == 1, "DCE removed 1 dead instruction");
    ASSERT(bb->instructions[0].opcode == IR_OP_NOP, "dead1 -> nop");

    ir_function_destroy(func);
}

static void test_optimizer_peephole(void) {
    printf("\n--- Optimizer: Peephole ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("ph", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");

    IRInstruction m1 = ir_inst_create_2(IR_OP_MOV, ir_value_create_register(1, i64),
        ir_value_create_int(42, i64), ir_value_null());
    ir_bb_add_instruction(bb, m1);
    IRInstruction m2 = ir_inst_create_2(IR_OP_MOV, ir_value_create_register(2, i64),
        ir_value_create_register(1, i64), ir_value_null());
    ir_bb_add_instruction(bb, m2);
    IRInstruction m3 = ir_inst_create_2(IR_OP_MOV, ir_value_create_register(3, i64),
        ir_value_create_register(2, i64), ir_value_null());
    ir_bb_add_instruction(bb, m3);
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));

    Optimizer* opt = optimizer_create();
    optimizer_add_pass(opt, OPT_PASS_PEEPHOLE);
    int changed = optimizer_run_single(opt, func, OPT_PASS_PEEPHOLE);
    optimizer_destroy(opt);

    ASSERT(changed >= 1, "peephole made at least 1 change");

    ir_function_destroy(func);
}

static void test_optimizer_run_all(void) {
    printf("\n--- Optimizer: Run All Passes ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("all_opt", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");

    IRInstruction add = ir_inst_create_2(IR_OP_ADD, ir_value_create_register(1, i64),
        ir_value_create_int(3, i64), ir_value_create_int(4, i64));
    ir_bb_add_instruction(bb, add);
    IRInstruction mul = ir_inst_create_2(IR_OP_MUL, ir_value_create_register(2, i64),
        ir_value_create_register(1, i64), ir_value_create_int(2, i64));
    ir_bb_add_instruction(bb, mul);
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_register(2, i64)));

    Optimizer* opt = optimizer_create();
    optimizer_add_default_passes(opt);
    int total = optimizer_run_all(opt, func);
    optimizer_destroy(opt);

    ASSERT(total > 0, "run_all made transformations");
    ASSERT(bb->instructions[bb->instruction_count - 1].opcode == IR_OP_RET, "ret still exists");

    ir_function_destroy(func);
}

static void test_optimizer_constant_propagation(void) {
    printf("\n--- Optimizer: Constant Propagation ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("cp", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");

    IRInstruction mov = ir_inst_create_2(IR_OP_MOV, ir_value_create_register(1, i64),
        ir_value_create_int(99, i64), ir_value_null());
    ir_bb_add_instruction(bb, mov);
    IRInstruction add = ir_inst_create_2(IR_OP_ADD, ir_value_create_register(2, i64),
        ir_value_create_register(1, i64), ir_value_create_int(1, i64));
    ir_bb_add_instruction(bb, add);
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));

    Optimizer* opt = optimizer_create();
    optimizer_add_pass(opt, OPT_PASS_CONSTANT_PROPAGATION);
    int changed = optimizer_run_single(opt, func, OPT_PASS_CONSTANT_PROPAGATION);
    optimizer_destroy(opt);

    ASSERT(changed == 1, "constant propagation changed 1");
    ASSERT(bb->instructions[1].operands[0].kind == IR_VALUE_CONSTANT_INT, "operand propagated");
    ASSERT(bb->instructions[1].operands[0].as.int_val == 99, "value 99 propagated");

    ir_function_destroy(func);
}

static void test_ssa_phi_insertion(void) {
    printf("\n--- SSA: Phi Node Insertion ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("ssa_phi", i64);
    IRBasicBlock* entry = ir_function_add_block(func, "entry");
    IRBasicBlock* left = ir_function_add_block(func, "left");
    IRBasicBlock* merge = ir_function_add_block(func, "merge");

    ir_bb_add_instruction(entry, ir_inst_create_2(IR_OP_MOV, ir_value_create_register(1, i64),
        ir_value_null(), ir_value_create_int(1, i64)));
    ir_bb_add_instruction(entry, ir_inst_jmp("left"));

    ir_bb_add_instruction(left, ir_inst_create_2(IR_OP_MOV, ir_value_create_register(1, i64),
        ir_value_null(), ir_value_create_int(2, i64)));
    ir_bb_add_instruction(left, ir_inst_jmp("merge"));

    ir_bb_add_instruction(merge, ir_inst_ret(ir_value_create_int(0, i64)));

    ir_bb_add_successor(entry, left);
    ir_bb_add_predecessor(left, entry);
    ir_bb_add_successor(left, merge);
    ir_bb_add_predecessor(merge, entry);
    ir_bb_add_predecessor(merge, left);

    SSAContext* ssa = ssa_create(func);
    ssa_insert_phi_nodes(ssa);
    ssa_destroy(ssa);

    ASSERT(merge->instruction_count > 0, "merge block has instructions");
    int has_phi = 0;
    for (int i = 0; i < merge->instruction_count; i++) {
        if (merge->instructions[i].opcode == IR_OP_PHI) { has_phi = 1; break; }
    }
    ASSERT(has_phi == 1, "merge block contains a phi instruction");

    ir_function_destroy(func);
}

static void test_ssa_validate(void) {
    printf("\n--- SSA: Validate ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("ssa_val", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));

    SSAContext* ssa = ssa_create(func);
    int valid = ssa_validate(ssa);
    ssa_destroy(ssa);

    ASSERT(valid == 1, "simple function validates");
    ir_function_destroy(func);
}

static void test_type_registry_all_types(void) {
    printf("\n--- Type Registry: All Types ---\n");
    TypeRegistry* reg = type_registry_create();
    ASSERT(reg != NULL, "registry created");
    ASSERT(reg->int_type != NULL, "int_type exists");
    ASSERT(reg->float_type != NULL, "float_type exists");
    ASSERT(reg->char_type != NULL, "char_type exists");
    ASSERT(reg->bool_type != NULL, "bool_type exists");
    ASSERT(reg->void_type != NULL, "void_type exists");
    ASSERT(reg->string_type != NULL, "string_type exists");
    ASSERT(reg->int_type->kind == TYPE_INT, "int kind");
    ASSERT(reg->float_type->kind == TYPE_FLOAT, "float kind");
    ASSERT(reg->char_type->kind == TYPE_CHAR, "char kind");
    ASSERT(reg->bool_type->kind == TYPE_BOOL, "bool kind");
    ASSERT(reg->void_type->kind == TYPE_VOID, "void kind");
    ASSERT(reg->string_type->kind == TYPE_STRING, "string kind");
    type_registry_destroy(reg);
}

static void test_type_checker_binary_ops(void) {
    printf("\n--- Type Checker: Binary Ops ---\n");
    TypeRegistry* reg = type_registry_create();
    SemanticErrorList* errors = semantic_error_list_create();
    TypeChecker* tc = type_checker_create(reg, errors);

    DaadType* r1 = type_checker_check_binary(tc, OP_ADD, reg->int_type, reg->int_type, 1, 1, NULL);
    ASSERT(r1 != NULL, "int + int");
    ASSERT(r1->kind == TYPE_INT, "int + int = int");

    DaadType* r2 = type_checker_check_binary(tc, OP_ADD, reg->int_type, reg->float_type, 1, 1, NULL);
    ASSERT(r2 != NULL, "int + float");
    ASSERT(r2->kind == TYPE_FLOAT, "int + float = float");

    DaadType* r3 = type_checker_check_binary(tc, OP_ADD, reg->float_type, reg->int_type, 1, 1, NULL);
    ASSERT(r3->kind == TYPE_FLOAT, "float + int = float");

    DaadType* r4 = type_checker_check_binary(tc, OP_MUL, reg->int_type, reg->int_type, 1, 1, NULL);
    ASSERT(r4->kind == TYPE_INT, "int * int = int");

    DaadType* r5 = type_checker_check_binary(tc, OP_DIV, reg->int_type, reg->int_type, 1, 1, NULL);
    ASSERT(r5->kind == TYPE_INT, "int / int = int");

    DaadType* r6 = type_checker_check_binary(tc, OP_MOD, reg->int_type, reg->int_type, 1, 1, NULL);
    ASSERT(r6->kind == TYPE_INT, "int % int = int");

    type_checker_destroy(tc);
    semantic_error_list_destroy(errors);
    type_registry_destroy(reg);
}

static void test_type_checker_comparison_ops(void) {
    printf("\n--- Type Checker: Comparison Ops ---\n");
    TypeRegistry* reg = type_registry_create();
    SemanticErrorList* errors = semantic_error_list_create();
    TypeChecker* tc = type_checker_create(reg, errors);

    DaadType* r1 = type_checker_check_binary(tc, OP_EQUAL, reg->int_type, reg->int_type, 1, 1, NULL);
    ASSERT(r1 != NULL, "int == int");
    ASSERT(r1->kind == TYPE_BOOL, "int == int = bool");

    DaadType* r2 = type_checker_check_binary(tc, OP_LESS, reg->int_type, reg->float_type, 1, 1, NULL);
    ASSERT(r2->kind == TYPE_BOOL, "int < float = bool");

    DaadType* r3 = type_checker_check_binary(tc, OP_GREATER, reg->float_type, reg->int_type, 1, 1, NULL);
    ASSERT(r3->kind == TYPE_BOOL, "float > int = bool");

    DaadType* r4 = type_checker_check_binary(tc, OP_AND, reg->bool_type, reg->bool_type, 1, 1, NULL);
    ASSERT(r4->kind == TYPE_BOOL, "bool and bool = bool");

    DaadType* r5 = type_checker_check_binary(tc, OP_OR, reg->bool_type, reg->bool_type, 1, 1, NULL);
    ASSERT(r5->kind == TYPE_BOOL, "bool or bool = bool");

    type_checker_destroy(tc);
    semantic_error_list_destroy(errors);
    type_registry_destroy(reg);
}

static void test_type_checker_unary_ops(void) {
    printf("\n--- Type Checker: Unary Ops ---\n");
    TypeRegistry* reg = type_registry_create();
    SemanticErrorList* errors = semantic_error_list_create();
    TypeChecker* tc = type_checker_create(reg, errors);

    DaadType* r1 = type_checker_check_unary(tc, UNARY_NOT, reg->bool_type, 1, 1, NULL);
    ASSERT(r1 != NULL, "not bool");
    ASSERT(r1->kind == TYPE_BOOL, "not bool = bool");

    DaadType* r2 = type_checker_check_unary(tc, UNARY_NEGATE, reg->int_type, 1, 1, NULL);
    ASSERT(r2 != NULL, "negate int");
    ASSERT(r2->kind == TYPE_INT, "negate int = int");

    DaadType* r3 = type_checker_check_unary(tc, UNARY_BITWISE_NOT, reg->int_type, 1, 1, NULL);
    ASSERT(r3 != NULL, "bitwise_not int");
    ASSERT(r3->kind == TYPE_INT, "bitwise_not int = int");

    type_checker_destroy(tc);
    semantic_error_list_destroy(errors);
    type_registry_destroy(reg);
}

static void test_type_checker_assignment(void) {
    printf("\n--- Type Checker: Assignment ---\n");
    TypeRegistry* reg = type_registry_create();
    SemanticErrorList* errors = semantic_error_list_create();
    TypeChecker* tc = type_checker_create(reg, errors);

    int ok1 = type_checker_check_assignment(tc, reg->int_type, reg->int_type, 1, 1, NULL);
    ASSERT(ok1 == 1, "int = int ok");

    int ok2 = type_checker_check_assignment(tc, reg->float_type, reg->int_type, 1, 1, NULL);
    ASSERT(ok2 == 1, "float = int ok (numeric)");

    int ok3 = type_checker_check_assignment(tc, reg->int_type, reg->float_type, 1, 1, NULL);
    ASSERT(ok3 == 1, "int = float ok (numeric)");

    type_checker_destroy(tc);
    semantic_error_list_destroy(errors);
    type_registry_destroy(reg);
}

static void test_type_checker_return(void) {
    printf("\n--- Type Checker: Return ---\n");
    TypeRegistry* reg = type_registry_create();
    SemanticErrorList* errors = semantic_error_list_create();
    TypeChecker* tc = type_checker_create(reg, errors);

    int ok1 = type_checker_check_return(tc, reg->int_type, reg->int_type, 1, 1, NULL);
    ASSERT(ok1 == 1, "return int matches int");

    int ok2 = type_checker_check_return(tc, reg->void_type, reg->void_type, 1, 1, NULL);
    ASSERT(ok2 == 1, "return void matches void");

    type_checker_destroy(tc);
    semantic_error_list_destroy(errors);
    type_registry_destroy(reg);
}

static void test_type_checker_call(void) {
    printf("\n--- Type Checker: Call ---\n");
    TypeRegistry* reg = type_registry_create();
    SemanticErrorList* errors = semantic_error_list_create();
    TypeChecker* tc = type_checker_create(reg, errors);

    DaadType** params = (DaadType**)malloc(sizeof(DaadType*) * 2);
    params[0] = reg->int_type;
    params[1] = reg->int_type;
    DaadType* func_type = daad_type_create_function(params, 2, reg->int_type);

    DaadType** args1 = (DaadType**)malloc(sizeof(DaadType*) * 2);
    args1[0] = reg->int_type;
    args1[1] = reg->int_type;
    int ok1 = type_checker_check_call(tc, func_type, args1, 2, 1, 1, NULL);
    ASSERT(ok1 == 1, "call(int, int) ok");

    DaadType** args2 = (DaadType**)malloc(sizeof(DaadType*) * 1);
    args2[0] = reg->int_type;
    int ok2 = type_checker_check_call(tc, func_type, args2, 1, 1, 1, NULL);
    ASSERT(ok2 == 0, "call(int) wrong count");

    free(args1);
    free(args2);
    free(func_type);
    type_checker_destroy(tc);
    semantic_error_list_destroy(errors);
    type_registry_destroy(reg);
}

static void test_scope_stack_operations(void) {
    printf("\n--- Scope Stack: Operations ---\n");
    ScopeStack* stack = scope_stack_create();
    ASSERT(stack != NULL, "scope stack created");
    ASSERT(stack->current_level >= 0, "initial level >= 0");

    Symbol* sym1 = symbol_create("x", SYMBOL_VARIABLE, NULL, stack->current_level);
    ASSERT(sym1 != NULL, "symbol x created");
    int def_ok = scope_stack_define(stack, sym1);
    ASSERT(def_ok == 1, "defined x");

    Symbol* found = scope_stack_lookup(stack, "x");
    ASSERT(found != NULL, "found x");

    Symbol* not_found = scope_stack_lookup(stack, "y");
    ASSERT(not_found == NULL, "y not found");

    scope_stack_destroy(stack);
}

static void test_symbol_operations(void) {
    printf("\n--- Symbol: Operations ---\n");
    Symbol* s1 = symbol_create("var1", SYMBOL_VARIABLE, NULL, 0);
    ASSERT(s1 != NULL, "var1 created");
    ASSERT(s1->is_mutable == 1, "variable is mutable");
    ASSERT(s1->is_initialized == 0, "variable not initialized");
    ASSERT(s1->scope_level == 0, "scope level 0");

    Symbol* s2 = symbol_create("const1", SYMBOL_CONSTANT, NULL, 1);
    ASSERT(s2 != NULL, "const1 created");
    ASSERT(s2->is_mutable == 0, "constant not mutable");

    Symbol* s3 = symbol_create("func1", SYMBOL_FUNCTION, NULL, 0);
    ASSERT(s3 != NULL, "func1 created");

    Symbol* s4 = symbol_create("param1", SYMBOL_PARAMETER, NULL, 2);
    ASSERT(s4 != NULL, "param1 created");
    ASSERT(s4->is_mutable == 1, "parameter is mutable");

    Symbol* s5 = symbol_create(NULL, SYMBOL_VARIABLE, NULL, 0);
    ASSERT(s5 == NULL, "NULL name returns NULL");

    symbol_destroy(s1);
    symbol_destroy(s2);
    symbol_destroy(s3);
    symbol_destroy(s4);
}

static void test_scope_operations(void) {
    printf("\n--- Scope: Operations ---\n");
    Scope* global = scope_create(NULL, 0);
    ASSERT(global != NULL, "global scope created");
    ASSERT(global->level == 0, "global level 0");

    Symbol* s1 = symbol_create("g1", SYMBOL_VARIABLE, NULL, 0);
    int ok = scope_define(global, s1);
    ASSERT(ok == 1, "defined g1 in global");

    Symbol* found = scope_lookup(global, "g1");
    ASSERT(found != NULL, "found g1 in global");

    Scope* child = scope_create(global, 1);
    ASSERT(child != NULL, "child scope created");
    ASSERT(child->parent == global, "child parent is global");

    Symbol* s2 = symbol_create("c1", SYMBOL_VARIABLE, NULL, 1);
    scope_define(child, s2);

    Symbol* found_c = scope_lookup(child, "c1");
    ASSERT(found_c != NULL, "found c1 in child");

    Symbol* found_g = scope_lookup(child, "g1");
    ASSERT(found_g != NULL, "found g1 from child (parent lookup)");

    Symbol* not_found = scope_lookup(child, "nonexistent");
    ASSERT(not_found == NULL, "nonexistent not found");

    scope_destroy(child);
    scope_destroy(global);
}

static void test_type_compatibility(void) {
    printf("\n--- Type: Compatibility ---\n");
    DaadType* t1 = daad_type_create(TYPE_INT, "رقم", 8);
    DaadType* t2 = daad_type_create(TYPE_INT, "رقم", 8);
    DaadType* t3 = daad_type_create(TYPE_FLOAT, "رقم_عشري", 8);
    DaadType* t4 = daad_type_create(TYPE_STRING, "نص", 8);

    ASSERT(daad_type_is_compatible(t1, t2), "int compatible with int");
    ASSERT(daad_type_is_numeric(t1), "int is numeric");
    ASSERT(daad_type_is_numeric(t3), "float is numeric");
    ASSERT(!daad_type_is_numeric(t4), "string not numeric");

    daad_type_destroy(t1);
    daad_type_destroy(t2);
    daad_type_destroy(t3);
    daad_type_destroy(t4);
}

static void test_ir_verifier(void) {
    printf("\n--- IR Verifier ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("verify", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));

    IRVerifier* v = ir_verifier_create();
    ASSERT(v != NULL, "verifier created");
    int ok = ir_verifier_verify_function(v, func);
    ASSERT(ok == 1, "simple function verifies");

    ir_verifier_destroy(v);
    ir_function_destroy(func);
}

static void test_liveness_comprehensive(void) {
    printf("\n--- Liveness: Comprehensive ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("live_comp", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");

    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, ir_value_create_register(1, i64),
        ir_value_null(), ir_value_create_int(10, i64)));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, ir_value_create_register(2, i64),
        ir_value_null(), ir_value_create_int(20, i64)));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, ir_value_create_register(3, i64),
        ir_value_create_register(1, i64), ir_value_create_register(2, i64)));
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_register(3, i64)));

    LivenessResult* info = liveness_analyze(func);
    ASSERT(info != NULL, "liveness result created");
    liveness_destroy(info);
    ir_function_destroy(func);
}

static void test_register_allocator_intervals(void) {
    reg_alloc_set_stack_mode(0); /* فحص المخصص الفيزيائي */
    printf("\n--- Register Allocator: Intervals ---\n");
    RegisterAllocator* alloc = reg_alloc_create();
    ASSERT(alloc != NULL, "reg alloc created");

    for (int i = 0; i < 20; i++) {
        reg_alloc_add_interval(alloc, i + 1, 0, 20);
    }

    reg_alloc_set_stack_mode(0);
    reg_alloc_linear_scan(alloc);
    int frame = reg_alloc_get_frame_size(alloc);
    ASSERT(frame > 0, "frame size > 0 with 20 intervals");

    int assigned = 0;
    for (int i = 0; i < 20; i++) {
        if (reg_alloc_get_physical(alloc, i + 1) != REG_NONE) assigned++;
    }
    ASSERT(assigned > 0, "at least one register assigned");

        reg_alloc_set_stack_mode(1);
    reg_alloc_destroy(alloc);
    reg_alloc_set_stack_mode(1);
}

static void test_stack_manager_comprehensive(void) {
    printf("\n--- Stack Manager: Comprehensive ---\n");
    StackManager* sm = stack_manager_create();
    ASSERT(sm != NULL, "stack manager created");

    int off1 = stack_manager_push(sm, "a", 8);
    ASSERT(off1 != 0, "local a allocated");
    int off2 = stack_manager_push(sm, "b", 8);
    ASSERT(off2 != off1, "local b different offset");

    int off3 = stack_manager_push_spill(sm, "s1");
    ASSERT(off3 != 0, "spill s1 allocated");

    int size = stack_manager_get_frame_size(sm);
    ASSERT(size > 0, "frame size > 0");

    stack_manager_destroy(sm);
}

static void test_label_manager_comprehensive(void) {
    printf("\n--- Label Manager: Comprehensive ---\n");
    LabelManager* lm = label_manager_create();
    ASSERT(lm != NULL, "label manager created");

    const char* l1 = label_manager_generate(lm, "if");
    ASSERT(l1 != NULL, "generated if label");
    const char* l2 = label_manager_generate(lm, "while");
    ASSERT(l2 != NULL, "generated while label");
    const char* l3 = label_manager_generate(lm, "if");
    ASSERT(l3 != NULL, "generated second if label");
    ASSERT(strcmp(l1, l3) != 0, "two if labels different");

    label_manager_destroy(lm);
}

static void test_cfg_dominance(void) {
    printf("\n--- CFG: Dominance ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("cfg_dom", i64);
    IRBasicBlock* entry = ir_function_add_block(func, "entry");
    IRBasicBlock* body = ir_function_add_block(func, "body");
    IRBasicBlock* exit = ir_function_add_block(func, "exit");

    ir_bb_add_instruction(entry, ir_inst_jmp("body"));
    ir_bb_add_instruction(body, ir_inst_jmp("exit"));
    ir_bb_add_instruction(exit, ir_inst_ret(ir_value_create_int(0, i64)));

    ir_bb_add_successor(entry, body);
    ir_bb_add_predecessor(body, entry);
    ir_bb_add_successor(body, exit);
    ir_bb_add_predecessor(exit, body);

    CFG* cfg = cfg_build(func);
    ASSERT(cfg != NULL, "CFG built");
    ASSERT(cfg->block_count == 3, "3 blocks in CFG");
    cfg_destroy(cfg);
    ir_function_destroy(func);
}

static void test_backend_x86_names(void) {
    printf("\n--- Backend: x86 Register Names ---\n");
    ASSERT(strcmp(x86_reg_name(1), "%rax") == 0, "r1 = %rax");
    ASSERT(strcmp(x86_reg_name(2), "%rbx") == 0, "r2 = %rbx");
    ASSERT(strcmp(x86_reg_name(3), "%rcx") == 0, "r3 = %rcx");
    ASSERT(strcmp(x86_reg_name(4), "%rdx") == 0, "r4 = %rdx");
    ASSERT(strcmp(x86_reg_name(5), "%r8") == 0, "r5 = %r8");
    ASSERT(strcmp(x86_reg_name(6), "%r9") == 0, "r6 = %r9");
    ASSERT(strcmp(x86_reg_name(7), "%r10") == 0, "r7 = %r10");
    ASSERT(strcmp(x86_reg_name(8), "%r11") == 0, "r8 = %r11");
}

static void test_backend_x86_type_suffix(void) {
    printf("\n--- Backend: x86 Type Suffix ---\n");
    IRType i8 = ir_type_i8();
    IRType i16 = ir_type_i16();
    IRType i32 = ir_type_i32();
    IRType i64 = ir_type_i64();
    IRType f32 = ir_type_f32();
    IRType f64 = ir_type_f64();

    ASSERT(strcmp(x86_type_suffix(i8), "b") == 0, "i8 = b");
    ASSERT(strcmp(x86_type_suffix(i16), "w") == 0, "i16 = w");
    ASSERT(strcmp(x86_type_suffix(i32), "l") == 0, "i32 = l");
    ASSERT(strcmp(x86_type_suffix(i64), "q") == 0, "i64 = q");
    ASSERT(strcmp(x86_type_suffix(f32), "ss") == 0, "f32 = ss");
    ASSERT(strcmp(x86_type_suffix(f64), "sd") == 0, "f64 = sd");
}

static void test_backend_x86_frame_size(void) {
    printf("\n--- Backend: x86 Frame Size ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("frame", i64);
    func->param_count = 2;
    func->alloca_count = 3;

    int size = x86_calculate_frame_size(func);
    ASSERT(size >= 40, "frame size >= 40 (2 params + 3 allocas)");
    ASSERT(size % 16 == 0, "frame size aligned to 16");

    func->param_count = 0;
    func->alloca_count = 0;
    int min_size = x86_calculate_frame_size(func);
    ASSERT(min_size == 16, "minimum frame size = 16");

    ir_function_destroy(func);
}

static void test_backend_emit_simple(void) {
    printf("\n--- Backend: Emit Simple ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("emit_simple", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(42, i64)));

    FILE* f = tmpfile();
    ASSERT(f != NULL, "tmpfile created");
    int count = backend_emit_function(func, BACKEND_X86, f);
    ASSERT(count > 0, "emitted instructions");
    fclose(f);

    ir_function_destroy(func);
}

static void test_backend_emit_module(void) {
    printf("\n--- Backend: Emit Module ---\n");
    IRType i64 = ir_type_i64();
    IRModule* mod = ir_module_create("test_mod");

    IRFunction* f1 = ir_module_add_function(mod, "func1", i64);
    IRBasicBlock* bb1 = ir_function_add_block(f1, "entry");
    ir_bb_add_instruction(bb1, ir_inst_ret(ir_value_create_int(0, i64)));

    IRFunction* f2 = ir_module_add_function(mod, "func2", i64);
    IRBasicBlock* bb2 = ir_function_add_block(f2, "entry");
    ir_bb_add_instruction(bb2, ir_inst_ret(ir_value_create_int(1, i64)));

    FILE* f = tmpfile();
    int count = backend_emit_module(mod, BACKEND_X86, f);
    ASSERT(count > 0, "module emitted");
    fclose(f);

    ir_module_destroy(mod);
}

static void test_backend_arm_riscv_null(void) {
    printf("\n--- Backend: ARM/RISC-V Null ---\n");
    Backend* arm = backend_get(BACKEND_ARM);
    ASSERT(arm != NULL, "ARM backend created");
    ASSERT(arm->emit_function == NULL, "ARM emit_function is NULL");
    backend_destroy(arm);

    Backend* riscv = backend_get(BACKEND_RISCV);
    ASSERT(riscv != NULL, "RISC-V backend created");
    ASSERT(riscv->emit_function == NULL, "RISC-V emit_function is NULL");
    backend_destroy(riscv);
}

static void test_ir_function_comprehensive(void) {
    printf("\n--- IR Function: Comprehensive ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("comp", i64);
    ASSERT(func != NULL, "function created");
    ASSERT(strcmp(func->name, "comp") == 0, "name is comp");
    ASSERT(func->param_count == 0, "no params");

    ir_function_add_param(func, "p1", i64);
    ir_function_add_param(func, "p2", i64);
    ASSERT(func->param_count == 2, "2 params");
    ASSERT(strcmp(func->param_names[0], "p1") == 0, "param 0 is p1");
    ASSERT(strcmp(func->param_names[1], "p2") == 0, "param 1 is p2");

    IRBasicBlock* bb = ir_function_add_block(func, "entry");
    ASSERT(bb != NULL, "block added");
    ASSERT(func->block_count == 1, "1 block");
    ASSERT(func->entry == bb, "entry block set");

    IRValue reg = ir_function_alloc_reg(func, i64);
    ASSERT(reg.kind == IR_VALUE_REGISTER, "allocated register");
    ASSERT(reg.id > 0, "register id > 0");

    ir_function_destroy(func);
}

static void test_ir_module_comprehensive(void) {
    printf("\n--- IR Module: Comprehensive ---\n");
    IRModule* mod = ir_module_create("test");
    ASSERT(mod != NULL, "module created");
    ASSERT(strcmp(mod->name, "test") == 0, "name is test");
    ASSERT(mod->function_count == 0, "no functions");

    IRType i64 = ir_type_i64();
    IRFunction* f1 = ir_module_add_function(mod, "f1", i64);
    (void)f1;
    ASSERT(mod->function_count == 1, "1 function");

    IRFunction* f2 = ir_module_add_function(mod, "f2", i64);
    (void)f2;
    ASSERT(mod->function_count == 2, "2 functions");

    ir_module_destroy(mod);
}

static void test_optimizer_jump_optimization(void) {
    printf("\n--- Optimizer: Jump Optimization ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("jump_opt", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");

    IRInstruction je = ir_inst_create(IR_OP_JE);
    je.result = ir_value_null();
    je.operands[0] = ir_value_create_int(1, i64);
    je.operands[1] = ir_value_create_int(0, i64);
    je.operands[2] = ir_value_create_label("true_label");
    je.operands[3] = ir_value_create_label("false_label");
    je.operand_count = 4;
    ir_bb_add_instruction(bb, je);
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));

    Optimizer* opt = optimizer_create();
    optimizer_add_pass(opt, OPT_PASS_JUMP_OPTIMIZATION);
    int changed = optimizer_run_single(opt, func, OPT_PASS_JUMP_OPTIMIZATION);
    optimizer_destroy(opt);

    ASSERT(changed == 1, "jump optimization changed 1");
    ASSERT(bb->instructions[0].opcode == IR_OP_JMP, "JE with const 1 -> JMP");

    ir_function_destroy(func);
}

static void test_optimizer_merge_blocks(void) {
    printf("\n--- Optimizer: Merge Blocks ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("merge_blk", i64);
    IRBasicBlock* bb1 = ir_function_add_block(func, "bb1");
    IRBasicBlock* bb2 = ir_function_add_block(func, "bb2");

    ir_bb_add_instruction(bb1, ir_inst_create_2(IR_OP_MOV, ir_value_create_register(1, i64),
        ir_value_null(), ir_value_create_int(10, i64)));
    ir_bb_add_instruction(bb1, ir_inst_jmp("bb2"));
    ir_bb_add_instruction(bb2, ir_inst_ret(ir_value_create_int(0, i64)));

    ir_bb_add_successor(bb1, bb2);
    ir_bb_add_predecessor(bb2, bb1);
    bb2->predecessor_count = 1;

    Optimizer* opt = optimizer_create();
    optimizer_add_pass(opt, OPT_PASS_MERGE_BLOCKS);
    int changed = optimizer_run_single(opt, func, OPT_PASS_MERGE_BLOCKS);
    optimizer_destroy(opt);

    ASSERT(changed == 1, "merge blocks changed 1");
    ASSERT(bb1->instruction_count > 2, "bb1 absorbed bb2 instructions");

    ir_function_destroy(func);
}

static void test_backend_param_load(void) {
    printf("\n--- Backend: Param Load ---\n");
    FILE* f = tmpfile();
    IRValue result;
    int ok = backend_emit_param_load(0, &result, f);
    ASSERT(ok == 1, "param 0 loaded");
    ASSERT(result.kind == IR_VALUE_REGISTER, "result is register");

    ok = backend_emit_param_load(5, &result, f);
    ASSERT(ok == 1, "param 5 loaded");

    fclose(f);
}

static void test_backend_return(void) {
    printf("\n--- Backend: Return ---\n");
    FILE* f = tmpfile();
    IRValue val = ir_value_create_int(42, ir_type_i64());
    int ok = backend_emit_return(val, f);
    ASSERT(ok == 1, "return emitted");

    IRValue reg = ir_value_create_register(1, ir_type_i64());
    ok = backend_emit_return(reg, f);
    ASSERT(ok == 1, "return reg emitted");

    fclose(f);
}

static void test_ir_printer_comprehensive(void) {
    printf("\n--- IR Printer: Comprehensive ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("print_comp", i64);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, ir_value_create_register(1, i64),
        ir_value_null(), ir_value_create_int(42, i64)));
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_register(1, i64)));

    FILE* f = tmpfile();
    ir_print_function(func, f);
    ASSERT(ftell(f) > 0, "function printed output");
    fclose(f);

    IRModule* mod = ir_module_create("test");
    ir_module_add_function(mod, "f1", i64);
    FILE* f2 = tmpfile();
    ir_print_module(mod, f2);
    ASSERT(ftell(f2) > 0, "module printed output");
    fclose(f2);

    ir_module_destroy(mod);
    ir_function_destroy(func);
}

static void test_ir_value_comprehensive(void) {
    printf("\n--- IR Value: Comprehensive ---\n");
    IRType i64 = ir_type_i64();

    IRValue r1 = ir_value_create_register(42, i64);
    ASSERT(r1.kind == IR_VALUE_REGISTER, "register kind");
    ASSERT(r1.id == 42, "register id 42");
    ASSERT(strcmp(ir_value_name(r1), "r42") == 0, "register name r42");

    IRValue i1 = ir_value_create_int(12345, i64);
    ASSERT(i1.kind == IR_VALUE_CONSTANT_INT, "int kind");
    ASSERT(i1.as.int_val == 12345, "int value 12345");

    IRValue f1 = ir_value_create_float(2.718, ir_type_f64());
    ASSERT(f1.kind == IR_VALUE_CONSTANT_FLOAT, "float kind");

    IRValue s1 = ir_value_create_string("test");
    ASSERT(s1.kind == IR_VALUE_CONSTANT_STRING, "string kind");
    ASSERT(strcmp(s1.as.string_val, "test") == 0, "string value");

    IRValue l1 = ir_value_create_label(".Ltest");
    ASSERT(l1.kind == IR_VALUE_LABEL, "label kind");
    ASSERT(strcmp(l1.as.label_val, ".Ltest") == 0, "label value");

    IRValue n1 = ir_value_null();
    ASSERT(n1.kind == IR_VALUE_NULL, "null kind");

    char buf[32];
    ir_value_name_buf(r1, buf, sizeof(buf));
    ASSERT(strcmp(buf, "r42") == 0, "name_buf r42");

    ir_value_name_buf(i1, buf, sizeof(buf));
    ASSERT(strcmp(buf, "12345") == 0, "name_buf 12345");

    ir_value_name_buf(n1, buf, sizeof(buf));
    ASSERT(strcmp(buf, "null") == 0, "name_buf null");

    ir_value_name_buf(r1, buf, 2);
    ASSERT(strcmp(buf, "r") == 0, "name_buf truncated");
}

static void test_ir_instruction_edge_cases(void) {
    printf("\n--- IR Instruction: Edge Cases ---\n");
    IRType i64 = ir_type_i64();
    IRType f64 = ir_type_f64();
    (void)f64;

    IRInstruction mov = ir_inst_create_2(IR_OP_MOV, ir_value_create_register(1, i64),
        ir_value_create_int(0, i64), ir_value_null());
    ASSERT(mov.operand_count == 2, "mov has 2 operands");
    ASSERT(mov.opcode == IR_OP_MOV, "mov opcode correct");

    IRInstruction add = ir_inst_create_2(IR_OP_ADD, ir_value_create_register(2, i64),
        ir_value_create_register(1, i64), ir_value_create_int(42, i64));
    ASSERT(add.operands[0].as.int_val == 1 || add.operands[0].kind == IR_VALUE_REGISTER, "add src is register");

    IRInstruction ret = ir_inst_ret(ir_value_create_int(0, i64));
    ASSERT(ret.opcode == IR_OP_RET, "ret opcode");

    IRInstruction nop = ir_inst_create_1(IR_OP_MOV, ir_value_null(), ir_value_null());
    ASSERT(nop.opcode == IR_OP_MOV, "nop is mov");

    IRValue incomings[2];
    incomings[0] = ir_value_create_register(10, i64);
    incomings[1] = ir_value_create_register(20, i64);
    IRInstruction phi = ir_inst_phi(ir_value_create_register(30, i64), incomings, 2);
    ASSERT(phi.opcode == IR_OP_PHI, "phi opcode");
    ASSERT(phi.operand_count == 2, "phi has 2 operands");

    IRInstruction jmp = ir_inst_jmp("target");
    ASSERT(jmp.opcode == IR_OP_JMP, "jmp opcode");

    IRInstruction call = ir_inst_create_1(IR_OP_CALL, ir_value_create_register(50, i64),
        ir_value_create_label("func"));
    ASSERT(call.opcode == IR_OP_CALL, "call opcode");
}

static void test_ir_value_edge_cases(void) {
    printf("\n--- IR Value: Edge Cases ---\n");
    IRType i64 = ir_type_i64();

    IRValue reg = ir_value_create_register(99, i64);
    ASSERT(reg.kind == IR_VALUE_REGISTER, "register kind");
    ASSERT(reg.id == 99, "register id 99");

    IRValue imm = ir_value_create_int(12345678, i64);
    ASSERT(imm.kind == IR_VALUE_CONSTANT_INT, "constant int kind");
    ASSERT(imm.as.int_val == 12345678, "constant int value");

    IRValue lbl = ir_value_create_label("bb_test");
    ASSERT(lbl.kind == IR_VALUE_LABEL, "label kind");

    IRValue null_val = ir_value_null();
    ASSERT(null_val.kind == IR_VALUE_NULL, "null kind");

    IRValue reg2 = ir_value_create_register(1, i64);
    IRValue reg3 = ir_value_create_register(1, i64);
    ASSERT(reg2.id == reg3.id, "same register id");

    IRType i16 = ir_type_i16();
    IRValue small = ir_value_create_register(5, i16);
    ASSERT(small.type.kind == IR_TYPE_I16, "i16 type");

    IRType f64 = ir_type_f64();
    IRValue flo = ir_value_create_int(3, f64);
    ASSERT(flo.type.kind == IR_TYPE_F64, "f64 type on value");

    char buf[64];
    ir_value_name_buf(reg, buf, sizeof(buf));
    ASSERT(strstr(buf, "99") != NULL, "name contains 99");
}

static void test_optimizer_edge_cases(void) {
    printf("\n--- Optimizer: Edge Cases ---\n");
    IRType i64 = ir_type_i64();

    IRFunction* func1 = ir_function_create("opt_edge", i64);
    IRBasicBlock* bb1 = ir_function_add_block(func1, "entry");
    ir_bb_add_instruction(bb1, ir_inst_ret(ir_value_create_int(42, i64)));

    Optimizer* opt1 = optimizer_create();
    optimizer_add_default_passes(opt1);
    int changed1 = optimizer_run_all(opt1, func1);
    ASSERT(changed1 >= 0, "run on simple function returns >= 0");
    ir_function_destroy(func1);
    optimizer_destroy(opt1);

    IRFunction* func2 = ir_function_create("opt_empty", i64);
    IRBasicBlock* bb2 = ir_function_add_block(func2, "entry");
    ir_bb_add_instruction(bb2, ir_inst_ret(ir_value_create_int(0, i64)));

    Optimizer* opt2 = optimizer_create();
    optimizer_add_pass(opt2, OPT_PASS_DEAD_CODE_ELIMINATION);
    int changed2 = optimizer_run_single(opt2, func2, OPT_PASS_DEAD_CODE_ELIMINATION);
    ASSERT(changed2 >= 0, "DCE on ret >= 0");
    ir_function_destroy(func2);
    optimizer_destroy(opt2);

    IRFunction* func3 = ir_function_create("opt_const", i64);
    IRBasicBlock* bb3 = ir_function_add_block(func3, "entry");
    ir_bb_add_instruction(bb3, ir_inst_create_2(IR_OP_ADD,
        ir_value_create_register(1, i64), ir_value_create_int(5, i64), ir_value_create_int(3, i64)));
    ir_bb_add_instruction(bb3, ir_inst_ret(ir_value_create_register(1, i64)));

    Optimizer* opt3 = optimizer_create();
    optimizer_add_pass(opt3, OPT_PASS_CONSTANT_FOLDING);
    int changed3 = optimizer_run_single(opt3, func3, OPT_PASS_CONSTANT_FOLDING);
    ASSERT(changed3 == 1, "constant folding fold add");
    ASSERT(bb3->instructions[0].opcode == IR_OP_MOV, "add folded to mov");
    ir_function_destroy(func3);
    optimizer_destroy(opt3);

    IRFunction* func4 = ir_function_create("opt_str", i64);
    IRBasicBlock* bb4 = ir_function_add_block(func4, "entry");
    ir_bb_add_instruction(bb4, ir_inst_create_2(IR_OP_MUL,
        ir_value_create_register(1, i64), ir_value_create_register(2, i64), ir_value_create_int(4, i64)));
    ir_bb_add_instruction(bb4, ir_inst_ret(ir_value_create_int(0, i64)));

    Optimizer* opt4 = optimizer_create();
    optimizer_add_pass(opt4, OPT_PASS_STRENGTH_REDUCTION);
    int changed4 = optimizer_run_single(opt4, func4, OPT_PASS_STRENGTH_REDUCTION);
    ASSERT(changed4 == 1, "strength reduction mul4 -> shl2");
    ASSERT(bb4->instructions[0].opcode == IR_OP_SHL, "mul -> shl");
    ir_function_destroy(func4);
    optimizer_destroy(opt4);
}

static void test_ssa_edge_cases(void) {
    printf("\n--- SSA: Edge Cases ---\n");
    IRType i64 = ir_type_i64();

    IRFunction* func1 = ir_function_create("ssa_single", i64);
    IRBasicBlock* bb1 = ir_function_add_block(func1, "entry");
    ir_bb_add_instruction(bb1, ir_inst_ret(ir_value_create_int(0, i64)));
    SSAContext* ssa1 = ssa_create(func1);
    ssa_insert_phi_nodes(ssa1);
    ASSERT(bb1->instruction_count == 1, "single block no phi");
    ssa_destroy(ssa1);
    ir_function_destroy(func1);

    IRFunction* func2 = ir_function_create("ssa_two", i64);
    IRBasicBlock* e2 = ir_function_add_block(func2, "entry");
    IRBasicBlock* m2 = ir_function_add_block(func2, "merge");
    ir_bb_add_instruction(e2, ir_inst_create_2(IR_OP_MOV,
        ir_value_create_register(1, i64), ir_value_null(), ir_value_create_int(1, i64)));
    ir_bb_add_instruction(e2, ir_inst_jmp("merge"));
    ir_bb_add_instruction(m2, ir_inst_ret(ir_value_create_int(0, i64)));
    ir_bb_add_successor(e2, m2);
    ir_bb_add_predecessor(m2, e2);
    SSAContext* ssa2 = ssa_create(func2);
    ssa_insert_phi_nodes(ssa2);
    ASSERT(m2->instruction_count == 1, "single pred no phi");
    ssa_destroy(ssa2);
    ir_function_destroy(func2);

    IRFunction* func3 = ir_function_create("ssa_chain", i64);
    IRBasicBlock* a = ir_function_add_block(func3, "a");
    IRBasicBlock* b = ir_function_add_block(func3, "b");
    IRBasicBlock* c = ir_function_add_block(func3, "c");
    ir_bb_add_instruction(a, ir_inst_create_2(IR_OP_MOV,
        ir_value_create_register(1, i64), ir_value_null(), ir_value_create_int(1, i64)));
    ir_bb_add_instruction(a, ir_inst_jmp("b"));
    ir_bb_add_instruction(b, ir_inst_create_2(IR_OP_MOV,
        ir_value_create_register(2, i64), ir_value_null(), ir_value_create_int(2, i64)));
    ir_bb_add_instruction(b, ir_inst_jmp("c"));
    ir_bb_add_instruction(c, ir_inst_ret(ir_value_create_int(0, i64)));
    ir_bb_add_successor(a, b);
    ir_bb_add_predecessor(b, a);
    ir_bb_add_successor(b, c);
    ir_bb_add_predecessor(c, b);
    SSAContext* ssa3 = ssa_create(func3);
    ssa_insert_phi_nodes(ssa3);
    ASSERT(c->instruction_count == 1, "linear chain no phi");
    ssa_destroy(ssa3);
    ir_function_destroy(func3);

    SSAContext* ssa4 = ssa_create(NULL);
    ASSERT(ssa4 != NULL, "ssa_create(NULL) returns context");
    ssa_destroy(ssa4);
}

static void test_register_allocator_edge_cases(void) {
    reg_alloc_set_stack_mode(0); /* فحص المخصص الفيزيائي */
    printf("\n--- Register Allocator: Edge Cases ---\n");

    RegisterAllocator* a1 = reg_alloc_create();
    ASSERT(a1 != NULL, "alloc created");
    reg_alloc_add_interval(a1, 1, 0, 5);
    reg_alloc_set_stack_mode(0);
    reg_alloc_linear_scan(a1);
    ASSERT(reg_alloc_get_frame_size(a1) == 0, "1 interval no spill");
    ASSERT(reg_alloc_get_physical(a1, 1) != REG_NONE, "1 interval gets phys reg");
    ASSERT(reg_alloc_is_spilled(a1, 1) == 0, "1 interval not spilled");
        reg_alloc_set_stack_mode(1);
    reg_alloc_destroy(a1);
    reg_alloc_set_stack_mode(1);

    RegisterAllocator* a2 = reg_alloc_create();
    for (int i = 0; i < 14; i++) {
        reg_alloc_add_interval(a2, i + 1, 0, 10);
    }
    reg_alloc_set_stack_mode(0);
    reg_alloc_linear_scan(a2);
    ASSERT(reg_alloc_get_frame_size(a2) == 0, "14 intervals fit in 14 regs");
        reg_alloc_set_stack_mode(1);
    reg_alloc_destroy(a2);
    reg_alloc_set_stack_mode(1);

    RegisterAllocator* a3 = reg_alloc_create();
    reg_alloc_add_interval(a3, 1, 0, 3);
    reg_alloc_add_interval(a3, 2, 5, 8);
    reg_alloc_set_stack_mode(0);
    reg_alloc_linear_scan(a3);
    ASSERT(reg_alloc_get_physical(a3, 1) != REG_NONE, "non-overlapping r1 phys");
    ASSERT(reg_alloc_get_physical(a3, 2) != REG_NONE, "non-overlapping r2 phys");
    reg_alloc_destroy(a3);
    reg_alloc_set_stack_mode(1);

    RegisterAllocator* a4 = reg_alloc_create();
    reg_alloc_add_interval(a4, 1, 0, 3);
    reg_alloc_add_interval(a4, 2, 1, 2);
    reg_alloc_add_interval(a4, 3, 0, 3);
    reg_alloc_set_stack_mode(0);
    reg_alloc_linear_scan(a4);
    int spills = 0;
    for (int i = 1; i <= 3; i++) {
        if (reg_alloc_is_spilled(a4, i)) spills++;
    }
    ASSERT(spills == 0, "3 intervals fit in 14 regs");
    reg_alloc_destroy(a4);
    reg_alloc_set_stack_mode(1);

    RegisterAllocator* a5 = reg_alloc_create();
    ASSERT(reg_alloc_get_physical(a5, 1) == REG_NONE, "empty alloc no phys");
    reg_alloc_destroy(a5);
    reg_alloc_set_stack_mode(1);

    RegisterAllocator* a6 = NULL;
    ASSERT(reg_alloc_get_frame_size(a6) == 0, "NULL alloc frame 0");
    ASSERT(reg_alloc_get_physical(a6, 1) == REG_NONE, "NULL alloc no phys");
}

static void test_stack_manager_edge_cases(void) {
    printf("\n--- Stack Manager: Edge Cases ---\n");

    StackManager* sm1 = stack_manager_create();
    ASSERT(sm1 != NULL, "stack manager created");
    int off_a = stack_manager_push(sm1, "a", 8);
    int off_b = stack_manager_push(sm1, "b", 4);
    ASSERT(off_a != off_b, "different offsets");
    int fs1 = stack_manager_get_frame_size(sm1);
    ASSERT(fs1 >= 12, "frame covers locals");
    stack_manager_destroy(sm1);

    StackManager* sm2 = stack_manager_create();
    int spill1 = stack_manager_push_spill(sm2, "s1");
    int spill2 = stack_manager_push_spill(sm2, "s2");
    ASSERT(spill1 != spill2, "spill offsets differ");
    int fs2 = stack_manager_get_frame_size(sm2);
    ASSERT(fs2 >= 16, "frame covers spills");
    stack_manager_destroy(sm2);

    StackManager* sm3 = stack_manager_create();
    int l1 = stack_manager_push(sm3, "x", 8);
    int s1 = stack_manager_push_spill(sm3, "sx");
    ASSERT(l1 != s1, "local and spill differ");
    int fs3 = stack_manager_get_frame_size(sm3);
    ASSERT(fs3 >= 16, "frame covers both");
    stack_manager_destroy(sm3);

    StackManager* sm4 = NULL;
    ASSERT(stack_manager_get_frame_size(sm4) == 0, "NULL sm frame 0");
    ASSERT(stack_manager_push(sm4, "z", 8) == -1, "NULL sm local -1");
    ASSERT(stack_manager_push_spill(sm4, "sz") == -1, "NULL sm spill -1");

    StackManager* sm5 = stack_manager_create();
    for (int i = 0; i < 10; i++) {
        char name[16];
        snprintf(name, sizeof(name), "v%d", i);
        stack_manager_push(sm5, name, 8);
    }
    int fs5 = stack_manager_get_frame_size(sm5);
    ASSERT(fs5 >= 80, "10 locals covered");
    stack_manager_destroy(sm5);
}

static void test_cfg_edge_cases(void) {
    printf("\n--- CFG: Edge Cases ---\n");
    IRType i64 = ir_type_i64();

    IRFunction* func1 = ir_function_create("cfg_single", i64);
    IRBasicBlock* bb1 = ir_function_add_block(func1, "entry");
    ir_bb_add_instruction(bb1, ir_inst_ret(ir_value_create_int(0, i64)));
    CFG* cfg1 = cfg_build(func1);
    ASSERT(cfg1 != NULL, "CFG built single block");
    cfg_destroy(cfg1);
    ir_function_destroy(func1);

    IRFunction* func2 = ir_function_create("cfg_two", i64);
    IRBasicBlock* a = ir_function_add_block(func2, "a");
    IRBasicBlock* b = ir_function_add_block(func2, "b");
    ir_bb_add_instruction(a, ir_inst_jmp("b"));
    ir_bb_add_instruction(b, ir_inst_ret(ir_value_create_int(0, i64)));
    ir_bb_add_successor(a, b);
    ir_bb_add_predecessor(b, a);
    CFG* cfg2 = cfg_build(func2);
    ASSERT(cfg2 != NULL, "CFG built two blocks");
    cfg_destroy(cfg2);
    ir_function_destroy(func2);

    IRFunction* func3 = ir_function_create("cfg_three", i64);
    IRBasicBlock* e = ir_function_add_block(func3, "entry");
    IRBasicBlock* t = ir_function_add_block(func3, "then");
    IRBasicBlock* m = ir_function_add_block(func3, "merge");
    ir_bb_add_instruction(e, ir_inst_create_3(IR_OP_JE,
        ir_value_create_register(1, i64), ir_value_create_int(0, i64),
        ir_value_create_label("then"), ir_value_create_label("merge")));
    ir_bb_add_instruction(t, ir_inst_jmp("merge"));
    ir_bb_add_instruction(m, ir_inst_ret(ir_value_create_int(0, i64)));
    ir_bb_add_successor(e, t);
    ir_bb_add_successor(e, m);
    ir_bb_add_predecessor(t, e);
    ir_bb_add_predecessor(m, e);
    ir_bb_add_predecessor(m, t);
    ir_bb_add_successor(t, m);
    CFG* cfg3 = cfg_build(func3);
    ASSERT(cfg3 != NULL, "CFG built three blocks");
    cfg_destroy(cfg3);
    ir_function_destroy(func3);

    IRFunction* func4 = ir_function_create("cfg_diamond", i64);
    IRBasicBlock* d_e = ir_function_add_block(func4, "entry");
    IRBasicBlock* d_l = ir_function_add_block(func4, "left");
    IRBasicBlock* d_r = ir_function_add_block(func4, "right");
    IRBasicBlock* d_m = ir_function_add_block(func4, "merge");
    ir_bb_add_instruction(d_e, ir_inst_jmp("left"));
    ir_bb_add_instruction(d_l, ir_inst_jmp("merge"));
    ir_bb_add_instruction(d_r, ir_inst_jmp("merge"));
    ir_bb_add_instruction(d_m, ir_inst_ret(ir_value_create_int(0, i64)));
    ir_bb_add_successor(d_e, d_l);
    ir_bb_add_successor(d_e, d_r);
    ir_bb_add_predecessor(d_l, d_e);
    ir_bb_add_predecessor(d_r, d_e);
    ir_bb_add_successor(d_l, d_m);
    ir_bb_add_successor(d_r, d_m);
    ir_bb_add_predecessor(d_m, d_l);
    ir_bb_add_predecessor(d_m, d_r);
    CFG* cfg4 = cfg_build(func4);
    ASSERT(cfg4 != NULL, "CFG built diamond");
    cfg_destroy(cfg4);
    ir_function_destroy(func4);
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("=== DAAD Test Expansion ===\n");

    test_optimizer_constant_folding_all_ops();
    test_optimizer_constant_folding_div_by_zero();
    test_optimizer_algebraic();
    test_optimizer_strength_reduction();
    test_optimizer_copy_propagation();
    test_optimizer_dead_code();
    test_optimizer_peephole();
    test_optimizer_run_all();
    test_optimizer_constant_propagation();
    test_optimizer_jump_optimization();
    test_optimizer_merge_blocks();
    test_ssa_phi_insertion();
    test_ssa_validate();
    test_type_registry_all_types();
    test_type_checker_binary_ops();
    test_type_checker_comparison_ops();
    test_type_checker_unary_ops();
    test_type_checker_assignment();
    test_type_checker_return();
    test_type_checker_call();
    test_scope_stack_operations();
    test_symbol_operations();
    test_scope_operations();
    test_type_compatibility();
    test_ir_verifier();
    test_liveness_comprehensive();
    test_register_allocator_intervals();
    test_stack_manager_comprehensive();
    test_label_manager_comprehensive();
    test_cfg_dominance();
    test_backend_x86_names();
    test_backend_x86_type_suffix();
    test_backend_x86_frame_size();
    test_backend_emit_simple();
    test_backend_emit_module();
    test_backend_arm_riscv_null();
    test_ir_function_comprehensive();
    test_ir_module_comprehensive();
    test_backend_param_load();
    test_backend_return();
    test_ir_printer_comprehensive();
    test_ir_value_comprehensive();

    test_ir_instruction_edge_cases();
    test_ir_value_edge_cases();
    test_optimizer_edge_cases();
    test_ssa_edge_cases();
    test_register_allocator_edge_cases();
    test_stack_manager_edge_cases();
    test_cfg_edge_cases();

    printf("\n=== Results: %d/%d passed ===\n", pass_count, test_count);
    return (pass_count == test_count) ? 0 : 1;
}
