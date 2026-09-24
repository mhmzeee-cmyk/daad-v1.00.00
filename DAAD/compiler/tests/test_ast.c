/**
 * @file test_ast.c
 * @brief اختبارات شجرة AST في لغة ض Core
 *
 * @version 0.1.0
 * @date 2026-08-03
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast/ast.h"
#include "ast/ast_node.h"
#include "ast/ast_builder.h"
#include "ast/ast_printer.h"

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

static ASTNodeList* make_list(void) {
    ASTNodeList* list = (ASTNodeList*)malloc(sizeof(ASTNodeList));
    ast_node_list_init(list);
    return list;
}

/* ===== اختبار إنشاء العقد ===== */

static void test_node_creation(void) {
    printf("\n=== اختبار إنشاء العقد ===\n");

    {
        ASTNode* node = ast_node_create(NODE_PROGRAM);
        assert_test("إنشاء عقدة PROGRAM", node != NULL && node->type == NODE_PROGRAM);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_BLOCK);
        assert_test("إنشاء عقدة BLOCK", node != NULL && node->type == NODE_BLOCK);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_VARIABLE_DECL);
        assert_test("إنشاء عقدة VARIABLE_DECL", node != NULL && node->type == NODE_VARIABLE_DECL);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_CONSTANT_DECL);
        assert_test("إنشاء عقدة CONSTANT_DECL", node != NULL && node->type == NODE_CONSTANT_DECL);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_FUNCTION_DECL);
        assert_test("إنشاء عقدة FUNCTION_DECL", node != NULL && node->type == NODE_FUNCTION_DECL);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_IF_STATEMENT);
        assert_test("إنشاء عقدة IF_STATEMENT", node != NULL && node->type == NODE_IF_STATEMENT);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_WHILE_STATEMENT);
        assert_test("إنشاء عقدة WHILE_STATEMENT", node != NULL && node->type == NODE_WHILE_STATEMENT);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_FOR_STATEMENT);
        assert_test("إنشاء عقدة FOR_STATEMENT", node != NULL && node->type == NODE_FOR_STATEMENT);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_RETURN_STATEMENT);
        assert_test("إنشاء عقدة RETURN_STATEMENT", node != NULL && node->type == NODE_RETURN_STATEMENT);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_BINARY_EXPRESSION);
        assert_test("إنشاء عقدة BINARY_EXPRESSION", node != NULL && node->type == NODE_BINARY_EXPRESSION);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_UNARY_EXPRESSION);
        assert_test("إنشاء عقدة UNARY_EXPRESSION", node != NULL && node->type == NODE_UNARY_EXPRESSION);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_LITERAL);
        assert_test("إنشاء عقدة LITERAL", node != NULL && node->type == NODE_LITERAL);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_IDENTIFIER);
        assert_test("إنشاء عقدة IDENTIFIER", node != NULL && node->type == NODE_IDENTIFIER);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_CALL_EXPRESSION);
        assert_test("إنشاء عقدة CALL_EXPRESSION", node != NULL && node->type == NODE_CALL_EXPRESSION);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_ASSIGNMENT_EXPRESSION);
        assert_test("إنشاء عقدة ASSIGNMENT_EXPRESSION", node != NULL && node->type == NODE_ASSIGNMENT_EXPRESSION);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_BREAK_STATEMENT);
        assert_test("إنشاء عقدة BREAK_STATEMENT", node != NULL && node->type == NODE_BREAK_STATEMENT);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_CONTINUE_STATEMENT);
        assert_test("إنشاء عقدة CONTINUE_STATEMENT", node != NULL && node->type == NODE_CONTINUE_STATEMENT);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_PARAMETER);
        assert_test("إنشاء عقدة PARAMETER", node != NULL && node->type == NODE_PARAMETER);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_MEMBER_EXPRESSION);
        assert_test("إنشاء عقدة MEMBER_EXPRESSION", node != NULL && node->type == NODE_MEMBER_EXPRESSION);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_INDEX_EXPRESSION);
        assert_test("إنشاء عقدة INDEX_EXPRESSION", node != NULL && node->type == NODE_INDEX_EXPRESSION);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_ARRAY_EXPRESSION);
        assert_test("إنشاء عقدة ARRAY_EXPRESSION", node != NULL && node->type == NODE_ARRAY_EXPRESSION);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_STRUCT_EXPRESSION);
        assert_test("إنشاء عقدة STRUCT_EXPRESSION", node != NULL && node->type == NODE_STRUCT_EXPRESSION);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_POINTER_EXPRESSION);
        assert_test("إنشاء عقدة POINTER_EXPRESSION", node != NULL && node->type == NODE_POINTER_EXPRESSION);
        if (node) ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_node_create(NODE_CAST_EXPRESSION);
        assert_test("إنشاء عقدة CAST_EXPRESSION", node != NULL && node->type == NODE_CAST_EXPRESSION);
        if (node) ast_node_destroy(node);
    }
}

/* ===== اختبار قائمة العقد ===== */

static void test_node_list(void) {
    printf("\n=== اختبار قائمة العقد ===\n");

    {
        ASTNodeList list;
        ast_node_list_init(&list);
        assert_test("تهيئة قائمة فارغة", list.size == 0 && list.items == NULL);
        ast_node_list_destroy(&list);
    }
    {
        ASTNodeList list;
        ast_node_list_init(&list);
        ast_node_list_add(&list, ast_node_create(NODE_LITERAL));
        ast_node_list_add(&list, ast_node_create(NODE_IDENTIFIER));
        ast_node_list_add(&list, ast_node_create(NODE_BREAK_STATEMENT));
        assert_test("إضافة 3 عقد - الحجم = 3", list.size == 3);
        assert_test("العقدة الأولى LITERAL", list.items[0]->type == NODE_LITERAL);
        assert_test("العقدة الثانية IDENTIFIER", list.items[1]->type == NODE_IDENTIFIER);
        assert_test("العقدة الثالثة BREAK_STATEMENT", list.items[2]->type == NODE_BREAK_STATEMENT);
        ast_node_list_destroy(&list);
        assert_test("تدمير القائمة", 1);
    }
}

/* ===== اختبار بناء الحروف ===== */

static void test_builder_literal(void) {
    printf("\n=== اختبار بناء الحروف ===\n");

    {
        ASTNode* node = ast_build_literal_int(42);
        assert_test("بناء عدد صحيح 42", node != NULL &&
            node->type == NODE_LITERAL &&
            node->as.literal.kind == LITERAL_INT &&
            node->as.literal.as.int_value == 42);
        ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_build_literal_float(3.14);
        assert_test("بناء عدد عشري 3.14", node != NULL &&
            node->type == NODE_LITERAL &&
            node->as.literal.kind == LITERAL_FLOAT &&
            node->as.literal.as.float_value == 3.14);
        ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_build_literal_string("test");
        assert_test("بناء نص \"test\"", node != NULL &&
            node->type == NODE_LITERAL &&
            node->as.literal.kind == LITERAL_STRING &&
            strcmp(node->as.literal.as.string_value, "test") == 0);
        ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_build_literal_char('a');
        assert_test("بناء حرف 'a'", node != NULL &&
            node->type == NODE_LITERAL &&
            node->as.literal.kind == LITERAL_CHAR &&
            node->as.literal.as.char_value == 'a');
        ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_build_literal_bool(1);
        assert_test("بناء منطقية صحيح (1)", node != NULL &&
            node->type == NODE_LITERAL &&
            node->as.literal.kind == LITERAL_BOOL &&
            node->as.literal.as.bool_value == 1);
        ast_node_destroy(node);
    }
    {
        ASTNode* node = ast_build_literal_bool(0);
        assert_test("بناء منطقية خطأ (0)", node != NULL &&
            node->type == NODE_LITERAL &&
            node->as.literal.kind == LITERAL_BOOL &&
            node->as.literal.as.bool_value == 0);
        ast_node_destroy(node);
    }
}

/* ===== اختبار بناء المعرف ===== */

static void test_builder_identifier(void) {
    printf("\n=== اختبار بناء المعرف ===\n");

    {
        ASTNode* node = ast_build_identifier("test_var");
        assert_test("بناء معرف \"test_var\"", node != NULL &&
            node->type == NODE_IDENTIFIER &&
            strcmp(node->as.identifier.name, "test_var") == 0);
        ast_node_destroy(node);
    }
}

/* ===== اختبار بناء الثنائي ===== */

static void test_builder_binary(void) {
    printf("\n=== اختبار بناء الثنائي ===\n");

    {
        ASTNode* left = ast_build_literal_int(1);
        ASTNode* right = ast_build_literal_int(2);
        ASTNode* node = ast_build_binary(OP_ADD, left, right);
        assert_test("بناء عملية جمع", node != NULL &&
            node->type == NODE_BINARY_EXPRESSION &&
            node->as.binary.op == OP_ADD &&
            node->as.binary.left == left &&
            node->as.binary.right == right);
        ast_node_destroy(node);
    }
}

/* ===== اختبار بناء الأحادي ===== */

static void test_builder_unary(void) {
    printf("\n=== اختبار بناء الأحادي ===\n");

    {
        ASTNode* operand = ast_build_literal_bool(1);
        ASTNode* node = ast_build_unary(UNARY_NOT, operand);
        assert_test("بناء عملية NOT", node != NULL &&
            node->type == NODE_UNARY_EXPRESSION &&
            node->as.unary.op == UNARY_NOT &&
            node->as.unary.operand == operand);
        ast_node_destroy(node);
    }
}

/* ===== اختبار بناء التعريفات ===== */

static void test_builder_declaration(void) {
    printf("\n=== اختبار بناء التعريفات ===\n");

    {
        ASTNode* type_node = ast_build_identifier("رقم");
        ASTNode* init_expr = ast_build_literal_int(10);
        ASTNode* node = ast_build_variable_decl("العدد", type_node, init_expr);
        assert_test("بناء تعريف متغير", node != NULL &&
            node->type == NODE_VARIABLE_DECL &&
            strcmp(node->as.var_decl.name, "العدد") == 0 &&
            node->as.var_decl.type_node == type_node &&
            node->as.var_decl.init_expr == init_expr);
        ast_node_destroy(node);
    }
    {
        ASTNode* value = ast_build_literal_int(100);
        ASTNode* node = ast_build_constant_decl("النهاية", value);
        assert_test("بناء تعريف ثابت", node != NULL &&
            node->type == NODE_CONSTANT_DECL &&
            strcmp(node->as.const_decl.name, "النهاية") == 0 &&
            node->as.const_decl.value == value);
        ast_node_destroy(node);
    }
    {
        ASTNodeList* params = make_list();
        ast_node_list_add(params, ast_build_parameter("أ", ast_build_identifier("رقم")));
        ASTNode* return_type = ast_build_identifier("رقم");
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, ast_build_return(ast_build_literal_int(0)));
        ASTNode* body = ast_build_block(stmts);
        ASTNode* node = ast_build_function_decl("المثال", params, return_type, body);
        assert_test("بناء تعريف دالة", node != NULL &&
            node->type == NODE_FUNCTION_DECL &&
            strcmp(node->as.func_decl.name, "المثال") == 0 &&
            node->as.func_decl.params.size == 1 &&
            node->as.func_decl.return_type == return_type &&
            node->as.func_decl.body != NULL &&
            node->as.func_decl.body->type == NODE_BLOCK);
        ast_node_destroy(node);
    }
}

/* ===== اختبار الطابع ===== */

static void test_printer(void) {
    printf("\n=== اختبار الطابع ===\n");

    {
        ASTNode* left = ast_build_literal_int(10);
        ASTNode* right = ast_build_literal_int(20);
        ASTNode* expr = ast_build_binary(OP_ADD, left, right);
        ASTNode* decl = ast_build_variable_decl("ن", NULL, expr);
        ASTNodeList* decls = make_list();
        ast_node_list_add(decls, decl);
        ASTNode* program = ast_build_program(decls);
        ast_print_to_file(program, stdout);
        assert_test("طباعة برنامج بسيط بدون انهيار", 1);
        ast_node_destroy(program);
    }
}

/* ===== اختبار تدمير المعقدة المركبة ===== */

static void test_destroy_complex(void) {
    printf("\n=== اختبار تدمير المعقدة المركبة ===\n");

    {
        ASTNode* inner_left = ast_build_literal_int(1);
        ASTNode* inner_right = ast_build_literal_int(2);
        ASTNode* inner = ast_build_binary(OP_MUL, inner_left, inner_right);
        ASTNode* outer_left = ast_build_literal_int(10);
        ASTNode* expr = ast_build_binary(OP_ADD, outer_left, inner);
        ASTNode* cond = ast_build_unary(UNARY_NOT, ast_build_literal_bool(1));
        ASTNodeList* then_stmts = make_list();
        ast_node_list_add(then_stmts, ast_build_return(expr));
        ASTNode* then_block = ast_build_block(then_stmts);
        ASTNode* if_stmt = ast_build_if(cond, then_block, NULL);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, if_stmt);
        ASTNodeList* params = make_list();
        ASTNode* body = ast_build_block(stmts);
        ASTNode* func = ast_build_function_decl("_COMPLEX", params, ast_build_identifier("رقم"), body);
        ASTNodeList* decls = make_list();
        ast_node_list_add(decls, func);
        ASTNode* program = ast_build_program(decls);
        ast_node_destroy(program);
        assert_test("تدمير شجرة مركبة بدون انهيار", 1);
    }
    {
        ASTNodeList* stmts = make_list();

        ASTNode* callee = ast_build_identifier("اططبع");
        ASTNodeList* args = make_list();
        ast_node_list_add(args, ast_build_literal_string("مرحباً"));
        ASTNode* call = ast_build_call(callee, args);
        ASTNode* assign = ast_build_assignment("أ", ASSIGN, call);
        ast_node_list_add(stmts, assign);

        ASTNode* member_obj = ast_build_identifier("الطالب");
        ASTNode* member = ast_build_member(member_obj, "الاسم");
        ast_node_list_add(stmts, member);

        ASTNode* arr = ast_build_index(ast_build_identifier("المصفوفة"), ast_build_literal_int(0));
        ast_node_list_add(stmts, arr);

        ASTNode* cast = ast_build_cast(ast_build_identifier("نص"), ast_build_literal_int(65));
        ast_node_list_add(stmts, cast);

        ASTNode* pointer = ast_build_pointer(ast_build_identifier("رقم"));
        ast_node_list_add(stmts, pointer);

        ASTNodeList* arr_elems = make_list();
        ast_node_list_add(arr_elems, ast_build_literal_int(1));
        ASTNode* arr_node = ast_build_array(arr_elems);
        ast_node_list_add(stmts, arr_node);

        ASTNodeList* struct_names = make_list();
        ASTNodeList* struct_vals = make_list();
        ast_node_list_add(struct_names, ast_build_literal_int(1));
        ast_node_list_add(struct_vals, ast_build_literal_int(1));
        ASTNode* struct_node = ast_build_struct(struct_names, struct_vals);
        ast_node_list_add(stmts, struct_node);

        ASTNode* block = ast_build_block(stmts);
        ASTNodeList* decls = make_list();
        ast_node_list_add(decls, block);
        ASTNode* program = ast_build_program(decls);
        ast_node_destroy(program);
        assert_test("تدمير أنواع متعددة من العقد بدون انهيار", 1);
    }
}

/* ===== الرئيسية ===== */

int main(void) {
    printf("=== اختبارات شجرة AST (AST) ===\n");

    test_node_creation();
    test_node_list();
    test_builder_literal();
    test_builder_identifier();
    test_builder_binary();
    test_builder_unary();
    test_builder_declaration();
    test_printer();
    test_destroy_complex();

    printf("\n=== النتائج ===\n");
    printf("اجتاز: %d\n", tests_passed);
    printf("فشل: %d\n", tests_failed);
    printf("الإجمالي: %d\n", tests_passed + tests_failed);

    return tests_failed > 0 ? 1 : 0;
}
