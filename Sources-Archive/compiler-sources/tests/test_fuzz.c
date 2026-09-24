/**
 * @file test_fuzz.c
 * @brief اختبارات استكشاف الأخطاء - Fuzzing & Edge Case Discovery
 */

#include "../token/token.h"
#include "../lexer/lexer.h"
#include "../ast/ast.h"
#include "../ast/ast_builder.h"
#include "../ast/ast_printer.h"
#include "../semantic/semantic_visitor.h"
#include "../semantic/type.h"
#include "../semantic/type_registry.h"
#include "../semantic/type_checker.h"
#include "../semantic/scope_stack.h"
#include "../semantic/symbol_table.h"
#include "../ir/ir_types.h"
#include "../ir/ir_value.h"
#include "../ir/ir_instruction.h"
#include "../ir/ir_basic_block.h"
#include "../ir/ir_function.h"
#include "../ir/ir_module.h"
#include "../ir/ir_printer.h"
#include "../ir/ir_validate.h"
#include "../ir/liveness.h"
#include "../cfg/cfg.h"
#include "../ssa/ssa.h"
#include "../optimizer/optimizer_pass.h"
#include "../codegen/register_allocator.h"
#include "../codegen/label_manager.h"
#include "../codegen/stack_manager.h"
#include "../backend/backend_interface.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) printf("  %-55s", name)
#define PASS() do { printf("[PASS]\n"); tests_passed++; } while(0)
#define FAIL(msg) do { printf("[FAIL] %s\n", msg); tests_failed++; } while(0)

static void test_null_safety(void) {
    printf("\n--- Fuzz: NULL Safety ---\n");

    TEST("lexer_create(NULL, NULL)");
    { Lexer* l = lexer_create(NULL, NULL); if (!l) PASS(); else { FAIL("should be NULL"); lexer_destroy(l); } }

    TEST("lexer_create(empty, NULL)");
    { Lexer* l = lexer_create("", NULL); size_t c = 0; Token* t = lexer_tokenize(l, &c); free(t); lexer_destroy(l); PASS(); }

    TEST("lexer_next_token on empty");
    { Lexer* l = lexer_create("", NULL); Token t = lexer_next_token(l); (void)t; lexer_destroy(l); PASS(); }

    TEST("ast_node_destroy(NULL)");
    { ast_node_destroy(NULL); PASS(); }

    TEST("ast_build_literal_int with LLONG_MIN");
    { ASTNode* n = ast_build_literal_int(-9223372036854775807LL - 1); int ok = (n != NULL); ast_node_destroy(n); if (ok) PASS(); else FAIL("LLONG_MIN failed"); }

    TEST("ast_build_literal_int with 0");
    { ASTNode* n = ast_build_literal_int(0); int ok = (n != NULL); ast_node_destroy(n); if (ok) PASS(); else FAIL("0 failed"); }

    TEST("ast_build_literal_int with LLONG_MAX");
    { ASTNode* n = ast_build_literal_int(9223372036854775807LL); int ok = (n != NULL); ast_node_destroy(n); if (ok) PASS(); else FAIL("LLONG_MAX failed"); }

    TEST("ast_build_literal_float with 0.0");
    { ASTNode* n = ast_build_literal_float(0.0); int ok = (n != NULL); ast_node_destroy(n); if (ok) PASS(); else FAIL("0.0 failed"); }

    TEST("ast_build_literal_float with NaN");
    { ASTNode* n = ast_build_literal_float(0.0/0.0); int ok = (n != NULL); ast_node_destroy(n); if (ok) PASS(); else FAIL("NaN failed"); }

    TEST("ast_build_literal_float with INFINITY");
    { ASTNode* n = ast_build_literal_float(1.0/0.0); int ok = (n != NULL); ast_node_destroy(n); if (ok) PASS(); else FAIL("INF failed"); }

    TEST("ast_build_literal_string(NULL)");
    { ASTNode* n = ast_build_literal_string(NULL); int ok = (n != NULL); ast_node_destroy(n); if (ok) PASS(); else FAIL("NULL string failed"); }

    TEST("ast_build_identifier(NULL)");
    { ASTNode* n = ast_build_identifier(NULL); int ok = (n != NULL); ast_node_destroy(n); if (ok) PASS(); else FAIL("NULL id failed"); }

    TEST("ast_build_binary with NULL operands");
    { ASTNode* n = ast_build_binary(OP_ADD, NULL, NULL); int ok = (n != NULL); ast_node_destroy(n); if (ok) PASS(); else FAIL("NULL bin failed"); }

    TEST("ast_build_unary with NULL operand");
    { ASTNode* n = ast_build_unary(UNARY_NEGATE, NULL); int ok = (n != NULL); ast_node_destroy(n); if (ok) PASS(); else FAIL("NULL unary failed"); }

    TEST("ast_build_if with NULLs");
    { ASTNode* n = ast_build_if(NULL, NULL, NULL); int ok = (n != NULL); ast_node_destroy(n); if (ok) PASS(); else FAIL("NULL if failed"); }

    TEST("ast_build_while with NULLs");
    { ASTNode* n = ast_build_while(NULL, NULL); int ok = (n != NULL); ast_node_destroy(n); if (ok) PASS(); else FAIL("NULL while failed"); }

    TEST("ast_build_return(NULL)");
    { ASTNode* n = ast_build_return(NULL); int ok = (n != NULL); ast_node_destroy(n); if (ok) PASS(); else FAIL("NULL ret failed"); }

    TEST("ir_value_create_register(0, void)");
    { IRValue v = ir_value_create_register(0, ir_type_void()); int ok = (v.kind == IR_VALUE_REGISTER); if (ok) PASS(); else FAIL("reg 0 failed"); }

    TEST("ir_value_name_buf with tiny buffer");
    { IRValue v = ir_value_create_register(42, ir_type_i64()); char buf[2]; ir_value_name_buf(v, buf, 2); PASS(); }

    TEST("ir_inst_create(0) - NOP");
    { IRInstruction i = ir_inst_create(IR_OP_NOP); int ok = (i.opcode == IR_OP_NOP); if (ok) PASS(); else FAIL("nop failed"); }

    TEST("ir_inst_create_1 with NULL IRValue");
    { IRValue v = ir_value_null(); ir_inst_create_1(IR_OP_MOV, v, v); PASS(); }

    TEST("ir_inst_create_2 with nulls");
    { IRValue v = ir_value_null(); ir_inst_create_2(IR_OP_ADD, v, v, v); PASS(); }

    TEST("ir_inst_create_3 with nulls");
    { IRValue v = ir_value_null(); ir_inst_create_3(IR_OP_JE, v, v, v, v); PASS(); }

    TEST("ir_bb_create(NULL)");
    { IRBasicBlock* bb = ir_bb_create(NULL, 0); int ok = (bb != NULL); ir_bb_destroy(bb); if (ok) PASS(); else FAIL("bb NULL label failed"); }

    TEST("ir_bb_create(empty)");
    { IRBasicBlock* bb = ir_bb_create("", 0); int ok = (bb != NULL); ir_bb_destroy(bb); if (ok) PASS(); else FAIL("bb empty label failed"); }

    TEST("ir_function_create(NULL, void)");
    { IRFunction* f = ir_function_create(NULL, ir_type_void()); int ok = (f != NULL); ir_function_destroy(f); if (ok) PASS(); else FAIL("func NULL name failed"); }

    TEST("ir_function_destroy(NULL)");
    { ir_function_destroy(NULL); PASS(); }

    TEST("ir_module_create(NULL)");
    { IRModule* m = ir_module_create(NULL); int ok = (m != NULL); ir_module_destroy(m); if (ok) PASS(); else FAIL("mod NULL failed"); }

    TEST("ir_module_destroy(NULL)");
    { ir_module_destroy(NULL); PASS(); }

    TEST("reg_alloc_create/destroy NULL");
    { reg_alloc_destroy(NULL); PASS(); }

    TEST("label_manager_create/destroy NULL");
    { label_manager_destroy(NULL); PASS(); }

    TEST("stack_manager_create/destroy NULL");
    { stack_manager_destroy(NULL); PASS(); }

    TEST("optimizer_create/destroy NULL");
    { optimizer_destroy(NULL); PASS(); }

    TEST("cfg_build(NULL)");
    { CFG* c = cfg_build(NULL); if (!c) PASS(); else { FAIL("should be NULL"); cfg_destroy(c); } }

    TEST("cfg_destroy(NULL)");
    { cfg_destroy(NULL); PASS(); }

    TEST("ssa_create(NULL)");
    { SSAContext* s = ssa_create(NULL); if (s) { ssa_destroy(s); PASS(); } else PASS(); }

    TEST("ssa_destroy(NULL)");
    { ssa_destroy(NULL); PASS(); }

    TEST("semantic_context_create/destroy cycle");
    {
        SemanticContext* ctx = semantic_context_create();
        if (ctx) { semantic_context_destroy(ctx); PASS(); }
        else FAIL("ctx create failed");
    }

    TEST("semantic_analyze(NULL, NULL)");
    { int r = semantic_analyze(NULL, NULL); if (r == 0) PASS(); else FAIL("should return 0"); }

    TEST("daad_type_equals(NULL, NULL)");
    { int r = daad_type_equals(NULL, NULL); if (r == 0) PASS(); else FAIL("should return 0"); }

    TEST("daad_type_is_compatible(NULL, NULL)");
    { int r = daad_type_is_compatible(NULL, NULL); if (r == 0) PASS(); else FAIL("should return 0"); }

    TEST("daad_type_is_numeric(NULL)");
    { int r = daad_type_is_numeric(NULL); if (r == 0) PASS(); else FAIL("should return 0"); }

    TEST("type_checker_check_binary(NULL, ...)");
    { type_checker_check_binary(NULL, OP_ADD, NULL, NULL, 0, 0, NULL); PASS(); }

    TEST("type_checker_check_return(NULL, ...)");
    { type_checker_check_return(NULL, NULL, NULL, 0, 0, NULL); PASS(); }

    TEST("type_checker_check_call(NULL, ...)");
    { type_checker_check_call(NULL, NULL, NULL, 0, 0, 0, NULL); PASS(); }

    TEST("type_checker_check_assignment(NULL, ...)");
    { type_checker_check_assignment(NULL, NULL, NULL, 0, 0, NULL); PASS(); }

    TEST("type_checker_check_unary(NULL, ...)");
    { type_checker_check_unary(NULL, UNARY_NEGATE, NULL, 0, 0, NULL); PASS(); }

    TEST("type_checker_check_cast(NULL, ...)");
    { type_checker_check_cast(NULL, NULL, NULL, 0, 0, NULL); PASS(); }

    TEST("backend_emit_function(NULL, x86, NULL)");
    { int r = backend_emit_function(NULL, BACKEND_X86, NULL); if (r == 0) PASS(); else FAIL("should return 0"); }

    TEST("backend_emit_instruction(NULL, NULL)");
    { int r = backend_emit_instruction(NULL, NULL); if (r == 0) PASS(); else FAIL("should return 0"); }

    TEST("backend_emit_return with NULL value");
    { IRValue v = ir_value_null(); FILE* devnull = fopen("NUL", "w"); if (devnull) { backend_emit_return(v, devnull); fclose(devnull); } PASS(); }

    TEST("ir_bb_add_instruction on block with nop");
    { IRBasicBlock* bb = ir_bb_create("test", 0); IRInstruction i = ir_inst_nop(); ir_bb_add_instruction(bb, i); ir_bb_destroy(bb); PASS(); }

    TEST("ir_bb_get_last_instruction(NULL)");
    { IRInstruction* i = ir_bb_get_last_instruction(NULL); if (!i) PASS(); else FAIL("should be NULL"); }

    TEST("ir_function_add_block(NULL, NULL)");
    { ir_function_add_block(NULL, NULL); PASS(); }
}

static void test_token_edge_cases(void) {
    printf("\n--- Fuzz: Token Edge Cases ---\n");

    TEST("Token with NULL value");
    { Token t = token_create(TOKEN_IDENTIFIER, NULL, 0); token_destroy(&t); PASS(); }

    TEST("token_is_value(NULL, NULL)");
    { int r = token_is_value(NULL, NULL); if (r == 0) PASS(); else FAIL("should be 0"); }

    TEST("Token with empty value");
    { Token t = token_create(TOKEN_STRING, "", 0); int ok = (t.value != NULL && strcmp(t.value, "") == 0); token_destroy(&t); if (ok) PASS(); else FAIL("empty value failed"); }

    TEST("token_create with every type");
    {
        TokenType types[] = { TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_STRING, TOKEN_CHARACTER,
            TOKEN_IDENTIFIER, TOKEN_NEWLINE, TOKEN_EOF, TOKEN_ERROR };
        int ok = 1;
        for (int i = 0; i < 8; i++) {
            Token t = token_create(types[i], "test", 4);
            if (t.type != types[i]) { ok = 0; break; }
            token_destroy(&t);
        }
        if (ok) PASS(); else FAIL("type roundtrip failed");
    }

    TEST("Token with very long value");
    {
        size_t sz = 5001;
        char* val = (char*)malloc(sz);
        memset(val, 'x', sz - 1); val[sz - 1] = '\0';
        Token t = token_create(TOKEN_IDENTIFIER, val, sz - 1);
        int ok = (t.value != NULL && strlen(t.value) == sz - 1);
        token_destroy(&t); free(val);
        if (ok) PASS(); else FAIL("long value failed");
    }

    TEST("Token position edge values");
    {
        Token t = token_create(TOKEN_INTEGER, "0", 1);
        t.start.line = 0; t.start.column = 0; t.start.offset = 0;
        t.end.line = 0; t.end.column = 0; t.end.offset = 0;
        token_destroy(&t); PASS();
    }
}

static void test_lexer_special_inputs(void) {
    printf("\n--- Fuzz: Lexer Special Inputs ---\n");

    TEST("Only null bytes");
    { Lexer* lex = lexer_create("\0\0\0", NULL); size_t c = 0; Token* t = lexer_tokenize(lex, &c); free(t); lexer_destroy(lex); PASS(); }

    TEST("Binary data");
    { char src[256]; for (int i = 0; i < 256; i++) src[i] = (char)i;
      Lexer* lex = lexer_create(src, NULL); size_t c = 0; Token* t = lexer_tokenize(lex, &c); free(t); lexer_destroy(lex); PASS(); }

    TEST("UTF-8 continuation bytes");
    { Lexer* lex = lexer_create("\x80\x81\x82\x83\x84\x85", NULL); size_t c = 0; Token* t = lexer_tokenize(lex, &c); free(t); lexer_destroy(lex); PASS(); }

    TEST("UTF-8 start bytes");
    { Lexer* lex = lexer_create("\xC0\xC1\xC2\xC3\xC4\xC5", NULL); size_t c = 0; Token* t = lexer_tokenize(lex, &c); free(t); lexer_destroy(lex); PASS(); }

    TEST("Invalid UTF-8 sequences");
    { Lexer* lex = lexer_create("\xFF\xFE\xFD", NULL); size_t c = 0; Token* t = lexer_tokenize(lex, &c); free(t); lexer_destroy(lex); PASS(); }

    TEST("Mixed valid+invalid UTF-8");
    { char src[] = "abc\xC0\xAF" "def"; Lexer* lex = lexer_create(src, NULL); size_t c = 0; Token* t = lexer_tokenize(lex, &c); free(t); lexer_destroy(lex); PASS(); }

    TEST("RTL mark in source");
    { Lexer* lex = lexer_create("abc\xE2\x80\x8F""def", NULL); size_t c = 0; Token* t = lexer_tokenize(lex, &c); free(t); lexer_destroy(lex); PASS(); }

    TEST("Zero-width spaces");
    { Lexer* lex = lexer_create("a\xE2\x80\x8B""b\xE2\x80\x8B""c", NULL); size_t c = 0; Token* t = lexer_tokenize(lex, &c); free(t); lexer_destroy(lex); PASS(); }

    TEST("Surrogate halves (U+D800)");
    { Lexer* lex = lexer_create("\xED\xA0\x80", NULL); size_t c = 0; Token* t = lexer_tokenize(lex, &c); free(t); lexer_destroy(lex); PASS(); }

    TEST("Overlong UTF-8 encoding");
    { Lexer* lex = lexer_create("\xC0\xAF", NULL); size_t c = 0; Token* t = lexer_tokenize(lex, &c); free(t); lexer_destroy(lex); PASS(); }

    TEST("Unbalanced string literal");
    { Lexer* lex = lexer_create("\"hello world", NULL); size_t c = 0; Token* t = lexer_tokenize(lex, &c); free(t); lexer_destroy(lex); PASS(); }

    TEST("Unbalanced comment");
    { Lexer* lex = lexer_create("/* no end", NULL); size_t c = 0; Token* t = lexer_tokenize(lex, &c); free(t); lexer_destroy(lex); PASS(); }

    TEST("Single character tokens");
    {
        Lexer* lex = lexer_create("= + - * / < > ! & | ^ ~ % , ; : . @ ( ) [ ] { }", NULL);
        size_t c = 0; Token* t = lexer_tokenize(lex, &c);
        int ok = (c > 20);
        free(t); lexer_destroy(lex);
        if (ok) PASS(); else FAIL("single char tokens failed");
    }

    TEST("Lexer reset and re-tokenize");
    {
        Lexer* lex = lexer_create("\xd8\xb1\xd9\x82\xd9\x85", NULL);
        size_t c1 = 0; Token* t1 = lexer_tokenize(lex, &c1); free(t1);
        lexer_reset(lex);
        size_t c2 = 0; Token* t2 = lexer_tokenize(lex, &c2); free(t2);
        int ok = (c1 == c2);
        lexer_destroy(lex);
        if (ok) PASS(); else FAIL("reset failed");
    }
}

static void test_ir_value_edge_cases(void) {
    printf("\n--- Fuzz: IR Value Edge Cases ---\n");

    TEST("ir_value_create_int with 0");
    { IRValue v = ir_value_create_int(0, ir_type_i64()); int ok = (v.as.int_val == 0); if (ok) PASS(); else FAIL("int 0 failed"); }

    TEST("ir_value_create_int negative");
    { IRValue v = ir_value_create_int(-1, ir_type_i64()); int ok = (v.as.int_val == -1); if (ok) PASS(); else FAIL("int -1 failed"); }

    TEST("ir_value_create_float with 0.0");
    { IRValue v = ir_value_create_float(0.0, ir_type_f64()); int ok = (v.as.float_val == 0.0); if (ok) PASS(); else FAIL("float 0.0 failed"); }

    TEST("ir_value_create_string(NULL)");
    { IRValue v = ir_value_create_string(NULL); int ok = (v.kind == IR_VALUE_CONSTANT_STRING); if (ok) PASS(); else FAIL("str NULL failed"); }

    TEST("ir_value_create_string(empty)");
    { IRValue v = ir_value_create_string(""); int ok = (v.kind == IR_VALUE_CONSTANT_STRING); if (ok) PASS(); else FAIL("str empty failed"); }

    TEST("ir_value_create_label(NULL)");
    { IRValue v = ir_value_create_label(NULL); int ok = (v.kind == IR_VALUE_LABEL); if (ok) PASS(); else FAIL("label NULL failed"); }

    TEST("ir_value_name for every kind");
    {
        IRValue vals[] = {
            ir_value_create_register(1, ir_type_i64()), ir_value_create_int(42, ir_type_i64()),
            ir_value_create_float(3.14, ir_type_f64()), ir_value_create_string("hello"),
            ir_value_create_label(".L1"), ir_value_null()
        };
        for (int i = 0; i < 6; i++) {
            const char* name = ir_value_name(vals[i]);
            if (!name) { FAIL("null name"); return; }
        }
        PASS();
    }

    TEST("ir_value_name_buf with various sizes");
    {
        IRValue v = ir_value_create_register(42, ir_type_i64());
        for (int sz = 0; sz <= 20; sz++) {
            char* buf = (char*)malloc(sz + 1);
            ir_value_name_buf(v, buf, sz);
            free(buf);
        }
        PASS();
    }

    TEST("IR type size_bytes for all types");
    {
        IRType types[] = {ir_type_void(), ir_type_i8(), ir_type_i16(), ir_type_i32(), ir_type_i64(), ir_type_f32(), ir_type_f64(), ir_type_ptr()};
        int sizes[] = {0, 1, 2, 4, 8, 4, 8, 8};
        for (int i = 0; i < 8; i++) {
            int sz = ir_type_size_bytes(types[i]);
            if (sz != sizes[i]) {
                char buf[64]; snprintf(buf, sizeof(buf), "type %d expected %d got %d", i, sizes[i], sz);
                FAIL(buf); return;
            }
        }
        PASS();
    }
}

static void test_ir_instruction_edge_cases(void) {
    printf("\n--- Fuzz: IR Instruction Edge Cases ---\n");

    TEST("All opcodes have names");
    {
        IROpcode ops[] = {
            IR_OP_NOP, IR_OP_LOAD, IR_OP_STORE, IR_OP_MOV, IR_OP_ADD, IR_OP_SUB,
            IR_OP_MUL, IR_OP_DIV, IR_OP_MOD, IR_OP_NEG, IR_OP_NOT, IR_OP_AND,
            IR_OP_OR, IR_OP_XOR, IR_OP_SHL, IR_OP_SHR, IR_OP_CMP, IR_OP_JMP,
            IR_OP_JE, IR_OP_JNE, IR_OP_JG, IR_OP_JL, IR_OP_JGE, IR_OP_JLE,
            IR_OP_CALL, IR_OP_RET, IR_OP_PUSH, IR_OP_POP, IR_OP_PHI, IR_OP_CAST,
            IR_OP_ALLOCA, IR_OP_LOAD_ELEMENT, IR_OP_STORE_ELEMENT, IR_OP_MEMBER_ACCESS, IR_OP_LEA
        };
        for (int i = 0; i < 35; i++) {
            const char* name = ir_opcode_name(ops[i]);
            if (!name || name[0] == '\0') {
                char buf[64]; snprintf(buf, sizeof(buf), "opcode %d has no name", i);
                FAIL(buf); return;
            }
        }
        PASS();
    }

    TEST("All compare ops have names");
    {
        IRCompareOp ops[] = {IR_CMP_EQ, IR_CMP_NE, IR_CMP_GT, IR_CMP_LT, IR_CMP_GE, IR_CMP_LE};
        for (int i = 0; i < 6; i++) {
            const char* name = ir_cmp_name(ops[i]);
            if (!name || name[0] == '\0') {
                char buf[64]; snprintf(buf, sizeof(buf), "cmp %d has no name", i);
                FAIL(buf); return;
            }
        }
        PASS();
    }

    TEST("IR call with max 7 arguments");
    {
        IRType i64 = ir_type_i64();
        IRValue result = ir_value_create_register(1, i64);
        IRValue args[7];
        for (int i = 0; i < 7; i++) args[i] = ir_value_create_int(i, i64);
        IRInstruction call = ir_inst_call(result, "f", args, 7);
        int ok = (call.operand_count == 8);
        if (ok) PASS(); else FAIL("call 7 args");
    }

    TEST("IR phi with max 8 incomings");
    {
        IRType i64 = ir_type_i64();
        IRValue result = ir_value_create_register(1, i64);
        IRValue incomings[16];
        for (int i = 0; i < 8; i++) {
            incomings[i * 2] = ir_value_create_label("src");
            incomings[i * 2 + 1] = ir_value_create_int(i, i64);
        }
        IRInstruction phi = ir_inst_phi(result, incomings, 16);
        int ok = (phi.operand_count == 8);
        if (ok) PASS(); else FAIL("phi 8 incomings");
    }
}

static void test_ast_print_edge_cases(void) {
    printf("\n--- Fuzz: AST Print Edge Cases ---\n");

    TEST("Print NULL node");
    { ast_print(NULL); PASS(); }

    TEST("Print all node types");
    {
        ASTNode* nodes[] = {
            ast_build_literal_int(42), ast_build_literal_float(3.14),
            ast_build_literal_string("hello"), ast_build_literal_char('x'),
            ast_build_literal_bool(1), ast_build_identifier("var"),
            ast_build_unary(UNARY_NEGATE, ast_build_literal_int(1)),
            ast_build_binary(OP_ADD, ast_build_literal_int(1), ast_build_literal_int(2)),
            ast_build_return(ast_build_literal_int(0)),
            ast_build_break(), ast_build_continue(),
        };
        for (int i = 0; i < 11; i++) { ast_print(nodes[i]); ast_node_destroy(nodes[i]); }
        PASS();
    }

    TEST("Print deeply nested node");
    {
        ASTNode* root = ast_build_literal_int(0);
        for (int i = 0; i < 50; i++) root = ast_build_binary(OP_ADD, ast_build_literal_int(i), root);
        ast_print(root); ast_node_destroy(root); PASS();
    }

    TEST("Print to FILE*");
    {
        FILE* devnull = fopen("NUL", "w");
        if (devnull) { ASTNode* n = ast_build_literal_int(42); ast_print_to_file(n, devnull); ast_node_destroy(n); fclose(devnull); }
        PASS();
    }
}

static void test_ir_printer_edge_cases(void) {
    printf("\n--- Fuzz: IR Printer Edge Cases ---\n");

    TEST("Print NULL module");
    { ir_print_module(NULL, NULL); PASS(); }

    TEST("Print empty module");
    { IRModule* mod = ir_module_create("empty"); ir_print_module(mod, NULL); ir_module_destroy(mod); PASS(); }

    TEST("Print module with function");
    {
        IRModule* mod = ir_module_create("test");
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_module_add_function(mod, "main", i64);
        IRBasicBlock* bb = ir_function_add_block(func, "entry");
        ir_bb_add_instruction(bb, ir_inst_create_1(IR_OP_MOV,
            ir_value_create_register(1, i64), ir_value_create_int(42, i64)));
        ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));
        FILE* devnull = fopen("NUL", "w");
        if (devnull) { ir_print_module(mod, devnull); fclose(devnull); }
        ir_module_destroy(mod);
        PASS();
    }
}

static void test_cfg_edge_cases(void) {
    printf("\n--- Fuzz: CFG Edge Cases ---\n");

    TEST("CFG single block");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("single", i64);
        IRBasicBlock* bb = ir_function_add_block(func, "entry");
        ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));
        CFG* cfg = cfg_build(func);
        int ok = (cfg != NULL && cfg->block_count == 1);
        cfg_destroy(cfg); ir_function_destroy(func);
        if (ok) PASS(); else FAIL("single block failed");
    }

    TEST("CFG two blocks");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("two", i64);
        IRBasicBlock* b1 = ir_function_add_block(func, "b1");
        IRBasicBlock* b2 = ir_function_add_block(func, "b2");
        ir_bb_add_instruction(b1, ir_inst_jmp("b2"));
        ir_bb_add_instruction(b2, ir_inst_ret(ir_value_create_int(0, i64)));
        CFG* cfg = cfg_build(func);
        int ok = (cfg != NULL && cfg->block_count == 2);
        cfg_destroy(cfg); ir_function_destroy(func);
        if (ok) PASS(); else FAIL("two blocks failed");
    }

    TEST("CFG dominance on diamond");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("diamond", i64);
        IRBasicBlock* entry = ir_function_add_block(func, "entry");
        (void)entry;
        IRBasicBlock* left = ir_function_add_block(func, "left");
        IRBasicBlock* right = ir_function_add_block(func, "right");
        IRBasicBlock* merge = ir_function_add_block(func, "merge");
        ir_bb_add_instruction(left, ir_inst_jmp("merge"));
        ir_bb_add_instruction(right, ir_inst_jmp("merge"));
        ir_bb_add_instruction(merge, ir_inst_ret(ir_value_create_int(0, i64)));
        CFG* cfg = cfg_build(func);
        cfg_compute_dominators(cfg);
        int ok = (cfg->dominators[0] >= 0);
        cfg_destroy(cfg); ir_function_destroy(func);
        if (ok) PASS(); else FAIL("diamond dom failed");
    }
}

static void test_ssa_edge_cases(void) {
    printf("\n--- Fuzz: SSA Edge Cases ---\n");

    TEST("SSA on single block");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("ssa_single", i64);
        IRBasicBlock* bb = ir_function_add_block(func, "entry");
        ir_bb_add_instruction(bb, ir_inst_create_1(IR_OP_MOV,
            ir_value_create_register(1, i64), ir_value_create_int(42, i64)));
        ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));
        SSAContext* ssa = ssa_create(func);
        ssa_convert_to_ssa(ssa);
        ssa_destroy(ssa); ir_function_destroy(func);
        PASS();
    }

    TEST("SSA convert to/from roundtrip");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("ssa_rt", i64);
        IRBasicBlock* bb = ir_function_add_block(func, "entry");
        ir_bb_add_instruction(bb, ir_inst_create_1(IR_OP_MOV,
            ir_value_create_register(1, i64), ir_value_create_int(42, i64)));
        ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));
        SSAContext* ssa = ssa_create(func);
        ssa_convert_to_ssa(ssa);
        ssa_convert_from_ssa(ssa);
        ssa_destroy(ssa); ir_function_destroy(func);
        PASS();
    }
}

static void test_optimizer_edge_cases(void) {
    printf("\n--- Fuzz: Optimizer Edge Cases ---\n");

    TEST("Optimizer with empty function");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("empty_opt", i64);
        Optimizer* opt = optimizer_create();
        optimizer_add_pass(opt, OPT_PASS_CONSTANT_FOLDING);
        optimizer_run_all(opt, func);
        optimizer_destroy(opt); ir_function_destroy(func);
        PASS();
    }

    TEST("Optimizer with single NOP");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("nop_opt", i64);
        IRBasicBlock* bb = ir_function_add_block(func, "entry");
        ir_bb_add_instruction(bb, ir_inst_nop());
        Optimizer* opt = optimizer_create();
        optimizer_add_pass(opt, OPT_PASS_CONSTANT_FOLDING);
        optimizer_add_pass(opt, OPT_PASS_PEEPHOLE);
        optimizer_add_pass(opt, OPT_PASS_DEAD_CODE_ELIMINATION);
        optimizer_add_pass(opt, OPT_PASS_JUMP_OPTIMIZATION);
        optimizer_add_pass(opt, OPT_PASS_REMOVE_EMPTY_BLOCKS);
        optimizer_add_pass(opt, OPT_PASS_MERGE_BLOCKS);
        optimizer_run_all(opt, func);
        optimizer_destroy(opt); ir_function_destroy(func);
        PASS();
    }

    TEST("Optimizer pass names");
    {
        OptPassType types[] = {OPT_PASS_CONSTANT_FOLDING, OPT_PASS_CONSTANT_PROPAGATION,
            OPT_PASS_COPY_PROPAGATION, OPT_PASS_DEAD_CODE_ELIMINATION,
            OPT_PASS_DEAD_STORE_ELIMINATION, OPT_PASS_ALGEBRAIC_SIMPLIFICATION,
            OPT_PASS_STRENGTH_REDUCTION, OPT_PASS_JUMP_OPTIMIZATION,
            OPT_PASS_REMOVE_EMPTY_BLOCKS, OPT_PASS_MERGE_BLOCKS, OPT_PASS_PEEPHOLE};
        for (int i = 0; i < 11; i++) {
            const char* name = optimizer_pass_name(types[i]);
            if (!name || name[0] == '\0') { FAIL("null pass name"); return; }
        }
        PASS();
    }

    TEST("Optimizer with division by zero in constant fold");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("div0", i64);
        IRBasicBlock* bb = ir_function_add_block(func, "entry");
        ir_bb_add_instruction(bb, ir_inst_create_2(IR_OP_DIV,
            ir_value_create_register(1, i64), ir_value_create_int(10, i64), ir_value_create_int(0, i64)));
        ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));
        Optimizer* opt = optimizer_create();
        optimizer_add_pass(opt, OPT_PASS_CONSTANT_FOLDING);
        optimizer_run_all(opt, func);
        optimizer_destroy(opt); ir_function_destroy(func);
        PASS();
    }
}

static void test_type_registry_edge_cases(void) {
    printf("\n--- Fuzz: Type Registry Edge Cases ---\n");

    TEST("Type registry create/destroy");
    { TypeRegistry* reg = type_registry_create(); int ok = (reg != NULL); type_registry_destroy(reg); if (ok) PASS(); else FAIL("failed"); }

    TEST("Type registry has all basic types");
    {
        TypeRegistry* reg = type_registry_create();
        int ok = (reg->int_type != NULL && reg->float_type != NULL && reg->char_type != NULL &&
                  reg->bool_type != NULL && reg->void_type != NULL && reg->string_type != NULL);
        type_registry_destroy(reg);
        if (ok) PASS(); else FAIL("missing basic types");
    }

    TEST("Scope stack push/pop cycle");
    {
        ScopeStack* stack = scope_stack_create();
        int initial_level = stack->current_level;
        scope_stack_push(stack);
        int ok = (stack->current_level == initial_level + 1);
        scope_stack_pop(stack);
        ok = ok && (stack->current_level == initial_level);
        scope_stack_destroy(stack);
        if (ok) PASS(); else FAIL("scope stack failed");
    }

    TEST("Symbol create NULL name");
    {
        Symbol* sym = symbol_create(NULL, SYMBOL_VARIABLE, NULL, 0);
        if (!sym) PASS(); else { FAIL("should be NULL"); symbol_destroy(sym); }
    }
}

static void test_liveness_edge_cases(void) {
    printf("\n--- Fuzz: Liveness Edge Cases ---\n");

    TEST("Liveness on empty function");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("empty_live", i64);
        LivenessResult* info = liveness_analyze(func);
        liveness_destroy(info); ir_function_destroy(func);
        PASS();
    }

    TEST("Liveness on single block");
    {
        IRType i64 = ir_type_i64();
        IRFunction* func = ir_function_create("single_live", i64);
        IRBasicBlock* bb = ir_function_add_block(func, "entry");
        ir_bb_add_instruction(bb, ir_inst_create_1(IR_OP_MOV,
            ir_value_create_register(1, i64), ir_value_create_int(42, i64)));
        ir_bb_add_instruction(bb, ir_inst_ret(ir_value_create_int(0, i64)));
        LivenessResult* info = liveness_analyze(func);
        liveness_destroy(info); ir_function_destroy(func);
        PASS();
    }
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("=== DAAD Fuzzing & Edge Case Discovery ===\n");

    test_null_safety();
    test_token_edge_cases();
    test_lexer_special_inputs();
    test_ir_value_edge_cases();
    test_ir_instruction_edge_cases();
    test_ast_print_edge_cases();
    test_ir_printer_edge_cases();
    test_cfg_edge_cases();
    test_ssa_edge_cases();
    test_optimizer_edge_cases();
    test_type_registry_edge_cases();
    test_liveness_edge_cases();

    printf("\n=== Results: %d passed, %d failed ===\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
