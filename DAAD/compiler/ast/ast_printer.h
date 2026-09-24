/**
 * @file ast_printer.h
 * @brief واجهة طباعة AST في لغة ض Core
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#ifndef DAAD_AST_PRINTER_H
#define DAAD_AST_PRINTER_H

#include "ast_node.h"
#include <stdio.h>

/**
 * @brief طباعة AST إلى stdout
 * @param node العقدة الرئيسية
 */
void ast_print(ASTNode* node);

/**
 * @brief طباعة AST إلى ملف
 * @param node العقدة الرئيسية
 * @param file ملف الإخراج
 */
void ast_print_to_file(ASTNode* node, FILE* file);

#endif /* DAAD_AST_PRINTER_H */
