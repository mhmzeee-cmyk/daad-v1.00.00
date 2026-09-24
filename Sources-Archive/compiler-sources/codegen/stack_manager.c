/**
 * @file stack_manager.c
 * @brief تنفيذ مدير المكدّس - محدّث مع Backend الموحد
 */

#include "stack_manager.h"
#include <stdlib.h>
#include <string.h>

StackManager* stack_manager_create(void) {
    StackManager* mgr = (StackManager*)calloc(1, sizeof(StackManager));
    if (!mgr) return NULL;
    mgr->capacity = 128;
    mgr->offsets = (int*)calloc(mgr->capacity, sizeof(int));
    mgr->names = (char**)calloc(mgr->capacity, sizeof(char*));
    if (!mgr->offsets || !mgr->names) {
        free(mgr->offsets);
        free(mgr->names);
        free(mgr);
        return NULL;
    }
    mgr->current_offset = 0;
    return mgr;
}

void stack_manager_destroy(StackManager* mgr) {
    if (!mgr) return;
    for (int i = 0; i < mgr->count; i++) {
        free(mgr->names[i]);
    }
    free(mgr->offsets);
    free(mgr->names);
    free(mgr);
}

int stack_manager_push(StackManager* mgr, const char* name, int size) {
    if (!mgr) return -1;
    if (mgr->count >= mgr->capacity) {
        int new_cap = mgr->capacity * 2;
        int* tmp_offsets = (int*)realloc(mgr->offsets, new_cap * sizeof(int));
        char** tmp_names = (char**)realloc(mgr->names, new_cap * sizeof(char*));
        if (!tmp_offsets || !tmp_names) {
            free(tmp_offsets);
            free(tmp_names);
            return -1;
        }
        mgr->offsets = tmp_offsets;
        mgr->names = tmp_names;
        mgr->capacity = new_cap;
    }
    mgr->current_offset += size;
    mgr->offsets[mgr->count] = mgr->current_offset;
    mgr->names[mgr->count] = strdup(name ? name : "");
    mgr->count++;
    return mgr->current_offset;
}

int stack_manager_push_spill(StackManager* mgr, const char* name) {
    return stack_manager_push(mgr, name ? name : "__spill__", 8);
}

int stack_manager_push_param(StackManager* mgr, const char* name, int index) {
    if (!mgr) return -1;
    (void)index;
    return stack_manager_push(mgr, name ? name : "__param__", 8);
}

int stack_manager_get_offset(StackManager* mgr, const char* name) {
    if (!mgr || !name) return 0;
    for (int i = mgr->count - 1; i >= 0; i--) {
        if (strcmp(mgr->names[i], name) == 0) {
            return mgr->offsets[i];
        }
    }
    return 0;
}

int stack_manager_get_frame_size(StackManager* mgr) {
    if (!mgr) return 0;
    int size = mgr->current_offset;
    if (size % 16 != 0) {
        size = ((size + 15) / 16) * 16;
    }
    return size;
}

int stack_manager_get_local_count(StackManager* mgr) {
    if (!mgr) return 0;
    int count = 0;
    for (int i = 0; i < mgr->count; i++) {
        if (mgr->names[i] && strcmp(mgr->names[i], "__spill__") != 0 &&
            strcmp(mgr->names[i], "__param__") != 0) {
            count++;
        }
    }
    return count;
}

int stack_manager_get_spill_count(StackManager* mgr) {
    if (!mgr) return 0;
    int count = 0;
    for (int i = 0; i < mgr->count; i++) {
        if (mgr->names[i] && strcmp(mgr->names[i], "__spill__") == 0) {
            count++;
        }
    }
    return count;
}

int stack_manager_get_param_count(StackManager* mgr) {
    if (!mgr) return 0;
    int count = 0;
    for (int i = 0; i < mgr->count; i++) {
        if (mgr->names[i] && strcmp(mgr->names[i], "__param__") == 0) {
            count++;
        }
    }
    return count;
}

void stack_manager_reset(StackManager* mgr) {
    if (!mgr) return;
    mgr->count = 0;
    mgr->current_offset = 0;
}
