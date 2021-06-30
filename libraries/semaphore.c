#include <stdint.h>
#include <stdbool.h>
#include <arch/arch.h>
#include <libraries/semaphore.h>

#ifndef CONFIG_SEMAPHORE_COUNT
#define CONFIG_SEMAPHORE_COUNT 1
#endif

typedef struct semaphore {
    bool is_initialized;
    uint32_t count;
    uint32_t capacity;
} semaphore_t;

static semaphore_t g_semaphores[CONFIG_SEMAPHORE_COUNT];

static bool semaphore_invalid_handle(semaphore_handle_t handle)
{
    return (handle >= CONFIG_SEMAPHORE_COUNT || !g_semaphores[handle].is_initialized);
}

static error_t semaphore_find_free_semaphore(semaphore_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    semaphore_handle_t semaphore_handle;
    for (semaphore_handle = 0; semaphore_handle < CONFIG_SEMAPHORE_COUNT; semaphore_handle++) {
        if (!g_semaphores[semaphore_handle].is_initialized) {
            *handle = semaphore_handle;
            return SUCCESS;
        }
    }

    return ERROR_NO_MEMORY;
}

error_t semaphore_init(uint32_t capacity, semaphore_handle_t *handle)
{
    if (capacity == 0 || !handle) {
        return ERROR_INVALID;
    }

    error_t error;
    semaphore_t *semaphore;

    error = semaphore_find_free_semaphore(handle);
    if (error) {
        return error;
    }

    semaphore = &g_semaphores[*handle];
    semaphore->is_initialized = true;
    semaphore->count = capacity;
    semaphore->capacity = capacity;

    return SUCCESS;
}

error_t semaphore_deinit(semaphore_handle_t handle)
{
    if (semaphore_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    semaphore_t *semaphore = &g_semaphores[handle];
    semaphore->is_initialized = false;

    return SUCCESS;
}

error_t semaphore_lock(semaphore_handle_t handle)
{
    if (semaphore_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    semaphore_t *semaphore = &g_semaphores[handle];

    arch_disable_irq();
    if (semaphore->count == 0) {
        arch_enable_irq();
        return ERROR_LOCKED;
    }

    semaphore->count--;
    arch_enable_irq();

    return SUCCESS;
}

error_t semaphore_release(semaphore_handle_t handle)
{
    if (semaphore_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    semaphore_t *semaphore = &g_semaphores[handle];

    arch_disable_irq();
    if (semaphore->count >= semaphore->capacity) {
        arch_enable_irq();
        return ERROR_INVALID;
    }

    semaphore->count++;
    arch_enable_irq();

    return SUCCESS;
}
