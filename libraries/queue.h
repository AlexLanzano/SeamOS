#ifndef QUEUE_H
#define QUEUE_H

#include <libraries/error.h>

typedef uint32_t queue_handle_t;

error_t queue_init(uint32_t queue_capacity,
                   uint32_t queue_item_size,
                   void *queue_buffer,
                   queue_handle_t *handle);
error_t queue_deinit(queue_handle_t handle);
error_t queue_push(queue_handle_t handle, void *item);
error_t queue_pop(queue_handle_t handle, void *item);
uint32_t queue_length(queue_handle_t handle);

#endif
