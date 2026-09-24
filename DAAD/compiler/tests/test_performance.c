/**
 * @file test_performance.c
 * @brief اختبارات الأداء - Performance Benchmarks
 */

#include "../token/token.h"
#include "../lexer/lexer.h"
#include "../ast/ast.h"
#include "../ast/ast_builder.h"
#include "../ast/ast_printer.h"
#include "../parser/parser.h"
#include "../semantic/semantic_visitor.h"
#include "../ir/ir_types.h"
#include "../ir/ir_value.h"
#include "../ir/ir_instruction.h"
#include "../ir/ir_basic_block.h"
#include "../ir/ir_function.h"
#include "../ir/ir_module.h"
#include "../ir/ir_printer.h"
#include "../ir/liveness.h"
#include "../cfg/cfg.h"
#include "../ssa/ssa.h"
#include "../optimizer/optimizer_pass.h"
#include "../codegen/register_allocator.h"
#include "../backend/backend_interface.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) printf("  %-50s", name)
#define PASS() do { printf("[PASS]\n"); tests_passed++; } while(0)
#define FAIL(msg) do { printf("[FAIL] %s\n", msg); tests_failed++; } while(0)

static clock_t _bench_start, _bench_end;

static void bench_lexer_tokenization(void) {
    printf("\n--- Performance: Lexer Tokenization ---\n");

    TEST("Lex 100 keywords (10000 iters)");
    {
        const char* src = "رقم متغير ثابت دالة إذا وإلا طالما لكل أكمل توقف ارجع اقرأ اطبع";
        _bench_start = clock();
        for (int i = 0; i < 10000; i++) {
            Lexer* lex = lexer_create(src, NULL);
            size_t count = 0;
            Token* tokens = lexer_tokenize(lex, &count);
            free(tokens);
            lexer_destroy(lex);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 10000 * 1000000.0;
        if (us < 500000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.0f us", us); FAIL(b); }
    }

    TEST("Lex 500 identifiers (5000 iters)");
    {
        char src[4096] = "";
        for (int i = 0; i < 100; i++) {
            char buf[32]; snprintf(buf, sizeof(buf), "variable%d ", i);
            strcat(src, buf);
        }
        _bench_start = clock();
        for (int i = 0; i < 5000; i++) {
            Lexer* lex = lexer_create(src, NULL);
            size_t count = 0;
            Token* tokens = lexer_tokenize(lex, &count);
            free(tokens);
            lexer_destroy(lex);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 5000 * 1000000.0;
        if (us < 50000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.0f us/iter", us); FAIL(b); }
    }

    TEST("Lex 200 operators (5000 iters)");
    {
        const char* src = "+ - * / = == != < > <= >= && || & | ^ ~ << >> += -= *= /= %= !";
        _bench_start = clock();
        for (int i = 0; i < 5000; i++) {
            Lexer* lex = lexer_create(src, NULL);
            size_t count = 0;
            Token* tokens = lexer_tokenize(lex, &count);
            free(tokens);
            lexer_destroy(lex);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 5000 * 1000000.0;
        if (us < 50000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.0f us/iter", us); FAIL(b); }
    }

    TEST("Lex 100 numbers (10000 iters)");
    {
        const char* src = "0 1 42 255 65535 2147483647 9223372036854775807 314 100 999";
        _bench_start = clock();
        for (int i = 0; i < 10000; i++) {
            Lexer* lex = lexer_create(src, NULL);
            size_t count = 0;
            Token* tokens = lexer_tokenize(lex, &count);
            free(tokens);
            lexer_destroy(lex);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 10000 * 1000000.0;
        if (us < 50000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.0f us/iter", us); FAIL(b); }
    }

    TEST("Lex empty input (50000 iters)");
    {
        _bench_start = clock();
        for (int i = 0; i < 50000; i++) {
            Lexer* lex = lexer_create("", NULL);
            size_t count = 0;
            Token* tokens = lexer_tokenize(lex, &count);
            free(tokens);
            lexer_destroy(lex);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 50000 * 1000000.0;
        if (us < 10000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.0f us/iter", us); FAIL(b); }
    }
}

static void bench_ast_creation(void) {
    printf("\n--- Performance: AST Creation ---\n");

    TEST("Create 100000 literal ints (100000 iters)");
    {
        _bench_start = clock();
        for (int i = 0; i < 100000; i++) {
            ASTNode* n = ast_build_literal_int(i);
            ast_node_destroy(n);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 100000 * 1000000.0;
        if (us < 1000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.1f us/iter", us); FAIL(b); }
    }

    TEST("Create 50000 binary expressions (50000 iters)");
    {
        _bench_start = clock();
        for (int i = 0; i < 50000; i++) {
            ASTNode* l = ast_build_literal_int(i);
            ASTNode* r = ast_build_literal_int(i + 1);
            ASTNode* bin = ast_build_binary(OP_ADD, l, r);
            ast_node_destroy(bin);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 50000 * 1000000.0;
        if (us < 2000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.1f us/iter", us); FAIL(b); }
    }

    TEST("Create 50000 identifiers (50000 iters)");
    {
        _bench_start = clock();
        for (int i = 0; i < 50000; i++) {
            ASTNode* n = ast_build_identifier("var");
            ast_node_destroy(n);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 50000 * 1000000.0;
        if (us < 1000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.1f us/iter", us); FAIL(b); }
    }
}

static void bench_ir_creation(void) {
    printf("\n--- Performance: IR Creation ---\n");

    TEST("Create 100000 IR instructions (50000 iters)");
    {
        IRType i64 = ir_type_i64();
        _bench_start = clock();
        for (int i = 0; i < 50000; i++) {
            IRValue res = ir_value_create_register(i + 1, i64);
            IRValue op1 = ir_value_create_int(i, i64);
            IRValue op2 = ir_value_create_int(i + 1, i64);
            IRInstruction inst = ir_inst_create_2(IR_OP_ADD, res, op1, op2);
            (void)inst;
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 50000 * 1000000.0;
        if (us < 2000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.1f us/iter", us); FAIL(b); }
    }

    TEST("IR value name 100000 (50000 iters)");
    {
        IRType i64 = ir_type_i64();
        IRValue reg = ir_value_create_register(42, i64);
        IRValue intv = ir_value_create_int(12345, i64);
        char buf[64];
        _bench_start = clock();
        for (int i = 0; i < 50000; i++) {
            ir_value_name_buf(reg, buf, sizeof(buf));
            ir_value_name_buf(intv, buf, sizeof(buf));
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 50000 * 1000000.0;
        if (us < 1000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.1f us/iter", us); FAIL(b); }
    }

    TEST("Create IRFunction 100 blocks (500 iters)");
    {
        IRType i64 = ir_type_i64();
        _bench_start = clock();
        for (int i = 0; i < 500; i++) {
            IRFunction* func = ir_function_create("bench_func", i64);
            for (int j = 0; j < 100; j++) {
                char label[32]; snprintf(label, sizeof(label), "bb_%d", j);
                IRBasicBlock* bb = ir_function_add_block(func, label);
                IRValue res = ir_value_create_register(j + 1, i64);
                IRValue op = ir_value_create_int(j, i64);
                IRInstruction inst = ir_inst_create_1(IR_OP_MOV, res, op);
                ir_bb_add_instruction(bb, inst);
            }
            ir_function_destroy(func);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 500 * 1000000.0;
        if (us < 20000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.0f us/iter", us); FAIL(b); }
    }
}

static void bench_semantic(void) {
    printf("\n--- Performance: Semantic Analysis ---\n");

    TEST("Semantic context create/destroy (10000 iters)");
    {
        _bench_start = clock();
        for (int i = 0; i < 10000; i++) {
            SemanticContext* ctx = semantic_context_create();
            semantic_context_destroy(ctx);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 10000 * 1000000.0;
        if (us < 10000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.0f us/iter", us); FAIL(b); }
    }

    TEST("Type creation (50000 iters)");
    {
        _bench_start = clock();
        for (int i = 0; i < 50000; i++) {
            DaadType* t = daad_type_create(TYPE_INT, "رقم", 8);
            free(t);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 50000 * 1000000.0;
        if (us < 2000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.1f us/iter", us); FAIL(b); }
    }

    TEST("Type equality check (50000 iters)");
    {
        DaadType* a = daad_type_create(TYPE_INT, "رقم", 8);
        DaadType* b = daad_type_create(TYPE_INT, "رقم", 8);
        _bench_start = clock();
        for (int i = 0; i < 50000; i++) {
            daad_type_equals(a, b);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 50000 * 1000000.0;
        if (us < 5000.0) PASS(); else { char b2[64]; snprintf(b2, 64, "%.1f us/iter", us); FAIL(b2); }
        free(a);
        free(b);
    }
}

static void bench_optimizer(void) {
    printf("\n--- Performance: Optimizer ---\n");

    TEST("Create optimizer (10000 iters)");
    {
        _bench_start = clock();
        for (int i = 0; i < 10000; i++) {
            Optimizer* opt = optimizer_create();
            optimizer_destroy(opt);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 10000 * 1000000.0;
        if (us < 5000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.0f us/iter", us); FAIL(b); }
    }

    TEST("Constant fold 100 ops (1000 iters)");
    {
        IRType i64 = ir_type_i64();
        _bench_start = clock();
        for (int iter = 0; iter < 1000; iter++) {
            IRFunction* func = ir_function_create("fold_test", i64);
            IRBasicBlock* bb = ir_function_add_block(func, "entry");
            for (int j = 0; j < 100; j++) {
                IRValue res = ir_value_create_register(j + 1, i64);
                IRValue op1 = ir_value_create_int(j, i64);
                IRValue op2 = ir_value_create_int(j + 1, i64);
                IRInstruction inst = ir_inst_create_2(IR_OP_ADD, res, op1, op2);
                ir_bb_add_instruction(bb, inst);
            }
            Optimizer* opt = optimizer_create();
            optimizer_add_pass(opt, OPT_PASS_CONSTANT_FOLDING);
            optimizer_run_all(opt, func);
            optimizer_destroy(opt);
            ir_function_destroy(func);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 1000 * 1000000.0;
        if (us < 50000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.0f us/iter", us); FAIL(b); }
    }
}

static void bench_register_allocator(void) {
    printf("\n--- Performance: Register Allocator ---\n");

    TEST("Linear scan 50 intervals (1000 iters)");
    {
        _bench_start = clock();
        for (int i = 0; i < 1000; i++) {
            RegisterAllocator* alloc = reg_alloc_create();
            for (int j = 0; j < 50; j++) {
                reg_alloc_add_interval(alloc, j + 1, j * 2, j * 2 + 10);
            }
            reg_alloc_linear_scan(alloc);
            reg_alloc_destroy(alloc);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 1000 * 1000000.0;
        if (us < 50000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.0f us/iter", us); FAIL(b); }
    }
}

static void bench_backend(void) {
    printf("\n--- Performance: Backend Emission ---\n");

    TEST("Emit 100 instructions (500 iters)");
    {
        IRType i64 = ir_type_i64();
        _bench_start = clock();
        for (int iter = 0; iter < 500; iter++) {
            IRFunction* func = ir_function_create("emit_test", i64);
            IRBasicBlock* bb = ir_function_add_block(func, "entry");
            for (int j = 0; j < 100; j++) {
                IRValue res = ir_value_create_register(j + 1, i64);
                IRValue op1 = ir_value_create_register(j + 1, i64);
                IRValue op2 = ir_value_create_int(j, i64);
                IRInstruction inst = ir_inst_create_2(IR_OP_ADD, res, op1, op2);
                ir_bb_add_instruction(bb, inst);
            }
            IRValue ret_op = ir_value_create_int(0, i64);
            IRInstruction ret = ir_inst_ret(ret_op);
            ir_bb_add_instruction(bb, ret);

            FILE* devnull = fopen("NUL", "w");
            if (devnull) {
                backend_emit_function(func, BACKEND_X86, devnull);
                fclose(devnull);
            }
            ir_function_destroy(func);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 500 * 1000000.0;
        if (us < 50000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.0f us/iter", us); FAIL(b); }
    }

    TEST("Backend context create/destroy (5000 iters)");
    {
        _bench_start = clock();
        for (int i = 0; i < 5000; i++) {
            BackendContext* ctx = backend_context_create();
            backend_context_destroy(ctx);
        }
        _bench_end = clock();
        double us = (double)(_bench_end - _bench_start) / CLOCKS_PER_SEC / 5000 * 1000000.0;
        if (us < 5000.0) PASS(); else { char b[64]; snprintf(b, 64, "%.0f us/iter", us); FAIL(b); }
    }
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("=== DAAD Performance Benchmarks ===\n");

    bench_lexer_tokenization();
    bench_ast_creation();
    bench_ir_creation();
    bench_semantic();
    bench_optimizer();
    bench_register_allocator();
    bench_backend();

    printf("\n=== Results: %d passed, %d failed ===\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
