/**
 * @file semantic_visitor.h
 * @brief زائر التحليل الدلالي
 */

#ifndef DAAD_SEMANTIC_VISITOR_H
#define DAAD_SEMANTIC_VISITOR_H

#include "../ast/ast_node.h"
#include "../ast/ast_visitor.h"
#include "scope_stack.h"
#include "type_registry.h"
#include "type_checker.h"
#include "semantic_error.h"

typedef struct {
    ScopeStack* scopes;
    TypeRegistry* type_registry;
    TypeChecker* type_checker;
    SemanticErrorList* errors;
    int in_loop;
    DaadType* current_return_type;
} SemanticContext;

SemanticContext* semantic_context_create(void);
void semantic_context_destroy(SemanticContext* ctx);
ASTVisitor semantic_create_visitor(void);
int semantic_analyze(ASTNode* root, SemanticContext* ctx);

#endif