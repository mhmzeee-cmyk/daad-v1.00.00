/**
 * @file ast_visitor.h
 * @brief واجهة الزيارة في AST في لغة ض Core
 * 
 * هذا الملف يحتوي على هيكل الزائر ودوال الزيارة لـ AST.
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#ifndef DAAD_AST_VISITOR_H
#define DAAD_AST_VISITOR_H

#include "ast_node.h"

/**
 * @brief هيكل الزائر
 * 
 * يحتوي على مؤشرات دوال لكل نوع من أنواع العقد.
 * يمكن تعيين أي مؤشر إلى NULL لتجاهل ذلك النوع.
 */
typedef struct {
    void (*visit_program)(ASTNode* node, void* data);
    void (*visit_block)(ASTNode* node, void* data);
    void (*visit_variable_decl)(ASTNode* node, void* data);
    void (*visit_constant_decl)(ASTNode* node, void* data);
    void (*visit_function_decl)(ASTNode* node, void* data);
    void (*visit_parameter)(ASTNode* node, void* data);
    void (*visit_return_statement)(ASTNode* node, void* data);
    void (*visit_if_statement)(ASTNode* node, void* data);
    void (*visit_while_statement)(ASTNode* node, void* data);
    void (*visit_for_statement)(ASTNode* node, void* data);
    void (*visit_break_statement)(ASTNode* node, void* data);
    void (*visit_continue_statement)(ASTNode* node, void* data);
    void (*visit_assignment_expression)(ASTNode* node, void* data);
    void (*visit_binary_expression)(ASTNode* node, void* data);
    void (*visit_unary_expression)(ASTNode* node, void* data);
    void (*visit_literal)(ASTNode* node, void* data);
    void (*visit_identifier)(ASTNode* node, void* data);
    void (*visit_call_expression)(ASTNode* node, void* data);
    void (*visit_array_expression)(ASTNode* node, void* data);
    void (*visit_struct_expression)(ASTNode* node, void* data);
    void (*visit_pointer_expression)(ASTNode* node, void* data);
    void (*visit_member_expression)(ASTNode* node, void* data);
    void (*visit_index_expression)(ASTNode* node, void* data);
    void (*visit_cast_expression)(ASTNode* node, void* data);
    void (*visit_try_statement)(ASTNode* node, void* data);
    void (*visit_catch_statement)(ASTNode* node, void* data);
    void (*visit_throw_statement)(ASTNode* node, void* data);
} ASTVisitor;

/**
 * @brief إنشاء زائر مع جميع المؤشرات NULL
 * @return زائر فارغ
 */
ASTVisitor ast_visitor_create(void);

/**
 * @brief زيارة عقدة AST
 * 
 * تقوم بإعادة توجيه الدالة المناسبة حسب نوع العقدة.
 * إذا كان المؤشر NULL، يتم تجاهل العقدة.
 * 
 * @param node العقدة المراد زيارتها
 * @param visitor الزائر
 * @param data بيانات إضافية تمرر للدوال
 */
void ast_visit(ASTNode* node, ASTVisitor* visitor, void* data);

#endif /* DAAD_AST_VISITOR_H */
