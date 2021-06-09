#include <stdint.h>
#include <stdbool.h>
#include <config.h>
#include <libraries/queue.h>
#include <libraries/error.h>
#include <libraries/string.h>

#ifndef CONFIG_QUEUE_COUNT
#define CONFIG_QUEUE_COUNT 1
#endif

typedef struct queue {
    bool is_initialized;
    uint32_t capacity;
    uint32_t length;
    uint32_t item_size;
    void *buffer_start;
    void *buffer_end;
    uint8_t *head;
    uint8_t *tail;
} queue_t;

queue_t g_queues[CONFIG_QUEUE_COUNT] = {0};

static error_t queue_find_free_queue(queue_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    queue_handle_t queue_handle;

    for (queue_handle = 0; queue_handle < CONFIG_QUEUE_COUNT; queue_handle++) {
        if (!g_queues[queue_handle].is_initialized) {
            *handle = queue_handle;
            return SUCCESS;
        }
    }

    return ERROR_NO_MEMORY;
}

static error_t queue_invalid_handle(queue_handle_t handle)
{
    return (handle >= CONFIG_QUEUE_COUNT || !g_queues[handle].is_initialized);
}

error_t queue_init(uint32_t queue_capacity,
                   uint32_t queue_item_size,
                   void *queue_buffer,
                   queue_handle_t *handle)
{
    error_t error;
    error = queue_find_free_queue(handle);
    if (error) {
        return error;
    }

    queue_t *queue = &g_queues[*handle];
    queue->is_initialized = true;
    queue->capacity = queue_capacity;
    queue->length = 0;
    queue->item_size = queue_item_size;
    queue->buffer_start = queue_buffer;
    queue->buffer_end = queue_buffer + ((queue_capacity - 1) * queue_item_size);
    queue->head = queue->buffer_end;
    queue->tail = queue->buffer_start;

    return SUCCESS;
}

error_t queue_deinit(queue_handle_t handle)
{
    if (queue_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    g_queues[handle].is_initialized = false;

    return SUCCESS;
}

error_t queue_push(queue_handle_t handle, void *item)
{
    if (queue_invalid_handle(handle) || !item) {
        return ERROR_INVALID;
    }

    queue_t *queue = &g_queues[handle];
    if (queue->length >= queue->capacity) {
        return ERROR_NO_MEMORY;
    }

    if (queue->tail > (uint8_t *)queue->buffer_end) {
        queue->tail = queue->buffer_start;
    }

    memcpy(queue->tail, item, queue->item_size);
    queue->tail += queue->item_size;
    queue->length += 1;

    return SUCCESS;
}

error_t queue_pop(queue_handle_t handle, void *item)
{
    if (queue_invalid_handle(handle) || !item) {
        return ERROR_INVALID;
    }

    queue_t *queue = &g_queues[handle];
    if (queue->length == 0) {
        return ERROR_NO_MEMORY;
    }

    queue->head += queue->item_size;
    if (queue->head > (uint8_t *)queue->buffer_end) {
        queue->head = queue->buffer_start;
    }

    memcpy(item, queue->head, queue->item_size);
    queue->length -= 1;

    return SUCCESS;
}
