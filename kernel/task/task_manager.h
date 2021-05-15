#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <libraries/string.h>

#define TASK_LIST_MAX 16
#define TASK_NAME_MAX 16

typedef enum task_status {
    TASK_STATUS_STOPPED,
    TASK_STATUS_STOP,
    TASK_STATUS_RUNNING,
    TASK_STATUS_START
} task_status_t;

typedef struct task {
    task_status_t status;
    char name_data[TASK_NAME_MAX];
    string_t name;
    void *task_data;
    void (*task_start)(void);
} task_t;

typedef struct task_manager{
    uint32_t task_count;
    task_t task_list[TASK_LIST_MAX];
} task_manager_t;


task_t *task_manager_get_task_by_name(string_t name);
uint32_t task_manager_start_task_by_name(string_t name);
void task_manager_add_task(string_t task_name, void (*task_start)(void), void *task_data);
task_manager_t *task_manager_get();
bool task_manager_task_is_running(task_t *task);
void task_manager_init();
void task_manager_start();

#endif
