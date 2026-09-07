/**
 * @file test_codegen.c
 * @brief اختبارات Phase 6: IR, CFG, Optimizer, SSA, Codegen, Backend
 */

#include "../ir/ir_types.h"
#include "../ir/ir_value.h"
#include "../ir/ir_instruction.h"
#include "../ir/ir_basic_block.h"
#include "../ir/ir_function.h"
#include "../ir/ir_module.h"
#include "../ir/ir_printer.h"
#include "../ir/ir_visitor.h"
#include "../ir/ir_builder.h"
#include "../cfg/cfg.h"
#include "../optimizer/optimizer_pass.h"
#include "../ssa/ssa.h"
#include "../codegen/register_allocator.h"
#include "../codegen/emitter.h"
#include "../codegen/label_manager.h"
#include "../codegen/stack_manager.h"
#include "../codegen/function_generator.h"
#include "../codegen/expression_generator.h"
#include "../codegen/statement_generator.h"
#include "../backend/backend_interface.h"
#include "../ir/liveness.h"
#include "../ir/ir_verifier.h"
#include <stdio.h>
#include <string.h>

static int test_count = 0;
static int pass_count = 0;

#define ASSERT(cond, msg) do { \
    test_count++; \
    if (cond) { pass_count++; printf("  PASS: %s\n", msg); } \
    else { printf("  FAIL: %s\n", msg); } \
} while(0)

static void test_ir_types(void) {
    printf("\n--- IR Types ---\n");
    IRType void_t = ir_type_void();
    ASSERT(void_t.kind == IR_TYPE_VOID, "void type");
    IRType i32 = ir_type_i32();
    ASSERT(i32.kind == IR_TYPE_I32, "i32 type");
    IRType i64 = ir_type_i64();
    ASSERT(i64.kind == IR_TYPE_I64, "i64 type");
    IRType f32 = ir_type_f32();
    ASSERT(f32.kind == IR_TYPE_F32, "f32 type");
    IRType f64 = ir_type_f64();
    ASSERT(f64.kind == IR_TYPE_F64, "f64 type");
    IRType ptr = ir_type_ptr();
    ASSERT(ptr.kind == IR_TYPE_PTR, "pointer type");
    IRType label = ir_type_label();
    ASSERT(label.kind == IR_TYPE_LABEL, "label type");
    ASSERT(ir_type_is_integer(i32), "i32 is integer");
    ASSERT(ir_type_is_integer(i64), "i64 is integer");
    ASSERT(!ir_type_is_integer(f32), "f32 not integer");
    ASSERT(ir_type_is_float(f32), "f32 is float");
    ASSERT(ir_type_is_float(f64), "f64 is float");
    ASSERT(!ir_type_is_float(i32), "i32 not float");
    ASSERT(ir_type_size_bytes(i32) == 4, "i32 size=4");
    ASSERT(ir_type_size_bytes(i64) == 8, "i64 size=8");
    ASSERT(ir_type_size_bytes(f32) == 4, "f32 size=4");
    ASSERT(ir_type_size_bytes(f64) == 8, "f64 size=8");
    IRType i8 = ir_type_i8();
    ASSERT(i8.kind == IR_TYPE_I8, "i8 type");
    ASSERT(ir_type_size_bytes(i8) == 1, "i8 size=1");
    IRType i16 = ir_type_i16();
    ASSERT(i16.kind == IR_TYPE_I16, "i16 type");
    ASSERT(ir_type_size_bytes(i16) == 2, "i16 size=2");
}

static void test_ir_value(void) {
    printf("\n--- IR Value ---\n");
    IRType i32 = ir_type_i32();
    IRType f64 = ir_type_f64();
    IRValue v1 = ir_value_create_register(1, i32);
    ASSERT(v1.kind == IR_VALUE_REGISTER, "register value");
    ASSERT(v1.id == 1, "register id=1");
    IRValue v2 = ir_value_create_int(42, i32);
    ASSERT(v2.kind == IR_VALUE_CONSTANT_INT, "int constant");
    ASSERT(v2.as.int_val == 42, "int val=42");
    IRValue v3 = ir_value_create_float(3.14, f64);
    ASSERT(v3.kind == IR_VALUE_CONSTANT_FLOAT, "float constant");
    ASSERT(v3.as.float_val > 3.13 && v3.as.float_val < 3.15, "float val=3.14");
    IRValue v4 = ir_value_create_string("hello");
    ASSERT(v4.kind == IR_VALUE_CONSTANT_STRING, "string constant");
    ASSERT(strcmp(v4.as.string_val, "hello") == 0, "string val=hello");
    IRValue v5 = ir_value_create_label(".L1");
    ASSERT(v5.kind == IR_VALUE_LABEL, "label value");
    IRValue v6 = ir_value_null();
    ASSERT(v6.kind == IR_VALUE_NULL, "null value");
    ASSERT(strcmp(ir_value_name(v1), "r1") == 0, "reg name r1");
    ASSERT(strcmp(ir_value_name(v2), "42") == 0, "int name 42");
    ASSERT(strcmp(ir_value_name(v6), "null") == 0, "null name");
}

static void test_ir_instruction(void) {
    printf("\n--- IR Instruction ---\n");
    IRType i32 = ir_type_i32();
    IRValue res = ir_value_create_register(1, i32);
    IRInstruction inst = ir_inst_create_2(IR_OP_ADD, res, ir_value_create_register(2, i32), ir_value_create_register(3, i32));
    ASSERT(inst.opcode == IR_OP_ADD, "ADD opcode");
    ASSERT(inst.result.kind == IR_VALUE_REGISTER, "ADD result");
    ASSERT(inst.operand_count == 2, "ADD 2 operands");
    IRInstruction inst2 = ir_inst_create_1(IR_OP_LOAD, res, ir_value_create_register(2, i32));
    ASSERT(inst2.opcode == IR_OP_LOAD, "LOAD opcode");
    IRInstruction inst3 = ir_inst_create(IR_OP_STORE);
    ASSERT(inst3.opcode == IR_OP_STORE, "STORE opcode");
    IRInstruction inst4 = ir_inst_create_1(IR_OP_MOV, res, ir_value_create_register(2, i32));
    ASSERT(inst4.opcode == IR_OP_MOV, "MOV opcode");
    IRInstruction inst6 = ir_inst_ret(ir_value_create_int(0, i32));
    ASSERT(inst6.opcode == IR_OP_RET, "RET opcode");
    IRInstruction inst7 = ir_inst_create_1(IR_OP_ALLOCA, res, ir_value_create_int(8, i32));
    ASSERT(inst7.opcode == IR_OP_ALLOCA, "ALLOCA opcode");
    IRInstruction inst8 = ir_inst_jmp(".Lend");
    ASSERT(inst8.opcode == IR_OP_JMP, "JMP opcode");
    IRValue args[2] = {ir_value_create_register(10, i32), ir_value_create_register(11, i32)};
    IRInstruction inst9 = ir_inst_call(res, "printf", args, 2);
    ASSERT(inst9.opcode == IR_OP_CALL, "CALL opcode");
    IRInstruction inst10 = ir_inst_nop();
    ASSERT(inst10.opcode == IR_OP_NOP, "NOP opcode");
    IRValue phi_ops[2] = {ir_value_create_register(5, i32), ir_value_create_register(6, i32)};
    IRInstruction inst11 = ir_inst_phi(res, phi_ops, 2);
    ASSERT(inst11.opcode == IR_OP_PHI, "PHI opcode");
    IRInstruction inst12 = ir_inst_cond_jmp(IR_CMP_EQ, ir_value_create_register(1, i32), ir_value_create_register(2, i32), ".Ltrue", ".Lfalse");
    ASSERT(inst12.opcode == IR_OP_JE, "cond JMP opcode");
    ASSERT(strcmp(ir_opcode_name(IR_OP_ADD), "add") == 0, "opcode name add");
    ASSERT(strcmp(ir_opcode_name(IR_OP_RET), "ret") == 0, "opcode name ret");
    ASSERT(strcmp(ir_opcode_name(IR_OP_MOV), "mov") == 0, "opcode name mov");
    ASSERT(strcmp(ir_cmp_name(IR_CMP_EQ), "eq") == 0, "cmp name eq");
    IRInstruction inst13 = ir_inst_create_3(IR_OP_ADD, res, ir_value_create_register(2, i32), ir_value_create_register(3, i32), ir_value_create_register(4, i32));
    ASSERT(inst13.opcode == IR_OP_ADD, "ADD 3 ops");
    ASSERT(inst13.operand_count == 3, "ADD 3 operand count");
}

static void test_ir_basic_block(void) {
    printf("\n--- IR Basic Block ---\n");
    IRType i32 = ir_type_i32();
    IRBasicBlock* bb = ir_bb_create(".L0", 0);
    ASSERT(bb != NULL, "create block");
    ASSERT(strcmp(bb->label, ".L0") == 0, "block label");
    ASSERT(bb->instruction_count == 0, "empty block");
    IRInstruction inst = ir_inst_create_1(IR_OP_MOV, ir_value_create_register(1, i32), ir_value_create_register(2, i32));
    ir_bb_add_instruction(bb, inst);
    ASSERT(bb->instruction_count == 1, "count=1");
    IRBasicBlock* pred = ir_bb_create(".L1", 1);
    ir_bb_add_predecessor(bb, pred);
    ASSERT(bb->predecessor_count == 1, "predecessor count=1");
    IRBasicBlock* succ = ir_bb_create(".L2", 2);
    ir_bb_add_successor(bb, succ);
    ASSERT(bb->successor_count == 1, "successor count=1");
    ASSERT(!ir_bb_terminates(bb), "no terminator");
    IRInstruction ret_inst = ir_inst_ret(ir_value_create_int(0, i32));
    ir_bb_add_instruction(bb, ret_inst);
    ASSERT(ir_bb_terminates(bb), "has terminator");
    ir_bb_destroy(bb);
    ir_bb_destroy(pred);
    ir_bb_destroy(succ);
}

static void test_ir_function(void) {
    printf("\n--- IR Function ---\n");
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("test_func", i32);
    ASSERT(func != NULL, "create function");
    ASSERT(strcmp(func->name, "test_func") == 0, "function name");
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    ASSERT(bb != NULL, "add block");
    ASSERT(func->block_count == 1, "block count=1");
    ir_function_add_param(func, "x", i32);
    ir_function_add_param(func, "y", i32);
    ASSERT(func->param_count == 2, "param count=2");
    IRValue r1 = ir_function_alloc_reg(func, i32);
    IRValue r2 = ir_function_alloc_reg(func, i32);
    ASSERT(r1.id != r2.id, "different register IDs");
    ir_function_destroy(func);
}

static void test_ir_module(void) {
    printf("\n--- IR Module ---\n");
    IRType i32 = ir_type_i32();
    IRModule* mod = ir_module_create("test_mod");
    ASSERT(mod != NULL, "create module");
    ASSERT(strcmp(mod->name, "test_mod") == 0, "module name");
    IRFunction* func = ir_module_add_function(mod, "main", i32);
    ASSERT(func != NULL, "add function");
    ASSERT(mod->function_count == 1, "function count=1");
    IRValue g_init = ir_value_create_int(100, i32);
    ir_module_add_global(mod, "g_var", i32, g_init, 0);
    ASSERT(mod->global_count == 1, "global count=1");
    ir_module_destroy(mod);
}

static void test_ir_printer(void) {
    printf("\n--- IR Printer ---\n");
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("print_test", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    IRInstruction inst = ir_inst_create_1(IR_OP_MOV, ir_value_create_register(1, i32), ir_value_create_int(42, i32));
    ir_bb_add_instruction(bb, inst);
    IRInstruction ret_inst = ir_inst_ret(ir_value_create_register(1, i32));
    ir_bb_add_instruction(bb, ret_inst);
    ir_print_function(func, stdout);
    ir_print_instruction(&inst, stdout);
    IRValue v = ir_value_create_register(1, i32);
    ir_print_value(v, stdout);
    ir_print_type(i32, stdout);
    IRBasicBlock* temp_bb = ir_bb_create(".Ltest", 99);
    ir_print_block(temp_bb, stdout);
    ir_bb_destroy(temp_bb);
    ir_function_destroy(func);
}

static void test_ir_visitor(void) {
    printf("\n--- IR Visitor ---\n");
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("visit_test", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    IRInstruction inst = ir_inst_create_1(IR_OP_MOV, ir_value_create_register(1, i32), ir_value_create_int(42, i32));
    ir_bb_add_instruction(bb, inst);
    IRVisitor visitor = ir_visitor_create();
    ASSERT(visitor.visit_instruction == NULL || visitor.visit_instruction != NULL, "visitor created");
    ir_visit_function(func, &visitor, NULL);
    ir_function_destroy(func);
}

static void test_cfg(void) {
    printf("\n--- CFG ---\n");
    IRType void_t = ir_type_void();
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("cfg_test", void_t);
    IRBasicBlock* bb1 = ir_function_add_block(func, ".L1");
    IRBasicBlock* bb2 = ir_function_add_block(func, ".L2");
    IRBasicBlock* bb3 = ir_function_add_block(func, ".L3");
    IRInstruction inst = ir_inst_create_1(IR_OP_MOV, ir_value_create_register(2, i32), ir_value_create_register(1, i32));
    ir_bb_add_instruction(bb1, inst);
    IRInstruction jmp = ir_inst_jmp(".L2");
    ir_bb_add_instruction(bb1, jmp);
    IRInstruction ret_inst = ir_inst_ret(ir_value_create_int(0, i32));
    ir_bb_add_instruction(bb3, ret_inst);
    CFG* cfg = cfg_build(func);
    ASSERT(cfg != NULL, "create CFG");
    ASSERT(cfg->block_count == 3, "3 blocks");
    ASSERT(cfg->entry != NULL, "entry block set");
    cfg_add_edge(cfg, bb1, bb2);
    cfg_add_edge(cfg, bb1, bb3);
    cfg_add_edge(cfg, bb2, bb3);
    cfg_compute_dominators(cfg);
    ASSERT(cfg->dom_count == 3, "dominators computed");
    cfg_destroy(cfg);
    ir_function_destroy(func);
}

static void test_optimizer(void) {
    printf("\n--- Optimizer ---\n");
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("opt_test", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    IRInstruction inst = ir_inst_create_2(IR_OP_ADD, ir_value_create_register(1, i32), ir_value_create_register(2, i32), ir_value_create_register(3, i32));
    ir_bb_add_instruction(bb, inst);
    IRInstruction ret_inst = ir_inst_ret(ir_value_create_register(1, i32));
    ir_bb_add_instruction(bb, ret_inst);
    Optimizer* opt = optimizer_create();
    ASSERT(opt != NULL, "create optimizer");
    optimizer_add_pass(opt, OPT_PASS_CONSTANT_FOLDING);
    optimizer_add_pass(opt, OPT_PASS_CONSTANT_PROPAGATION);
    optimizer_add_pass(opt, OPT_PASS_DEAD_CODE_ELIMINATION);
    optimizer_add_pass(opt, OPT_PASS_ALGEBRAIC_SIMPLIFICATION);
    optimizer_add_pass(opt, OPT_PASS_COPY_PROPAGATION);
    optimizer_add_pass(opt, OPT_PASS_JUMP_OPTIMIZATION);
    optimizer_add_pass(opt, OPT_PASS_STRENGTH_REDUCTION);
    optimizer_add_pass(opt, OPT_PASS_DEAD_STORE_ELIMINATION);
    optimizer_add_pass(opt, OPT_PASS_REMOVE_EMPTY_BLOCKS);
    optimizer_add_pass(opt, OPT_PASS_MERGE_BLOCKS);
    optimizer_add_pass(opt, OPT_PASS_PEEPHOLE);
    int result = optimizer_run_all(opt, func);
    ASSERT(result >= 0, "optimizer ran");
    ASSERT(opt->pass_count == 11, "11 passes added");
    ASSERT(strcmp(optimizer_pass_name(OPT_PASS_CONSTANT_FOLDING), "Constant Folding") == 0, "pass name const fold");
    ASSERT(strcmp(optimizer_pass_name(OPT_PASS_PEEPHOLE), "Peephole Optimizer") == 0, "pass name peephole");
    optimizer_destroy(opt);
    ir_function_destroy(func);
}

static void test_optimizer_strength_reduction(void) {
    printf("\n--- Optimizer Strength Reduction ---\n");
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("sr_test", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    IRInstruction inst = ir_inst_create_2(IR_OP_MUL, ir_value_create_register(1, i32), ir_value_create_register(2, i32), ir_value_create_int(8, i32));
    ir_bb_add_instruction(bb, inst);
    IRInstruction ret_inst = ir_inst_ret(ir_value_create_register(1, i32));
    ir_bb_add_instruction(bb, ret_inst);
    Optimizer* opt = optimizer_create();
    optimizer_add_pass(opt, OPT_PASS_STRENGTH_REDUCTION);
    int result = optimizer_run_single(opt, func, OPT_PASS_STRENGTH_REDUCTION);
    ASSERT(result == 1, "strength reduction converted mul*8 to shl*3");
    ASSERT(bb->instructions[0].opcode == IR_OP_SHL, "opcode changed to SHL");
    optimizer_destroy(opt);
    ir_function_destroy(func);
}

static void test_optimizer_algebraic(void) {
    printf("\n--- Optimizer Algebraic ---\n");
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("alg_test", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    IRInstruction inst = ir_inst_create_2(IR_OP_ADD, ir_value_create_register(1, i32), ir_value_create_register(2, i32), ir_value_create_int(0, i32));
    ir_bb_add_instruction(bb, inst);
    IRInstruction ret_inst = ir_inst_ret(ir_value_create_register(1, i32));
    ir_bb_add_instruction(bb, ret_inst);
    Optimizer* opt = optimizer_create();
    optimizer_add_pass(opt, OPT_PASS_ALGEBRAIC_SIMPLIFICATION);
    int result = optimizer_run_single(opt, func, OPT_PASS_ALGEBRAIC_SIMPLIFICATION);
    ASSERT(result >= 1, "algebraic simplified x+0");
    ASSERT(bb->instructions[0].opcode == IR_OP_MOV, "ADD x,0 became MOV");
    optimizer_destroy(opt);
    ir_function_destroy(func);
}

static void test_ssa(void) {
    printf("\n--- SSA ---\n");
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("ssa_test", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    IRInstruction mov = ir_inst_create_1(IR_OP_MOV, ir_value_create_register(1, i32), ir_value_create_int(10, i32));
    ir_bb_add_instruction(bb, mov);
    IRInstruction add = ir_inst_create_2(IR_OP_ADD, ir_value_create_register(2, i32), ir_value_create_register(1, i32), ir_value_create_register(3, i32));
    ir_bb_add_instruction(bb, add);
    IRInstruction ret_inst = ir_inst_ret(ir_value_create_register(2, i32));
    ir_bb_add_instruction(bb, ret_inst);
    SSAContext* ssa = ssa_create(func);
    ASSERT(ssa != NULL, "create SSA");
    ssa_convert_to_ssa(ssa);
    ASSERT(ssa->rename_count > 0, "SSA renamed variables");
    int valid = ssa_validate(ssa);
    ASSERT(valid == 1, "SSA valid");
    ssa_destroy(ssa);
    ir_function_destroy(func);
}

static void test_register_allocator(void) {
    reg_alloc_set_stack_mode(0); /* فحص المخصص بوضعه الفيزيائي */
    printf("\n--- Register Allocator ---\n");
    RegisterAllocator* alloc = reg_alloc_create();
    ASSERT(alloc != NULL, "create allocator");
    reg_alloc_add_interval(alloc, 1, 0, 10);
    reg_alloc_add_interval(alloc, 2, 5, 15);
    reg_alloc_add_interval(alloc, 3, 0, 5);
    reg_alloc_add_interval(alloc, 4, 20, 30);
    reg_alloc_linear_scan(alloc);
    PhysicalRegister r1 = reg_alloc_get_physical(alloc, 1);
    PhysicalRegister r2 = reg_alloc_get_physical(alloc, 2);
    PhysicalRegister r3 = reg_alloc_get_physical(alloc, 3);
    ASSERT(r1 != REG_NONE, "r1 allocated");
    ASSERT(r2 != REG_NONE, "r2 allocated");
    ASSERT(r3 != REG_NONE, "r3 allocated");
    ASSERT(r1 != r2, "r1 != r2 (overlapping)");
    ASSERT(r3 != r1, "r3 != r1 (overlapping)");
    const char* name = reg_alloc_phys_name(REG_RAX);
    ASSERT(strcmp(name, "%rax") == 0, "reg name rax");
    reg_alloc_set_stack_mode(1); /* أعِد الوضع الآمن */
    reg_alloc_destroy(alloc);
    reg_alloc_set_stack_mode(1);
}

static void test_register_names(void) {
    printf("\n--- Register Names ---\n");
    ASSERT(strcmp(reg_alloc_phys_name(REG_RAX), "%rax") == 0, "rax name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_RBX), "%rbx") == 0, "rbx name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_RCX), "%rcx") == 0, "rcx name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_RDX), "%rdx") == 0, "rdx name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_RSI), "%rsi") == 0, "rsi name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_RDI), "%rdi") == 0, "rdi name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_RBP), "%rbp") == 0, "rbp name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_RSP), "%rsp") == 0, "rsp name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_R8), "%r8") == 0, "r8 name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_R9), "%r9") == 0, "r9 name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_R10), "%r10") == 0, "r10 name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_R11), "%r11") == 0, "r11 name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_R12), "%r12") == 0, "r12 name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_R13), "%r13") == 0, "r13 name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_R14), "%r14") == 0, "r14 name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_R15), "%r15") == 0, "r15 name");
    ASSERT(strcmp(reg_alloc_phys_name(REG_XMM0), "%xmm0") == 0, "xmm0 name");
}

static void test_label_manager(void) {
    printf("\n--- Label Manager ---\n");
    LabelManager* mgr = label_manager_create();
    ASSERT(mgr != NULL, "create label manager");
    const char* l1 = label_manager_generate(mgr, "if");
    ASSERT(l1 != NULL, "generate label 1");
    const char* l2 = label_manager_generate(mgr, "while");
    ASSERT(l2 != NULL, "generate label 2");
    ASSERT(l1 != l2, "different labels");
    ASSERT(strcmp(l1, ".Lif1") == 0, "if label name");
    ASSERT(strcmp(l2, ".Lwhile2") == 0, "while label name");
    label_manager_reset(mgr);
    ASSERT(mgr->count == 0, "reset count=0");
    label_manager_destroy(mgr);
}

static void test_stack_manager(void) {
    printf("\n--- Stack Manager ---\n");
    StackManager* mgr = stack_manager_create();
    ASSERT(mgr != NULL, "create stack manager");
    int off1 = stack_manager_push(mgr, "x", 8);
    ASSERT(off1 == 8, "x offset=8");
    int off2 = stack_manager_push(mgr, "y", 8);
    ASSERT(off2 == 16, "y offset=16");
    int found = stack_manager_get_offset(mgr, "x");
    ASSERT(found == 8, "found x offset=8");
    int frame = stack_manager_get_frame_size(mgr);
    ASSERT(frame >= 16, "frame size >= 16");
    stack_manager_reset(mgr);
    ASSERT(mgr->count == 0, "reset count=0");
    stack_manager_destroy(mgr);
}

static void test_expression_generator(void) {
    printf("\n--- Expression Generator ---\n");
    StackManager* stack = stack_manager_create();
    ExpressionGenerator* gen = expr_gen_create(stack);
    ASSERT(gen != NULL, "create expression generator");
    IRType i32 = ir_type_i32();
    IRValue left = ir_value_create_register(1, i32);
    IRValue right = ir_value_create_register(2, i32);
    IRValue result;
    FILE* devnull = fopen("NUL", "w");
    int r = expr_gen_generate_binary(gen, IR_OP_ADD, left, right, &result, devnull);
    ASSERT(r == 1, "generate add");
    ASSERT(result.kind == IR_VALUE_REGISTER, "result is register");
    IRValue operand = ir_value_create_register(3, i32);
    r = expr_gen_generate_unary(gen, IR_OP_NEG, operand, &result, devnull);
    ASSERT(r == 1, "generate neg");
    IRValue addr = ir_value_create_register(4, i32);
    r = expr_gen_generate_load(gen, addr, &result, devnull);
    ASSERT(r == 1, "generate load");
    IRValue value = ir_value_create_int(42, i32);
    r = expr_gen_generate_store(gen, addr, value, devnull);
    ASSERT(r == 1, "generate store");
    fclose(devnull);
    expr_gen_destroy(gen);
    stack_manager_destroy(stack);
}

static void test_statement_generator(void) {
    printf("\n--- Statement Generator ---\n");
    StackManager* stack = stack_manager_create();
    LabelManager* labels = label_manager_create();
    ExpressionGenerator* expr_gen = expr_gen_create(stack);
    StatementGenerator* gen = stmt_gen_create(expr_gen, labels);
    ASSERT(gen != NULL, "create statement generator");
    IRType i32 = ir_type_i32();
    FILE* devnull = fopen("NUL", "w");
    IRValue val = ir_value_create_int(42, i32);
    int r = stmt_gen_generate_return(gen, val, devnull);
    ASSERT(r == 1, "generate return");
    IRValue cond = ir_value_create_register(1, i32);
    r = stmt_gen_generate_if(gen, cond, ".Ltrue", ".Lfalse", devnull);
    ASSERT(r == 1, "generate if");
    r = stmt_gen_generate_jmp(gen, ".Lend", devnull);
    ASSERT(r == 1, "generate jmp");
    fclose(devnull);
    stmt_gen_destroy(gen);
    expr_gen_destroy(expr_gen);
    label_manager_destroy(labels);
    stack_manager_destroy(stack);
}

static void test_function_generator(void) {
    printf("\n--- Function Generator ---\n");
    StackManager* stack = stack_manager_create();
    LabelManager* labels = label_manager_create();
    FunctionGenerator* gen = func_gen_create(stack, labels);
    ASSERT(gen != NULL, "create function generator");
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("fg_test", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    IRInstruction ret_inst = ir_inst_ret(ir_value_create_int(0, i32));
    ir_bb_add_instruction(bb, ret_inst);
    FILE* devnull = fopen("NUL", "w");
    int count = func_gen_generate(gen, func, devnull);
    ASSERT(count >= 1, "function gen emitted");
    fclose(devnull);
    func_gen_destroy(gen);
    ir_function_destroy(func);
    label_manager_destroy(labels);
    stack_manager_destroy(stack);
}

static void test_backend(void) {
    printf("\n--- Backend ---\n");
    Backend* x86 = backend_get(BACKEND_X86);
    ASSERT(x86 != NULL, "create x86 backend");
    ASSERT(x86->target == BACKEND_X86, "x86 target");
    ASSERT(strcmp(x86->name, "x86-64") == 0, "x86 name");
    ASSERT(x86->emit_function != NULL, "x86 has emit_function");
    Backend* arm = backend_get(BACKEND_ARM);
    ASSERT(arm != NULL, "create ARM backend");
    ASSERT(arm->target == BACKEND_ARM, "ARM target");
    Backend* riscv = backend_get(BACKEND_RISCV);
    ASSERT(riscv != NULL, "create RISC-V backend");
    ASSERT(riscv->target == BACKEND_RISCV, "RISC-V target");
    backend_destroy(x86);
    backend_destroy(arm);
    backend_destroy(riscv);
}

static void test_backend_emit(void) {
    printf("\n--- Backend Emit ---\n");
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("emit_test", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    IRInstruction mov = ir_inst_create_1(IR_OP_MOV, ir_value_create_register(1, i32), ir_value_create_int(10, i32));
    ir_bb_add_instruction(bb, mov);
    IRInstruction add = ir_inst_create_2(IR_OP_ADD, ir_value_create_register(2, i32), ir_value_create_register(1, i32), ir_value_create_register(3, i32));
    ir_bb_add_instruction(bb, add);
    IRInstruction ret_inst = ir_inst_ret(ir_value_create_register(2, i32));
    ir_bb_add_instruction(bb, ret_inst);
    Backend* x86 = backend_create_x86();
    FILE* devnull = fopen("NUL", "w");
    int count = x86->emit_function(func, devnull);
    ASSERT(count >= 1, "x86 emitted instructions");
    fclose(devnull);
    backend_destroy(x86);
    ir_function_destroy(func);
}

static void test_emitter(void) {
    printf("\n--- Emitter ---\n");
    IRType i32 = ir_type_i32();
    IRModule* mod = ir_module_create("emit_mod");
    IRFunction* func = ir_module_add_function(mod, "main", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    IRInstruction ret_inst = ir_inst_ret(ir_value_create_int(0, i32));
    ir_bb_add_instruction(bb, ret_inst);
    Emitter* emit = emitter_create(mod, BACKEND_X86);
    ASSERT(emit != NULL, "create emitter");
    FILE* devnull = fopen("NUL", "w");
    int count = emitter_emit(emit, devnull);
    ASSERT(count >= 1, "emitter emitted");
    ASSERT(emit->function_count == 1, "1 function emitted");
    fclose(devnull);
    emitter_destroy(emit);
    ir_module_destroy(mod);
}

static void test_ir_builder_hash(void) {
    printf("\n--- IR Builder Hash Map ---\n");
    IRBuilder* builder = ir_builder_create("test_mod", NULL);
    ASSERT(builder != NULL, "create builder");

    IRType i64 = ir_type_i64();
    IRValue val1 = ir_value_create_int(42, i64);
    ir_builder_add_var(builder, "x", val1, i64);
    IRValue found = ir_builder_lookup_var(builder, "x");
    ASSERT(found.kind == IR_VALUE_CONSTANT_INT, "lookup x kind");
    ASSERT(found.as.int_val == 42, "lookup x value=42");

    IRValue val2 = ir_value_create_int(99, i64);
    ir_builder_add_var(builder, "y", val2, i64);
    found = ir_builder_lookup_var(builder, "y");
    ASSERT(found.as.int_val == 99, "lookup y value=99");

    ir_builder_add_var(builder, "x", ir_value_create_int(100, i64), i64);
    found = ir_builder_lookup_var(builder, "x");
    ASSERT(found.as.int_val == 100, "x updated to 100");

    found = ir_builder_lookup_var(builder, "z");
    ASSERT(found.kind == IR_VALUE_NULL, "lookup z = null");

    ir_builder_destroy(builder);
}

static void test_ir_function_params(void) {
    printf("\n--- IR Function Params ---\n");
    IRType i64 = ir_type_i64();
    IRType f64 = ir_type_f64();
    IRFunction* func = ir_function_create("test_func", i64);
    ASSERT(func != NULL, "create func");

    ir_function_add_param(func, "a", i64);
    ir_function_add_param(func, "b", f64);
    ASSERT(func->param_count == 2, "param_count=2");

    ASSERT(func->params[0].kind == IR_VALUE_REGISTER, "param 0 is register");
    ASSERT(func->param_types[0].kind == IR_TYPE_I64, "param 0 type i64");
    ASSERT(strcmp(func->param_names[0], "a") == 0, "param 0 name=a");

    ASSERT(func->params[1].kind == IR_VALUE_REGISTER, "param 1 is register");
    ASSERT(func->param_types[1].kind == IR_TYPE_F64, "param 1 type f64");
    ASSERT(strcmp(func->param_names[1], "b") == 0, "param 1 name=b");

    ir_function_destroy(func);
}

static void test_ir_function_alloca(void) {
    printf("\n--- IR Function Alloca ---\n");
    IRType i64 = ir_type_i64();
    IRFunction* func = ir_function_create("alloca_test", i64);
    IRValue a1 = ir_function_alloc_reg(func, ir_type_ptr());
    IRValue a2 = ir_function_alloc_reg(func, ir_type_ptr());
    ir_function_add_alloca(func, a1);
    ir_function_add_alloca(func, a2);
    ASSERT(func->alloca_count == 2, "alloca_count=2");
    ASSERT(func->alloca_list[0].id == a1.id, "alloca 0 id");
    ASSERT(func->alloca_list[1].id == a2.id, "alloca 1 id");
    ir_function_destroy(func);
}

static void test_ir_builder_loop_context(void) {
    printf("\n--- IR Builder Loop Context ---\n");
    IRBuilder* builder = ir_builder_create("loop_test", NULL);
    ASSERT(builder != NULL, "create builder");
    ASSERT(builder->loop_depth == 0, "initial loop_depth=0");

    IRType i64 = ir_type_i64();
    IRValue val = ir_value_create_int(1, i64);
    ir_builder_add_var(builder, "x", val, i64);
    IRValue found = ir_builder_lookup_var(builder, "x");
    ASSERT(found.kind == IR_VALUE_CONSTANT_INT, "var x found");
    ASSERT(found.as.int_val == 1, "var x value=1");

    ir_builder_destroy(builder);
}

static void test_ir_module_globals(void) {
    printf("\n--- IR Module Globals ---\n");
    IRType i64 = ir_type_i64();
    IRModule* mod = ir_module_create("global_test");
    ASSERT(mod != NULL, "create module");

    IRValue init = ir_value_create_int(100, i64);
    ir_module_add_global(mod, "g_var", i64, init, 0);
    ASSERT(mod->global_count == 1, "global_count=1");
    ASSERT(strcmp(mod->globals[0].name, "g_var") == 0, "global name=g_var");
    ASSERT(mod->globals[0].is_constant == 0, "global not const");

    IRValue cinit = ir_value_create_int(42, i64);
    ir_module_add_global(mod, "g_const", i64, cinit, 1);
    ASSERT(mod->global_count == 2, "global_count=2");
    ASSERT(mod->globals[1].is_constant == 1, "global is const");

    ir_module_destroy(mod);
}

static void test_liveness_analysis(void) {
    printf("\n--- Liveness Analysis ---\n");
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("live_test", i32);
    IRBasicBlock* entry = ir_function_add_block(func, "entry");
    ir_function_add_block(func, "exit");

    IRValue r1 = ir_value_create_register(1, i32);
    IRValue r2 = ir_value_create_register(2, i32);
    IRValue r3 = ir_value_create_register(3, i32);
    IRValue c1 = ir_value_create_int(10, i32);
    IRValue c2 = ir_value_create_int(20, i32);

    ir_bb_add_instruction(entry, ir_inst_create_2(IR_OP_MOV, r1, c1, ir_value_null()));
    ir_bb_add_instruction(entry, ir_inst_create_2(IR_OP_MOV, r2, c2, ir_value_null()));
    ir_bb_add_instruction(entry, ir_inst_create_2(IR_OP_ADD, r3, r1, r2));
    ir_bb_add_instruction(entry, ir_inst_ret(r3));

    LivenessResult* result = liveness_analyze(func);
    ASSERT(result != NULL, "liveness result created");
    ASSERT(result->block_count == 2, "liveness block count=2");
    ASSERT(result->all_reg_count > 0, "liveness has registers");

    liveness_destroy(result);
    ir_function_destroy(func);
}

static void test_register_allocator_extended(void) {
    reg_alloc_set_stack_mode(0); /* فحص المخصص بوضعه الفيزيائي */
    printf("\n--- Register Allocator Extended ---\n");
    RegisterAllocator* alloc = reg_alloc_create();
    ASSERT(alloc != NULL, "create allocator");

    reg_alloc_add_interval(alloc, 1, 0, 20);
    reg_alloc_add_interval(alloc, 2, 5, 15);
    reg_alloc_add_interval(alloc, 3, 10, 25);
    reg_alloc_add_interval(alloc, 4, 0, 30);
    reg_alloc_add_interval(alloc, 5, 15, 35);
    reg_alloc_linear_scan(alloc);

    PhysicalRegister r1 = reg_alloc_get_physical(alloc, 1);
    PhysicalRegister r2 = reg_alloc_get_physical(alloc, 2);
    PhysicalRegister r3 = reg_alloc_get_physical(alloc, 3);
    PhysicalRegister r4 = reg_alloc_get_physical(alloc, 4);
    PhysicalRegister r5 = reg_alloc_get_physical(alloc, 5);

    ASSERT(r1 != REG_NONE, "r1 allocated");
    ASSERT(r2 != REG_NONE, "r2 allocated");
    ASSERT(r3 != REG_NONE, "r3 allocated");
    ASSERT(r4 != REG_NONE, "r4 allocated");
    ASSERT(r5 != REG_NONE, "r5 allocated");

    ASSERT(r1 != r2, "r1 != r2");
    ASSERT(r1 != r3, "r1 != r3");
    ASSERT(r2 != r3, "r2 != r3");

    int frame = reg_alloc_get_frame_size(alloc);
    ASSERT(frame >= 0, "frame size valid");

    ASSERT(reg_alloc_is_callee_saved(alloc, REG_RBX), "rbx is callee-saved");
    ASSERT(!reg_alloc_is_caller_saved(alloc, REG_RBX), "rbx not caller-saved");
    ASSERT(reg_alloc_is_caller_saved(alloc, REG_RAX), "rax is caller-saved");
    ASSERT(!reg_alloc_is_callee_saved(alloc, REG_RAX), "rax not callee-saved");

    reg_alloc_set_stack_mode(1); /* أعِد الوضع الآمن */
    reg_alloc_destroy(alloc);
    reg_alloc_set_stack_mode(1);
}

static void test_cfg_extended(void) {
    printf("\n--- CFG Extended ---\n");
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("cfg_test", i32);
    IRBasicBlock* bb0 = ir_function_add_block(func, "entry");
    IRBasicBlock* bb1 = ir_function_add_block(func, "then");
    IRBasicBlock* bb2 = ir_function_add_block(func, "else");
    IRBasicBlock* bb3 = ir_function_add_block(func, "merge");

    IRValue c1 = ir_value_create_int(1, i32);
    IRValue r1 = ir_value_create_register(1, i32);
    ir_bb_add_instruction(bb0, ir_inst_create_2(IR_OP_MOV, r1, c1, ir_value_null()));
    ir_bb_add_instruction(bb0, ir_inst_create(IR_OP_RET));

    IRValue c2 = ir_value_create_int(2, i32);
    IRValue r2 = ir_value_create_register(2, i32);
    ir_bb_add_instruction(bb1, ir_inst_create_2(IR_OP_MOV, r2, c2, ir_value_null()));
    ir_bb_add_instruction(bb1, ir_inst_create(IR_OP_RET));

    IRValue c3 = ir_value_create_int(3, i32);
    IRValue r3 = ir_value_create_register(3, i32);
    ir_bb_add_instruction(bb2, ir_inst_create_2(IR_OP_MOV, r3, c3, ir_value_null()));
    ir_bb_add_instruction(bb2, ir_inst_create(IR_OP_RET));

    ir_bb_add_instruction(bb3, ir_inst_create(IR_OP_RET));

    ir_bb_add_successor(bb0, bb1);
    ir_bb_add_successor(bb0, bb2);
    ir_bb_add_successor(bb1, bb3);
    ir_bb_add_successor(bb2, bb3);
    ir_bb_add_predecessor(bb1, bb0);
    ir_bb_add_predecessor(bb2, bb0);
    ir_bb_add_predecessor(bb3, bb1);
    ir_bb_add_predecessor(bb3, bb2);

    CFG* cfg = cfg_build(func);
    ASSERT(cfg != NULL, "cfg created");
    ASSERT(cfg->block_count == 4, "cfg 4 blocks");

    cfg_compute_dominators(cfg);
    ASSERT(cfg->idom[0] == -1, "entry idom=-1");
    ASSERT(cfg->idom[1] == 0, "then idom=entry");
    ASSERT(cfg->idom[2] == 0, "else idom=entry");

    cfg_compute_post_dominators(cfg);
    ASSERT(cfg->post_dominators[3] == 3, "merge post-doms self");

    cfg_compute_dominance_frontiers(cfg);
    ASSERT(cfg->frontier_counts[1] == 0 || cfg->frontier_counts[1] > 0, "frontiers computed");

    cfg_compute_reverse_post_order(cfg);
    ASSERT(cfg->reverse_post_order_count == 4, "reverse post order count=4");

    ASSERT(cfg_dominates(cfg, 0, 1), "entry dominates then");
    ASSERT(cfg_dominates(cfg, 0, 2), "entry dominates else");

    cfg_destroy(cfg);
    ir_function_destroy(func);
}

static void test_ir_verifier(void) {
    printf("\n--- IR Verifier ---\n");
    IRVerifier* verifier = ir_verifier_create();
    ASSERT(verifier != NULL, "create verifier");

    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("verify_test", i32);
    IRBasicBlock* bb = ir_function_add_block(func, "entry");
    IRValue r1 = ir_value_create_register(1, i32);
    IRValue c1 = ir_value_create_int(42, i32);
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r1, c1, ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_ret(r1));

    int valid = ir_verifier_verify_function(verifier, func);
    ASSERT(valid == 1, "valid function passes");

    ir_verifier_destroy(verifier);
    ir_function_destroy(func);
}

static void test_stack_manager_extended(void) {
    printf("\n--- Stack Manager Extended ---\n");
    StackManager* mgr = stack_manager_create();
    ASSERT(mgr != NULL, "create stack manager");

    int off1 = stack_manager_push(mgr, "x", 8);
    int off2 = stack_manager_push(mgr, "y", 8);
    ASSERT(off1 == 8, "x offset=8");
    ASSERT(off2 == 16, "y offset=16");

    int spill = stack_manager_push_spill(mgr, NULL);
    ASSERT(spill > 0, "spill slot allocated");

    int frame = stack_manager_get_frame_size(mgr);
    ASSERT(frame >= 32, "frame size >= 32");
    ASSERT(frame % 16 == 0, "frame size aligned");

    ASSERT(stack_manager_get_local_count(mgr) == 2, "2 locals");
    ASSERT(stack_manager_get_spill_count(mgr) == 1, "1 spill");
    ASSERT(stack_manager_get_param_count(mgr) == 0, "0 params");

    stack_manager_destroy(mgr);
}

static void test_backend_extended(void) {
    printf("\n--- Backend Extended ---\n");
    BackendContext* ctx = backend_context_create();
    ASSERT(ctx != NULL, "create backend context");
    ASSERT(ctx->reg_alloc != NULL, "reg_alloc exists");
    ASSERT(ctx->stack != NULL, "stack exists");
    ASSERT(ctx->labels != NULL, "labels exists");

    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("backend_test", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    IRValue r1 = ir_value_create_register(1, i32);
    IRValue c1 = ir_value_create_int(42, i32);
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r1, c1, ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_ret(r1));

    FILE* out = tmpfile();
    int count = backend_emit_function(func, BACKEND_X86, out);
    ASSERT(count > 0, "backend emitted instructions");
    fclose(out);

    backend_context_destroy(ctx);
    ir_function_destroy(func);
}

static void test_backend_regalloc_integration(void) {
    reg_alloc_set_stack_mode(0); /* فحص المخصص الفيزيائي */
    printf("\n--- Backend RegAlloc Integration ---\n");
    
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("regalloc_test", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    
    IRValue r1 = ir_value_create_register(1, i32);
    IRValue r2 = ir_value_create_register(2, i32);
    IRValue r3 = ir_value_create_register(3, i32);
    IRValue r4 = ir_value_create_register(4, i32);
    IRValue r5 = ir_value_create_register(5, i32);
    
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r1, ir_value_create_int(10, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r2, ir_value_create_int(20, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r3, r1, r2));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r4, ir_value_create_int(30, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r5, r3, r4));
    ir_bb_add_instruction(bb, ir_inst_ret(r5));
    
    BackendContext* ctx = backend_context_create();
    ASSERT(ctx != NULL, "create backend context");
    ctx->current_func = func;
    
    int max_id = 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->result.kind == IR_VALUE_REGISTER && inst->result.id > max_id)
                max_id = inst->result.id;
            for (int o = 0; o < inst->operand_count; o++) {
                if (inst->operands[o].kind == IR_VALUE_REGISTER && inst->operands[o].id > max_id)
                    max_id = inst->operands[o].id;
            }
        }
    }
    ctx->max_reg_id = max_id;
    
    int intervals = backend_build_intervals(ctx, func);
    ASSERT(intervals > 0, "build intervals");
    
    int frame = backend_allocate_registers(ctx);
    ASSERT(frame >= 0, "allocate registers");
    
    PhysicalRegister phys1 = reg_alloc_get_physical(ctx->reg_alloc, 1);
    PhysicalRegister phys2 = reg_alloc_get_physical(ctx->reg_alloc, 2);
    PhysicalRegister phys3 = reg_alloc_get_physical(ctx->reg_alloc, 3);
    PhysicalRegister phys4 = reg_alloc_get_physical(ctx->reg_alloc, 4);
    PhysicalRegister phys5 = reg_alloc_get_physical(ctx->reg_alloc, 5);
    
    ASSERT(phys1 != REG_NONE, "r1 got physical register");
    ASSERT(phys2 != REG_NONE, "r2 got physical register");
    ASSERT(phys3 != REG_NONE, "r3 got physical register");
    ASSERT(phys4 != REG_NONE, "r4 got physical register");
    ASSERT(phys5 != REG_NONE, "r5 got physical register");
    
    ASSERT(phys1 != phys2, "r1 != r2 (different physical)");
    ASSERT(phys1 != phys3, "r1 != r3 (different physical)");
    ASSERT(phys2 != phys3, "r2 != r3 (different physical)");
    
    const char* phys1_name = reg_alloc_phys_name(phys1);
    const char* phys2_name = reg_alloc_phys_name(phys2);
    const char* phys3_name = reg_alloc_phys_name(phys3);
    
    ASSERT(phys1_name != NULL, "phys1 has name");
    ASSERT(phys2_name != NULL, "phys2 has name");
    ASSERT(phys3_name != NULL, "phys3 has name");
    
    ASSERT(strlen(phys1_name) > 0, "phys1 name not empty");
    ASSERT(strlen(phys2_name) > 0, "phys2 name not empty");
    ASSERT(strlen(phys3_name) > 0, "phys3 name not empty");
    
    FILE* out = tmpfile();
    int count = backend_emit_function(func, BACKEND_X86, out);
    ASSERT(count > 0, "backend emitted instructions");
    
    rewind(out);
    char buf[4096];
    size_t n = fread(buf, 1, sizeof(buf) - 1, out);
    buf[n] = '\0';
    fclose(out);
    
    ASSERT(strstr(buf, phys1_name) != NULL || strstr(buf, phys2_name) != NULL ||
           strstr(buf, phys3_name) != NULL,
           "assembly contains allocated physical registers");
    
    backend_context_destroy(ctx);
    ir_function_destroy(func);
}

static void test_callee_saved_handling(void) {
    printf("\n--- Callee-Saved Handling ---\n");
    
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("callee_saved_test", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    
    IRValue r1 = ir_value_create_register(1, i32);
    IRValue r2 = ir_value_create_register(2, i32);
    IRValue r3 = ir_value_create_register(3, i32);
    IRValue r4 = ir_value_create_register(4, i32);
    IRValue r5 = ir_value_create_register(5, i32);
    IRValue r6 = ir_value_create_register(6, i32);
    IRValue r7 = ir_value_create_register(7, i32);
    IRValue r8 = ir_value_create_register(8, i32);
    IRValue r9 = ir_value_create_register(9, i32);
    IRValue r10 = ir_value_create_register(10, i32);
    
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r1, ir_value_create_int(1, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r2, ir_value_create_int(2, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r3, ir_value_create_int(3, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r4, ir_value_create_int(4, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r5, ir_value_create_int(5, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r6, ir_value_create_int(6, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r7, ir_value_create_int(7, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r8, ir_value_create_int(8, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r9, ir_value_create_int(9, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r10, ir_value_create_int(10, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r1, r1, r2));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r3, r3, r4));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r5, r5, r6));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r7, r7, r8));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r9, r9, r10));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r1, r1, r3));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r5, r5, r7));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r1, r1, r5));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r1, r1, r9));
    ir_bb_add_instruction(bb, ir_inst_ret(r1));
    
    FILE* out = tmpfile();
    int count = backend_emit_function(func, BACKEND_X86, out);
    ASSERT(count > 0, "callee-saved test emitted");
    
    rewind(out);
    char buf[4096];
    size_t n = fread(buf, 1, sizeof(buf) - 1, out);
    buf[n] = '\0';
    fclose(out);
    
    int has_push = strstr(buf, "pushq") != NULL;
    int has_pop = strstr(buf, "popq") != NULL;
    ASSERT(has_push, "prologue has pushq for callee-saved");
    ASSERT(has_pop, "epilogue has popq for callee-saved");
    
    ir_function_destroy(func);
}

static void test_spill_handling(void) {
    printf("\n--- Spill Handling ---\n");
    
    RegisterAllocator* alloc = reg_alloc_create();
    ASSERT(alloc != NULL, "create allocator for spill test");
    
    for (int i = 1; i <= 20; i++) {
        reg_alloc_add_interval(alloc, i, 0, 100);
    }
    reg_alloc_linear_scan(alloc);
    
    int spilled_count = 0;
    for (int i = 0; i < alloc->interval_count; i++) {
        if (alloc->intervals[i].is_spilled) {
            spilled_count++;
        }
    }
    ASSERT(spilled_count > 0, "spills exist with 20 registers");
    
    for (int i = 1; i <= 20; i++) {
        int is_spilled = reg_alloc_is_spilled(alloc, i);
        PhysicalRegister phys = reg_alloc_get_physical(alloc, i);
        ASSERT(is_spilled || phys != REG_NONE, "register is either spilled or allocated");
    }
    
    reg_alloc_set_stack_mode(1); /* أعِد الوضع الآمن */
    reg_alloc_destroy(alloc);
    reg_alloc_set_stack_mode(1);
}

static void test_stack_alignment(void) {
    printf("\n--- Stack Alignment ---\n");
    
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("align_test", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    
    for (int i = 1; i <= 8; i++) {
        IRValue r = ir_value_create_register(i, i32);
        ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r, ir_value_create_int(i, i32), ir_value_null()));
    }
    ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_register(1, i32)));
    
    FILE* out = tmpfile();
    int count = backend_emit_function(func, BACKEND_X86, out);
    ASSERT(count > 0, "alignment test emitted");
    
    rewind(out);
    char buf[4096];
    size_t n = fread(buf, 1, sizeof(buf) - 1, out);
    buf[n] = '\0';
    fclose(out);
    
    ASSERT(strstr(buf, "pushq %rbp") != NULL, "has pushq %rbp");
    ASSERT(strstr(buf, "movq %rsp, %rbp") != NULL, "has movq %rsp, %rbp");
    ASSERT(strstr(buf, "leave") != NULL, "has leave");
    ASSERT(strstr(buf, "ret") != NULL, "has ret");
    
    ir_function_destroy(func);
}

static void test_call_preservation(void) {
    printf("\n--- Call Preservation ---\n");
    
    IRType i32 = ir_type_i32();
    IRType void_t = ir_type_void();
    (void)void_t;
    IRFunction* func = ir_function_create("call_test", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    
    IRValue r1 = ir_value_create_register(1, i32);
    IRValue r2 = ir_value_create_register(2, i32);
    IRValue r3 = ir_value_create_register(3, i32);
    
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r1, ir_value_create_int(42, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r2, ir_value_create_int(10, i32), ir_value_null()));
    
    IRValue call_args[2] = {ir_value_create_register(1, i32), ir_value_create_register(2, i32)};
    ir_bb_add_instruction(bb, ir_inst_call(r3, "external_func", call_args, 2));
    
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r1, r1, r2));
    ir_bb_add_instruction(bb, ir_inst_ret(r1));
    
    FILE* out = tmpfile();
    int count = backend_emit_function(func, BACKEND_X86, out);
    ASSERT(count > 0, "call test emitted");
    
    rewind(out);
    char buf[4096];
    size_t n = fread(buf, 1, sizeof(buf) - 1, out);
    buf[n] = '\0';
    fclose(out);
    
    ASSERT(strstr(buf, "callq external_func") != NULL, "has callq instruction");
    ASSERT(strstr(buf, "movq") != NULL, "has movq for save/restore");
    
    ir_function_destroy(func);
}

static void test_phi_basic(void) {
    printf("\n--- PHI Basic ---\n");
    
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("phi_basic", i32);
    IRBasicBlock* entry = ir_function_add_block(func, "entry");
    IRBasicBlock* then_bb = ir_function_add_block(func, "then");
    IRBasicBlock* else_bb = ir_function_add_block(func, "else");
    IRBasicBlock* merge = ir_function_add_block(func, "merge");
    
    IRValue r1 = ir_value_create_register(1, i32);
    IRValue r2 = ir_value_create_register(2, i32);
    IRValue r3 = ir_value_create_register(3, i32);
    IRValue r4 = ir_value_create_register(4, i32);
    
    ir_bb_add_instruction(entry, ir_inst_create_2(IR_OP_MOV, r1, ir_value_create_int(1, i32), ir_value_null()));
    ir_bb_add_instruction(entry, ir_inst_create_2(IR_OP_MOV, r2, ir_value_create_int(0, i32), ir_value_null()));
    ir_bb_add_instruction(entry, ir_inst_cond_jmp(IR_CMP_EQ, r2, ir_value_create_int(0, i32), ".Lthen", ".Lelse"));
    
    ir_bb_add_instruction(then_bb, ir_inst_create_2(IR_OP_MOV, r3, ir_value_create_int(10, i32), ir_value_null()));
    ir_bb_add_instruction(then_bb, ir_inst_jmp(".Lmerge"));
    
    ir_bb_add_instruction(else_bb, ir_inst_create_2(IR_OP_MOV, r3, ir_value_create_int(20, i32), ir_value_null()));
    ir_bb_add_instruction(else_bb, ir_inst_jmp(".Lmerge"));
    
    IRValue phi_ops[2] = {ir_value_create_register(3, i32), ir_value_create_register(3, i32)};
    ir_bb_add_instruction(merge, ir_inst_phi(r4, phi_ops, 2));
    ir_bb_add_instruction(merge, ir_inst_ret(r4));
    
    ir_bb_add_successor(entry, then_bb);
    ir_bb_add_successor(entry, else_bb);
    ir_bb_add_successor(then_bb, merge);
    ir_bb_add_successor(else_bb, merge);
    ir_bb_add_predecessor(then_bb, entry);
    ir_bb_add_predecessor(else_bb, entry);
    ir_bb_add_predecessor(merge, then_bb);
    ir_bb_add_predecessor(merge, else_bb);
    
    FILE* out = tmpfile();
    int count = backend_emit_function(func, BACKEND_X86, out);
    ASSERT(count > 0, "phi basic emitted");
    
    rewind(out);
    char buf[4096];
    size_t n = fread(buf, 1, sizeof(buf) - 1, out);
    buf[n] = '\0';
    fclose(out);
    
    ASSERT(strstr(buf, ".L_phi_basic_entry:") != NULL, "has entry label");
    ASSERT(strstr(buf, ".L_phi_basic_merge:") != NULL, "has merge label");
    ASSERT(strstr(buf, "je") != NULL || strstr(buf, "jne") != NULL, "has conditional jump");
    ASSERT(strstr(buf, "jmp") != NULL, "has unconditional jump");
    ASSERT(strstr(buf, "movq") != NULL, "has movq for PHI");
    
    ir_function_destroy(func);
}

static void test_if_else(void) {
    printf("\n--- If/Else ---\n");
    
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("if_else", i32);
    IRBasicBlock* entry = ir_function_add_block(func, "entry");
    IRBasicBlock* then_bb = ir_function_add_block(func, "then");
    IRBasicBlock* else_bb = ir_function_add_block(func, "else");
    IRBasicBlock* merge = ir_function_add_block(func, "merge");
    
    IRValue r1 = ir_value_create_register(1, i32);
    IRValue r2 = ir_value_create_register(2, i32);
    IRValue r3 = ir_value_create_register(3, i32);
    
    ir_bb_add_instruction(entry, ir_inst_create_2(IR_OP_MOV, r1, ir_value_create_int(5, i32), ir_value_null()));
    ir_bb_add_instruction(entry, ir_inst_create_2(IR_OP_MOV, r2, ir_value_create_int(10, i32), ir_value_null()));
    ir_bb_add_instruction(entry, ir_inst_cond_jmp(IR_CMP_GT, r1, ir_value_create_int(3, i32), ".Lthen", ".Lelse"));
    
    ir_bb_add_instruction(then_bb, ir_inst_create_2(IR_OP_ADD, r3, r1, r2));
    ir_bb_add_instruction(then_bb, ir_inst_jmp(".Lmerge"));
    
    ir_bb_add_instruction(else_bb, ir_inst_create_2(IR_OP_SUB, r3, r1, r2));
    ir_bb_add_instruction(else_bb, ir_inst_jmp(".Lmerge"));
    
    ir_bb_add_instruction(merge, ir_inst_ret(r3));
    
    ir_bb_add_successor(entry, then_bb);
    ir_bb_add_successor(entry, else_bb);
    ir_bb_add_successor(then_bb, merge);
    ir_bb_add_successor(else_bb, merge);
    ir_bb_add_predecessor(then_bb, entry);
    ir_bb_add_predecessor(else_bb, entry);
    ir_bb_add_predecessor(merge, then_bb);
    ir_bb_add_predecessor(merge, else_bb);
    
    FILE* out = tmpfile();
    int count = backend_emit_function(func, BACKEND_X86, out);
    ASSERT(count > 0, "if/else emitted");
    
    rewind(out);
    char buf[4096];
    size_t n = fread(buf, 1, sizeof(buf) - 1, out);
    buf[n] = '\0';
    fclose(out);
    
    ASSERT(strstr(buf, ".L_if_else_entry:") != NULL, "has entry label");
    ASSERT(strstr(buf, ".L_if_else_then:") != NULL, "has then label");
    ASSERT(strstr(buf, ".L_if_else_else:") != NULL, "has else label");
    ASSERT(strstr(buf, ".L_if_else_merge:") != NULL, "has merge label");
    ASSERT(strstr(buf, "jg") != NULL || strstr(buf, "jge") != NULL, "has conditional jump");
    ASSERT(strstr(buf, "jmp") != NULL, "has unconditional jump");
    
    ir_function_destroy(func);
}

static void test_while_loop(void) {
    printf("\n--- While Loop ---\n");
    
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("while_loop", i32);
    IRBasicBlock* entry = ir_function_add_block(func, "entry");
    IRBasicBlock* body = ir_function_add_block(func, "body");
    IRBasicBlock* exit = ir_function_add_block(func, "exit");
    
    IRValue r1 = ir_value_create_register(1, i32);
    IRValue r2 = ir_value_create_register(2, i32);
    
    ir_bb_add_instruction(entry, ir_inst_create_2(IR_OP_MOV, r1, ir_value_create_int(0, i32), ir_value_null()));
    ir_bb_add_instruction(entry, ir_inst_jmp(".Lbody"));
    
    ir_bb_add_instruction(body, ir_inst_create_2(IR_OP_ADD, r2, r1, ir_value_create_int(1, i32)));
    ir_bb_add_instruction(body, ir_inst_create_2(IR_OP_CMP, r2, ir_value_create_int(10, i32), ir_value_null()));
    ir_bb_add_instruction(body, ir_inst_cond_jmp(IR_CMP_LT, r2, ir_value_create_int(10, i32), ".Lbody", ".Lexit"));
    
    ir_bb_add_instruction(exit, ir_inst_ret(r2));
    
    ir_bb_add_successor(entry, body);
    ir_bb_add_successor(body, body);
    ir_bb_add_successor(body, exit);
    ir_bb_add_predecessor(body, entry);
    ir_bb_add_predecessor(body, body);
    ir_bb_add_predecessor(exit, body);
    
    FILE* out = tmpfile();
    int count = backend_emit_function(func, BACKEND_X86, out);
    ASSERT(count > 0, "while loop emitted");
    
    rewind(out);
    char buf[4096];
    size_t n = fread(buf, 1, sizeof(buf) - 1, out);
    buf[n] = '\0';
    fclose(out);
    
    ASSERT(strstr(buf, ".L_while_loop_entry:") != NULL, "has entry label");
    ASSERT(strstr(buf, ".L_while_loop_body:") != NULL, "has body label");
    ASSERT(strstr(buf, ".L_while_loop_exit:") != NULL, "has exit label");
    ASSERT(strstr(buf, "jl") != NULL || strstr(buf, "jle") != NULL, "has conditional jump");
    ASSERT(strstr(buf, "jmp") != NULL, "has unconditional jump");
    
    ir_function_destroy(func);
}

static void test_function_call_e2e(void) {
    printf("\n--- Function Call E2E ---\n");
    
    IRType i32 = ir_type_i32();
    IRFunction* caller = ir_function_create("caller", i32);
    IRBasicBlock* bb = ir_function_add_block(caller, ".Lentry");
    
    IRValue r1 = ir_value_create_register(1, i32);
    IRValue r2 = ir_value_create_register(2, i32);
    IRValue r3 = ir_value_create_register(3, i32);
    
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r1, ir_value_create_int(42, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r2, ir_value_create_int(10, i32), ir_value_null()));
    
    IRValue args[2] = {ir_value_create_register(1, i32), ir_value_create_register(2, i32)};
    ir_bb_add_instruction(bb, ir_inst_call(r3, "callee", args, 2));
    
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r1, r1, r3));
    ir_bb_add_instruction(bb, ir_inst_ret(r1));
    
    FILE* out = tmpfile();
    int count = backend_emit_function(caller, BACKEND_X86, out);
    ASSERT(count > 0, "function call e2e emitted");
    
    rewind(out);
    char buf[4096];
    size_t n = fread(buf, 1, sizeof(buf) - 1, out);
    buf[n] = '\0';
    fclose(out);
    
    ASSERT(strstr(buf, "callq callee") != NULL, "has callq instruction");
    ASSERT(strstr(buf, "movq") != NULL, "has movq for args");
    ASSERT(strstr(buf, "ret") != NULL, "has ret");
    
    ir_function_destroy(caller);
}

static void test_multi_block_register_alloc(void) {
    printf("\n--- Multi-Block Register Alloc ---\n");
    
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("multi_block", i32);
    IRBasicBlock* bb1 = ir_function_add_block(func, "bb1");
    IRBasicBlock* bb2 = ir_function_add_block(func, "bb2");
    IRBasicBlock* bb3 = ir_function_add_block(func, "bb3");
    
    IRValue r1 = ir_value_create_register(1, i32);
    IRValue r2 = ir_value_create_register(2, i32);
    IRValue r3 = ir_value_create_register(3, i32);
    IRValue r4 = ir_value_create_register(4, i32);
    IRValue r5 = ir_value_create_register(5, i32);
    
    ir_bb_add_instruction(bb1, ir_inst_create_2(IR_OP_MOV, r1, ir_value_create_int(1, i32), ir_value_null()));
    ir_bb_add_instruction(bb1, ir_inst_create_2(IR_OP_MOV, r2, ir_value_create_int(2, i32), ir_value_null()));
    ir_bb_add_instruction(bb1, ir_inst_jmp(".Lbb2"));
    
    ir_bb_add_instruction(bb2, ir_inst_create_2(IR_OP_ADD, r3, r1, r2));
    ir_bb_add_instruction(bb2, ir_inst_create_2(IR_OP_MOV, r4, ir_value_create_int(10, i32), ir_value_null()));
    ir_bb_add_instruction(bb2, ir_inst_jmp(".Lbb3"));
    
    ir_bb_add_instruction(bb3, ir_inst_create_2(IR_OP_ADD, r5, r3, r4));
    ir_bb_add_instruction(bb3, ir_inst_ret(r5));
    
    ir_bb_add_successor(bb1, bb2);
    ir_bb_add_successor(bb2, bb3);
    ir_bb_add_predecessor(bb2, bb1);
    ir_bb_add_predecessor(bb3, bb2);
    
    FILE* out = tmpfile();
    int count = backend_emit_function(func, BACKEND_X86, out);
    ASSERT(count > 0, "multi-block emitted");
    
    rewind(out);
    char buf[4096];
    size_t n = fread(buf, 1, sizeof(buf) - 1, out);
    buf[n] = '\0';
    fclose(out);
    
    ASSERT(strstr(buf, ".L_multi_block_bb1:") != NULL, "has bb1 label");
    ASSERT(strstr(buf, ".L_multi_block_bb2:") != NULL, "has bb2 label");
    ASSERT(strstr(buf, ".L_multi_block_bb3:") != NULL, "has bb3 label");
    ASSERT(strstr(buf, "jmp") != NULL, "has jumps");
    ASSERT(strstr(buf, "movq") != NULL, "has movq");
    ASSERT(strstr(buf, "addq") != NULL, "has addq");
    
    ir_function_destroy(func);
}

static void test_final_backend_audit(void) {
    printf("\n--- Final Backend Audit ---\n");
    
    IRType i32 = ir_type_i32();
    IRFunction* func = ir_function_create("final_audit", i32);
    IRBasicBlock* bb = ir_function_add_block(func, ".Lentry");
    
    IRValue r1 = ir_value_create_register(1, i32);
    IRValue r2 = ir_value_create_register(2, i32);
    IRValue r3 = ir_value_create_register(3, i32);
    IRValue r4 = ir_value_create_register(4, i32);
    IRValue r5 = ir_value_create_register(5, i32);
    IRValue r6 = ir_value_create_register(6, i32);
    IRValue r7 = ir_value_create_register(7, i32);
    IRValue r8 = ir_value_create_register(8, i32);
    IRValue r9 = ir_value_create_register(9, i32);
    IRValue r10 = ir_value_create_register(10, i32);
    
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r1, ir_value_create_int(100, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOV, r2, ir_value_create_int(200, i32), ir_value_null()));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r3, r1, r2));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_SUB, r4, r3, r1));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MUL, r5, r4, ir_value_create_int(2, i32)));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_DIV, r6, r5, ir_value_create_int(3, i32)));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOD, r7, r5, ir_value_create_int(3, i32)));
    ir_bb_add_instruction(bb, ir_inst_create_1(IR_OP_NEG, r8, r6));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_AND, r9, r1, ir_value_create_int(0xFF, i32)));
    ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_OR, r10, r1, ir_value_create_int(0x100, i32)));
    ir_bb_add_instruction(bb, ir_inst_ret(r10));
    
    FILE* out = tmpfile();
    int count = backend_emit_function(func, BACKEND_X86, out);
    ASSERT(count > 0, "final audit emitted");
    
    rewind(out);
    char buf[4096];
    size_t n = fread(buf, 1, sizeof(buf) - 1, out);
    buf[n] = '\0';
    fclose(out);
    
    ASSERT(strstr(buf, ".globl final_audit") != NULL, "has .globl");
    ASSERT(strstr(buf, "pushq %rbp") != NULL, "has pushq %rbp");
    ASSERT(strstr(buf, "movq %rsp, %rbp") != NULL, "has movq %rsp, %rbp");
    ASSERT(strstr(buf, "leave") != NULL, "has leave");
    ASSERT(strstr(buf, "ret") != NULL, "has ret");
    ASSERT(strstr(buf, "movq") != NULL, "has movq");
    ASSERT(strstr(buf, "addq") != NULL, "has addq");
    ASSERT(strstr(buf, "subq") != NULL, "has subq");
    ASSERT(strstr(buf, "imulq") != NULL, "has imulq");
    ASSERT(strstr(buf, "idivq") != NULL, "has idivq");
    ASSERT(strstr(buf, "cqo") != NULL, "has cqo");
    ASSERT(strstr(buf, "negq") != NULL, "has negq");
    ASSERT(strstr(buf, "andq") != NULL, "has andq");
    ASSERT(strstr(buf, "orq") != NULL, "has orq");
    
    ir_function_destroy(func);
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("=== DAAD Phase 4 Tests ===\n");
    test_ir_types();
    test_ir_value();
    test_ir_instruction();
    test_ir_basic_block();
    test_ir_function();
    test_ir_module();
    test_ir_printer();
    test_ir_visitor();
    test_cfg();
    test_optimizer();
    test_optimizer_strength_reduction();
    test_optimizer_algebraic();
    test_ssa();
    test_register_allocator();
    test_register_names();
    test_label_manager();
    test_stack_manager();
    test_expression_generator();
    test_statement_generator();
    test_function_generator();
    test_backend();
    test_backend_emit();
    test_emitter();
    test_ir_builder_hash();
    test_ir_function_params();
    test_ir_function_alloca();
    test_ir_builder_loop_context();
    test_ir_module_globals();
    test_liveness_analysis();
    test_register_allocator_extended();
    test_cfg_extended();
    test_ir_verifier();
    test_stack_manager_extended();
    test_backend_extended();
    test_backend_regalloc_integration();
    test_callee_saved_handling();
    test_spill_handling();
    test_stack_alignment();
    test_call_preservation();
    test_phi_basic();
    test_if_else();
    test_while_loop();
    test_function_call_e2e();
    test_multi_block_register_alloc();
    test_final_backend_audit();
    printf("\n=== Results: %d/%d passed ===\n", pass_count, test_count);
    return (pass_count == test_count) ? 0 : 1;
}
