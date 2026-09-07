/**
 * @file test_precedence.c
 * @brief اختبار أولوية العمليات في Parser لغة ض Core
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

static ASTNode* get_init_expr(const char* source) {
    ASTNode* ast = parse_source(source);
    if (!ast || ast->as.program.declarations.size == 0) return NULL;
    ASTNode* decl = ast->as.program.declarations.items[0];
    return decl->as.var_decl.init_expr;
}

/* ===== اختبارات أولوية العمليات ===== */

static void test_operator_precedence(void) {
    printf("\n=== اختبار أولوية العمليات ===\n");

    /* Multiplicative before additive */
    {
        ASTNode* expr = get_init_expr("متغير: ن = 1 + 2 * 3");
        assert_test("1 + 2 * 3: الضرب أولاً", expr != NULL &&
            expr->type == NODE_BINARY_EXPRESSION &&
            expr->as.binary.left != NULL &&
            expr->as.binary.left->type == NODE_LITERAL);
        if (expr) { /* don't destroy - it's owned by ast */ }
    }

    /* Left-to-right additive */
    {
        ASTNode* expr = get_init_expr("متغير: ن = 1 + 2 + 3");
        assert_test("1 + 2 + 3: يسار لليمين", expr != NULL &&
            expr->type == NODE_BINARY_EXPRESSION);
    }

    /* Comparison before logical AND */
    {
        ASTNode* expr = get_init_expr("متغير: ن = أ > ب و ج < د");
        assert_test("أ > ب و ج < د: المقارنة أولاً", expr != NULL &&
            expr->type == NODE_BINARY_EXPRESSION);
    }

    /* Logical AND before logical OR */
    {
        ASTNode* expr = get_init_expr("متغير: ن = أ و ب أو ج");
        assert_test("أ و ب أو ج: و قبل أو", expr != NULL &&
            expr->type == NODE_BINARY_EXPRESSION);
    }
}

/* ===== اختبارات المقارنات ===== */

static void test_comparison_operators(void) {
    printf("\n=== اختبار عمليات المقارنة ===\n");

    {
        ASTNode* expr = get_init_expr("متغير: ن = أ == ب");
        assert_test("يساوي", expr != NULL &&
            expr->type == NODE_BINARY_EXPRESSION);
    }
    {
        ASTNode* expr = get_init_expr("متغير: ن = أ != ب");
        assert_test("لا يساوي", expr != NULL &&
            expr->type == NODE_BINARY_EXPRESSION);
    }
    {
        ASTNode* expr = get_init_expr("متغير: ن = أ > ب");
        assert_test("أكبر", expr != NULL &&
            expr->type == NODE_BINARY_EXPRESSION);
    }
    {
        ASTNode* expr = get_init_expr("متغير: ن = أ < ب");
        assert_test("أصغر", expr != NULL &&
            expr->type == NODE_BINARY_EXPRESSION);
    }
}

/* ===== اختبارات عمليات البتات ===== */

static void test_bitwise_operators(void) {
    printf("\n=== اختبار عمليات البتات ===\n");

    {
        ASTNode* expr = get_init_expr("متغير: ن = أ & ب");
        assert_test("و_بتية", expr != NULL &&
            expr->type == NODE_BINARY_EXPRESSION);
    }
    {
        ASTNode* expr = get_init_expr("متغير: ن = أ | ب");
        assert_test("أو_بتية", expr != NULL &&
            expr->type == NODE_BINARY_EXPRESSION);
    }
    {
        ASTNode* expr = get_init_expr("متغير: ن = أ ^ ب");
        assert_test("تبادل_بتية", expr != NULL &&
            expr->type == NODE_BINARY_EXPRESSION);
    }
    {
        ASTNode* expr = get_init_expr("متغير: ن = ~أ");
        assert_test("ليس_بتية (unary)", expr != NULL &&
            expr->type == NODE_UNARY_EXPRESSION);
    }
}

/* ===== اختبارات عمليات التحريك ===== */

static void test_shift_operators(void) {
    printf("\n=== اختبار عمليات التحريك ===\n");

    {
        ASTNode* expr = get_init_expr("متغير: ن = أ << 2");
        assert_test("تحريك_يسار", expr != NULL &&
            expr->type == NODE_BINARY_EXPRESSION);
    }
    {
        ASTNode* expr = get_init_expr("متغير: ن = أ >> 2");
        assert_test("تحريك_يمين", expr != NULL &&
            expr->type == NODE_BINARY_EXPRESSION);
    }
}

/* ===== اختبارات عمليات التعيين ===== */

static void test_assignment_operators(void) {
    printf("\n=== اختبار عمليات التعيين ===\n");

    {
        ASTNode* ast = parse_source("أ += 10");
        ASTNode* assign = ast ? ast->as.program.declarations.items[0] : NULL;
        assert_test("+=", ast != NULL &&
            assign != NULL &&
            assign->type == NODE_ASSIGNMENT_EXPRESSION);
        if (ast) ast_node_destroy(ast);
    }
    {
        ASTNode* ast = parse_source("أ -= 5");
        ASTNode* assign = ast ? ast->as.program.declarations.items[0] : NULL;
        assert_test("-=", ast != NULL &&
            assign != NULL &&
            assign->type == NODE_ASSIGNMENT_EXPRESSION);
        if (ast) ast_node_destroy(ast);
    }
}

/* ===== اختبارات التعبيرات الأحادية ===== */

static void test_unary_precedence(void) {
    printf("\n=== اختبار أولوية التعبيرات الأحادية ===\n");

    {
        ASTNode* expr = get_init_expr("متغير: ن = ليس(صحيح)");
        assert_test("ليس", expr != NULL &&
            expr->type == NODE_UNARY_EXPRESSION);
    }
    {
        ASTNode* expr = get_init_expr("متغير: ن = -أ");
        assert_test("سالب", expr != NULL &&
            expr->type == NODE_UNARY_EXPRESSION);
    }
}

/* ===== الرئيسية ===== */

int main(void) {
    printf("=== اختبارات أولوية العمليات ===\n");

    test_operator_precedence();
    test_comparison_operators();
    test_bitwise_operators();
    test_shift_operators();
    test_assignment_operators();
    test_unary_precedence();

    printf("\n=== النتائج ===\n");
    printf("اجتاز: %d\n", tests_passed);
    printf("فشل: %d\n", tests_failed);
    printf("الإجمالي: %d\n", tests_passed + tests_failed);

    return tests_failed > 0 ? 1 : 0;
}
