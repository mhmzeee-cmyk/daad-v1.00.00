/**
 * @file cfg.c
 * @brief تنفيذ Control Flow Graph - محدّث مع دعم Post-dominators و Dominance Frontier
 */

#include "cfg.h"
#include <stdlib.h>
#include <string.h>

CFG* cfg_build(IRFunction* func) {
    if (!func) return NULL;
    CFG* cfg = (CFG*)calloc(1, sizeof(CFG));
    if (!cfg) return NULL;
    cfg->block_count = func->block_count;
    cfg->blocks = (IRBasicBlock**)malloc(cfg->block_count * sizeof(IRBasicBlock*));
    if (!cfg->blocks) { free(cfg); return NULL; }
    for (int i = 0; i < cfg->block_count; i++) {
        cfg->blocks[i] = func->blocks[i];
    }
    cfg->entry = func->entry;
    cfg->dominators = (int*)calloc(cfg->block_count, sizeof(int));
    cfg->dom_tree_parent = (int*)calloc(cfg->block_count, sizeof(int));
    cfg->dom_count = cfg->block_count;
    cfg->idom = (int*)calloc(cfg->block_count, sizeof(int));
    cfg->post_dominators = (int*)calloc(cfg->block_count, sizeof(int));
    cfg->post_dom_tree_parent = (int*)calloc(cfg->block_count, sizeof(int));
    cfg->post_dom_count = cfg->block_count;
    cfg->dominance_frontiers = (int**)calloc(cfg->block_count, sizeof(int*));
    cfg->frontier_counts = (int*)calloc(cfg->block_count, sizeof(int));
    if (!cfg->dominators || !cfg->dom_tree_parent || !cfg->idom ||
        !cfg->post_dominators || !cfg->post_dom_tree_parent ||
        !cfg->dominance_frontiers || !cfg->frontier_counts) {
        cfg_destroy(cfg);
        return NULL;
    }
    for (int i = 0; i < cfg->block_count; i++) {
        cfg->dominance_frontiers[i] = (int*)calloc(MAX_DOMINANCE_FRONTIER, sizeof(int));
        if (!cfg->dominance_frontiers[i]) {
            cfg_destroy(cfg);
            return NULL;
        }
    }
    cfg->reverse_post_order = (int*)calloc(cfg->block_count, sizeof(int));
    if (!cfg->reverse_post_order) {
        cfg_destroy(cfg);
        return NULL;
    }
    cfg->reverse_post_order_count = 0;
    return cfg;
}

void cfg_destroy(CFG* cfg) {
    if (!cfg) return;
    free(cfg->blocks);
    free(cfg->dominators);
    free(cfg->dom_tree_parent);
    free(cfg->idom);
    free(cfg->post_dominators);
    free(cfg->post_dom_tree_parent);
    if (cfg->dominance_frontiers) {
        for (int i = 0; i < cfg->block_count; i++) {
            free(cfg->dominance_frontiers[i]);
        }
        free(cfg->dominance_frontiers);
    }
    free(cfg->frontier_counts);
    free(cfg->reverse_post_order);
    free(cfg);
}

void cfg_compute_dominators(CFG* cfg) {
    if (!cfg) return;
    for (int i = 0; i < cfg->block_count; i++) {
        cfg->dominators[i] = -1;
        cfg->dom_tree_parent[i] = -1;
        cfg->idom[i] = -1;
    }
    if (cfg->block_count == 0) return;
    cfg->dominators[0] = 0;
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 1; i < cfg->block_count; i++) {
            int new_idom = -1;
            IRBasicBlock* bb = cfg->blocks[i];
            for (int p = 0; p < bb->predecessor_count; p++) {
                for (int j = 0; j < cfg->block_count; j++) {
                    if (cfg->blocks[j] == bb->predecessors[p]) {
                        if (cfg->dominators[j] != -1) {
                            if (new_idom == -1) new_idom = j;
                            else {
                                int a = new_idom, b = j;
                                while (a != b) {
                                    while (a > b) a = cfg->dom_tree_parent[a] >= 0 ? cfg->dom_tree_parent[a] : a;
                                    while (b > a) b = cfg->dom_tree_parent[b] >= 0 ? cfg->dom_tree_parent[b] : b;
                                }
                                new_idom = a;
                            }
                        }
                        break;
                    }
                }
            }
            if (new_idom != -1 && cfg->dominators[i] != new_idom) {
                cfg->dominators[i] = new_idom;
                cfg->dom_tree_parent[i] = new_idom;
                cfg->idom[i] = new_idom;
                changed = 1;
            }
        }
    }
}

void cfg_compute_post_dominators(CFG* cfg) {
    if (!cfg) return;
    for (int i = 0; i < cfg->block_count; i++) {
        cfg->post_dominators[i] = -1;
        cfg->post_dom_tree_parent[i] = -1;
    }
    if (cfg->block_count == 0) return;
    int last = cfg->block_count - 1;
    cfg->post_dominators[last] = last;
    cfg->post_dom_tree_parent[last] = -1;
    int changed = 1;
    int max_iter = cfg->block_count * cfg->block_count + 1;
    while (changed && max_iter-- > 0) {
        changed = 0;
        for (int i = cfg->block_count - 2; i >= 0; i--) {
            IRBasicBlock* bb = cfg->blocks[i];
            if (bb->successor_count == 0) {
                if (cfg->post_dominators[i] != last) {
                    cfg->post_dominators[i] = last;
                    cfg->post_dom_tree_parent[i] = last;
                    changed = 1;
                }
                continue;
            }
            int new_idom = -1;
            for (int s = 0; s < bb->successor_count; s++) {
                for (int j = 0; j < cfg->block_count; j++) {
                    if (cfg->blocks[j] == bb->successors[s]) {
                        if (cfg->post_dominators[j] != -1) {
                            if (new_idom == -1) {
                                new_idom = cfg->post_dominators[j];
                            } else {
                                int a = new_idom, b = cfg->post_dominators[j];
                                while (a != b) {
                                    if (a < b) { int t = a; a = b; b = t; }
                                    int parent_a = -1;
                                    for (int k = 0; k < cfg->block_count; k++) {
                                        if (cfg->blocks[k] == cfg->blocks[a]->successors[0]) {
                                            parent_a = cfg->post_dominators[k];
                                            break;
                                        }
                                    }
                                    if (parent_a == -1 || parent_a == a) { a = b; break; }
                                    a = parent_a;
                                }
                                new_idom = a;
                            }
                        }
                        break;
                    }
                }
            }
            if (new_idom != -1 && cfg->post_dominators[i] != new_idom) {
                cfg->post_dominators[i] = new_idom;
                cfg->post_dom_tree_parent[i] = new_idom;
                changed = 1;
            }
        }
    }
}

void cfg_compute_dominance_frontiers(CFG* cfg) {
    if (!cfg) return;
    for (int i = 0; i < cfg->block_count; i++) {
        cfg->frontier_counts[i] = 0;
    }
    for (int i = 0; i < cfg->block_count; i++) {
        IRBasicBlock* bb = cfg->blocks[i];
        if (bb->predecessor_count >= 2) {
            for (int p = 0; p < bb->predecessor_count; p++) {
                for (int j = 0; j < cfg->block_count; j++) {
                    if (cfg->blocks[j] == bb->predecessors[p]) {
                        int runner = j;
                        while (runner != -1 && runner != cfg->idom[i]) {
                            if (cfg->frontier_counts[runner] < MAX_DOMINANCE_FRONTIER) {
                                cfg->dominance_frontiers[runner][cfg->frontier_counts[runner]++] = i;
                            }
                            runner = cfg->idom[runner];
                        }
                        break;
                    }
                }
            }
        }
    }
}

typedef struct {
    CFG* cfg;
    int* visited;
    int* order;
    int order_count;
} DFSContext;

static void cfg_dfs(DFSContext* ctx, int idx) {
    if (idx < 0 || idx >= ctx->cfg->block_count || ctx->visited[idx]) return;
    ctx->visited[idx] = 1;
    IRBasicBlock* bb = ctx->cfg->blocks[idx];
    for (int s = 0; s < bb->successor_count; s++) {
        for (int j = 0; j < ctx->cfg->block_count; j++) {
            if (ctx->cfg->blocks[j] == bb->successors[s]) {
                cfg_dfs(ctx, j);
                break;
            }
        }
    }
    if (ctx->order_count < 256) {
        ctx->order[ctx->order_count++] = idx;
    }
}

void cfg_compute_reverse_post_order(CFG* cfg) {
    if (!cfg) return;
    int* visited = (int*)calloc(cfg->block_count, sizeof(int));
    int* order = (int*)calloc(cfg->block_count, sizeof(int));
    if (!visited || !order) { free(visited); free(order); return; }
    DFSContext ctx;
    ctx.cfg = cfg;
    ctx.visited = visited;
    ctx.order = order;
    ctx.order_count = 0;
    cfg_dfs(&ctx, 0);
    cfg->reverse_post_order_count = ctx.order_count;
    for (int i = 0; i < ctx.order_count; i++) {
        cfg->reverse_post_order[i] = order[ctx.order_count - 1 - i];
    }
    free(visited);
    free(order);
}

int cfg_dominates(CFG* cfg, int a, int b) {
    if (!cfg || a < 0 || b < 0) return 0;
    int cur = b;
    while (cur > a) {
        cur = cfg->dom_tree_parent[cur];
        if (cur < 0) return 0;
    }
    return cur == a;
}

int cfg_post_dominates(CFG* cfg, int a, int b) {
    if (!cfg || a < 0 || b < 0) return 0;
    int cur = b;
    while (cur != a) {
        cur = cfg->post_dom_tree_parent[cur];
        if (cur < 0) return 0;
    }
    return 1;
}

void cfg_add_edge(CFG* cfg, IRBasicBlock* from, IRBasicBlock* to) {
    if (!cfg || !from || !to) return;
    for (int i = 0; i < from->successor_count; i++) {
        if (from->successors[i] == to) return;
    }
    ir_bb_add_successor(from, to);
    ir_bb_add_predecessor(to, from);
}

int* cfg_get_dominance_frontier(CFG* cfg, int block_index, int* count) {
    if (!cfg || block_index < 0 || block_index >= cfg->block_count) {
        if (count) *count = 0;
        return NULL;
    }
    if (count) *count = cfg->frontier_counts[block_index];
    return cfg->dominance_frontiers[block_index];
}

int cfg_get_idom(CFG* cfg, int block_index) {
    if (!cfg || block_index < 0 || block_index >= cfg->block_count) return -1;
    return cfg->idom[block_index];
}

int cfg_get_reverse_post_order(CFG* cfg, int* order, int max_count) {
    if (!cfg || !order) return 0;
    int count = cfg->reverse_post_order_count;
    if (count > max_count) count = max_count;
    for (int i = 0; i < count; i++) {
        order[i] = cfg->reverse_post_order[i];
    }
    return count;
}
