/**
 * @file cfg.h
 * @brief Control Flow Graph - محدّث مع دعم Post-dominators و Dominance Frontier
 */

#ifndef DAAD_CFG_H
#define DAAD_CFG_H

#include "../ir/ir_basic_block.h"
#include "../ir/ir_function.h"

#define MAX_DOMINANCE_FRONTIER 64

typedef struct {
    IRBasicBlock** blocks;
    int block_count;
    IRBasicBlock* entry;
    int* dominators;
    int* dom_tree_parent;
    int dom_count;
    int* idom;
    int* post_dominators;
    int* post_dom_tree_parent;
    int post_dom_count;
    int** dominance_frontiers;
    int* frontier_counts;
    int* reverse_post_order;
    int reverse_post_order_count;
} CFG;

CFG* cfg_build(IRFunction* func);
void cfg_destroy(CFG* cfg);
void cfg_compute_dominators(CFG* cfg);
void cfg_compute_post_dominators(CFG* cfg);
void cfg_compute_dominance_frontiers(CFG* cfg);
void cfg_compute_reverse_post_order(CFG* cfg);
int cfg_dominates(CFG* cfg, int a, int b);
int cfg_post_dominates(CFG* cfg, int a, int b);
void cfg_add_edge(CFG* cfg, IRBasicBlock* from, IRBasicBlock* to);
int* cfg_get_dominance_frontier(CFG* cfg, int block_index, int* count);
int cfg_get_idom(CFG* cfg, int block_index);
int cfg_get_reverse_post_order(CFG* cfg, int* order, int max_count);

#endif
