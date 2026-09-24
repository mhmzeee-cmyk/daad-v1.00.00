/**
 * @file ast.h
 * @brief واجهة AST في لغة ض Core
 * 
 * هذا الملف يحتوي على الدوال لإنشاء وتدمير وإدارة عقد AST.
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#ifndef DAAD_AST_H
#define DAAD_AST_H

#include "ast_node.h"
#include <stdio.h>

/**
 * @brief إنشاء عقدة AST جديدة
 * @param type نوع العقدة
 * @return مؤشر إلى العقدة الجديدة
 */
ASTNode* ast_node_create(NodeType type);

/**
 * @brief تحرير ذاكرة عقدة AST وجميع أبنائها
 * @param node مؤشر إلى العقدة
 */
void ast_node_destroy(ASTNode* node);

/**
 * @brief تهيئة قائمة عقد فارغة
 * @param list مؤشر إلى القائمة
 */
void ast_node_list_init(ASTNodeList* list);

/**
 * @brief إضافة عقدة إلى القائمة
 * @param list مؤشر إلى القائمة
 * @param node العقدة المراد إضافتها (تتم ملكيتها من القائمة)
 */
void ast_node_list_add(ASTNodeList* list, ASTNode* node);

/**
 * @brief تحرير ذاكرة القائمة وجميع عقدها
 * @param list مؤشر إلى القائمة
 */
void ast_node_list_destroy(ASTNodeList* list);

/**
 * @brief تعيين موقع العقدة
 * @param node العقدة
 * @param line رقم السطر
 * @param column رقم العمود
 * @param offset الإزاحة
 */
void ast_node_set_position(ASTNode* node, size_t line, size_t column, size_t offset);

/**
 * @brief تعيين اسم الملف للعقدة
 * @param node العقدة
 * @param filename اسم الملف (يتم نسخه)
 */
void ast_node_set_filename(ASTNode* node, const char* filename);

/**
 * @brief تعيين موقع البداية والنهاية من Token
 * @param node العقدة
 * @param start الموقع البداية
 * @param end الموقع النهاية
 */
void ast_node_set_positions(ASTNode* node, Position start, Position end);

#endif /* DAAD_AST_H */
