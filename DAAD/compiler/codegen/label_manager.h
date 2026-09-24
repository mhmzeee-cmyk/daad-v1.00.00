/**
 * @file label_manager.h
 * @brief مدير التسميات
 */

#ifndef DAAD_LABEL_MANAGER_H
#define DAAD_LABEL_MANAGER_H

typedef struct {
    char** labels;
    int* line_numbers;
    int count;
    int capacity;
    int next_id;
} LabelManager;

LabelManager* label_manager_create(void);
void label_manager_destroy(LabelManager* mgr);
const char* label_manager_generate(LabelManager* mgr, const char* prefix);
int label_manager_get_line(LabelManager* mgr, const char* label);
void label_manager_reset(LabelManager* mgr);

#endif