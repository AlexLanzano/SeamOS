#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <config.h>
#include <libraries/error.h>
#include <libraries/string.h>

typedef uint32_t task_handle_t;

typedef enum task_status {
    TASK_STATUS_IDLE,
    TASK_STATUS_READY,
    TASK_STATUS_BLOCKED,
} task_status_t;

typedef struct task {
    uint32_t *stack;
    uint32_t stack_size;
    uint32_t priority;
    task_status_t status;
    uint32_t block_until;
    void (*task_entry)(void);
} task_t;

error_t task_manager_init();
error_t task_manager_deinit();
void task_manager_start();
void task_manager_schedule();

error_t task_manager_init_task(void (*task_entry)(void),
                               uint32_t priority,
                               uint32_t stack_size,
                               uint32_t *stack,
                               task_handle_t *handle);
error_t task_manager_deinit_task(task_handle_t handle);
void task_manager_task_wait_ms(uint32_t ms);
error_t task_manager_block_task(task_handle_t handle);
error_t task_manager_unblock_task(task_handle_t handle);

#endif
