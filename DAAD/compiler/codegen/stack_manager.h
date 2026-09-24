/**
 * @file stack_manager.h
 * @brief مدير المكدّس - محدّث مع Backend الموحد
 */

#ifndef DAAD_STACK_MANAGER_H
#define DAAD_STACK_MANAGER_H

typedef struct {
    int* offsets;
    char** names;
    int count;
    int capacity;
    int current_offset;
} StackManager;

StackManager* stack_manager_create(void);
void stack_manager_destroy(StackManager* mgr);
int stack_manager_push(StackManager* mgr, const char* name, int size);
int stack_manager_push_spill(StackManager* mgr, const char* name);
int stack_manager_push_param(StackManager* mgr, const char* name, int index);
int stack_manager_get_offset(StackManager* mgr, const char* name);
int stack_manager_get_frame_size(StackManager* mgr);
int stack_manager_get_local_count(StackManager* mgr);
int stack_manager_get_spill_count(StackManager* mgr);
int stack_manager_get_param_count(StackManager* mgr);
void stack_manager_reset(StackManager* mgr);

#endif
