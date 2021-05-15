#include <stdint.h>
#include <stdbool.h>
#include <kernel/task/task_manager.h>
#include <libraries/string.h>
#include <kernel/debug/log.h>

task_manager_t g_task_manager;
extern uint32_t *_bootloader_magic[];

task_t *task_manager_get_task_by_name(string_t name)
{
    if (name.error) {
        return (task_t *)0;
    }

    task_t *task = 0;
    for (uint32_t i = 0; i < g_task_manager.task_count; i++) {
        if (string_is_equal(g_task_manager.task_list[i].name, name)) {
            task = &g_task_manager.task_list[i];
            break;
        }
    }

    return task;
}

uint32_t task_manager_start_task_by_name(string_t name)
{
    if (name.error) {
        return 1;
    }

    task_t *task = task_manager_get_task_by_name(name);
    if (!task) {
        log_error(ERROR_INVALID, "Failed to find task %s", name);
        return -1;
    }
    task->status = TASK_STATUS_START;
    return 0;
}

void task_manager_add_task(string_t task_name, void (*task_start)(void), void *task_data)
{
    if (g_task_manager.task_count == TASK_LIST_MAX || task_name.error) {
        return;
    }

    task_t *task = &g_task_manager.task_list[g_task_manager.task_count];
    task->name = string_init(task->name_data, 0, TASK_NAME_MAX);
    string_copy(&task->name, task_name);
    task->task_start = task_start;
    task->task_data = task_data;
    g_task_manager.task_count++;
}

task_manager_t *task_manager_get()
{
    return &g_task_manager;
}

bool task_manager_task_is_running(task_t *task)
{
    if (!task) {
        return false;
    }

    return task->status == TASK_STATUS_RUNNING;
}

void task_manager_init()
{
    memset(&g_task_manager, 0, sizeof(task_manager_t));
}

void task_manager_start()
{
    while (1) {
        task_t *running_task = 0;
        for (uint32_t i = 0; i < g_task_manager.task_count; i++) {
            task_t *task = &g_task_manager.task_list[i];

            if (task->status == TASK_STATUS_STOP) {
                task->status = TASK_STATUS_STOPPED;
            }

            if (task->status == TASK_STATUS_START) {
                running_task = task;
            }
        }

        if (running_task) {
            running_task->status = TASK_STATUS_RUNNING;
            running_task->task_start();
        }
    }
}
