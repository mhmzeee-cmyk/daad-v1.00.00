#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic/type.h"
#include "semantic/type_registry.h"
#include "semantic/symbol.h"
#include "semantic/symbol_table.h"
#include "semantic/scope.h"
#include "semantic/scope_stack.h"
#include "semantic/semantic_error.h"
#include "semantic/type_checker.h"
#include "semantic/semantic_visitor.h"
#include "semantic/constant_folding.h"
#include "ast/ast.h"
#include "ast/ast_builder.h"

static int tests_passed = 0;
static int tests_failed = 0;

static void assert_test(const char* test_name, int condition) {
    if (condition) {
        printf("   %s\n", test_name);
        tests_passed++;
    } else {
        printf("   %s\n", test_name);
        tests_failed++;
    }
    fflush(stdout);
}

static ASTNodeList* make_list(void) {
    ASTNodeList* list = (ASTNodeList*)malloc(sizeof(ASTNodeList));
    ast_node_list_init(list);
    return list;
}

static ASTNode* make_program_with_stmts(ASTNodeList* stmts) {
    ASTNode* block = ast_build_block(stmts);
    ASTNodeList* decls = make_list();
    ast_node_list_add(decls, block);
    return ast_build_program(decls);
}

int main(void) {
    printf("=== Type Tests ===\n"); fflush(stdout);
    {
        DaadType* int_t = daad_type_create(TYPE_INT, "int", 8);
        assert_test("create int type", int_t != NULL);
        DaadType* float_t = daad_type_create(TYPE_FLOAT, "float", 8);
        assert_test("create float type", float_t != NULL);
        assert_test("int != float", !daad_type_equals(int_t, float_t));
        DaadType* int2 = daad_type_create(TYPE_INT, "int", 8);
        assert_test("int == int", daad_type_equals(int_t, int2));
        assert_test("int is numeric", daad_type_is_numeric(int_t));
        assert_test("float is numeric", daad_type_is_numeric(float_t));
        assert_test("int compatible float", daad_type_is_compatible(int_t, float_t));
        daad_type_destroy(int_t);
        daad_type_destroy(float_t);
        daad_type_destroy(int2);
    }

    printf("\n=== Pointer Type ===\n"); fflush(stdout);
    {
        DaadType* int_t = daad_type_create(TYPE_INT, "int", 8);
        DaadType* ptr = daad_type_create_pointer(int_t);
        assert_test("create pointer", ptr != NULL && ptr->kind == TYPE_POINTER);
        assert_test("pointer is pointer", daad_type_is_pointer(ptr));
        assert_test("int not pointer", !daad_type_is_pointer(int_t));
        daad_type_destroy(ptr);
    }

    printf("\n=== Array Type ===\n"); fflush(stdout);
    {
        DaadType* int_t = daad_type_create(TYPE_INT, "int", 8);
        DaadType* arr = daad_type_create_array(int_t, 10);
        assert_test("create array", arr != NULL && arr->kind == TYPE_ARRAY);
        assert_test("array length", arr->as.array.length == 10);
        assert_test("array size", arr->size == 80);
        daad_type_destroy(arr);
    }

    printf("\n=== Struct Type ===\n"); fflush(stdout);
    {
        char** names = (char**)malloc(2 * sizeof(char*));
        names[0] = strdup("x");
        names[1] = strdup("y");
        DaadType** types = (DaadType**)malloc(2 * sizeof(DaadType*));
        types[0] = daad_type_create(TYPE_INT, "int", 8);
        types[1] = daad_type_create(TYPE_INT, "int", 8);
        DaadType* s = daad_type_create_struct(names, types, 2);
        assert_test("create struct", s != NULL && s->kind == TYPE_STRUCT);
        assert_test("struct members", s->as.struct_type.member_count == 2);
        assert_test("struct size", s->size == 16);
        daad_type_destroy(s);
    }

    printf("\n=== Function Type ===\n"); fflush(stdout);
    {
        DaadType** params = (DaadType**)malloc(2 * sizeof(DaadType*));
        params[0] = daad_type_create(TYPE_INT, "int", 8);
        params[1] = daad_type_create(TYPE_FLOAT, "float", 8);
        DaadType* ret = daad_type_create(TYPE_INT, "int", 8);
        DaadType* func = daad_type_create_function(params, 2, ret);
        assert_test("create function type", func != NULL && func->kind == TYPE_FUNCTION);
        assert_test("function params", func->as.function.param_count == 2);
        daad_type_destroy(func);
    }

    printf("\n=== Type Registry ===\n"); fflush(stdout);
    {
        TypeRegistry* reg = type_registry_create();
        assert_test("create registry", reg != NULL);
        assert_test("int type", type_registry_get_int(reg) != NULL);
        assert_test("float type", type_registry_get_float(reg) != NULL);
        assert_test("char type", type_registry_get_char(reg) != NULL);
        assert_test("bool type", type_registry_get_bool(reg) != NULL);
        assert_test("void type", type_registry_get_void(reg) != NULL);
        assert_test("string type", type_registry_get_string(reg) != NULL);
        type_registry_destroy(reg);
    }

    printf("\n=== Symbol ===\n"); fflush(stdout);
    {
        DaadType* int_t = daad_type_create(TYPE_INT, "int", 8);
        Symbol* sym = symbol_create("x", SYMBOL_VARIABLE, int_t, 0);
        assert_test("create symbol", sym != NULL);
        assert_test("symbol name", strcmp(sym->name, "x") == 0);
        assert_test("symbol kind", sym->kind == SYMBOL_VARIABLE);
        assert_test("symbol mutable", sym->is_mutable == 1);
        symbol_destroy(sym);
        daad_type_destroy(int_t);
    }

    printf("\n=== Symbol Table ===\n"); fflush(stdout);
    {
        SymbolTable* table = symbol_table_create();
        assert_test("create table", table != NULL);
        DaadType* int_t = daad_type_create(TYPE_INT, "int", 8);
        Symbol* s1 = symbol_create("x", SYMBOL_VARIABLE, int_t, 0);
        Symbol* s2 = symbol_create("y", SYMBOL_VARIABLE, int_t, 0);
        symbol_table_insert(table, s1);
        symbol_table_insert(table, s2);
        assert_test("table size", table->size == 2);
        Symbol* found = symbol_table_lookup(table, "x");
        assert_test("lookup x", found != NULL);
        found = symbol_table_lookup(table, "z");
        assert_test("lookup z (not found)", found == NULL);
        symbol_table_destroy(table);
        daad_type_destroy(int_t);
    }

    printf("\n=== Scope ===\n"); fflush(stdout);
    {
        Scope* global = scope_create(NULL, 0);
        assert_test("create scope", global != NULL);
        DaadType* int_t = daad_type_create(TYPE_INT, "int", 8);
        Symbol* sym = symbol_create("x", SYMBOL_VARIABLE, int_t, 0);
        int ok = scope_define(global, sym);
        assert_test("define x", ok);
        Symbol* found = scope_lookup_current(global, "x");
        assert_test("lookup x in scope", found != NULL);
        Scope* local = scope_create(global, 1);
        Symbol* local_sym = symbol_create("y", SYMBOL_VARIABLE, int_t, 1);
        scope_define(local, local_sym);
        found = scope_lookup(local, "x");
        assert_test("lookup x from child scope", found != NULL);
        found = scope_lookup(local, "y");
        assert_test("lookup y in child", found != NULL);
        scope_destroy(local);
        scope_destroy(global);
        daad_type_destroy(int_t);
    }

    printf("\n=== Scope Stack ===\n"); fflush(stdout);
    {
        ScopeStack* stack = scope_stack_create();
        assert_test("create scope stack", stack != NULL);
        assert_test("scope not empty", scope_stack_peek(stack) != NULL);
        DaadType* int_t = daad_type_create(TYPE_INT, "int", 8);
        Symbol* sym = symbol_create("x", SYMBOL_VARIABLE, int_t, 0);
        scope_stack_define(stack, sym);
        Symbol* found = scope_stack_lookup(stack, "x");
        assert_test("lookup x in stack", found != NULL);
        scope_stack_push(stack);
        assert_test("push scope", stack->top == 1);
        Symbol* local_sym = symbol_create("y", SYMBOL_VARIABLE, int_t, 1);
        scope_stack_define(stack, local_sym);
        found = scope_stack_lookup(stack, "x");
        assert_test("lookup x from inner scope", found != NULL);
        scope_stack_pop(stack);
        assert_test("pop scope", stack->top == 0);
        found = scope_stack_lookup(stack, "y");
        assert_test("y not found after pop", found == NULL);
        scope_stack_destroy(stack);
        daad_type_destroy(int_t);
    }

    printf("\n=== Semantic Errors ===\n"); fflush(stdout);
    {
        SemanticErrorList* list = semantic_error_list_create();
        assert_test("create error list", list != NULL);
        semantic_error_add(list, S001_UNDEFINED_VARIABLE, 10, 5, "test.daad", "undefined var: %s", "x");
        assert_test("add error", list->size == 1);
        assert_test("error code", list->errors[0].code == S001_UNDEFINED_VARIABLE);
        assert_test("error line", list->errors[0].line == 10);
        const char* name = semantic_error_code_name(S001_UNDEFINED_VARIABLE);
        assert_test("error code name", strcmp(name, "S001") == 0);
        semantic_error_list_destroy(list);
    }

    printf("\n=== Type Checker ===\n"); fflush(stdout);
    {
        TypeRegistry* reg = type_registry_create();
        SemanticErrorList* errors = semantic_error_list_create();
        TypeChecker* checker = type_checker_create(reg, errors);
        DaadType* int_t = type_registry_get_int(reg);
        DaadType* float_t = type_registry_get_float(reg);
        DaadType* bool_t = type_registry_get_bool(reg);

        DaadType* result = type_checker_check_binary(checker, OP_ADD, int_t, int_t, 1, 1, "test.daad");
        assert_test("int + int = int", result == int_t);
        result = type_checker_check_binary(checker, OP_ADD, int_t, float_t, 1, 1, "test.daad");
        assert_test("int + float = float", result == float_t);
        result = type_checker_check_binary(checker, OP_EQUAL, int_t, int_t, 1, 1, "test.daad");
        assert_test("int == int = bool", result == bool_t);
        result = type_checker_check_binary(checker, OP_AND, bool_t, bool_t, 1, 1, "test.daad");
        assert_test("bool and bool = bool", result == bool_t);
        int ok = type_checker_check_assignment(checker, int_t, int_t, 1, 1, "test.daad");
        assert_test("assign int = int", ok);
        ok = type_checker_check_return(checker, int_t, int_t, 1, 1, "test.daad");
        assert_test("return int = int", ok);
        type_checker_destroy(checker);
        semantic_error_list_destroy(errors);
        type_registry_destroy(reg);
    }

    printf("\n=== Visitor Basic ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        assert_test("create context", ctx != NULL);
        ASTNode* type_node = ast_build_identifier("رقم");
        ASTNode* init = ast_build_literal_int(42);
        ASTNode* decl = ast_build_variable_decl("x", type_node, init);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, decl);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("valid var decl = 0 errors", errors == 0);
        Symbol* sym = scope_stack_lookup(ctx->scopes, "x");
        assert_test("x defined after analysis", sym != NULL);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor Undefined Var ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* ref = ast_build_identifier("y");
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, ref);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("undefined var = error", errors > 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor Duplicate Var ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* d1 = ast_build_variable_decl("x", ast_build_identifier("رقم"), ast_build_literal_int(1));
        ASTNode* d2 = ast_build_variable_decl("x", ast_build_identifier("رقم"), ast_build_literal_int(2));
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, d1);
        ast_node_list_add(stmts, d2);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("duplicate var = error", errors > 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor Constant Reassign ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* cdecl = ast_build_constant_decl("PI", ast_build_literal_int(42));
        ASTNode* assign = ast_build_assignment("PI", ASSIGN, ast_build_literal_int(3));
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, cdecl);
        ast_node_list_add(stmts, assign);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("constant reassign = error", errors > 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor Assignment ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* decl = ast_build_variable_decl("x", ast_build_identifier("رقم"), ast_build_literal_int(10));
        ASTNode* assign = ast_build_assignment("x", ASSIGN, ast_build_literal_int(20));
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, decl);
        ast_node_list_add(stmts, assign);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("valid assignment = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Constant Folding Int ===\n"); fflush(stdout);
    {
        ASTNode* add = ast_build_binary(OP_ADD, ast_build_literal_int(3), ast_build_literal_int(5));
        ASTNode* folded = constant_fold(add);
        assert_test("fold 3+5 = 8", folded != NULL && folded->type == NODE_LITERAL && folded->as.literal.as.int_value == 8);

        ASTNode* sub = ast_build_binary(OP_SUB, ast_build_literal_int(10), ast_build_literal_int(3));
        folded = constant_fold(sub);
        assert_test("fold 10-3 = 7", folded->as.literal.as.int_value == 7);

        ASTNode* mul = ast_build_binary(OP_MUL, ast_build_literal_int(4), ast_build_literal_int(5));
        folded = constant_fold(mul);
        assert_test("fold 4*5 = 20", folded->as.literal.as.int_value == 20);

        ASTNode* div = ast_build_binary(OP_DIV, ast_build_literal_int(10), ast_build_literal_int(3));
        folded = constant_fold(div);
        assert_test("fold 10/3 = 3", folded->as.literal.as.int_value == 3);

        ASTNode* mod = ast_build_binary(OP_MOD, ast_build_literal_int(10), ast_build_literal_int(3));
        folded = constant_fold(mod);
        assert_test("fold 10%3 = 1", folded->as.literal.as.int_value == 1);
    }

    printf("\n=== Constant Folding Comparison ===\n"); fflush(stdout);
    {
        ASTNode* eq = ast_build_binary(OP_EQUAL, ast_build_literal_int(5), ast_build_literal_int(3));
        ASTNode* folded = constant_fold(eq);
        assert_test("fold 5==3 = 0", folded->as.literal.as.int_value == 0);

        eq = ast_build_binary(OP_EQUAL, ast_build_literal_int(5), ast_build_literal_int(5));
        folded = constant_fold(eq);
        assert_test("fold 5==5 = 1", folded->as.literal.as.int_value == 1);

        ASTNode* gt = ast_build_binary(OP_GREATER, ast_build_literal_int(5), ast_build_literal_int(3));
        folded = constant_fold(gt);
        assert_test("fold 5>3 = 1", folded->as.literal.as.int_value == 1);
    }

    printf("\n=== Constant Folding Float ===\n"); fflush(stdout);
    {
        ASTNode* add = ast_build_binary(OP_ADD, ast_build_literal_float(3.0), ast_build_literal_float(5.0));
        ASTNode* folded = constant_fold(add);
        assert_test("fold 3.0+5.0=8.0", folded != NULL && folded->as.literal.as.float_value == 8.0);
    }

    printf("\n=== Constant Folding Division By Zero ===\n"); fflush(stdout);
    {
        ASTNode* div = ast_build_binary(OP_DIV, ast_build_literal_int(5), ast_build_literal_int(0));
        ASTNode* folded = constant_fold(div);
        assert_test("div by zero not folded", folded == div);
    }

    printf("\n=== Constant Folding Nested ===\n"); fflush(stdout);
    {
        ASTNode* inner = ast_build_binary(OP_MUL, ast_build_literal_int(2), ast_build_literal_int(3));
        ASTNode* outer = ast_build_binary(OP_ADD, inner, ast_build_literal_int(4));
        ASTNode* folded = constant_fold(outer);
        assert_test("fold (2*3)+4=10", folded != NULL && folded->type == NODE_LITERAL && folded->as.literal.as.int_value == 10);
    }

    printf("\n=== Constant Folding Unary ===\n"); fflush(stdout);
    {
        ASTNode* neg = ast_build_unary(UNARY_NEGATE, ast_build_literal_int(42));
        ASTNode* folded = constant_fold(neg);
        assert_test("fold -42=-42", folded != NULL && folded->type == NODE_LITERAL && folded->as.literal.as.int_value == -42);
    }

    printf("\n=== Constant Folding Is Foldable ===\n"); fflush(stdout);
    {
        ASTNode* lit = ast_build_literal_int(5);
        assert_test("int literal foldable", constant_folding_is_foldable(lit));
        ASTNode* id = ast_build_identifier("x");
        assert_test("identifier not foldable", !constant_folding_is_foldable(id));
        ASTNode* bin = ast_build_binary(OP_ADD, ast_build_literal_int(3), ast_build_literal_int(5));
        assert_test("int+int foldable", constant_folding_is_foldable(bin));
        ASTNode* bin2 = ast_build_binary(OP_ADD, ast_build_literal_int(3), ast_build_identifier("y"));
        assert_test("int+id not foldable", !constant_folding_is_foldable(bin2));
        ast_node_destroy(lit);
        ast_node_destroy(id);
    }

    printf("\n=== Visitor If Statement ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* cond = ast_build_literal_bool(1);
        ASTNode* decl = ast_build_variable_decl("y", ast_build_identifier("رقم"), ast_build_literal_int(5));
        ASTNodeList* then_stmts = make_list();
        ast_node_list_add(then_stmts, decl);
        ASTNode* then_block = ast_build_block(then_stmts);
        ASTNode* if_stmt = ast_build_if(cond, then_block, NULL);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, if_stmt);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("if statement = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor If Else ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* cond = ast_build_literal_bool(1);
        ASTNode* then_decl = ast_build_variable_decl("x", ast_build_identifier("رقم"), ast_build_literal_int(1));
        ASTNodeList* then_stmts = make_list();
        ast_node_list_add(then_stmts, then_decl);
        ASTNode* then_block = ast_build_block(then_stmts);
        ASTNode* else_decl = ast_build_variable_decl("y", ast_build_identifier("رقم"), ast_build_literal_int(2));
        ASTNodeList* else_stmts = make_list();
        ast_node_list_add(else_stmts, else_decl);
        ASTNode* else_block = ast_build_block(else_stmts);
        ASTNode* if_stmt = ast_build_if(cond, then_block, else_block);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, if_stmt);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("if-else statement = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor While ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* cond = ast_build_literal_bool(1);
        ASTNode* brk = ast_build_break();
        ASTNodeList* body_stmts = make_list();
        ast_node_list_add(body_stmts, brk);
        ASTNode* body = ast_build_block(body_stmts);
        ASTNode* while_stmt = ast_build_while(cond, body);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, while_stmt);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("while with break = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor While Continue ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* cond = ast_build_literal_bool(1);
        ASTNode* cont = ast_build_continue();
        ASTNodeList* body_stmts = make_list();
        ast_node_list_add(body_stmts, cont);
        ASTNode* body = ast_build_block(body_stmts);
        ASTNode* while_stmt = ast_build_while(cond, body);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, while_stmt);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("while with continue = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor For ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* init = ast_build_variable_decl("i", ast_build_identifier("رقم"), ast_build_literal_int(0));
        ASTNode* cond = ast_build_binary(OP_LESS, ast_build_identifier("i"), ast_build_literal_int(10));
        ASTNode* update = ast_build_assignment("i", ASSIGN_ADD, ast_build_literal_int(1));
        ASTNodeList* body_stmts = make_list();
        ASTNode* body = ast_build_block(body_stmts);
        ASTNode* for_stmt = ast_build_for(init, cond, update, body);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, for_stmt);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("for statement = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor For Break Continue ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* init = ast_build_variable_decl("i", ast_build_identifier("رقم"), ast_build_literal_int(0));
        ASTNode* cond = ast_build_binary(OP_LESS, ast_build_identifier("i"), ast_build_literal_int(10));
        ASTNode* update = ast_build_assignment("i", ASSIGN_ADD, ast_build_literal_int(1));
        ASTNodeList* body_stmts = make_list();
        ast_node_list_add(body_stmts, ast_build_break());
        ast_node_list_add(body_stmts, ast_build_continue());
        ASTNode* body = ast_build_block(body_stmts);
        ASTNode* for_stmt = ast_build_for(init, cond, update, body);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, for_stmt);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("for with break+continue = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor Scope ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* outer = ast_build_variable_decl("x", ast_build_identifier("رقم"), ast_build_literal_int(1));
        ASTNode* inner = ast_build_variable_decl("y", ast_build_identifier("رقم"), ast_build_literal_int(2));
        ASTNodeList* inner_stmts = make_list();
        ast_node_list_add(inner_stmts, ast_build_identifier("x"));
        ast_node_list_add(inner_stmts, ast_build_identifier("y"));
        ASTNode* inner_block = ast_build_block(inner_stmts);
        ASTNode* if_stmt = ast_build_if(ast_build_literal_bool(1), inner_block, NULL);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, outer);
        ast_node_list_add(stmts, inner);
        ast_node_list_add(stmts, if_stmt);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("scope access from child = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor Function ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* param = ast_build_parameter("a", ast_build_identifier("رقم"));
        ASTNodeList* params = make_list();
        ast_node_list_add(params, param);
        ASTNodeList* body_stmts = make_list();
        ast_node_list_add(body_stmts, ast_build_return(ast_build_literal_int(10)));
        ASTNode* body = ast_build_block(body_stmts);
        ASTNode* func = ast_build_function_decl("add", params, ast_build_identifier("رقم"), body);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, func);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("function decl = 0 errors", errors == 0);
        Symbol* sym = scope_stack_lookup(ctx->scopes, "add");
        assert_test("function add defined", sym != NULL && sym->kind == SYMBOL_FUNCTION);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor Binary Expr ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* add = ast_build_binary(OP_ADD, ast_build_literal_int(5), ast_build_literal_int(3));
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, add);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("5+3 expr = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor Unary Expr ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* neg = ast_build_unary(UNARY_NEGATE, ast_build_literal_int(5));
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, neg);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("-5 expr = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor Literal ===\n"); fflush(stdout);
    {
        ASTNode* int_lit = ast_build_literal_int(42);
        assert_test("int literal type", int_lit->type == NODE_LITERAL && int_lit->as.literal.kind == LITERAL_INT);
        ASTNode* float_lit = ast_build_literal_float(3.14);
        assert_test("float literal type", float_lit->type == NODE_LITERAL && float_lit->as.literal.kind == LITERAL_FLOAT);
        ASTNode* str_lit = ast_build_literal_string("hello");
        assert_test("string literal type", str_lit->type == NODE_LITERAL && str_lit->as.literal.kind == LITERAL_STRING);
        ASTNode* char_lit = ast_build_literal_char('A');
        assert_test("char literal type", char_lit->type == NODE_LITERAL && char_lit->as.literal.kind == LITERAL_CHAR);
        ASTNode* bool_lit = ast_build_literal_bool(1);
        assert_test("bool literal type", bool_lit->type == NODE_LITERAL && bool_lit->as.literal.kind == LITERAL_BOOL);
        ast_node_destroy(int_lit);
        ast_node_destroy(float_lit);
        ast_node_destroy(str_lit);
        ast_node_destroy(char_lit);
        ast_node_destroy(bool_lit);
    }

    printf("\n=== Visitor Call ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNodeList* args = make_list();
        ast_node_list_add(args, ast_build_literal_int(10));
        ASTNode* call = ast_build_call(ast_build_identifier("myFunc"), args);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, call);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("undefined function call = error", errors > 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor Cast ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* cast = ast_build_cast(ast_build_identifier("رقم"), ast_build_literal_int(10));
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, cast);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("cast int->int = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Multiple Declarations ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, ast_build_variable_decl("a", ast_build_identifier("رقم"), ast_build_literal_int(1)));
        ast_node_list_add(stmts, ast_build_variable_decl("b", ast_build_identifier("رقم"), ast_build_literal_int(2)));
        ast_node_list_add(stmts, ast_build_variable_decl("c", ast_build_identifier("رقم"), ast_build_literal_int(3)));
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("3 different vars = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Nested Scopes ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* outer = ast_build_variable_decl("x", ast_build_identifier("رقم"), ast_build_literal_int(1));
        ASTNodeList* inner_stmts = make_list();
        ast_node_list_add(inner_stmts, ast_build_identifier("x"));
        ASTNode* inner_block = ast_build_block(inner_stmts);
        ASTNode* if_stmt = ast_build_if(ast_build_literal_bool(1), inner_block, NULL);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, outer);
        ast_node_list_add(stmts, if_stmt);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("nested scopes access parent = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Multiple Functions ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* ret_type = ast_build_identifier("رقم");
        ASTNodeList* body1_stmts = make_list();
        ast_node_list_add(body1_stmts, ast_build_return(ast_build_literal_int(1)));
        ASTNode* func1 = ast_build_function_decl("f1", make_list(), ret_type, ast_build_block(body1_stmts));
        ASTNodeList* body2_stmts = make_list();
        ast_node_list_add(body2_stmts, ast_build_return(ast_build_literal_int(2)));
        ASTNode* func2 = ast_build_function_decl("f2", make_list(), ast_build_identifier("رقم"), ast_build_block(body2_stmts));
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, func1);
        ast_node_list_add(stmts, func2);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("2 different functions = 0 errors", errors == 0);
        Symbol* s1 = scope_stack_lookup(ctx->scopes, "f1");
        Symbol* s2 = scope_stack_lookup(ctx->scopes, "f2");
        assert_test("f1 defined", s1 != NULL);
        assert_test("f2 defined", s2 != NULL);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Duplicate Function Error ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* func1 = ast_build_function_decl("myFunc", make_list(), ast_build_identifier("رقم"), ast_build_block(make_list()));
        ASTNode* func2 = ast_build_function_decl("myFunc", make_list(), ast_build_identifier("رقم"), ast_build_block(make_list()));
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, func1);
        ast_node_list_add(stmts, func2);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("duplicate function = error", errors > 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Empty Program ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* program = make_program_with_stmts(make_list());
        int errors = semantic_analyze(program, ctx);
        assert_test("empty program = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Multiple Errors ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, ast_build_identifier("a"));
        ast_node_list_add(stmts, ast_build_identifier("b"));
        ast_node_list_add(stmts, ast_build_identifier("c"));
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("3 undefined refs = 3 errors", errors == 3);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Nested Binary Expressions ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* inner = ast_build_binary(OP_ADD, ast_build_literal_int(2), ast_build_literal_int(3));
        ASTNode* outer = ast_build_binary(OP_MUL, ast_build_literal_int(1), inner);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, outer);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("(1+(2+3)) = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Constant Folding Bitwise ===\n"); fflush(stdout);
    {
        ASTNode* and = ast_build_binary(OP_BITWISE_AND, ast_build_literal_int(0xFF), ast_build_literal_int(0x0F));
        ASTNode* folded = constant_fold(and);
        assert_test("fold 0xFF & 0x0F", folded->as.literal.as.int_value == 0x0F);

        ASTNode* bor = ast_build_binary(OP_BITWISE_OR, ast_build_literal_int(0xF0), ast_build_literal_int(0x0F));
        folded = constant_fold(bor);
        assert_test("fold 0xF0 | 0x0F", folded->as.literal.as.int_value == 0xFF);

        ASTNode* shl = ast_build_binary(OP_SHIFT_LEFT, ast_build_literal_int(5), ast_build_literal_int(2));
        folded = constant_fold(shl);
        assert_test("fold 5 << 2", folded->as.literal.as.int_value == 20);

        ASTNode* shr = ast_build_binary(OP_SHIFT_RIGHT, ast_build_literal_int(20), ast_build_literal_int(2));
        folded = constant_fold(shr);
        assert_test("fold 20 >> 2", folded->as.literal.as.int_value == 5);
    }

    printf("\n=== Constant Folding Float Div By Zero ===\n"); fflush(stdout);
    {
        ASTNode* div = ast_build_binary(OP_DIV, ast_build_literal_float(5.0), ast_build_literal_float(0.0));
        ASTNode* folded = constant_fold(div);
        assert_test("float div by zero not folded", folded == div);
    }

    printf("\n=== Constant Folding Bitwise Not ===\n"); fflush(stdout);
    {
        ASTNode* bnot = ast_build_unary(UNARY_BITWISE_NOT, ast_build_literal_int(0xFF));
        ASTNode* folded = constant_fold(bnot);
        assert_test("fold ~0xFF", folded != NULL && folded->as.literal.as.int_value == ~0xFF);
    }

    printf("\n=== Function With Local Vars ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNodeList* body_stmts = make_list();
        ast_node_list_add(body_stmts, ast_build_variable_decl("result", ast_build_identifier("رقم"), ast_build_literal_int(42)));
        ast_node_list_add(body_stmts, ast_build_return(ast_build_identifier("result")));
        ASTNode* body = ast_build_block(body_stmts);
        ASTNode* func = ast_build_function_decl("get_value", make_list(), ast_build_identifier("رقم"), body);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, func);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("function with local var = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Bool Is Void Helpers ===\n"); fflush(stdout);
    {
        DaadType* bool_t = daad_type_create(TYPE_BOOL, "bool", 1);
        DaadType* void_t = daad_type_create(TYPE_VOID, "void", 0);
        assert_test("bool not numeric", !daad_type_is_numeric(bool_t));
        assert_test("bool not pointer", !daad_type_is_pointer(bool_t));
        assert_test("bool not void", !daad_type_is_void(bool_t));
        assert_test("void not numeric", !daad_type_is_numeric(void_t));
        assert_test("void not pointer", !daad_type_is_pointer(void_t));
        assert_test("void is void", daad_type_is_void(void_t));
        daad_type_destroy(bool_t);
        daad_type_destroy(void_t);
    }

    printf("\n=== Type Compatibility Extended ===\n"); fflush(stdout);
    {
        DaadType* int_t = daad_type_create(TYPE_INT, "int", 8);
        DaadType* float_t = daad_type_create(TYPE_FLOAT, "float", 8);
        DaadType* char_t = daad_type_create(TYPE_CHAR, "char", 1);
        DaadType* bool_t = daad_type_create(TYPE_BOOL, "bool", 1);
        assert_test("float compatible bool", daad_type_is_compatible(float_t, bool_t));
        assert_test("bool not compatible char", !daad_type_is_compatible(bool_t, char_t));
        assert_test("int not compatible NULL", !daad_type_is_compatible(int_t, NULL));
        assert_test("NULL not compatible int", !daad_type_is_compatible(NULL, int_t));
        assert_test("NULL not compatible NULL", !daad_type_is_compatible(NULL, NULL));
        daad_type_destroy(int_t);
        daad_type_destroy(float_t);
        daad_type_destroy(char_t);
        daad_type_destroy(bool_t);
    }

    printf("\n=== Type Equality Extended ===\n"); fflush(stdout);
    {
        DaadType* int_t = daad_type_create(TYPE_INT, "int", 8);
        assert_test("NULL != int", !daad_type_equals(NULL, int_t));
        assert_test("int != NULL", !daad_type_equals(int_t, NULL));
        assert_test("NULL == NULL", !daad_type_equals(NULL, NULL));
        daad_type_destroy(int_t);
    }

    printf("\n=== Struct Equality With Names ===\n"); fflush(stdout);
    {
        char** names1 = (char**)malloc(2 * sizeof(char*));
        names1[0] = strdup("x");
        names1[1] = strdup("y");
        DaadType** types1 = (DaadType**)malloc(2 * sizeof(DaadType*));
        types1[0] = daad_type_create(TYPE_INT, "int", 8);
        types1[1] = daad_type_create(TYPE_INT, "int", 8);
        DaadType* s1 = daad_type_create_struct(names1, types1, 2);

        char** names2 = (char**)malloc(2 * sizeof(char*));
        names2[0] = strdup("x");
        names2[1] = strdup("y");
        DaadType** types2 = (DaadType**)malloc(2 * sizeof(DaadType*));
        types2[0] = daad_type_create(TYPE_INT, "int", 8);
        types2[1] = daad_type_create(TYPE_INT, "int", 8);
        DaadType* s2 = daad_type_create_struct(names2, types2, 2);

        char** names3 = (char**)malloc(2 * sizeof(char*));
        names3[0] = strdup("a");
        names3[1] = strdup("b");
        DaadType** types3 = (DaadType**)malloc(2 * sizeof(DaadType*));
        types3[0] = daad_type_create(TYPE_INT, "int", 8);
        types3[1] = daad_type_create(TYPE_INT, "int", 8);
        DaadType* s3 = daad_type_create_struct(names3, types3, 2);

        assert_test("struct with same names = equal", daad_type_equals(s1, s2));
        assert_test("struct with different names != equal", !daad_type_equals(s1, s3));
        daad_type_destroy(s1);
        daad_type_destroy(s2);
        daad_type_destroy(s3);
    }

    printf("\n=== Type Checker Numeric Promotion ===\n"); fflush(stdout);
    {
        TypeRegistry* reg = type_registry_create();
        SemanticErrorList* errors = semantic_error_list_create();
        TypeChecker* checker = type_checker_create(reg, errors);
        DaadType* int_t = type_registry_get_int(reg);
        DaadType* float_t = type_registry_get_float(reg);
        DaadType* char_t = type_registry_get_char(reg);

        DaadType* result = type_checker_get_promoted_type(checker, int_t, int_t);
        assert_test("int + int = int", result == int_t);
        result = type_checker_get_promoted_type(checker, int_t, float_t);
        assert_test("int + float = float", result == float_t);
        result = type_checker_get_promoted_type(checker, float_t, int_t);
        assert_test("float + int = float", result == float_t);
        result = type_checker_get_promoted_type(checker, char_t, int_t);
        assert_test("char + int = int", result == int_t);
        type_checker_destroy(checker);
        semantic_error_list_destroy(errors);
        type_registry_destroy(reg);
    }

    printf("\n=== Type Checker Call ===\n"); fflush(stdout);
    {
        TypeRegistry* reg = type_registry_create();
        SemanticErrorList* errors = semantic_error_list_create();
        TypeChecker* checker = type_checker_create(reg, errors);

        DaadType** params = (DaadType**)malloc(2 * sizeof(DaadType*));
        params[0] = daad_type_create(TYPE_INT, "int", 8);
        params[1] = daad_type_create(TYPE_FLOAT, "float", 8);
        DaadType* func_type = daad_type_create_function(params, 2, daad_type_create(TYPE_INT, "int", 8));

        DaadType** arg_types = (DaadType**)malloc(2 * sizeof(DaadType*));
        arg_types[0] = type_registry_get_int(reg);
        arg_types[1] = type_registry_get_float(reg);
        int ok = type_checker_check_call(checker, func_type, arg_types, 2, 1, 1, "test.daad");
        assert_test("call with correct args = ok", ok);

        arg_types[0] = type_registry_get_int(reg);
        arg_types[1] = type_registry_get_int(reg);
        ok = type_checker_check_call(checker, func_type, arg_types, 2, 1, 1, "test.daad");
        assert_test("call with int,int = ok (compatible)", ok);

        ok = type_checker_check_call(checker, func_type, arg_types, 1, 1, 1, "test.daad");
        assert_test("call with wrong count = error", !ok);

        free(arg_types);
        daad_type_destroy(func_type);
        type_checker_destroy(checker);
        semantic_error_list_destroy(errors);
        type_registry_destroy(reg);
    }

    printf("\n=== Type Checker Call Non-Callable ===\n"); fflush(stdout);
    {
        TypeRegistry* reg = type_registry_create();
        SemanticErrorList* errors = semantic_error_list_create();
        TypeChecker* checker = type_checker_create(reg, errors);
        DaadType* int_t = type_registry_get_int(reg);

        int ok = type_checker_check_call(checker, int_t, NULL, 0, 1, 1, "test.daad");
        assert_test("call on non-callable = error", !ok);
        type_checker_destroy(checker);
        semantic_error_list_destroy(errors);
        type_registry_destroy(reg);
    }

    printf("\n=== Visitor Function Call Type Check ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* param = ast_build_parameter("a", ast_build_identifier("رقم"));
        ASTNodeList* params = make_list();
        ast_node_list_add(params, param);
        ASTNodeList* body_stmts = make_list();
        ast_node_list_add(body_stmts, ast_build_return(ast_build_literal_int(10)));
        ASTNode* body = ast_build_block(body_stmts);
        ASTNode* func = ast_build_function_decl("add", params, ast_build_identifier("رقم"), body);

        ASTNodeList* call_args = make_list();
        ast_node_list_add(call_args, ast_build_literal_int(5));
        ASTNode* call = ast_build_call(ast_build_identifier("add"), call_args);

        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, func);
        ast_node_list_add(stmts, call);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("function call with correct args = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Visitor Function Call Wrong Args ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* param = ast_build_parameter("a", ast_build_identifier("رقم"));
        ASTNode* param2 = ast_build_parameter("b", ast_build_identifier("رقم"));
        ASTNodeList* params = make_list();
        ast_node_list_add(params, param);
        ast_node_list_add(params, param2);
        ASTNodeList* body_stmts = make_list();
        ast_node_list_add(body_stmts, ast_build_return(ast_build_literal_int(10)));
        ASTNode* body = ast_build_block(body_stmts);
        ASTNode* func = ast_build_function_decl("add2", params, ast_build_identifier("رقم"), body);

        ASTNodeList* call_args = make_list();
        ast_node_list_add(call_args, ast_build_literal_int(5));
        ASTNode* call = ast_build_call(ast_build_identifier("add2"), call_args);

        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, func);
        ast_node_list_add(stmts, call);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("function call with wrong arg count = error", errors > 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Constant Folding No Memory Leak ===\n"); fflush(stdout);
    {
        ASTNode* left = ast_build_literal_int(3);
        ASTNode* right = ast_build_literal_int(5);
        ASTNode* add = ast_build_binary(OP_ADD, left, right);
        ASTNode* folded = constant_fold(add);
        assert_test("fold 3+5 = 8", folded != NULL && folded->type == NODE_LITERAL && folded->as.literal.as.int_value == 8);
        assert_test("folded is not original", folded != add);
    }

    printf("\n=== Break Inside While ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* cond = ast_build_literal_bool(1);
        ASTNode* brk = ast_build_break();
        ASTNodeList* body_stmts = make_list();
        ast_node_list_add(body_stmts, brk);
        ASTNode* body = ast_build_block(body_stmts);
        ASTNode* while_stmt = ast_build_while(cond, body);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, while_stmt);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("break inside while = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Break Outside Loop ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* brk = ast_build_break();
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, brk);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("break outside loop = error", errors > 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Continue Inside For ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* init = ast_build_variable_decl("i", ast_build_identifier("رقم"), ast_build_literal_int(0));
        ASTNode* cond = ast_build_binary(OP_LESS, ast_build_identifier("i"), ast_build_literal_int(10));
        ASTNode* update = ast_build_assignment("i", ASSIGN_ADD, ast_build_literal_int(1));
        ASTNodeList* body_stmts = make_list();
        ast_node_list_add(body_stmts, ast_build_continue());
        ASTNode* body = ast_build_block(body_stmts);
        ASTNode* for_stmt = ast_build_for(init, cond, update, body);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, for_stmt);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("continue inside for = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Continue Outside Loop ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* cont = ast_build_continue();
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, cont);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("continue outside loop = error", errors > 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Break Inside Nested Loop ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* inner_cond = ast_build_literal_bool(1);
        ASTNode* brk = ast_build_break();
        ASTNodeList* inner_stmts = make_list();
        ast_node_list_add(inner_stmts, brk);
        ASTNode* inner_body = ast_build_block(inner_stmts);
        ASTNode* inner_while = ast_build_while(inner_cond, inner_body);
        ASTNodeList* outer_stmts = make_list();
        ast_node_list_add(outer_stmts, inner_while);
        ASTNode* outer_body = ast_build_block(outer_stmts);
        ASTNode* outer_while = ast_build_while(ast_build_literal_bool(1), outer_body);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, outer_while);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("break inside nested loop = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Continue Inside Nested Loop ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* inner_cond = ast_build_literal_bool(1);
        ASTNode* cont = ast_build_continue();
        ASTNodeList* inner_stmts = make_list();
        ast_node_list_add(inner_stmts, cont);
        ASTNode* inner_body = ast_build_block(inner_stmts);
        ASTNode* inner_while = ast_build_while(inner_cond, inner_body);
        ASTNodeList* outer_stmts = make_list();
        ast_node_list_add(outer_stmts, inner_while);
        ASTNode* outer_body = ast_build_block(outer_stmts);
        ASTNode* outer_while = ast_build_while(ast_build_literal_bool(1), outer_body);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, outer_while);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("continue inside nested loop = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Constant Type Float ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* cdecl = ast_build_constant_decl("PI", ast_build_literal_float(3.14));
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, cdecl);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("const PI = 3.14 = 0 errors", errors == 0);
        Symbol* sym = scope_stack_lookup(ctx->scopes, "PI");
        assert_test("PI type is float", sym != NULL && sym->type != NULL && sym->type->kind == TYPE_FLOAT);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Constant Type Int ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* cdecl = ast_build_constant_decl("X", ast_build_literal_int(42));
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, cdecl);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("const X = 42 = 0 errors", errors == 0);
        Symbol* sym = scope_stack_lookup(ctx->scopes, "X");
        assert_test("X type is int", sym != NULL && sym->type != NULL && sym->type->kind == TYPE_INT);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Constant Type Bool ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* cdecl = ast_build_constant_decl("B", ast_build_literal_bool(1));
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, cdecl);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("const B = true = 0 errors", errors == 0);
        Symbol* sym = scope_stack_lookup(ctx->scopes, "B");
        assert_test("B type is bool", sym != NULL && sym->type != NULL && sym->type->kind == TYPE_BOOL);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Constant Folding Integration ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* cdecl = ast_build_constant_decl("X", ast_build_binary(OP_ADD, ast_build_literal_int(3), ast_build_literal_int(5)));
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, cdecl);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("const X = 3+5 = 0 errors", errors == 0);
        assert_test("value folded to literal", cdecl->as.const_decl.value->type == NODE_LITERAL && cdecl->as.const_decl.value->as.literal.as.int_value == 8);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Parameter Visitor ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNode* param = ast_build_parameter("a", ast_build_identifier("رقم"));
        ASTNodeList* params = make_list();
        ast_node_list_add(params, param);
        ASTNodeList* body_stmts = make_list();
        ast_node_list_add(body_stmts, ast_build_return(ast_build_identifier("a")));
        ASTNode* body = ast_build_block(body_stmts);
        ASTNode* func = ast_build_function_decl("getId", params, ast_build_identifier("رقم"), body);
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, func);
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("function with parameter = 0 errors", errors == 0);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n=== Multiple Semantic Errors Collected ===\n"); fflush(stdout);
    {
        SemanticContext* ctx = semantic_context_create();
        ASTNodeList* stmts = make_list();
        ast_node_list_add(stmts, ast_build_identifier("a"));
        ast_node_list_add(stmts, ast_build_identifier("b"));
        ast_node_list_add(stmts, ast_build_break());
        ASTNode* program = make_program_with_stmts(stmts);
        int errors = semantic_analyze(program, ctx);
        assert_test("2 undefined + 1 break outside = 3 errors", errors == 3);
        ast_node_destroy(program);
        semantic_context_destroy(ctx);
    }

    printf("\n========================================\n");
    printf("  Results: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("========================================\n");
    fflush(stdout);

    return tests_failed > 0 ? 1 : 0;
}