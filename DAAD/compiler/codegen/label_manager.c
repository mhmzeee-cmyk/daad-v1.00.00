/**
 * @file label_manager.c
 * @brief تنفيذ مدير التسميات
 */

#include "label_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

LabelManager* label_manager_create(void) {
    LabelManager* mgr = (LabelManager*)calloc(1, sizeof(LabelManager));
    if (!mgr) return NULL;
    mgr->capacity = 256;
    mgr->labels = (char**)calloc(mgr->capacity, sizeof(char*));
    mgr->line_numbers = (int*)calloc(mgr->capacity, sizeof(int));
    if (!mgr->labels || !mgr->line_numbers) {
        free(mgr->labels);
        free(mgr->line_numbers);
        free(mgr);
        return NULL;
    }
    mgr->next_id = 1;
    return mgr;
}

void label_manager_destroy(LabelManager* mgr) {
    if (!mgr) return;
    for (int i = 0; i < mgr->count; i++) {
        free(mgr->labels[i]);
    }
    free(mgr->labels);
    free(mgr->line_numbers);
    free(mgr);
}

const char* label_manager_generate(LabelManager* mgr, const char* prefix) {
    if (!mgr) return NULL;
    if (mgr->count >= mgr->capacity) {
        int new_cap = mgr->capacity * 2;
        char** tmp_labels = (char**)realloc(mgr->labels, new_cap * sizeof(char*));
        int* tmp_lines = (int*)realloc(mgr->line_numbers, new_cap * sizeof(int));
        if (!tmp_labels || !tmp_lines) {
            free(tmp_labels);
            free(tmp_lines);
            return NULL;
        }
        mgr->labels = tmp_labels;
        mgr->line_numbers = tmp_lines;
        mgr->capacity = new_cap;
    }
    char buf[128];
    snprintf(buf, sizeof(buf), ".L%s%d", prefix ? prefix : "", mgr->next_id++);
    mgr->labels[mgr->count] = strdup(buf);
    mgr->line_numbers[mgr->count] = 0;
    return mgr->labels[mgr->count++];
}

int label_manager_get_line(LabelManager* mgr, const char* label) {
    if (!mgr || !label) return -1;
    for (int i = 0; i < mgr->count; i++) {
        if (strcmp(mgr->labels[i], label) == 0) {
            return mgr->line_numbers[i];
        }
    }
    return -1;
}

void label_manager_reset(LabelManager* mgr) {
    if (!mgr) return;
    for (int i = 0; i < mgr->count; i++) {
        free(mgr->labels[i]);
        mgr->labels[i] = NULL;
    }
    mgr->count = 0;
    mgr->next_id = 1;
}