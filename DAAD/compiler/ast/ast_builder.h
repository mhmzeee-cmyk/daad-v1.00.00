/**
 * @file ast_builder.h
 * @brief واجهة بناء عقد AST في لغة ض Core
 * 
 * هذا الملف يحتوي على دوال بناء العقد المختلفة لـ AST.
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#ifndef DAAD_AST_BUILDER_H
#define DAAD_AST_BUILDER_H

#include "ast_node.h"

/**
 * @brief بناء عقدة البرنامج (الجذر)
 * @param declarations قائمة التعريفات العليا
 * @return عقدة البرنامج
 */
ASTNode* ast_build_program(ASTNodeList* declarations);

/**
 * @brief بناء عقدة الكتلة
 * @param statements قائمة العبارات
 * @return عقدة الكتلة
 */
ASTNode* ast_build_block(ASTNodeList* statements);

/**
 * @brief بناء تعريف متغير
 * @param name اسم المتغير
 * @param type_node عقدة النوع (NULL إذا لم يُحدد)
 * @param init_expr عقدة التعبير الابتدائي (NULL إذا لم يكن هناك تهيئة)
 * @return عقدة تعريف المتغير
 */
ASTNode* ast_build_variable_decl(const char* name, ASTNode* type_node, ASTNode* init_expr);

/**
 * @brief بناء تعريف ثابت
 * @param name اسم الثابت
 * @param value عقدة القيمة
 * @return عقدة تعريف الثابت
 */
ASTNode* ast_build_constant_decl(const char* name, ASTNode* value);

/**
 * @brief بناء تعريف دالة
 * @param name اسم الدالة
 * @param params قائمة المعاملات
 * @param return_type عقدة نوع الإرجاع (NULL للـ void)
 * @param body عقدة جسم الدالة
 * @return عقدة تعريف الدالة
 */
ASTNode* ast_build_function_decl(const char* name, ASTNodeList* params, ASTNode* return_type, ASTNode* body);

/**
 * @brief بناء معامل دالة
 * @param name اسم المعامل
 * @param type_node عقدة نوع المعامل
 * @return عقدة المعامل
 */
ASTNode* ast_build_parameter(const char* name, ASTNode* type_node);

/**
 * @brief بناء عبارة الإرجاع
 * @param value قيمة الإرجاع (NULL لـ return بدون قيمة)
 * @return عقدة الإرجاع
 */
ASTNode* ast_build_return(ASTNode* value);

/**
 * @brief بناء عبارة إذا
 * @param condition عقدة الشرط
 * @param then_block عقدة كتلة then
 * @param else_block عقدة كتلة else (NULL إذا لا يوجد)
 * @return عقدة إذا
 */
ASTNode* ast_build_if(ASTNode* condition, ASTNode* then_block, ASTNode* else_block);

/**
 * @brief بناء عبارة طالما
 * @param condition عقدة الشرط
 * @param body عقدة الجسم
 * @return عقدة طالما
 */
ASTNode* ast_build_while(ASTNode* condition, ASTNode* body);

/**
 * @brief بناء عبارة لكل
 * @param init عقدة التهيئة (NULL)
 * @param condition عقدة الشرط (NULL)
 * @param update عقدة التحديث (NULL)
 * @param body عقدة الجسم
 * @return عقدة لكل
 */
ASTNode* ast_build_for(ASTNode* init, ASTNode* condition, ASTNode* update, ASTNode* body);

/**
 * @brief بناء عبارة توقف (break)
 * @return عقدة توقف
 */
ASTNode* ast_build_break(void);

/**
 * @brief بناء عبارة أكمل (continue)
 * @return عقدة أكمل
 */
ASTNode* ast_build_continue(void);

/**
 * @brief بناء عبارة الإسناد
 * @param name اسم المتغير
 * @param op عملية الإسناد
 * @param value عقدة القيمة
 * @return عقدة الإسناد
 */
ASTNode* ast_build_assignment(const char* name, AssignmentOperator op, ASTNode* value);

/**
 * @brief بناء عبارة إسناد مع هدف معقد (فهرسة/عضو)
 * @param target الهدف (NODE_IDENTIFIER, NODE_INDEX_EXPRESSION, NODE_MEMBER_EXPRESSION)
 * @param op العملية
 * @param value عقدة القيمة
 * @return عقدة الإسناد
 */
ASTNode* ast_build_assignment_target(ASTNode* target, AssignmentOperator op, ASTNode* value);

/**
 * @brief بناء عبارة ثنائية
 * @param op العملية الثنائية
 * @param left العقدة اليسرى
 * @param right العقدة اليمنى
 * @return عقدة ثنائية
 */
ASTNode* ast_build_binary(BinaryOperator op, ASTNode* left, ASTNode* right);

/**
 * @brief بناء عبارة أحادية
 * @param op العملية الأحادية
 * @param operand العقدة
 * @return عقدة أحادية
 */
ASTNode* ast_build_unary(UnaryOperator op, ASTNode* operand);

/**
 * @brief بناء عقدة عدد صحيح
 * @param value القيمة
 * @return عقدة Liter
 */
ASTNode* ast_build_literal_int(long long value);

/**
 * @brief بناء عقدة عدد عشري
 * @param value القيمة
 * @return عقدة Literal
 */
ASTNode* ast_build_literal_float(double value);

/**
 * @brief بناء عقدة نص
 * @param value النص (يتم نسخه)
 * @return عقدة Literal
 */
ASTNode* ast_build_literal_string(const char* value);

/**
 * @brief بناء عقدة حرف
 * @param value الحرف
 * @return عقدة Literal
 */
ASTNode* ast_build_literal_char(char value);

/**
 * @brief بناء عقدة منطقية
 * @param value 1 لصحيح، 0 لخطأ
 * @return عقدة Literal
 */
ASTNode* ast_build_literal_bool(int value);

/**
 * @brief بناء عقدة معرف
 * @param name اسم المعرف (يتم نسخه)
 * @return عقدة Identifier
 */
ASTNode* ast_build_identifier(const char* name);

/**
 * @brief بناء عقدة استدعاء دالة
 * @param callee عقدة الدالة
 * @param args قائمة المعاملات
 * @return عقدة Call
 */
ASTNode* ast_build_call(ASTNode* callee, ASTNodeList* args);

/**
 * @brief بناء عقدة مصفوفة
 * @param elements قائمة العناصر
 * @return عقدة Array
 */
ASTNode* ast_build_array(ASTNodeList* elements);

/**
 * @brief بناء عقدة هيكل
 * @param names قائمة أسماء الحقول
 * @param values قائمة قيم الحقول
 * @return عقدة Struct
 */
ASTNode* ast_build_struct(ASTNodeList* names, ASTNodeList* values);

/**
 * @brief بناء عقدة مؤشر نوع
 * @param type_node عقدة النوع
 * @return عقدة Pointer
 */
ASTNode* ast_build_pointer(ASTNode* type_node);

/**
 * @brief بناء عقدة الوصول لعضو
 * @param object عقدة الكائن
 * @param member اسم العضو (يتم نسخه)
 * @return عقدة Member
 */
ASTNode* ast_build_member(ASTNode* object, const char* member);

/**
 * @brief بناء عقدة فهرسة مصفوفة
 * @param array عقدة المصفوفة
 * @param index عقدة الفهرس
 * @return عقدة Index
 */
ASTNode* ast_build_index(ASTNode* array, ASTNode* index);

/**
 * @brief بناء عقدة تحويل نوع
 * @param type_node عقدة النوع المراد التحويل إليه
 * @param expr عقدة التعبير
 * @return عقدة Cast
 */
ASTNode* ast_build_cast(ASTNode* type_node, ASTNode* expr);

#endif /* DAAD_AST_BUILDER_H */
