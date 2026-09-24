/**
 * @file constant_folding.h
 * @brief تقليص الثوابت (Constant Folding)
 */

#ifndef DAAD_CONSTANT_FOLDING_H
#define DAAD_CONSTANT_FOLDING_H

#include "../ast/ast_node.h"

ASTNode* constant_fold(ASTNode* node);
int constant_folding_is_foldable(ASTNode* node);

#endif