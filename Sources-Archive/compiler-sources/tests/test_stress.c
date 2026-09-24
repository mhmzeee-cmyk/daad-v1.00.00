/**
 * @file test_stress.c
 * @brief اختبارات الضغط - Stress Tests
 */

#include "../token/token.h"
#include "../lexer/lexer.h"
#include "../ast/ast.h"
#include "../ast/ast_builder.h"
#include "../semantic/semantic_visitor.h"
#include "../ir/ir_types.h"
#include "../ir/ir_value.h"
#include "../ir/ir_instruction.h"
#include "../ir/ir_basic_block.h"
#include "../ir/ir_function.h"
#include "../ir/ir_module.h"
#include "../ir/liveness.h"
#include "../cfg/cfg.h"
#include "../ssa/ssa.h"
#include "../optimizer/optimizer_pass.h"
#include "../codegen/register_allocator.h"
#include "../backend/backend_interface.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) printf("  %-50s", name)
#define PASS() do { printf("[PASS]\n"); tests_passed++; } while(0)
#define FAIL(msg) do { printf("[FAIL] %s\n", msg); tests_failed++; } while(0)

static void test_lexer_large_input(void) {
    printf("\n--- Stress: Lexer Large Input ---\n");

    TEST("Lex 10000 characters");
    {
        size_t sz = 10001;
        char* src = (char*)malloc(sz);
        memset(src, 'a', sz - 1); src[sz - 1] = '\0';
        Lexer* lex = lexer_create(src, NULL);
        size_t count = 0; Token* tokens = lexer_tokenize(lex, &count);
        int ok = (count > 0);
        free(tokens); lexer_destroy(lex); free(src);
        if (ok) PASS(); else FAIL("failed");
    }

    TEST("Lex 10000 mixed chars");
    {
        size_t sz = 10001;
        char* src = (char*)malloc(sz);
        for (size_t i = 0; i < sz - 1; i++) src[i] = "abcd012345+-*/"[i % 14];
        src[sz - 1] = '\0';
        Lexer* lex = lexer_create(src, NULL);
        size_t count = 0; Token* tokens = lexer_tokenize(lex, &count);
        int ok = (count > 0);
        free(tokens); lexer_destroy(lex); free(src);
        if (ok) PASS(); else FAIL("failed");
    }

    TEST("Lex 1000 newlines");
    {
        size_t sz = 3001;
        char* src = (char*)malloc(sz);
        int pos = 0;
        for (int i = 0; i < 1000; i++) { src[pos++] = 'a'; src[pos++] = '\n'; src[pos++] = ' '; }
        src[pos] = '\0';
        Lexer* lex = lexer_create(src, NULL);
        size_t count = 0; Token* tokens = lexer_tokenize(lex, &count);
        int ok = (count > 0);
        free(tokens); lexer_destroy(lex); free(src);
        if (ok) PASS(); else FAIL("failed");
    }

    TEST("Lex many string literals");
    {
        size_t sz = 8001;
        char* src = (char*)malloc(sz);
        int pos = 0;
        for (int i = 0; i < 100 && pos < (int)sz - 20; i++) {
            src[pos++] = '"';
            for (int j = 0; j < 50 && pos < (int)sz - 5; j++) src[pos++] = 'x';
            src[pos++] = '"'; src[pos++] = ' ';
        }
        src[pos] = '\0';
        Lexer* lex = lexer_create(src, NULL);
        size_t count = 0; Token* tokens = lexer_tokenize(lex, &count);
        int ok = (count > 0);
        free(tokens); lexer_destroy(lex); free(src);
        if (ok) PASS(); else FAIL("failed");
    }

    TEST("Lex 50 Arabic keywords in sequence");
    {
        size_t sz = 50 * 7 + 1;
        char* src = (char*)malloc(sz); src[0] = '\0';
        for (int i = 0; i < 50; i++) strcat(src, "رقم ");
        Lexer* lex = lexer_create(src, NULL);
        size_t count = 0; Token* tokens = lexer_tokenize(lex, &count);
        int ok = (count >= 40);
        free(tokens); lexer_destroy(lex); free(src);
        if (ok) PASS(); else FAIL("too few tokens");
    }
}

static void test_lexer_edge_cases(void) {
    printf("\n--- Stress: Lexer Edge Cases ---\n");

    TEST("10 sequential comments");
    {
        size_t sz = 10 * 6 + 3;
        char* src = (char*)malloc(sz); int pos = 0;
        for (int i = 0; i < 10; i++) {
            src[pos++] = '/'; src[pos++] = '*';
            src[pos++] = 'c'; src[pos++] = 'm';
            src[pos++] = '*'; src[pos++] = '/';
        }
        src[pos] = '\0';
        Lexer* lex = lexer_create(src, NULL);
        size_t count = 0; Token* tokens = lexer_tokenize(lex, &count);
        free(tokens); lexer_destroy(lex); free(src); PASS();
    }

    TEST("Only whitespace (1000 spaces)");
    {
        size_t sz = 1001;
        char* src = (char*)malloc(sz);
        memset(src, ' ', sz - 1); src[sz - 1] = '\0';
        Lexer* lex = lexer_create(src, NULL);
        size_t count = 0; Token* tokens = lexer_tokenize(lex, &count);
        int ok = (count <= 1);
        free(tokens); lexer_destroy(lex); free(src);
        if (ok) PASS(); else FAIL("too many tokens");
    }

    TEST("Long identifier (100 chars)");
    {
        size_t sz = 102;
        char* src = (char*)malloc(sz);
        src[0] = 'x'; memset(src + 1, 'a', sz - 2); src[sz - 1] = '\0';
        Lexer* lex = lexer_create(src, NULL);
        size_t count = 0; Token* tokens = lexer_tokenize(lex, &count);
        int ok = (count >= 1);
        free(tokens); lexer_destroy(lex); free(src);
        if (ok) PASS(); else FAIL("long id failed");
    }

    TEST("Number overflow (30 digits)");
    {
        Lexer* lex = lexer_create("999999999999999999999999999999", NULL);
        size_t count = 0; Token* tokens = lexer_tokenize(lex, &count);
        free(tokens); lexer_destroy(lex); PASS();
    }

    TEST("Operators sequence (+ - * /)");
    {
        Lexer* lex = lexer_create("+ - * / + - * /", NULL);
        size_t count = 0; Token* tokens = lexer_tokenize(lex, &count);
        int ok = (count == 9);
        free(tokens); lexer_destroy(lex);
        if (ok) PASS(); else FAIL("wrong count");
    }
}

static void test_ast_deep_nesting(void) {
    printf("\n--- Stress: AST Deep Nesting ---\n");

    TEST("100-level deep binary tree");
    {
        ASTNode* root = ast_build_literal_int(0);
        for (int i = 1; i <= 100; i++) {
            ASTNode* lit = ast_build_literal_int(i);
            root = ast_build_binary(OP_ADD, lit, root);
        }
        int ok = (root != NULL && root->type == NODE_BINARY_EXPRESSION);
        ast_node_destroy(root);
        if (ok) PASS(); else FAIL("deep binary tree failed");
    }

    TEST("100-level deep unary chain");
    {
        ASTNode* inner = ast_build_literal_int(42);
        for (int i = 0; i < 100; i++) inner = ast_build_unary(UNARY_NEGATE, inner);
        int ok = (inner != NULL);
        ast_node_destroy(inner);
        if (ok) PASS(); else FAIL("deep unary chain failed");
    }

    TEST("20 if-else chains");
    {
        ASTNode* body = ast_build_literal_int(0);
        for (int i = 0; i < 20; i++) {
            ASTNode* cond = ast_build_binary(OP_NOT_EQUAL, ast_build_literal_int(i), ast_build_literal_int(0));
            body = ast_build_if(cond, body, NULL);
        }
        int ok = (body != NULL);
        ast_node_destroy(body);
        if (ok) PASS(); else FAIL("if-else chain failed");
    }

    TEST("10 nested while loops");
    {
        ASTNode* body = ast_build_break();
        for (int i = 0; i < 10; i++) {
            ASTNode* cond = ast_build_binary(OP_NOT_EQUAL, ast_build_literal_int(i), ast_build_literal_int(0));
            body = ast_build_while(cond, body);
        }
        int ok = (body != NULL);
        ast_node_destroy(body);
        if (ok) PASS(); else FAIL("nested while failed");
    }

    TEST("10-level nested function calls");
    {
        ASTNode* inner = ast_build_literal_int(0);
        for (int i = 0; i < 10; i++) {
            ASTNode* callee = ast_build_identifier("fn");
            ASTNodeList* a = (ASTNodeList*)malloc(sizeof(ASTNodeList));
            ast_node_list_init(a);
            ast_node_list_add(a, inner);
            inner = ast_build_call(callee, a);
        }
        int ok = (inner != NULL);
        ast_node_destroy(inner);
        if (ok) PASS(); else FAIL("nested calls failed");
    }
}

static void test_ir_large_cfg(void) {
    printf("\n--- Stress: IR Large CFG ---\n");

    TEST("200 basic blocks linear chain");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("chain", i64);
        for (int i = 0; i < 200; i++) {
            char label[32]; snprintf(label, sizeof(label), "bb_%d", i);
            IRBasicBlock* bb = ir_function_add_block(func, label);
            IRValue res = ir_value_create_register(i + 1, i64);
            IRValue op = ir_value_create_int(i, i64);
            ir_bb_add_instruction(bb, ir_inst_create_1(IR_OP_MOV, res, op));
        }
        int ok = (func->block_count == 200);
        ir_function_destroy(func);
        if (ok) PASS(); else FAIL("200 blocks failed");
    }

    TEST("200 instructions per block");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("heavy", i64);
        IRBasicBlock* bb = ir_function_add_block(func, "entry");
        for (int i = 0; i < 200; i++) {
            IRValue res = ir_value_create_register(i + 1, i64);
            IRValue op = ir_value_create_int(i, i64);
            ir_bb_add_instruction(bb, ir_inst_create_1(IR_OP_MOV, res, op));
        }
        int ok = (bb->instruction_count == 200);
        ir_function_destroy(func);
        if (ok) PASS(); else FAIL("200 insts per block failed");
    }

    TEST("Diamond CFG (1 entry -> 100 branches -> 1 merge)");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("diamond", i64);
        IRBasicBlock* entry = ir_function_add_block(func, "entry");
        ir_bb_add_instruction(entry, ir_inst_create_1(IR_OP_MOV, ir_value_create_register(0, i64), ir_value_create_int(0, i64)));

        for (int i = 0; i < 100; i++) {
            char label[32]; snprintf(label, sizeof(label), "branch_%d", i);
            IRBasicBlock* bb = ir_function_add_block(func, label);
            ir_bb_add_instruction(bb, ir_inst_create_1(IR_OP_MOV, ir_value_create_register(i + 1, i64), ir_value_create_int(i, i64)));
        }

        IRBasicBlock* merge = ir_function_add_block(func, "merge");
        ir_bb_add_instruction(merge, ir_inst_ret(ir_value_create_int(0, i64)));
        int ok = (func->block_count == 102);
        ir_function_destroy(func);
        if (ok) PASS(); else FAIL("diamond CFG failed");
    }

    TEST("Fan-in: 200 predecessors");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("fanin", i64);
        for (int i = 0; i < 200; i++) {
            char label[32]; snprintf(label, sizeof(label), "pred_%d", i);
            IRBasicBlock* bb = ir_function_add_block(func, label);
            ir_bb_add_instruction(bb, ir_inst_create_1(IR_OP_MOV, ir_value_create_register(i + 1, i64), ir_value_create_int(i, i64)));
        }
        IRBasicBlock* sink = ir_function_add_block(func, "sink");
        ir_bb_add_instruction(sink, ir_inst_ret(ir_value_create_int(0, i64)));
        int ok = (func->block_count == 201);
        ir_function_destroy(func);
        if (ok) PASS(); else FAIL("fan-in failed");
    }
}

static void test_ir_large_instructions(void) {
    printf("\n--- Stress: IR Instruction Limits ---\n");

    TEST("IR call with 7 arguments (max)");
    {
        IRType i64 = ir_type_i64();
        IRValue result = ir_value_create_register(999, i64);
        IRValue args[7];
        for (int i = 0; i < 7; i++) args[i] = ir_value_create_int(i, i64);
        IRInstruction inst = ir_inst_call(result, "test_func", args, 7);
        int ok = (inst.operand_count == 8);
        if (ok) PASS(); else FAIL("call 7 args failed");
    }

    TEST("IR phi with 8 incomings (max)");
    {
        IRType i64 = ir_type_i64();
        IRValue result = ir_value_create_register(1, i64);
        IRValue incomings[16];
        for (int i = 0; i < 8; i++) {
            incomings[i * 2] = ir_value_create_label("src");
            incomings[i * 2 + 1] = ir_value_create_int(i, i64);
        }
        IRInstruction inst = ir_inst_phi(result, incomings, 16);
        int ok = (inst.operand_count == 8);
        if (ok) PASS(); else FAIL("phi 8 incomings failed");
    }

    TEST("IR cond_jmp with all compare ops");
    {
        IRType i64 = ir_type_i64();
        IRCompareOp ops[] = {IR_CMP_EQ, IR_CMP_NE, IR_CMP_GT, IR_CMP_LT, IR_CMP_GE, IR_CMP_LE};
        for (int i = 0; i < 6; i++) {
            IRInstruction inst = ir_inst_cond_jmp(ops[i],
                ir_value_create_register(1, i64), ir_value_create_int(2, i64),
                "true_label", "false_label");
            if (inst.operand_count < 4) { FAIL("cond_jmp operand count"); return; }
        }
        PASS();
    }
}

static void test_optimizer_stress(void) {
    printf("\n--- Stress: Optimizer Stress ---\n");

    TEST("Run ALL passes on 500 instructions");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("opt_stress", i64);
        IRBasicBlock* bb = ir_function_add_block(func, "entry");
        for (int i = 0; i < 500; i++) {
            IRValue res = ir_value_create_register(i + 1, i64);
            IRValue op1 = ir_value_create_int(i, i64);
            IRValue op2 = ir_value_create_int(i + 1, i64);
            ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, res, op1, op2));
        }
        ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));

        Optimizer* opt = optimizer_create();
        optimizer_add_pass(opt, OPT_PASS_CONSTANT_FOLDING);
        optimizer_add_pass(opt, OPT_PASS_CONSTANT_PROPAGATION);
        optimizer_add_pass(opt, OPT_PASS_COPY_PROPAGATION);
        optimizer_add_pass(opt, OPT_PASS_DEAD_CODE_ELIMINATION);
        optimizer_add_pass(opt, OPT_PASS_DEAD_STORE_ELIMINATION);
        optimizer_add_pass(opt, OPT_PASS_ALGEBRAIC_SIMPLIFICATION);
        optimizer_add_pass(opt, OPT_PASS_STRENGTH_REDUCTION);
        optimizer_add_pass(opt, OPT_PASS_JUMP_OPTIMIZATION);
        optimizer_add_pass(opt, OPT_PASS_REMOVE_EMPTY_BLOCKS);
        optimizer_add_pass(opt, OPT_PASS_MERGE_BLOCKS);
        optimizer_add_pass(opt, OPT_PASS_PEEPHOLE);
        optimizer_run_all(opt, func);
        optimizer_destroy(opt);
        ir_function_destroy(func);
        PASS();
    }

    TEST("Run optimizer 100 times on same function");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("opt_repeat", i64);
        IRBasicBlock* bb = ir_function_add_block(func, "entry");
        IRValue res = ir_value_create_register(1, i64);
        ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, res, ir_value_create_int(5, i64), ir_value_create_int(3, i64)));
        ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));

        for (int i = 0; i < 100; i++) {
            Optimizer* opt = optimizer_create();
            optimizer_add_pass(opt, OPT_PASS_CONSTANT_FOLDING);
            optimizer_add_pass(opt, OPT_PASS_PEEPHOLE);
            optimizer_run_all(opt, func);
            optimizer_destroy(opt);
        }
        ir_function_destroy(func);
        PASS();
    }
}

static void test_type_system_stress(void) {
    printf("\n--- Stress: Type System ---\n");

    TEST("Create 10000 types");
    {
        int ok = 1;
        for (int i = 0; i < 10000; i++) {
            DaadType* t = daad_type_create(TYPE_INT, "stress", 8);
            if (!t) { ok = 0; break; }
            free(t);
        }
        if (ok) PASS(); else FAIL("type creation stress failed");
    }

    TEST("Create 1000 pointer types");
    {
        DaadType* base = daad_type_create(TYPE_INT, "int", 8);
        int ok = 1; DaadType* last = base;
        for (int i = 0; i < 1000; i++) {
            DaadType* p = daad_type_create_pointer(last);
            if (!p) { ok = 0; break; }
            last = p;
        }
        while (last != base) { DaadType* tmp = last->as.pointer.pointee; free(last); last = tmp; }
        free(base);
        if (ok) PASS(); else FAIL("pointer chain stress failed");
    }

    TEST("Create 100-element struct type");
    {
        char** names = (char**)calloc(100, sizeof(char*));
        DaadType** types = (DaadType**)calloc(100, sizeof(DaadType*));
        DaadType* int_t = daad_type_create(TYPE_INT, "int", 8);
        for (int i = 0; i < 100; i++) {
            names[i] = (char*)malloc(32);
            snprintf(names[i], 32, "field_%d", i);
            types[i] = int_t;
        }
        DaadType* s = daad_type_create_struct(names, types, 100);
        int ok = (s != NULL && s->as.struct_type.member_count == 100);
        for (int i = 0; i < 100; i++) free(names[i]);
        free(names); free(types); free(s); free(int_t);
        if (ok) PASS(); else FAIL("large struct failed");
    }

    TEST("10000 type compatibility checks");
    {
        DaadType* a = daad_type_create(TYPE_INT, "int", 8);
        DaadType* b = daad_type_create(TYPE_FLOAT, "float", 8);
        int count = 0;
        for (int i = 0; i < 10000; i++) {
            count += daad_type_is_compatible(a, a);
            count += daad_type_is_compatible(b, b);
        }
        free(a); free(b);
        if (count == 20000) PASS(); else FAIL("compatibility check stress");
    }
}

static void test_register_allocator_stress(void) {
    printf("\n--- Stress: Register Allocator ---\n");

    TEST("500 intervals with heavy spilling");
    {
        RegisterAllocator* alloc = reg_alloc_create();
        for (int i = 0; i < 500; i++) reg_alloc_add_interval(alloc, i + 1, i, i + 100);
        reg_alloc_linear_scan(alloc);
        reg_alloc_destroy(alloc);
        PASS();
    }

    TEST("500 intervals");
    {
        RegisterAllocator* alloc = reg_alloc_create();
        for (int i = 0; i < 500; i++) reg_alloc_add_interval(alloc, i + 1, i * 3, i * 3 + 5);
        reg_alloc_linear_scan(alloc);
        reg_alloc_destroy(alloc);
        PASS();
    }

    TEST("Register names for all registers");
    {
        PhysicalRegister all[] = {REG_RAX, REG_RBX, REG_RCX, REG_RDX, REG_RSI, REG_RDI,
                                   REG_RBP, REG_RSP, REG_R8, REG_R9, REG_R10, REG_R11,
                                   REG_R12, REG_R13, REG_R14, REG_R15,
                                   REG_XMM0, REG_XMM1, REG_XMM2, REG_XMM3,
                                   REG_XMM4, REG_XMM5, REG_XMM6, REG_XMM7};
        int ok = 1;
        for (int i = 0; i < 24; i++) {
            const char* name = reg_alloc_phys_name(all[i]);
            if (!name || name[0] == '\0') { ok = 0; break; }
        }
        if (ok) PASS(); else FAIL("reg names failed");
    }
}

static void test_backend_stress(void) {
    printf("\n--- Stress: Backend Emission ---\n");

    TEST("Emit 500-instruction function");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("big_func", i64);
        IRBasicBlock* bb = ir_function_add_block(func, "entry");
        for (int i = 0; i < 500; i++) {
            ir_bb_add_instruction(bb, ir_inst_create_1(IR_OP_MOV,
                ir_value_create_register(i + 1, i64), ir_value_create_int(i, i64)));
        }
        ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));
        FILE* devnull = fopen("NUL", "w");
        if (devnull) { backend_emit_function(func, BACKEND_X86, devnull); fclose(devnull); }
        ir_function_destroy(func);
        PASS();
    }

    TEST("Emit 100 basic blocks");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("multi_bb", i64);
        for (int i = 0; i < 100; i++) {
            char label[32]; snprintf(label, sizeof(label), "bb_%d", i);
            IRBasicBlock* bb = ir_function_add_block(func, label);
            ir_bb_add_instruction(bb, ir_inst_create_1(IR_OP_MOV,
                ir_value_create_register(i + 1, i64), ir_value_create_int(i, i64)));
        }
        IRBasicBlock* last = func->blocks[func->block_count - 1];
        ir_bb_add_instruction(last, ir_inst_ret(ir_value_create_int(0, i64)));
        FILE* devnull = fopen("NUL", "w");
        if (devnull) { backend_emit_function(func, BACKEND_X86, devnull); fclose(devnull); }
        ir_function_destroy(func);
        PASS();
    }

    TEST("Emit function with all instruction types");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("all_ops", i64);
        IRBasicBlock* bb = ir_function_add_block(func, "entry");

        IRValue r1 = ir_value_create_register(1, i64);
        IRValue c1 = ir_value_create_int(42, i64);
        ir_bb_add_instruction(bb, ir_inst_create_1(IR_OP_MOV, r1, c1));

        IRValue r2 = ir_value_create_register(2, i64);
        ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_ADD, r2, r1, c1));
        ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_SUB, r2, r2, c1));
        ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MUL, r2, r2, c1));
        ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_DIV, r2, r2, c1));
        ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_MOD, r2, r2, c1));
        ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_AND, r2, r2, c1));
        ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_OR, r2, r2, c1));
        ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_XOR, r2, r2, c1));
        ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_SHL, r2, r2, c1));
        ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_SHR, r2, r2, c1));
        ir_bb_add_instruction(bb, ir_inst_create_1(IR_OP_NEG, r2, r2));
        ir_bb_add_instruction(bb, ir_inst_create_1(IR_OP_NOT, r2, r2));
        ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_CMP, r2, r2, c1));
        ir_bb_add_instruction(bb, ir_inst_jmp("label"));
        ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));

        FILE* devnull = fopen("NUL", "w");
        if (devnull) { backend_emit_function(func, BACKEND_X86, devnull); fclose(devnull); }
        ir_function_destroy(func);
        PASS();
    }
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("=== DAAD Stress Tests ===\n");

    test_lexer_large_input();
    test_lexer_edge_cases();
    test_ast_deep_nesting();
    test_ir_large_cfg();
    test_ir_large_instructions();
    test_optimizer_stress();
    test_type_system_stress();
    test_register_allocator_stress();
    test_backend_stress();

    printf("\n=== Results: %d passed, %d failed ===\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
