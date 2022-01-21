#include <stdint.h>
#include <stdbool.h>
#include <config.h>
#include <libraries/string.h>
#include <libraries/queue.h>
#include <mcu/system_timer.h>
#include <kernel/debug/log.h>
#include <kernel/task/task_manager.h>
#include <arch/arch.h>

task_t g_current_task = {0};
uint32_t g_idle_task_stack[500] = {0};
static queue_handle_t g_task_queue_handle;
static task_t g_tasks[CONFIG_TASK_MAX] = {0};
static task_handle_t g_idle_task_handle = 0;
static bool g_task_manager_started = false;

static void idle_task_entry()
{
    //log_info("IDLE TASK!");
    while(1);
}

error_t task_manager_init()
{
    error_t error;
    system_timer_init();

    error = queue_init(CONFIG_TASK_MAX,
                       sizeof(task_t),
                       g_tasks,
                       &g_task_queue_handle);
    if (error) {
        return error;
    }

    error = task_manager_init_task(&idle_task_entry,
                                   0,
                                   500,
                                   g_idle_task_stack,
                                   &g_idle_task_handle);
    if (error) {
        return error;
    }

    return SUCCESS;
}

error_t task_manager_deinit()
{
    // TODO: Implement
    return SUCCESS;
}

void task_manager_start()
{
    g_task_manager_started = true;
    system_timer_start();
    arch_disable_irq();
    queue_pop(g_task_queue_handle, &g_current_task);
    arch_start_first_task();
}

void task_manager_schedule()
{
    error_t error;
    bool looking_for_task = true;

    queue_push(g_task_queue_handle, &g_current_task);

    while (looking_for_task) {
        error = queue_pop(g_task_queue_handle, &g_current_task);
        if (error) {
            // A very bad thing happened
            // TODO: jump to fault
            while(1);
        }

        if (g_current_task.status == TASK_STATUS_BLOCKED) {
            uint32_t tick = system_timer_get_tick_count();
            if (tick < g_current_task.block_until) {
                queue_push(g_task_queue_handle, &g_current_task);
                continue;
            }

            g_current_task.status = TASK_STATUS_READY;
        }

        looking_for_task = false;
    }
}

error_t task_manager_init_task(void (*task_entry)(void),
                               uint32_t priority,
                               uint32_t stack_size,
                               uint32_t *stack,
                               task_handle_t *handle)
{
    error_t error;
    task_t task = {0};

    task.stack = arch_context_init(&stack[stack_size-1], task_entry);
    task.stack_size = stack_size;
    task.task_entry = task_entry;
    task.priority = priority;
    task.status = TASK_STATUS_READY;

    error = queue_push(g_task_queue_handle, &task);
    if (error) {
        return error;
    }

    *handle = queue_length(g_task_queue_handle) - 1;

    return SUCCESS;
}

error_t task_manager_deinit_task(task_handle_t handle)
{
    // TODO: Implement
    return SUCCESS;
}

void task_manager_task_wait_ms(uint32_t ms)
{
    g_current_task.block_until = system_timer_get_tick_count() + ms;
    g_current_task.status = TASK_STATUS_BLOCKED;

    arch_context_switch();
}

error_t task_manager_block_task(task_handle_t handle)
{
    // TODO: Implement
    return SUCCESS;
}

error_t task_manager_unblock_task(task_handle_t handle)
{
    // TODO: Implement
    return SUCCESS;
}

bool task_manager_started()
{
    return g_task_manager_started;
}
