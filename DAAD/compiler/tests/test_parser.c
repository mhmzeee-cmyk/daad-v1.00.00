/**
 * @file test_parser.c
 * @brief اختبارات المحلل النحوي (Parser) لغة ض Core
 *
 * @version 0.1.0
 * @date 2026-08-03
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/ast.h"
#include "ast/ast_node.h"

static int tests_passed = 0;
static int tests_failed = 0;

static void assert_test(const char* test_name, int condition) {
    if (condition) {
        printf("  ✓ %s\n", test_name);
        tests_passed++;
    } else {
        printf("  ✗ %s\n", test_name);
        tests_failed++;
    }
}

static ASTNode* parse_source(const char* source) {
    Lexer* lexer = lexer_create(source, "test");
    size_t count = 0;
    Token* tokens = lexer_tokenize(lexer, &count);
    Parser* parser = parser_create(tokens, (int)count, "test");
    ASTNode* ast = parser_parse(parser);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
    return ast;
}

static int parse_has_error(const char* source) {
    Lexer* lexer = lexer_create(source, "test");
    size_t count = 0;
    Token* tokens = lexer_tokenize(lexer, &count);
    Parser* parser = parser_create(tokens, (int)count, "test");
    (void)parser_parse(parser);
    int has_err = parser_has_errors(parser);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
    return has_err;
}

/* ===== اختبارات تعريف المتغيرات ===== */

static void test_variable_declaration(void) {
    printf("\n=== اختبار تعريف المتغيرات ===\n");

    {
        ASTNode* ast = parse_source("متغير: عدد = 10");
        assert_test("تعريف متغير بسيط", ast != NULL &&
            ast->type == NODE_PROGRAM &&
            ast->as.program.declarations.size == 1 &&
            ast->as.program.declarations.items[0]->type == NODE_VARIABLE_DECL);
        if (ast) ast_node_destroy(ast);
    }
    {
        ASTNode* ast = parse_source("متغير: عدد: رقم = 10");
        assert_test("تعريف متغير مع نوع", ast != NULL &&
            ast->type == NODE_PROGRAM &&
            ast->as.program.declarations.size == 1 &&
            ast->as.program.declarations.items[0]->type == NODE_VARIABLE_DECL &&
            ast->as.program.declarations.items[0]->as.var_decl.type_node != NULL);
        if (ast) ast_node_destroy(ast);
    }
    {
        ASTNode* ast = parse_source("متغير: اسم = \"أحمد\"");
        assert_test("تعريف متغير نص", ast != NULL &&
            ast->type == NODE_PROGRAM &&
            ast->as.program.declarations.size == 1 &&
            ast->as.program.declarations.items[0]->type == NODE_VARIABLE_DECL);
        if (ast) ast_node_destroy(ast);
    }
    {
        ASTNode* ast = parse_source("متغير: أ = 10\nمتغير: ب = 20");
        assert_test("متغيران", ast != NULL &&
            ast->type == NODE_PROGRAM &&
            ast->as.program.declarations.size == 2);
        if (ast) ast_node_destroy(ast);
    }
}

/* ===== اختبارات تعريف الثوابت ===== */

static void test_constant_declaration(void) {
    printf("\n=== اختبار تعريف الثوابت ===\n");

    {
        ASTNode* ast = parse_source("ثابت: النهاية = 100");
        assert_test("تعريف ثابت", ast != NULL &&
            ast->type == NODE_PROGRAM &&
            ast->as.program.declarations.size == 1 &&
            ast->as.program.declarations.items[0]->type == NODE_CONSTANT_DECL);
        if (ast) ast_node_destroy(ast);
    }
    {
        ASTNode* ast = parse_source("ثابت: الاسم = \"أحمد\"");
        assert_test("تعريف ثابت نص", ast != NULL &&
            ast->type == NODE_PROGRAM &&
            ast->as.program.declarations.size == 1 &&
            ast->as.program.declarations.items[0]->type == NODE_CONSTANT_DECL);
        if (ast) ast_node_destroy(ast);
    }
}

/* ===== اختبارات تعريف الدوال ===== */

static void test_function_declaration(void) {
    printf("\n=== اختبار تعريف الدوال ===\n");

    {
        ASTNode* ast = parse_source("دالة: الرئيسية() :- ارجع(0) نهاية");
        assert_test("دالة بسيطة", ast != NULL &&
            ast->type == NODE_PROGRAM &&
            ast->as.program.declarations.size == 1 &&
            ast->as.program.declarations.items[0]->type == NODE_FUNCTION_DECL);
        if (ast) ast_node_destroy(ast);
    }
    {
        ASTNode* ast = parse_source("دالة: اجمع(أ: رقم، ب: رقم) -> رقم :- ارجع(أ + ب) نهاية");
        ASTNode* func = ast ? ast->as.program.declarations.items[0] : NULL;
        assert_test("دالة مع معاملات وإرجاع", ast != NULL &&
            func != NULL &&
            func->type == NODE_FUNCTION_DECL &&
            func->as.func_decl.params.size == 2 &&
            func->as.func_decl.return_type != NULL &&
            func->as.func_decl.body != NULL);
        if (ast) ast_node_destroy(ast);
    }
    {
        ASTNode* ast = parse_source("أمامية: اجمع(أ: رقم، ب: رقم) -> رقم");
        assert_test("تعريف أمامي", ast != NULL &&
            ast->type == NODE_PROGRAM &&
            ast->as.program.declarations.size == 1 &&
            ast->as.program.declarations.items[0]->type == NODE_FUNCTION_DECL);
        if (ast) ast_node_destroy(ast);
    }
}

/* ===== اختبارات جمل الشرط ===== */

static void test_if_statement(void) {
    printf("\n=== اختبار جمل الشرط ===\n");

    {
        ASTNode* ast = parse_source("إذا(صحيح) :- اطبع(\"نعم\") نهاية");
        ASTNode* stmt = ast ? ast->as.program.declarations.items[0] : NULL;
        assert_test("إذا بسيط", ast != NULL &&
            stmt != NULL &&
            stmt->type == NODE_IF_STATEMENT &&
            stmt->as.if_stmt.condition != NULL &&
            stmt->as.if_stmt.then_block != NULL);
        if (ast) ast_node_destroy(ast);
    }
    {
        ASTNode* ast = parse_source("إذا(صحيح) :- اطبع(\"نعم\") وإلا :- اطبع(\"لا\") نهاية");
        ASTNode* stmt = ast ? ast->as.program.declarations.items[0] : NULL;
        assert_test("إذا وإلا", ast != NULL &&
            stmt != NULL &&
            stmt->type == NODE_IF_STATEMENT &&
            stmt->as.if_stmt.else_block != NULL);
        if (ast) ast_node_destroy(ast);
    }
}

/* ===== اختبارات الحلقات ===== */

static void test_while_loop(void) {
    printf("\n=== اختبار حلقة طالما ===\n");

    {
        ASTNode* ast = parse_source("طالما(صحيح) :- اطبع(\"مرحباً\") نهاية");
        ASTNode* stmt = ast ? ast->as.program.declarations.items[0] : NULL;
        assert_test("حلقة طالما", ast != NULL &&
            stmt != NULL &&
            stmt->type == NODE_WHILE_STATEMENT &&
            stmt->as.while_stmt.condition != NULL &&
            stmt->as.while_stmt.body != NULL);
        if (ast) ast_node_destroy(ast);
    }
}

static void test_for_loop(void) {
    printf("\n=== اختبار حلقة لكل ===\n");

    {
        ASTNode* ast = parse_source("لكل(متغير: عداد = 0؛ العداد < 10؛ العداد = العداد + 1) :- اطبع(العداد) نهاية");
        ASTNode* stmt = ast ? ast->as.program.declarations.items[0] : NULL;
        assert_test("حلقة لكل", ast != NULL &&
            stmt != NULL &&
            stmt->type == NODE_FOR_STATEMENT &&
            stmt->as.for_stmt.init != NULL &&
            stmt->as.for_stmt.condition != NULL &&
            stmt->as.for_stmt.update != NULL &&
            stmt->as.for_stmt.body != NULL);
        if (ast) ast_node_destroy(ast);
    }
}

/* ===== اختبارات الإرجاع ===== */

static void test_return_statement(void) {
    printf("\n=== اختبار جملة الإرجاع ===\n");

    {
        ASTNode* ast = parse_source("دالة: مثال() :- ارجع(10) نهاية");
        ASTNode* func = ast ? ast->as.program.declarations.items[0] : NULL;
        ASTNode* body = func ? func->as.func_decl.body : NULL;
        ASTNode* ret = body && body->as.block.statements.size > 0 ?
            body->as.block.statements.items[0] : NULL;
        assert_test("إرجاع قيمة", ast != NULL &&
            ret != NULL &&
            ret->type == NODE_RETURN_STATEMENT &&
            ret->as.return_stmt.value != NULL);
        if (ast) ast_node_destroy(ast);
    }
    {
        ASTNode* ast = parse_source("دالة: مثال() :- ارجع() نهاية");
        ASTNode* func = ast ? ast->as.program.declarations.items[0] : NULL;
        ASTNode* body = func ? func->as.func_decl.body : NULL;
        ASTNode* ret = body && body->as.block.statements.size > 0 ?
            body->as.block.statements.items[0] : NULL;
        assert_test("إرجاع بدون قيمة", ast != NULL &&
            ret != NULL &&
            ret->type == NODE_RETURN_STATEMENT &&
            ret->as.return_stmt.value == NULL);
        if (ast) ast_node_destroy(ast);
    }
}

/* ===== اختبارات التعبيرات الحسابية ===== */

static void test_binary_expressions(void) {
    printf("\n=== اختبار التعبيرات الثنائية ===\n");

    {
        ASTNode* ast = parse_source("متغير: ن = 1 + 2");
        ASTNode* decl = ast ? ast->as.program.declarations.items[0] : NULL;
        ASTNode* init = decl ? decl->as.var_decl.init_expr : NULL;
        assert_test("عملية جمع", ast != NULL &&
            init != NULL &&
            init->type == NODE_BINARY_EXPRESSION);
        if (ast) ast_node_destroy(ast);
    }
    {
        ASTNode* ast = parse_source("متغير: ن = 1 + 2 * 3");
        ASTNode* decl = ast ? ast->as.program.declarations.items[0] : NULL;
        ASTNode* init = decl ? decl->as.var_decl.init_expr : NULL;
        assert_test("ضرب قبل جمع", ast != NULL &&
            init != NULL &&
            init->type == NODE_BINARY_EXPRESSION);
        if (ast) ast_node_destroy(ast);
    }
}

/* ===== اختبارات التعريف الذاتي ===== */

static void test_unary_expressions(void) {
    printf("\n=== اختبار التعبيرات الأحادية ===\n");

    {
        ASTNode* ast = parse_source("متغير: ن = ليس(صحيح)");
        ASTNode* decl = ast ? ast->as.program.declarations.items[0] : NULL;
        ASTNode* init = decl ? decl->as.var_decl.init_expr : NULL;
        assert_test("نفي منطقي", ast != NULL &&
            init != NULL &&
            init->type == NODE_UNARY_EXPRESSION);
        if (ast) ast_node_destroy(ast);
    }
    {
        ASTNode* ast = parse_source("متغير: ن = -5");
        ASTNode* decl = ast ? ast->as.program.declarations.items[0] : NULL;
        ASTNode* init = decl ? decl->as.var_decl.init_expr : NULL;
        assert_test("سالب", ast != NULL &&
            init != NULL &&
            init->type == NODE_UNARY_EXPRESSION);
        if (ast) ast_node_destroy(ast);
    }
}

/* ===== اختبارات استدعاء الدوال ===== */

static void test_function_calls(void) {
    printf("\n=== اختبار استدعاء الدوال ===\n");

    {
        ASTNode* ast = parse_source("اططبع(\"مرحباً\")");
        ASTNode* call = ast ? ast->as.program.declarations.items[0] : NULL;
        assert_test("استدعاء اطبع", ast != NULL &&
            call != NULL &&
            call->type == NODE_CALL_EXPRESSION);
        if (ast) ast_node_destroy(ast);
    }
    {
        ASTNode* ast = parse_source("متغير: ن = اجمع(10، 20)");
        ASTNode* decl = ast ? ast->as.program.declarations.items[0] : NULL;
        ASTNode* init = decl ? decl->as.var_decl.init_expr : NULL;
        assert_test("استدعاء دالة مع معاملين", ast != NULL &&
            init != NULL &&
            init->type == NODE_CALL_EXPRESSION &&
            init->as.call.args.size == 2);
        if (ast) ast_node_destroy(ast);
    }
}

/* ===== اختبارات الوصول للمصفوفة والعضو ===== */

static void test_access_expressions(void) {
    printf("\n=== اختبار التعبيرات ===\n");

    {
        ASTNode* ast = parse_source("متغير: ن = المصفوفة[0]");
        ASTNode* decl = ast ? ast->as.program.declarations.items[0] : NULL;
        ASTNode* init = decl ? decl->as.var_decl.init_expr : NULL;
        assert_test("وصول مصفوفة", ast != NULL &&
            init != NULL &&
            init->type == NODE_INDEX_EXPRESSION);
        if (ast) ast_node_destroy(ast);
    }
    {
        ASTNode* ast = parse_source("متغير: ن = الطالب.الاسم");
        ASTNode* decl = ast ? ast->as.program.declarations.items[0] : NULL;
        ASTNode* init = decl ? decl->as.var_decl.init_expr : NULL;
        assert_test("وصول عضو هيكل", ast != NULL &&
            init != NULL &&
            init->type == NODE_MEMBER_EXPRESSION);
        if (ast) ast_node_destroy(ast);
    }
}

/* ===== اختبارات التعيين ===== */

static void test_assignment(void) {
    printf("\n=== اختبار التعيين ===\n");

    {
        ASTNode* ast = parse_source("أ = 10");
        ASTNode* assign = ast ? ast->as.program.declarations.items[0] : NULL;
        assert_test("تعيين بسيط", ast != NULL &&
            assign != NULL &&
            assign->type == NODE_ASSIGNMENT_EXPRESSION);
        if (ast) ast_node_destroy(ast);
    }
}

/* ===== اختبارات التعامل مع الأخطاء ===== */

static void test_error_recovery(void) {
    printf("\n=== اختبار التعامل مع الأخطاء ===\n");

    {
        int has_err = parse_has_error("إذا(صحيح :- اطبع(\"نعم\") نهاية");
        assert_test("أقواس غير متطابقة", has_err);
    }
    {
        int has_err = parse_has_error("متغير: عدد = ");
        assert_test("تعبير مفقود", has_err);
    }
}

/* ===== اختبارات البرامج الكاملة ===== */

static void test_full_programs(void) {
    printf("\n=== اختبار برامج كاملة ===\n");

    {
        const char* src =
            "دالة: مضروب(ن: رقم) -> رقم :-\n"
            "    إذا(ن <= 1) :-\n"
            "        ارجع(1)\n"
            "    نهاية\n"
            "    ارجع(ن * مضروب(ن - 1))\n"
            "نهاية";
        ASTNode* ast = parse_source(src);
        assert_test("برنامج مضروب", ast != NULL &&
            ast->type == NODE_PROGRAM &&
            ast->as.program.declarations.size == 1);
        if (ast) ast_node_destroy(ast);
    }
}

/* ===== الرئيسية ===== */

int main(void) {
    printf("=== اختبارات المحلل النحوي (Parser) ===\n");

    test_variable_declaration();
    test_constant_declaration();
    test_function_declaration();
    test_if_statement();
    test_while_loop();
    test_for_loop();
    test_return_statement();
    test_binary_expressions();
    test_unary_expressions();
    test_function_calls();
    test_access_expressions();
    test_assignment();
    test_error_recovery();
    test_full_programs();

    printf("\n=== النتائج ===\n");
    printf("اجتاز: %d\n", tests_passed);
    printf("فشل: %d\n", tests_failed);
    printf("الإجمالي: %d\n", tests_passed + tests_failed);

    return tests_failed > 0 ? 1 : 0;
}
